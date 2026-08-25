/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2025-2026  The Bochs Project
//
//  ATI Rage 128 PRO emulation: PM4 / CCE command processor.
//
//  Drivers submit 2D and 3D work as PM4 packets in a ring buffer in host
//  memory, bus-master fetched through the PCI GART or the AGP aperture.
//  Execution is split across two threads (the pattern of the Voodoo FIFO
//  thread): the CPU thread copies ring dwords into a device-local FIFO on
//  every write-pointer write (the "pump"), and the CCE thread parses and
//  executes the packets from that FIFO. Ported from the 86Box Rage 128
//  Pro emulation (vid_ati_rage128_pm4.c).
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
//
/////////////////////////////////////////////////////////////////////////

#define BX_PLUGGABLE

#include "iodev.h"
#if BX_SUPPORT_PCI && BX_SUPPORT_RAGE128

#include "vgacore.h"
#include "pci.h"
#include "ddc.h"
#include "rage128_regs.h"
#include "rage128.h"
#include "virt_timer.h"

#define LOG_THIS BX_RAGE128_THIS

// Thread-local marker: set on the CCE thread only, so register dispatch
// can tell engine pokes coming from the packet executor apart from
// direct CPU accesses.
#if defined(_MSC_VER)
static __declspec(thread) int rage128_on_cce = 0;
#else
static __thread int rage128_on_cce = 0;
#endif

bool bx_rage128_c::on_cce_thread(void)
{
  return rage128_on_cce != 0;
}

BX_THREAD_FUNC(rage128_cce_thread_func, indata)
{
  bx_rage128_c *dev = (bx_rage128_c *)indata;
  rage128_on_cce = 1;
  dev->cce_thread_main();
  BX_THREAD_EXIT;
}

// ---------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------

void bx_rage128_c::pm4_reset(void)
{
  pm4_drain_wait();
  if (cce_thread_started)
    bx_virt_timer.deactivate_timer(pump_timer_id);
  pm4_buffer_offset = 0;
  pm4_buffer_cntl = 0;
  pm4_wm_cntl = 0;
  pm4_rptr = 0;
  pm4_wptr = 0;
  pm4_rptr_addr = 0;
  pm4_wptr_delay = 0;
  pm4_microcode_addr = 0;
  pm4_micro_cntl = 0;
  cce_retire_rptr = 0;
  cce_shadow_last = 0xffffffff;
  pm4_iw_indoff = 0;
  pm4_iw_indsize = 0;
  pci_gart_page = 1;
  pm4_ind_busy = false;
  pm4_ind_pending = false;
  pump_frame_rem = 0;
  pump_ib_state = 0;
  pump_ib_addr = 0;
}

void bx_rage128_c::pm4_thread_init(void)
{
  if (cce_thread_started)
    return;
  cce_fifo = new Bit32u[RAGE128_CCE_FIFO_DWORDS];
  cce_fifo_rptr = new Bit32u[RAGE128_CCE_FIFO_DWORDS];
  cce_fifo_tag = new Bit8u[RAGE128_CCE_FIFO_DWORDS];
  cce_pl = new Bit32u[RAGE128_PM4_MAX_PAYLOAD];
  ind_pl = new Bit32u[RAGE128_PM4_MAX_PAYLOAD];
  cce_fifo_rd = 0;
  cce_fifo_wr = 0;
  cce_retire_rptr = 0;
  cce_retire_pending = 0;
  cce_batch_pending = 0;
  cce_shadow_last = 0xffffffff;
  cce_executing = 0;
  cce_drain_req = 0;
  cce_abort = 0;
  gui_idle_event = 0;
  pump_frame_rem = 0;
  pump_ib_state = 0;
  BX_INIT_MUTEX(cce_mutex);
  bx_create_sem(&cce_wake_sem);
  bx_create_sem(&cce_idle_sem);
  bx_create_sem(&cce_flip_sem);
  cce_thread_run = true;
  cce_thread_started = true;
  BX_THREAD_CREATE(rage128_cce_thread_func, this, cce_thread_var);
}

void bx_rage128_c::pm4_thread_close(void)
{
  if (!cce_thread_started)
    return;
  cce_thread_run = false;
  cce_abort = 1;
  bx_set_sem(&cce_wake_sem);
  bx_set_sem(&cce_flip_sem);
  BX_THREAD_JOIN(cce_thread_var);
  bx_destroy_sem(&cce_wake_sem);
  bx_destroy_sem(&cce_idle_sem);
  bx_destroy_sem(&cce_flip_sem);
  BX_FINI_MUTEX(cce_mutex);
  delete [] cce_fifo; cce_fifo = NULL;
  delete [] cce_fifo_rptr; cce_fifo_rptr = NULL;
  delete [] cce_fifo_tag; cce_fifo_tag = NULL;
  delete [] cce_pl; cce_pl = NULL;
  delete [] ind_pl; ind_pl = NULL;
  cce_thread_started = false;
}

