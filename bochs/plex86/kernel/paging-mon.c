/*
 *  plex86: run multiple x86 operating systems concurrently
 *  Copyright (C) 1999-2003 Kevin P. Lawton
 *
 *  paging-mon.c:  Virtualized (monitor) paging functionality.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */


#include "plex86.h"
#define IN_MONITOR_SPACE
#include "monitor.h"



static unsigned allocatePT(vm_t *, unsigned pdi);
static unsigned strengthenPagePermissions(vm_t *, phyPageInfo_t *usage,
                  unsigned new_access_perm);
/*static void sanity_check_pdir(vm_t *vm, unsigned id, Bit32u guest_laddr); */

/* +++ fix retrieve mon pages function and .base issue */
/*     also open_guest_phy_page expects shifted page val */
/* +++ write_physical() has hack to ignore when perm!=RW, fix! */
/* +++ add async handling in emulation.c, like in preGuest() */

/* Cases which would generate a mon #PF */
/* ==================================== */
/* lazy map */
/* r/w to current code page */
/* guest #PF (access checks of cpl,rw) */
/* w to RO construct */
/* r/w to NA construct */

/* inhibits */


#if 0
======= Old notes =====================================================
IDT,GDT,LDT: limit = 64K; TR is a dont care
What to do with PDir, PTbl?
What to do about coherence probs with page tables and TLB?
When are A,D bits copied between monitor and host?
Need check for mapping of space used by monitor

Code cache probably should not have laddr in it

guest.PG==0, how are phy pages unmarked when constructs move?
guest.PG transition: dump everything (flush)

remapping descriptor tables after page flush
make sure to validate phy_attr everywhere before using it.

checks for the phy_attr of page that PDir goes in
page fault because of monP?E.RW==0, but guestP?E==1

/* +++ what about virtualized linear structs like GDT, IDT, ... */
#endif

#warning "Have to be careful unpinning a page which is open"
#warning "  via open_guest_phy_page().  Multiple pages could be"
#warning "  open in the page walk at one time until D/A bits are set."


  static inline Bit32u
getHostOSPinnedPage(vm_t *vm, Bit32u ppi)
{
  /* If physical page is already pinned by host OS, then we already
   * know the physical address of the page.
   */
  if (vm->pageInfo[ppi].attr.fields.pinned)
    return( vm->pageInfo[ppi].hostPPI );

  /* Page is not already pinned by the host OS.  We need to request
   * from the host OS, that this page is pinned and find the
   * physical address.
   */
  toHostPinUserPage(vm, ppi);
  if ( !vm->pageInfo[ppi].attr.fields.pinned )
    monpanic(vm, "getHostOSPinnedPage: page was not marked pinned.\n");
  return( vm->pageInfo[ppi].hostPPI );
}


  unsigned
allocatePT(vm_t *vm, unsigned pdi)
{
  unsigned map_i;

  /* Allocate one of the (preallocated) pages for */
  /* the monitor to use for a page table at the PDI given. */

  map_i = vm->ptbl_laddr_map_i;

  if (map_i >= MON_PAGE_TABLES) {
    monpanic(vm, "allocatePT: out of page tables\n");
    }
#if ANAL_CHECKS
  if (vm->guest.addr.page_tbl_laddr_map[pdi] != -1) {
    monprint(vm, "allocatePT: check failed.\n");
    monpanic(vm, "  pdi=0x%x, laddr_map=0x%x\n",
      pdi, vm->guest.addr.page_tbl_laddr_map[pdi]);
    }
#endif
  vm->guest.addr.page_tbl_laddr_map[pdi] = map_i;
  vm->ptbl_laddr_map_i++;
  return(map_i);
}

  unsigned
getMonPTi(vm_t *vm, unsigned pdi, unsigned source)
{
  unsigned map_i;
  map_i = vm->guest.addr.page_tbl_laddr_map[pdi];
#if ANAL_CHECKS
  if (map_i == -1) {
    monprint(vm, "getMonPTi: check failed.\n");
    monpanic(vm, "  pdi=0x%x, map_i=0x%x, source=%u\n",
      pdi, map_i, source);
    }
  if (map_i >= MON_PAGE_TABLES)
    monpanic(vm, "getMonPTi: map_i OOB\n");
#endif
  return(map_i);
}