// ---------------------------------------------------------------------
// Bus access: PCI GART / AGP aperture
// ---------------------------------------------------------------------

bool bx_rage128_c::pm4_bus_master_ok(void)
{
  return (pci_conf[0x04] & 0x04) && !(bus_cntl & RAGE128_BUS_CNTL_BUS_MASTER_DIS);
}

// Resolve a CCE "VM space" address to the bus address: AGP mode adds
// AGP_BASE to offset[24:0]; PCI GART mode passes the bus offset through.
Bit32u bx_rage128_c::pm4_vm_addr(Bit32u vm)
{
  if (pci_gart_page & 1)
    return agp_base + (vm & 0x01ffffff);
  return vm;
}

static BX_CPP_INLINE Bit32u rage128_read_phys32(Bit32u addr)
{
  Bit8u buf[4];
  DEV_MEM_READ_PHYSICAL(addr, 4, buf);
  return ReadHostDWordFromLittleEndian((Bit32u*)buf);
}

static BX_CPP_INLINE void rage128_write_phys32(Bit32u addr, Bit32u val)
{
  Bit8u buf[4];
  WriteHostDWordToLittleEndian((Bit32u*)buf, val);
  DEV_MEM_WRITE_PHYSICAL(addr, 4, buf);
}

// Bochs memory accesses must stay within one 4 KB page: the memory
// handler is looked up for the start address and receives the whole
// length, and the i440BX AGP aperture handler translates only the page of
// the start address through the GART. A bulk transfer (texture staging,
// AGP surface spans) therefore has to be split at page boundaries, or
// everything past the first page is read from whatever physical pages
// happen to follow the first GART page.
void bx_rage128_c::phys_read_chunked(Bit32u addr, Bit8u *dst, Bit32u len)
{
  while (len) {
    Bit32u n = 0x1000 - (addr & 0xfff);
    if (n > len) n = len;
    DEV_MEM_READ_PHYSICAL(addr, n, dst);
    addr += n;
    dst += n;
    len -= n;
  }
}

void bx_rage128_c::phys_write_chunked(Bit32u addr, const Bit8u *src, Bit32u len)
{
  while (len) {
    Bit32u n = 0x1000 - (addr & 0xfff);
    if (n > len) n = len;
    DEV_MEM_WRITE_PHYSICAL(addr, n, (Bit8u*)src);
    addr += n;
    src += n;
    len -= n;
  }
}

bool bx_rage128_c::pm4_bus_read(Bit32u bus_addr, Bit32u *val)
{
  if (!pm4_bus_master_ok())
    return false;
  if (pci_gart_page & 1) {
    if (!agp_base)
      return false;
    *val = rage128_read_phys32(bus_addr);
    return true;
  }
  Bit32u table = pci_gart_page & 0xfffff000;
  Bit32u pte = rage128_read_phys32(table + ((bus_addr >> 12) & 0x1fff) * 4);
  Bit32u phys = (pte & 0xfffff000) | (bus_addr & 0xfff);
  *val = rage128_read_phys32(phys);
  return true;
}

bool bx_rage128_c::pm4_bus_read_block(Bit32u vm, Bit8u *dst, Bit32u len)
{
  if (!pm4_bus_master_ok())
    return false;
  if (pci_gart_page & 1) {
    if (!agp_base)
      return false;
    phys_read_chunked(pm4_vm_addr(vm), dst, len);
    return true;
  }
  if ((vm & R128_CARD_AGP_HALF) && agp_base) {
    phys_read_chunked(agp_base + (vm & 0x01ffffff), dst, len);
    return true;
  }
  // PCI GART: one page-table entry per 4 KB page
  {
    Bit32u table = pci_gart_page & 0xfffff000;
    while (len) {
      Bit32u n = 0x1000 - (vm & 0xfff);
      if (n > len) n = len;
      Bit32u pte = rage128_read_phys32(table + ((vm >> 12) & 0x1fff) * 4);
      DEV_MEM_READ_PHYSICAL((pte & 0xfffff000) | (vm & 0xfff), n, dst);
      vm += n;
      dst += n;
      len -= n;
    }
  }
  return true;
}

bool bx_rage128_c::pm4_bus_write(Bit32u bus_addr, Bit32u val)
{
  if (!pm4_bus_master_ok())
    return false;
  if (pci_gart_page & 1) {
    if (!agp_base)
      return false;
    rage128_write_phys32(bus_addr, val);
    return true;
  }
  Bit32u table = pci_gart_page & 0xfffff000;
  Bit32u pte = rage128_read_phys32(table + ((bus_addr >> 12) & 0x1fff) * 4);
  Bit32u phys = (pte & 0xfffff000) | (bus_addr & 0xfff);
  rage128_write_phys32(phys, val);
  return true;
}

bool bx_rage128_c::pm4_bus_write_block(Bit32u vm, const Bit8u *src, Bit32u len)
{
  if (!pm4_bus_master_ok())
    return false;
  if (pci_gart_page & 1) {
    if (!agp_base)
      return false;
    phys_write_chunked(pm4_vm_addr(vm), src, len);
    return true;
  }
  if ((vm & R128_CARD_AGP_HALF) && agp_base) {
    phys_write_chunked(agp_base + (vm & 0x01ffffff), src, len);
    return true;
  }
  // PCI GART: one page-table entry per 4 KB page
  {
    Bit32u table = pci_gart_page & 0xfffff000;
    while (len) {
      Bit32u n = 0x1000 - (vm & 0xfff);
      if (n > len) n = len;
      Bit32u pte = rage128_read_phys32(table + ((vm >> 12) & 0x1fff) * 4);
      DEV_MEM_WRITE_PHYSICAL((pte & 0xfffff000) | (vm & 0xfff), n, (Bit8u*)src);
      vm += n;
      src += n;
      len -= n;
    }
  }
  return true;
}

// ---------------------------------------------------------------------
// Engine state predicates
// ---------------------------------------------------------------------

Bit32u bx_rage128_c::pm4_ring_mask(void)
{
  Bit32u l2 = pm4_buffer_cntl & 0x3f;
  if ((l2 == 0) || (l2 > 20))
    return 0x3ffff;
  return (1u << (l2 + 1)) - 1;
}

bool bx_rage128_c::pm4_cce_running(void)
{
  return (pm4_buffer_cntl & 0xf0000000) != 0;
}

// Only the even modes 2/4/6/8 bus-master the primary stream
bool bx_rage128_c::pm4_ring_bm(void)
{
  Bit32u mode = pm4_buffer_cntl >> 28;
  return (mode != 0) && (mode <= 8) && ((mode & 1) == 0);
}

bool bx_rage128_c::pm4_active(void)
{
  return (cce_fifo_rd != cce_fifo_wr) || cce_executing || cce_batch_pending ||
         (pm4_ring_bm() && (pm4_rptr != (pm4_wptr & pm4_ring_mask())));
}

static BX_CPP_INLINE Bit32u rage128_cce_fifo_space(Bit32u wr, Bit32u rd)
{
  return RAGE128_CCE_FIFO_DWORDS - (wr - rd);
}

// Wait for the executor to free `need` FIFO dwords (CPU thread)
void bx_rage128_c::cce_fifo_reserve(Bit32u wr, Bit32u need)
{
  int waited = 0;
  if (rage128_cce_fifo_space(wr, cce_fifo_rd) >= need)
    return;
  int was_draining = cce_drain_req;
  cce_drain_req = 1;
  bx_set_sem(&cce_flip_sem);
  while (cce_thread_run && (rage128_cce_fifo_space(wr, cce_fifo_rd) < need) && (waited < 250)) {
    bx_set_sem(&cce_wake_sem);
    bx_wait_sem(&cce_idle_sem);
    waited++;
  }
  cce_drain_req = was_draining;
  if (rage128_cce_fifo_space(wr, cce_fifo_rd) < need)
    BX_ERROR(("IB splice gave up waiting for FIFO space: need=%u", need));
}

// Copy one indirect buffer into the FIFO behind its submit packet
Bit32u bx_rage128_c::pm4_splice_ib(Bit32u wr, Bit32u off, Bit32u n, Bit32u rptr)
{
  if (!n)
    return wr;
  if (n > RAGE128_CCE_FIFO_DWORDS / 2) {
    BX_ERROR(("IB too large to splice: off=%08x n=%u", off, n));
    return wr;
  }
  cce_fifo_reserve(wr, n);
  if (rage128_cce_fifo_space(wr, cce_fifo_rd) < n)
    return wr;
  for (Bit32u i = 0; i < n; i++) {
    Bit32u v;
    if (!pm4_bus_read(pm4_vm_addr(off + i * 4), &v)) {
      BX_ERROR(("IB splice dead bus: off=%08x n=%u at=%u", off, n, i));
      return wr - i;
    }
    cce_fifo[wr & RAGE128_CCE_FIFO_MASK] = v;
    cce_fifo_rptr[wr & RAGE128_CCE_FIFO_MASK] = rptr;
    cce_fifo_tag[wr & RAGE128_CCE_FIFO_MASK] = RAGE128_CCE_TAG_IB;
    wr++;
  }
  return wr;
}