/* Invalidate the mapping of a guest page in the monitor.
 * When situations change, such as a change in the permissions
 * necessary to virtualize the page properly, we'll need to do
 * this first, before remapping with the new permissions.
 */
  unsigned
strengthenPagePermissions(vm_t *vm, phyPageInfo_t *pusage,
                     unsigned new_access_perm)
{
  pusage->attr.fields.access_perm = new_access_perm;

  if (pusage->attr.fields.lmap_count == 0) {
    /* No linear addresses are mapped to this phy page yet.
     * Nothing to do. */
    return 0;
    }
  else if (pusage->attr.fields.lmap_count == 1) {
    /* One linear address is mapped to this phy page. */
    Bit32u pdi, pti;
    pageEntry_t *monPDE, *monPTE;
    page_t      *monPTbl;
    unsigned map_i;

    pdi = (pusage->attr.fields.laddr_backlink >> 10);
    pti = (pusage->attr.fields.laddr_backlink & 0x3ff);
    monPDE = &vm->guest.addr.page_dir[pdi];
    if ( !monPDE->fields.P )
      monpanic(vm, "strengthenPP: monPDE.P==0\n");
    map_i = getMonPTi(vm,pdi,10);
    monPTbl = &vm->guest.addr.page_tbl[map_i];
    monPTE = &monPTbl->pte[pti];
    if ( !monPTE->fields.P ) {

/*monprint(vm, "strengthenPP: bl=0x%x, AP=%u\n",
 *pusage->attr.fields.laddr_backlink, new_access_perm); */

      /*monpanic(vm, "strengthenPP: monPTE.P==0\n"); */
      }
    else if (pusage->attr.fields.access_perm==PagePermNA) {
      /* Permissions were changed to No Access */
      monPTE->raw = 0;
      }
    else if (pusage->attr.fields.access_perm==PagePermRO) {
      /* Permissions were changed to RO */
      monPTE->fields.RW = 0;
      }
    else {
      monpanic(vm, "strengthenPP: PagePermRW\n");
      }
    /* Flush the old TLB entry */
    invlpg_mon_offset(
      Guest2Monitor(vm, pusage->attr.fields.laddr_backlink<<12)
      );
    return 0;
    }
  else {
    /* Multiple linear addresses are mapped to this phy page. */
    /* Since we dont store enough backlink info to virtualize all */
    /* linear addresses which point to this phy page, we have to dump */
    /* all dynamic mappings and start over. */
monpanic(vm, "strengthenPP: multiple lin addr\n");
/*monPagingRemap(vm);*/
    return 1;
    }
}

  unsigned
addPageAttributes(vm_t *vm, Bit32u ppi, Bit32u req_attr)
{
  phyPageInfo_t *pusage;
  unsigned new_access_perm;

  VM_ASSERT(vm, ppi < vm->pages.guest_n_pages);

  pusage = &vm->pageInfo[ppi];
  if (pusage->tsc < vm->vpaging_tsc) {
    /* The dynamic attributes for this page are not valid since
     * the last remap.  getPageUsage() has logic to build attributes.
     */
    getPageUsage(vm, ppi);
    }

  /* Build new attributes based on old ones, and requested ones. */
  pusage->attr.raw |= req_attr;

  /* Look at strength of new access restrictions */
  if (pusage->attr.raw & PageUsageCausesNA)
    new_access_perm = PagePermNA;
  else if (pusage->attr.raw & PageUsageCausesRO)
    new_access_perm = PagePermRO;
  else
    new_access_perm = PagePermRW;

  if (new_access_perm > pusage->attr.fields.access_perm) {
    /* New usage causes a stronger access restriction.  Remap them. */
    return( strengthenPagePermissions(vm, pusage, new_access_perm) );
    }
  return 0;
}

  phyPageInfo_t *