// Pump: ring -> local FIFO copy (CPU thread). Returns 1 when the ring is
// fully fetched, 0 when the FIFO is full, -1 on a dead bus read.
int bx_rage128_c::pm4_pump(void)
{
  Bit32u mask, wptr, wr;
  bool copied = false;
  int result = 1;

  if (!pm4_ring_bm())
    return 1;
  mask = pm4_ring_mask();
  wptr = pm4_wptr & mask;
  wr = cce_fifo_wr;

  while (pm4_rptr != wptr) {
    Bit32u v;
    if (rage128_cce_fifo_space(wr, cce_fifo_rd) < 4) {
      cce_fifo_reserve(wr, 4);
      if (rage128_cce_fifo_space(wr, cce_fifo_rd) < 4) {
        result = 0;
        break;
      }
    }
    if (!pm4_bus_read(pm4_vm_addr(pm4_buffer_offset + pm4_rptr * 4), &v)) {
      result = -1;
      break;
    }
    pm4_rptr = (pm4_rptr + 1) & mask;
    cce_fifo[wr & RAGE128_CCE_FIFO_MASK] = v;
    cce_fifo_rptr[wr & RAGE128_CCE_FIFO_MASK] = pm4_rptr;
    cce_fifo_tag[wr & RAGE128_CCE_FIFO_MASK] = 0;
    wr++;
    copied = true;

    if (pump_frame_rem) {
      pump_frame_rem--;
      if (pump_ib_state == 1) {
        pump_ib_addr = v;
        pump_ib_state = 2;
      } else if (pump_ib_state == 2) {
        pump_ib_state = 0;
        wr = pm4_splice_ib(wr, pump_ib_addr, v, pm4_rptr);
      }
    } else {
      switch (RAGE128_PM4_TYPE(v)) {
        case 0:
          pump_frame_rem = RAGE128_PM4_COUNT(v);
          if (v == 0x000101ce) // PACKET0(IW_INDOFF, 2): indirect buffer submit
            pump_ib_state = 1;
          break;
        case 1:
          pump_frame_rem = 2;
          break;
        case 3:
          pump_frame_rem = RAGE128_PM4_COUNT(v);
          break;
        default:
          break;
      }
    }
    if (!pump_ib_state) {
      BX_LOCK(cce_mutex);
      cce_fifo_wr = wr;
      BX_UNLOCK(cce_mutex);
    }
  }
  BX_LOCK(cce_mutex);
  cce_fifo_wr = wr;
  BX_UNLOCK(cce_mutex);

  if (copied)
    bx_set_sem(&cce_wake_sem);

  // Read pointer writeback to the host mailbox at RETIRE
  {
    Bit32u retire = cce_retire_rptr;
    if (pm4_rptr_addr && (retire != cce_shadow_last) && pm4_bus_master_ok()) {
      cce_shadow_last = retire;
      rage128_write_phys32(pm4_rptr_addr, retire);
    }
    if ((result == 0) || (retire != pm4_rptr))
      bx_virt_timer.activate_timer(pump_timer_id, 100, 0);
  }
  return result;
}

void bx_rage128_c::pm4_kick(void)
{
  pm4_pump();
}

// SOFT_RESET_GUI recovery pulse: abandon pending CCE work
void bx_rage128_c::pm4_gui_reset(void)
{
  if (on_cce_thread())
    return;
  if (cce_thread_started && cce_executing) {
    cce_abort = 1;
    bx_set_sem(&cce_wake_sem);
    while (cce_executing)
      bx_wait_sem(&cce_idle_sem);
    cce_abort = 0;
  }
  BX_LOCK(cce_mutex);
  cce_fifo_rd = cce_fifo_wr;
  BX_UNLOCK(cce_mutex);
  pm4_ind_busy = false;
  pm4_ind_pending = false;
  pump_frame_rem = 0;
  pump_ib_state = 0;
  cce_retire_rptr = pm4_rptr;
}

// Does a register belong to the PM4 fetch-control block? A stream that
// writes them is a parse desync; refusing the poke keeps the fetch engine
// from repointing itself.
bool bx_rage128_c::pm4_reg_in_fetch_block(Bit32u reg)
{
  return ((reg >= RAGE128_PM4_BUFFER_OFFSET) && (reg <= RAGE128_PM4_BUFFER_DL_WPTR_DELAY)) ||
         (reg == RAGE128_PM4_MICRO_CNTL) || (reg == RAGE128_PM4_VC_DEBUG_CONFIG);
}

// Wait until the engine is idle: ring fetched, FIFO drained, executor
// between packets (CPU thread only).
void bx_rage128_c::pm4_drain_wait(void)
{
  if (!cce_thread_started || on_cce_thread())
    return;
  if (!pm4_active()) {
    raster_flush();
    return;
  }
  cce_drain_req = 1;
  bx_set_sem(&cce_flip_sem);
  for (long iter = 0;; iter++) {
    int pumped = pm4_pump();
    if (!pm4_active())
      break;
    bool wedged = false;
    if ((pumped == -1) && (cce_fifo_rd == cce_fifo_wr))
      wedged = true;
    if ((iter >= 8) && cce_executing && (cce_fifo_rd == cce_fifo_wr) &&
        (pm4_rptr == (pm4_wptr & pm4_ring_mask())))
      wedged = true;
    if (wedged) {
      if (cce_executing) {
        cce_abort = 1;
        bx_set_sem(&cce_wake_sem);
        while (cce_executing)
          bx_wait_sem(&cce_idle_sem);
        cce_abort = 0;
      }
      break;
    }
    bx_set_sem(&cce_wake_sem);
    bx_wait_sem(&cce_idle_sem);
  }
  raster_flush();
  cce_drain_req = 0;
}

void bx_rage128_c::pm4_flip_notify(void)
{
  if (cce_thread_started)
    bx_set_sem(&cce_flip_sem);
}

// Queue a direct CPU register write behind the engine's pending work as
// a synthetic one-register PACKET0.
bool bx_rage128_c::pm4_enqueue_write(Bit32u off, Bit32u val)
{
  Bit32u rd, wr;
  if (!cce_thread_started)
    return false;
  if ((pm4_rptr != (pm4_wptr & pm4_ring_mask())) && (pm4_pump() != 1))
    return false;
  rd = cce_fifo_rd;
  wr = cce_fifo_wr;
  if (RAGE128_CCE_FIFO_DWORDS - (wr - rd) < 2)
    return false;
  cce_fifo[wr & RAGE128_CCE_FIFO_MASK] = (off >> 2) & 0x1fff;
  cce_fifo[(wr + 1) & RAGE128_CCE_FIFO_MASK] = val;
  cce_fifo_rptr[wr & RAGE128_CCE_FIFO_MASK] = 0xffffffff;
  cce_fifo_rptr[(wr + 1) & RAGE128_CCE_FIFO_MASK] = 0xffffffff;
  cce_fifo_tag[wr & RAGE128_CCE_FIFO_MASK] = 0;
  cce_fifo_tag[(wr + 1) & RAGE128_CCE_FIFO_MASK] = 0;
  BX_LOCK(cce_mutex);
  cce_fifo_wr = wr + 2;
  BX_UNLOCK(cce_mutex);
  bx_set_sem(&cce_wake_sem);
  return true;
}

// Queue an indirect buffer fired by a direct CPU write of IW_INDSIZE behind
// the engine's pending work: a synthetic PACKET0(IW_INDOFF, 2) submit
// followed by the buffer body spliced from guest memory now (so a later
// recycle of the buffer cannot change what executes), exactly the form a
// ring-fired submit takes after the pump. Executing the buffer immediately
// on the CPU thread instead let its draws run ahead of state packets still
// queued in the ring for the executor: the Rage 128 PRO OpenGL ICD writes
// its texture state through the ring and fires the draw buffer by MMIO, so
// every draw sampled the texture the previous draw of the same size had
// bound (Quake III menus painted with the font atlases). Returns false when
// the buffer must be run inline instead (executor not up, ring residue
// unfetchable, FIFO full).
bool bx_rage128_c::pm4_enqueue_indirect(Bit32u off, Bit32u n)
{
  Bit32u wr;

  if (!cce_thread_started || on_cce_thread())
    return false;
  if (n == 0)
    return true;
  if (n > RAGE128_CCE_FIFO_DWORDS / 2)
    return false;
  // a synthetic submit must not jump ahead of unfetched ring dwords
  if ((pm4_rptr != (pm4_wptr & pm4_ring_mask())) && (pm4_pump() != 1))
    return false;
  wr = cce_fifo_wr;
  cce_fifo_reserve(wr, 3 + n);
  if (rage128_cce_fifo_space(wr, cce_fifo_rd) < 3 + n)
    return false;
  // PACKET0(IW_INDOFF, count=2) { off, n }: the executor's poke of
  // IW_INDSIZE runs the walk against the tagged body behind it
  cce_fifo[wr & RAGE128_CCE_FIFO_MASK] = 0x000101ce;
  cce_fifo[(wr + 1) & RAGE128_CCE_FIFO_MASK] = off;
  cce_fifo[(wr + 2) & RAGE128_CCE_FIFO_MASK] = n;
  for (Bit32u i = 0; i < 3; i++) {
    cce_fifo_rptr[(wr + i) & RAGE128_CCE_FIFO_MASK] = 0xffffffff;
    cce_fifo_tag[(wr + i) & RAGE128_CCE_FIFO_MASK] = 0;
  }
  wr = pm4_splice_ib(wr + 3, off, n, 0xffffffff);
  BX_LOCK(cce_mutex);
  cce_fifo_wr = wr;
  BX_UNLOCK(cce_mutex);
  bx_set_sem(&cce_wake_sem);
  return true;
}

// ---------------------------------------------------------------------
// Executor (CCE thread)
// ---------------------------------------------------------------------

// Blocking dword pop; returns false only on shutdown or an explicit abort
bool bx_rage128_c::cce_get(Bit32u *val)
{
  Bit32u rd = cce_fifo_rd;
  while (rd == cce_fifo_wr) {
    if (!cce_thread_run || cce_abort)
      return false;
    bx_set_sem(&cce_idle_sem);
    bx_wait_sem(&cce_wake_sem);
  }
  *val = cce_fifo[rd & RAGE128_CCE_FIFO_MASK];
  Bit32u rr = cce_fifo_rptr[rd & RAGE128_CCE_FIFO_MASK];
  if (rr != 0xffffffff)
    cce_retire_pending = rr;
  BX_LOCK(cce_mutex);
  cce_fifo_rd = rd + 1;
  BX_UNLOCK(cce_mutex);
  return true;
}