getPageUsage(vm_t *vm, Bit32u ppi)
{
  phyPageInfo_t *pusage;

  VM_ASSERT(vm, ppi < vm->pages.guest_n_pages);
  pusage = &vm->pageInfo[ppi];

  if (pusage->tsc < vm->vpaging_tsc) {
    /* The dynamic attributes for this page are not valid since
     * the last remap.  Clear them out, and timestamp.
     */
    pusage->tsc = vm_rdtsc();
    pusage->attr.raw &= PageUsageSticky;
    if (pusage->attr.raw & PageUsageCausesNA)
      pusage->attr.fields.access_perm = PagePermNA;
    else if (pusage->attr.raw & PageUsageCausesRO)
      pusage->attr.fields.access_perm = PagePermRO;
    else
      pusage->attr.fields.access_perm = PagePermRW;
    }
  return(pusage);
}


  void *
open_guest_phy_page(vm_t *vm, Bit32u ppi, Bit8u *mon_offset)
{
  page_t *pageTable;
  Bit32u  pti, mon_range_offset;

  VM_ASSERT(vm, ppi < vm->pages.guest_n_pages);
  /* Since we rewind our CS/DS.base so that the beginning of our */
  /* monitor pages land on the beginning of a new 4Meg boundary */
  /* (separate PDE), find out what mon_offset is in terms of */
  /* an offset from the beginning of the PDE boundary. */
  mon_range_offset = ( ((Bit32u) mon_offset) -
                       kernelModulePages.startOffsetPageAligned );
  pti = (mon_range_offset >> 12) & 0x3ff;
  pageTable = vm->guest.addr.nexus_page_tbl;

  /* Remap the base field.  All the rest of the fields are */
  /* set previously, and can remain the same. */
  pageTable->pte[pti].fields.base = getHostOSPinnedPage(vm, ppi);
  invlpg_mon_offset( (Bit32u) mon_offset );
  return(mon_offset);
}

  void
close_guest_phy_page(vm_t *vm, Bit32u ppi)
{
  /* ppi is >> 12 already */
  /* +++ */
}

  void
virtualize_lconstruct(vm_t *vm, Bit32u l0, Bit32u l1, unsigned perm)
{
  /* Mark pages for a protected construct in linear space as */
  /* virtualized (protected), if it is mapped into monitor space. */
  /* Pages which are not yet mapped in, are virtualized dynamically */
  /* when they are mapped in. */

  Bit32u pdi, pdi0, pdi1, pti, pti0, pti1;
  pageEntry_t *monPDE, *monPTE;
  page_t      *monPTbl;

/* +++ For now, can just dump all page mappings and start over */
/*     again.  Need to complete this function, so we can virtualize */
/*     only those pages which need it, and keep the other ones. */
/* +++ Need to look at perm also. */
monpanic(vm, "vir_lconstruct: unfinished.\n");
/*monPagingRemap(vm);*/
return;

  if (vm->guest.addr.guest_cpu->cr0.fields.pg)
    monpanic(vm, "virtualize_lconstruct: guest PG==1\n");
  if (l0 >= l1)
    monpanic(vm, "virtualize_lconstruct: l0>=l1!\n");
  if ( (l1-l0) > (64*1024) )
    monpanic(vm, "virtualize_lconstruct: span is > 64k!\n");
  pdi0 = l0 >> 22;
  pdi1 = l1 >> 22;
  pti0 = (l0 >> 12) & 0x000003ff;
  for (pdi=pdi0; pdi<=pdi1; pdi++) {
    if ( pdi == vm->mon_pdi )
      monpanic(vm, "virtualize_lconstruct: conflict with monitor space\n");
    monPDE = &vm->guest.addr.page_dir[pdi];
    if (monPDE->fields.P) {
      if (pdi<pdi1)
        pti1 = 0x3ff; /* spans multiple pdi's, use last index of range */
      else
        pti1 = (l1 >> 12) & 0x000003ff; /* use index of last address */
      for (pti=pti0; pti<=pti1; pti++) {
/* +++ */
/* +++ FIX THIS!!!, set depending on guest.CR0.PG */
/* +++ */
        monPTbl = &vm->guest.addr.page_tbl[pdi];
        monPTE = &monPTbl->pte[pti];
        if (monPTE->fields.P) {
          /* +++ finish this! */
          /* The physical page for this linear address is allocated */
          /* and mapped into the monitor.  We can access the attributes */
          /* for this physical page.  Even if it has been virtualized */
          /* before, we still need to mark it since it could have been */
          /* virtualized due to a physical page constraint. */
          monpanic(vm, "virtualize_lconstruct: finish.\n");
          }
        }
      }
    pti0 = 0; /* start address at boundary of next PDI */
    }
}

  unsigned