// Pop one spliced indirect buffer dword, or return false when the next
// FIFO dword is not body
bool bx_rage128_c::cce_get_ib(Bit32u *val)
{
  Bit32u rd = cce_fifo_rd;
  if ((rd == cce_fifo_wr) || (cce_fifo_tag[rd & RAGE128_CCE_FIFO_MASK] != RAGE128_CCE_TAG_IB))
    return false;
  *val = cce_fifo[rd & RAGE128_CCE_FIFO_MASK];
  BX_LOCK(cce_mutex);
  cce_fifo_rd = rd + 1;
  BX_UNLOCK(cce_mutex);
  return true;
}

// Type-3 dispatch shared by the ring executor and the indirect walker
void bx_rage128_c::pm4_exec_packet3(Bit32u hdr, Bit32u *pl, Bit32u n)
{
  Bit32u op = RAGE128_PM4_T3_OPCODE(hdr);
  if (op == RAGE128_PM4_OP_MPEG_MB)
    return; // MPEG-2 motion compensation assist is not modelled
  if (!r3d_packet3(hdr, pl, n)) {
    if ((op != RAGE128_PM4_OP_SET_SCISSORS) && (op != RAGE128_PM4_OP_NOP))
      raster_flush();
    r2d_packet3(hdr, pl, n);
  }
}

void bx_rage128_c::cce_packet(Bit32u hdr)
{
  Bit32u count;
  Bit32u *pl = cce_pl;

  switch (RAGE128_PM4_TYPE(hdr)) {
    case 0: {
      Bit32u reg = RAGE128_PM4_T0_REG(hdr);
      count = RAGE128_PM4_COUNT(hdr);
      for (Bit32u i = 0; i < count; i++) {
        Bit32u v;
        if (!cce_get(&v))
          return;
        // header bit 15 = one-register mode
        Bit32u treg = (hdr & 0x8000) ? reg : reg + i * 4;
        if (!pm4_reg_in_fetch_block(treg))
          reg_poke(treg, v);
      }
      break;
    }
    case 1:
      for (Bit32u i = 0; i < 2; i++) {
        Bit32u v;
        Bit32u treg = i ? RAGE128_PM4_T1_REG1(hdr) : RAGE128_PM4_T1_REG0(hdr);
        if (!cce_get(&v))
          return;
        if (!pm4_reg_in_fetch_block(treg))
          reg_poke(treg, v);
      }
      break;
    case 2:
      break;
    case 3: {
      Bit32u n;
      count = RAGE128_PM4_COUNT(hdr);
      n = count > RAGE128_PM4_MAX_PAYLOAD ? RAGE128_PM4_MAX_PAYLOAD : count;
      for (Bit32u i = 0; i < count; i++) {
        Bit32u v;
        if (!cce_get(&v))
          return;
        if (i < n)
          pl[i] = v;
      }
      pm4_exec_packet3(hdr, pl, n);
      break;
    }
    default:
      break;
  }
}

void bx_rage128_c::cce_thread_main(void)
{
  bool did_work = false;

  while (cce_thread_run) {
    Bit32u hdr;
    cce_executing = 1;
    if (cce_fifo_rd == cce_fifo_wr) {
      raster_flush();
      if (did_work) {
        did_work = false;
        gui_idle_event = 1;
      }
      cce_executing = 0;
      bx_set_sem(&cce_idle_sem);
      bx_wait_sem(&cce_wake_sem);
      continue;
    }
    if (cce_get(&hdr)) {
      cce_packet(hdr);
      did_work = true;
    }
    cce_retire_rptr = cce_retire_pending;
    cce_executing = 0;
  }
  cce_executing = 0;
  bx_set_sem(&cce_idle_sem);
}

// Indirect buffer execution: PACKET0(IW_INDOFF, count=1) { bus_addr, dwords }
void bx_rage128_c::pm4_run_indirect(void)
{
  if (pm4_ind_busy) {
    pm4_ind_pending = true;
    return;
  }
  pm4_ind_busy = true;
  in_indirect = true;

  do {
    Bit32u off = pm4_iw_indoff;
    Bit32u n = pm4_iw_indsize;
    Bit32u pos = 0;
    bool spliced = false;

    {
      Bit32u rd = cce_fifo_rd;
      spliced = on_cce_thread() && (rd != cce_fifo_wr) && (cce_fifo_tag[rd & RAGE128_CCE_FIFO_MASK] == RAGE128_CCE_TAG_IB);
    }
    pm4_ind_pending = false;

    while (pos < n) {
      Bit32u hdr, count;
      Bit32u *pl = ind_pl;

      if (spliced) {
        if (!cce_get_ib(&hdr))
          break;
      } else if (!pm4_bus_read(pm4_vm_addr(off + pos * 4), &hdr)) {
        BX_DEBUG(("IB dead bus: addr=%08x pos=%u size=%u", off, pos, n));
        break;
      }
      pos++;

      switch (RAGE128_PM4_TYPE(hdr)) {
        case 0: {
          Bit32u reg = RAGE128_PM4_T0_REG(hdr);
          count = RAGE128_PM4_COUNT(hdr);
          for (Bit32u i = 0; (i < count) && (pos < n); i++, pos++) {
            Bit32u v;
            if (spliced) {
              if (!cce_get_ib(&v)) break;
            } else if (!pm4_bus_read(pm4_vm_addr(off + pos * 4), &v)) {
              break;
            }
            Bit32u treg = (hdr & 0x8000) ? reg : reg + i * 4;
            if (!pm4_reg_in_fetch_block(treg))
              reg_poke(treg, v);
          }
          break;
        }
        case 1:
          for (Bit32u i = 0; (i < 2) && (pos < n); i++, pos++) {
            Bit32u v;
            Bit32u treg = i ? RAGE128_PM4_T1_REG1(hdr) : RAGE128_PM4_T1_REG0(hdr);
            if (spliced) {
              if (!cce_get_ib(&v)) break;
            } else if (!pm4_bus_read(pm4_vm_addr(off + pos * 4), &v)) {
              break;
            }
            if (!pm4_reg_in_fetch_block(treg))
              reg_poke(treg, v);
          }
          break;
        case 2:
          break;
        case 3: {
          Bit32u m, filled = 0;
          count = RAGE128_PM4_COUNT(hdr);
          m = count > RAGE128_PM4_MAX_PAYLOAD ? RAGE128_PM4_MAX_PAYLOAD : count;
          for (Bit32u i = 0; (i < count) && (pos < n); i++, pos++) {
            Bit32u v = 0;
            if (spliced) {
              if (!cce_get_ib(&v)) break;
            } else {
              pm4_bus_read(pm4_vm_addr(off + pos * 4), &v);
            }
            if (i < m)
              pl[i] = v;
            filled = i + 1;
          }
          if (m > filled)
            m = filled;
          pm4_exec_packet3(hdr, pl, m);
          break;
        }
        default:
          break;
      }
    }
    // discard the rest of a tagged body this walk did not consume
    if (on_cce_thread()) {
      Bit32u junk;
      while (cce_get_ib(&junk))
        pos++;
    }
  } while (pm4_ind_pending);

  in_indirect = false;
  pm4_ind_busy = false;
}

// WAIT_UNTIL EVENT_CRTC_OFFSET: stall the engine until a pending flip is
// consumed by scanout (CCE thread)
void bx_rage128_c::pm4_wait_until(Bit32u val)
{
  if (on_cce_thread() && (val & 1)) {
    raster_flush();
    while (cce_thread_run && !cce_drain_req && crtc_offset_pending && !crtc_offset_lock)
      bx_wait_sem(&cce_flip_sem);
  }
}

// ---------------------------------------------------------------------
// Register file interface
// ---------------------------------------------------------------------

bool bx_rage128_c::pm4_reg_read(Bit32u off, Bit32u *val)
{
  switch (off) {
    case RAGE128_PM4_BUFFER_OFFSET:   *val = pm4_buffer_offset; return true;
    case RAGE128_PM4_BUFFER_CNTL:     *val = pm4_buffer_cntl; return true;
    case RAGE128_PM4_BUFFER_WM_CNTL:  *val = pm4_wm_cntl; return true;
    case RAGE128_PM4_BUFFER_DL_RPTR_ADDR: *val = pm4_rptr_addr; return true;
    case RAGE128_PM4_BUFFER_DL_RPTR:  *val = cce_retire_rptr; return true;
    case RAGE128_PM4_BUFFER_DL_WPTR:  *val = pm4_wptr; return true;
    case RAGE128_PM4_BUFFER_DL_WPTR_DELAY: *val = pm4_wptr_delay; return true;
    case RAGE128_PM4_VC_FPU_SETUP:    *val = t3d.fpu_setup; return true;
    case RAGE128_PM4_VC_DEBUG_CONFIG: *val = pm4_vc_debug_config; return true;
    case RAGE128_PM4_VC_STAT:         *val = 0; return true;
    case RAGE128_PM4_STAT:
      if (pm4_active())
        *val = RAGE128_PM4_STAT_BUSY | RAGE128_PM4_STAT_GUI_ACTIVE;
      else
        *val = RAGE128_PM4_STAT_FIFOCNT_MASK;
      return true;
    case RAGE128_PM4_MICROCODE_ADDR:  *val = pm4_microcode_addr; return true;
    case RAGE128_PM4_MICROCODE_RADDR:
    case RAGE128_PM4_MICROCODE_DATAH:
    case RAGE128_PM4_MICROCODE_DATAL:
    case RAGE128_PM4_CMDFIFO_ADDR:
      *val = 0; return true;
    case RAGE128_PM4_MICRO_CNTL:      *val = pm4_micro_cntl; return true;
    case RAGE128_PM4_IW_INDOFF:       *val = pm4_iw_indoff; return true;
    case RAGE128_PM4_IW_INDSIZE:      *val = pm4_iw_indsize; return true;
    case RAGE128_PCI_GART_PAGE:       *val = pci_gart_page; return true;
    default:
      break;
  }
  return false;
}