mapGuestLinAddr(vm_t *vm, Bit32u guest_laddr, Bit32u *guest_ppi,
                unsigned req_us, unsigned req_rw, Bit32u attr,
                Bit32u *error)
{
  Bit32u       pdi, pti;
  Bit32u       guest_lpage_index, ptbl_ppi;
  page_t      *monPTbl;
  pageEntry_t *monPDE, *monPTE;
  pageEntry_t *guestPDir, guestPDE, *guestPTbl, guestPTE;
  Bit32u       guest_pdir_page_index;
  unsigned     pt_index, us, rw;
  phyPageInfo_t *pusage;
  unsigned wasRemap = 0;

  guest_lpage_index = guest_laddr >> 12;
  pdi = guest_lpage_index >> 10;
  pti = guest_lpage_index & 0x3ff;
  monPDE = &vm->guest.addr.page_dir[pdi];

  if (vm->guest.addr.guest_cpu->cr0.fields.pg) {
    /* Check out the guest's mapping of this address to see */
    /* if would allow for an access. */
    /* First, get the guest PDE */
    guest_pdir_page_index = A20Addr(vm, vm->guest.addr.guest_cpu->cr3) >> 12;
    if (guest_pdir_page_index >= vm->pages.guest_n_pages)
      monpanic(vm, "mapGuestLinAddr: PG=1 guest PDE OOB\n");
    /* Open a window into guest physical memory */
    guestPDir = open_guest_phy_page(vm, guest_pdir_page_index,
                                    vm->guest.addr.tmp_phy_page0);
    guestPDE = guestPDir[pdi];

    /* See if present, before fetching PTE */
    if (guestPDE.fields.P==0) {
      *error = 0x00000000; /* RSVD=0, P=0 */
      goto np_exception;
      }

#if 0
    if (vm->guestCpuIDInfo.procSignature.fields.family < 6) {
      /* Update A bit of PDE memory image if not already */
      if ( guestPDE.fields.A == 0 ) {
        guestPDE.fields.A = 1;
        guestPDir[pdi] = guestPDE;
        }
      }
#endif

    /* Second, get the guest PDE */
    ptbl_ppi = A20PageIndex(vm, guestPDE.fields.base);
    if (ptbl_ppi >= vm->pages.guest_n_pages)
      monpanic(vm, "mapGuestLinAddr: PG=1 guest PTE OOB\n");
    guestPTbl = open_guest_phy_page(vm, ptbl_ppi,
                                    vm->guest.addr.tmp_phy_page1);
    guestPTE = guestPTbl[pti];

    if (guestPTE.fields.P==0) {
      *error = 0x00000000; /* RSVD=0, P=0 */
      goto np_exception;
      }

#if 0
/* +++ */
    if (guestPDE.raw & PDEUnhandled)
      monpanic(vm, "mapGuestLinAddr: guestPDE 0x%08x\n", guestPDE.raw);
#endif
    /* See if requested guest priv is weaker than guest PDE priv */
    if (req_us > guestPDE.fields.US) {
      *error = 0x00000001; /* RSVD=0, P=1 */
      goto access_exception;
      }
    if ( (req_rw > guestPDE.fields.RW) &&
         (vm->guest.addr.guest_cpu->cr0.fields.wp || req_us) ) {
      *error = 0x00000001; /* RSVD=0, P=1 */
      goto access_exception;
      }

#warning "ignoring PTEUnhandled bits"
#if 0
    if (guestPTE.raw & PTEUnhandled)
      monpanic(vm, "mapGuestLinAddr: guestPTE 0x%08x\n", guestPTE.raw);
#endif
    if (req_us > guestPTE.fields.US) {
      *error = 0x00000001; /* RSVD=0, P=1 */
      goto access_exception;
      }
    if ( (req_rw > guestPTE.fields.RW) &&
         (vm->guest.addr.guest_cpu->cr0.fields.wp || req_us) ) {
      *error = 0x00000001; /* RSVD=0, P=1 */
      goto access_exception;
      }

#if 0
    if (vm->guestCpuIDInfo.procSignature.fields.family >= 6) {
      /* Update A bit of PDE memory image if not already */
      if ( guestPDE.fields.A == 0 ) {
        guestPDE.fields.A = 1;
        guestPDir[pdi] = guestPDE;
        }
      }

    /* Update A bit in PTE memory image if not already */
    if ( (guestPTE.fields.A == 0) ||
         ((req_rw==1) && !guestPTE.fields.D) ) {
      guestPTE.fields.A = 1;
      if (req_rw==1)
        guestPTE.fields.D = 1;
      guestPTbl[pti] = guestPTE;
      }
#endif
    
    *guest_ppi = A20PageIndex(vm, guestPTE.fields.base);
    }
  else {
    /* guest paging is off, linear address is physical address */
    guest_pdir_page_index = 0; /* keep compiler quiet */
    *guest_ppi = A20PageIndex(vm, guest_lpage_index);
    }
  if (*guest_ppi >= vm->pages.guest_n_pages)
    return(MapLinPPageOOB);

/* +++ mapping in guest pages, check static phy_attr bits first before */
/* +++ allowing non-protected. */


mapIntoMonitor:

  /* At this point, we know that the guest's paging system
   * (if enabled) would allow for this access.  Now we have to
   * see about mapping it into the monitor linear address space.
   */
  pusage = getPageUsage(vm, *guest_ppi);

  if (wasRemap > 1)
    monpanic(vm, "wasRemap>1\n");

  /*
   * Check monitor PDE
   */
  if (monPDE->fields.P == 0) {
    /* OK, Lazy PT map/allocate */
    if (vm->guest.addr.guest_cpu->cr0.fields.pg) {
      phyPageInfo_t *pde_pusage;

      pde_pusage =
          getPageUsage(vm, A20PageIndex(vm, guestPDE.fields.base));
      if (pde_pusage->attr.raw & PageBadUsage4PTbl) {

#warning "PDE->PDir hack"
/*monprint(vm, "PDE.base=0x%x CR3=0x%x\n",
 *  A20PageIndex(vm, guestPDE.fields.base),
 *  A20Addr(vm, vm->guest_cpu.cr3));
 */
return(MapLinEmulate);
        }

      if (pde_pusage->attr.raw & PageUsagePTbl) {
        /* It is possible that multiple PDE entries will point to */
        /* the same Page Table.  In this case, we need to search to */
        /* find which one the monitor already mapped in, and get */
        /* a pointer to the Page Table allocated by the monitor. */
        Bit32u guest_ptbl_index;
        unsigned i;

        guestPDir = open_guest_phy_page(vm, guest_pdir_page_index,
                                    vm->guest.addr.tmp_phy_page0);
        guest_ptbl_index = A20PageIndex(vm, guestPDir[pdi].fields.base);
        monPTbl = (void *) 0;
        pt_index = 0; /* keep compiler quiet */
        for (i=0; i<1024; i++) {
          if (i==pdi) continue; /* skip current PDI */
          guestPDE = guestPDir[i];
          if ( guestPDE.fields.P &&
               (A20PageIndex(vm, guestPDE.fields.base)==guest_ptbl_index) ) {
            /* OK, guest has a PDE which matches.  If it is mapped into */
            /* the monitor already, then we are done searching. */
            if (vm->guest.addr.page_dir[i].fields.P) {
              pt_index = getMonPTi(vm, i, 11);
              vm->guest.addr.page_tbl_laddr_map[pdi] = pt_index;
              monPTbl = &vm->guest.addr.page_tbl[pt_index];
              break;
              }
            }
          }
        close_guest_phy_page(vm, guest_pdir_page_index);
        if (i>=1024)
          monpanic(vm, "mapGuestLinAddr: PDE maps to existing PTbl.\n");
        }
      else {
        /* Allocate PT using paged scheme. */
        pt_index = allocatePT(vm, pdi);
        monPTbl = &vm->guest.addr.page_tbl[pt_index];
        mon_memzero(monPTbl, sizeof(*monPTbl));
        }

      if (vm->guest.addr.guest_cpu->sreg[SRegCS].des.dpl==3) {
        /* For user code, we can use the guest US & RW values as-is, */
        /* since they are honored as such with either CR0.WP value. */
        us = guestPDE.fields.US;
        rw = guestPDE.fields.RW;
        }
      else { /* guest supervisor code */
        /* For supervisor code, access rules are different dependent on */
        /* the value of CR0.WP. */
        if (vm->guest.addr.guest_cpu->cr0.fields.wp==0) {
          /* If CR0.WP=0, then supervisor code can write to any page, */
          /* and permissions are effectively ignored. */
          us = 1;
          rw = 1;
          }
        else { /* CR0.WP==1 */
          /* If CR0.WP=0, then supervisor code can read from any page, */
          /* but write permission depends on the RW bit. */
          us = 1;
          rw = guestPDE.fields.RW;
          }
        }


      /* Base/Avail=0/G=0/PS=0/D=d/A=a/PCD=0/PWT=0/US=us/RW=rw/P=1 */
      monPDE->raw =
          (vm->pages.page_tbl[pt_index] << 12) | (guestPDE.raw & 0x60) |
          (us<<2) | (rw<<1) | 1;
      if ( addPageAttributes(vm, A20PageIndex(vm, guestPDE.fields.base),
                             PageUsagePTbl) ) {
        wasRemap++;
        goto mapIntoMonitor;
        }
      }
    else {
      /* Allocate PT using non-paged scheme. */
      pt_index = allocatePT(vm, pdi);
      monPTbl = &vm->guest.addr.page_tbl[pt_index];
      mon_memzero(monPTbl, 4096);
      /* Base/Avail=0/G=0/PS=0/D=0/A=0/PCD=0/PWT=0/US=1/RW=1/P=1 */
      monPDE->raw =
          (vm->pages.page_tbl[pt_index] << 12) | 0x7;
      }
    }
  else {
    /* monPDE->P == 1 */

    /* Make sure this laddr does not conflict with monitor space */
    /* This can only happen when monPDE.P==1, since the monitor */
    /* is always mapped in. */
    if ( (guest_laddr & 0xffc00000) == vm->mon_pde_mask )
      return(MapLinMonConflict);

    pt_index = getMonPTi(vm, pdi, 12);
    monPTbl = &vm->guest.addr.page_tbl[pt_index];
    }

  monPTE = &monPTbl->pte[pti];

  /*
   * Check monitor PTE
   */
  if (monPTE->fields.P == 0) {
    if (vm->guest.addr.guest_cpu->cr0.fields.pg) {
      if (vm->guest.addr.guest_cpu->sreg[SRegCS].des.dpl==3) {
        /* For user code, we can use the guest US & RW values as-is, */
        /* since they are honored as such with either CR0.WP value. */
        us = guestPTE.fields.US;
        rw = guestPTE.fields.RW;
        }
      else { /* guest supervisor code */
        /* For supervisor code, access rules are different dependent on */
        /* the value of CR0.WP. */
        if (vm->guest.addr.guest_cpu->cr0.fields.wp==0) {
          /* If CR0.WP=0, then supervisor code can write to any page, */
          /* and permissions are effectively ignored. */
          us = 1; 
          rw = 1;
          }
        else { /* CR0.WP==1 */
          /* If CR0.WP=0, then supervisor code can read from any page, */
          /* but write permission depends on the RW bit. */
          us = 1;
          rw = guestPTE.fields.RW;
          }
        }
      if (pusage->attr.fields.access_perm==PagePermRO) {
        rw = 0;
        if (req_rw)
          return(MapLinEmulate);
        }
      else if (pusage->attr.fields.access_perm==PagePermNA)
        return(MapLinEmulate);

      /* Base/Avail=0/G=0/PS=0/D=d/A=a/PCD=0/PWT=0/US=1/RW=rw/P=1 */
      monPTE->raw =
          (getHostOSPinnedPage(vm, *guest_ppi) << 12) | (guestPTE.raw & 0x60) |
          0x5 | (rw<<1);
      }
    else { /* CR0.PG==0 */
      rw = 1; /* Paging off is effectively RW */
      if (pusage->attr.fields.access_perm==PagePermRO) {
        rw = 0;
        if (req_rw)
          return(MapLinEmulate);
        }
      else if (pusage->attr.fields.access_perm==PagePermNA)
        return(MapLinEmulate);
      /* Base/Avail=0/G=0/PS=0/D=0/A=0/PCD=0/PWT=0/US=1/RW=rw/P=1 */
      monPTE->raw =
          (getHostOSPinnedPage(vm, *guest_ppi) << 12) | 0x5 | (rw<<1);
      }

    /* Mark physical page as having an unvirtualized linear address
     * mapped to it.
     */
    if (pusage->attr.fields.lmap_count == 0) {
      pusage->attr.fields.lmap_count = 1;
      pusage->attr.fields.laddr_backlink = guest_lpage_index;
      }
    else if (pusage->attr.fields.lmap_count == 1) {
      pusage->attr.fields.lmap_count = 2; /* max out count */
      /* Count maxed out, we only store laddr_backlink of 1st mapping. */
      }
    else {
      /* Count maxed out, we don't store any more info. */
      }

    invlpg_mon_offset( Guest2Monitor(vm, guest_laddr) );
    return(MapLinOK);
    }
  else {
    /* PTE.P == 1 */
    return(MapLinAlreadyMapped);
    }

np_exception:
access_exception:
  *error |= (req_us<<2) | (req_rw<<1);
  return(MapLinException);
}


  void