bool bx_rage128_c::pm4_reg_write(Bit32u off, Bit32u val, Bit32u mask)
{
  // Direct CPU writes that reconfigure the ring fetch engine quiesce the
  // executor first (WPTR is the submit doorbell and pumps instead).
  if (!on_cce_thread() && (off != RAGE128_PM4_BUFFER_DL_WPTR) &&
      ((off == RAGE128_PCI_GART_PAGE) || pm4_reg_in_fetch_block(off)))
    pm4_drain_wait();

#define MERGE(field) ((field) = ((field) & ~mask) | (val & mask))
  switch (off) {
    case RAGE128_PM4_BUFFER_OFFSET: MERGE(pm4_buffer_offset); return true;
    case RAGE128_PM4_BUFFER_CNTL:   MERGE(pm4_buffer_cntl); return true;
    case RAGE128_PM4_BUFFER_WM_CNTL: MERGE(pm4_wm_cntl); return true;
    case RAGE128_PM4_BUFFER_DL_RPTR_ADDR: MERGE(pm4_rptr_addr); return true;
    case RAGE128_PM4_BUFFER_DL_RPTR:
      MERGE(pm4_rptr);
      cce_retire_rptr = pm4_rptr;
      pump_frame_rem = 0;
      pump_ib_state = 0;
      return true;
    case RAGE128_PM4_BUFFER_DL_WPTR: {
      Bit32u wmask = pm4_ring_mask();
      Bit32u owptr = pm4_wptr & wmask;
      MERGE(pm4_wptr);
      pm4_wptr &= 0x7fffffff;
      if (pm4_ring_bm()) {
        Bit32u nwptr = pm4_wptr & wmask;
        // a backward move is a ring rebase, not a submit
        if (((owptr - nwptr) & wmask) < ((nwptr - owptr) & wmask)) {
          pm4_rptr = nwptr;
          pump_frame_rem = 0;
          pump_ib_state = 0;
        }
      }
      if (!on_cce_thread())
        pm4_pump();
      return true;
    }
    case RAGE128_PM4_BUFFER_DL_WPTR_DELAY: MERGE(pm4_wptr_delay); return true;
    case RAGE128_PM4_VC_DEBUG_CONFIG: MERGE(pm4_vc_debug_config); return true;
    case RAGE128_PM4_VC_STAT: return true;
    case RAGE128_PM4_MICROCODE_ADDR:
      MERGE(pm4_microcode_addr);
      pm4_microcode_addr &= 0xff;
      return true;
    case RAGE128_PM4_MICROCODE_RADDR:
    case RAGE128_PM4_MICROCODE_DATAH:
      return true;
    case RAGE128_PM4_MICROCODE_DATAL:
      pm4_microcode_addr = (pm4_microcode_addr + 1) & 0xff;
      return true;
    case RAGE128_PM4_CMDFIFO_ADDR: return true;
    case RAGE128_PM4_MICRO_CNTL: MERGE(pm4_micro_cntl); return true;
    case RAGE128_PM4_IW_INDOFF:  MERGE(pm4_iw_indoff); return true;
    case RAGE128_PM4_IW_INDSIZE:
      MERGE(pm4_iw_indsize);
      if (!on_cce_thread() && cce_thread_started) {
        // Direct CPU submit: keep it behind the queued engine work
        if (pm4_enqueue_indirect(pm4_iw_indoff, pm4_iw_indsize))
          return true;
        pm4_drain_wait();
      }
      pm4_run_indirect();
      return true;
    case RAGE128_PM4_VC_FPU_SETUP:
      // 3D setup state written by MMIO must not overtake queued draws
      if (!on_cce_thread() && cce_thread_started && (mask == 0xffffffff) &&
          pm4_enqueue_write(off, val))
        return true;
      MERGE(t3d.fpu_setup);
      return true;
    case RAGE128_WAIT_UNTIL:
      pm4_wait_until(val & mask);
      return true;
    case RAGE128_FLUSH_1: case RAGE128_FLUSH_1 + 4: case RAGE128_FLUSH_1 + 8:
    case RAGE128_FLUSH_1 + 12: case RAGE128_FLUSH_1 + 16: case RAGE128_FLUSH_1 + 20:
    case RAGE128_FLUSH_7:
      return true;
    case RAGE128_PCI_GART_PAGE:
      MERGE(pci_gart_page);
      pci_gart_page &= 0xfffff001;
      return true;
    default:
      break;
  }
#undef MERGE
  return false;
}

#endif // BX_SUPPORT_PCI && BX_SUPPORT_RAGE128