guestPageFault(vm_t *vm, guest_context_t *context, Bit32u cr2)
{
  Bit32u   guest_ppi, error, gerror;
  unsigned us, rw;

  /* Make sure this laddr does not conflict with monitor space */
  if ( (cr2 & 0xffc00000) == vm->mon_pde_mask )
    monpanic(vm, "PageFault: guest access to monitor space\n");

  error = context->error;
  if (error & 0x8) /* If RSVD bits used in PDir */
    monpanic(vm, "guestPageFault: RSVD\n");

  us = vm->guest.addr.guest_cpu->sreg[SRegCS].des.dpl == 3;
  rw = (error >> 1) & 1;

/* +++ should base attr (currently 0) on whether this is */
/* code or data???  only if siv==1 */
  switch (mapGuestLinAddr(vm, cr2, &guest_ppi, us, rw, 0, &gerror)) {
    case MapLinOK:
      return;
    case MapLinMonConflict:
      monpanic(vm, "guestPageFault: MapLinMonConflict:\n");
    case MapLinAlreadyMapped:
      monpanic(vm, "guestPageFault: MapLinAlreadyMapped:\n");
      /*emulate_instr(vm, context, 2);*/
      return;
    case MapLinPPageOOB:
      monpanic(vm, "guestPageFault: MapLinPPageOOB (0x%x):\n", cr2);
    case MapLinEmulate:
      monpanic(vm, "guestPageFault: MapLinEmulate:\n");
      /*emulate_instr(vm, context, 3);*/
      return;

    case MapLinException:
      /*monpanic(vm, "guestPageFault: emulate_exception was here.\n");*/
      /*emulate_exception(vm, ExceptionPF, gerror);*/
      toHostGuestFault(vm, ExceptionPF);
      return;

    default:
      monpanic(vm, "guestPageFault: MapLin: default case:\n");
    }
}

#if 0
  void
sanity_check_pdir(vm_t *vm, unsigned id, Bit32u guest_laddr)
{
  pageEntry_t *monPDE;
  Bit32u       pdi;
  unsigned     pt_index;
 
  for (pdi=0; pdi<1024; pdi++) {
    monPDE = &vm->guest.addr.page_dir[pdi];
    if ( (pdi!=vm->mon_pdi) &&
        monPDE->fields.P ) {

      pt_index = vm->guest.addr.page_tbl_laddr_map[pdi];
      if (pt_index == -1)
        monpanic(vm, "sanity_check_pdir: pt_index==-1\n");
      if (pt_index >= vm->pages.guest_n_pages)
        monpanic(vm, "sanity_check_pdir: pt_index OOB\n");
      if ( monPDE->fields.base != vm->pages.page_tbl[pt_index] ) {
        monprint(vm, "gaddr=0x%x\n", guest_laddr);
        monprint(vm, "pt_index=%u\n", pt_index);
        monprint(vm, "map[0x302]=%u\n",
          vm->guest.addr.page_tbl_laddr_map[0x302]);
        monpanic(vm, "sanity_check_pdir: id=%u "
          "pdi=0x%x\n", id, pdi);
        }
      }
    }
}
#endif
