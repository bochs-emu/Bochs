/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2017  The Bochs Project
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
/////////////////////////////////////////////////////////////////////////

#include "bochs.h"

#if BX_DEBUGGER

static unsigned next_bpoint_id = 1;

void bx_dbg_breakpoint_changed(void)
{
#if (BX_DBG_MAX_VIR_BPOINTS > 0)
  if (bx_guard.iaddr.num_virtual)
    bx_guard.guard_for |= BX_DBG_GUARD_IADDR_VIR;
  else
    bx_guard.guard_for &= ~BX_DBG_GUARD_IADDR_VIR;
#endif

#if (BX_DBG_MAX_LIN_BPOINTS > 0)
  if (bx_guard.iaddr.num_linear)
    bx_guard.guard_for |= BX_DBG_GUARD_IADDR_LIN;
  else
    bx_guard.guard_for &= ~BX_DBG_GUARD_IADDR_LIN;
#endif

#if (BX_DBG_MAX_PHY_BPOINTS > 0)
  if (bx_guard.iaddr.num_physical)
    bx_guard.guard_for |= BX_DBG_GUARD_IADDR_PHY;
  else
    bx_guard.guard_for &= ~BX_DBG_GUARD_IADDR_PHY;
#endif
}

void bx_dbg_en_dis_breakpoint_command(unsigned handle, bx_bool enable)
{
#if (BX_DBG_MAX_VIR_BPOINTS > 0)
  if (bx_dbg_en_dis_vbreak(handle, enable))
    goto done;
#endif

#if (BX_DBG_MAX_LIN_BPOINTS > 0)
  if (bx_dbg_en_dis_lbreak(handle, enable))
    goto done;
#endif

#if (BX_DBG_MAX_PHY_BPOINTS > 0)
  if (bx_dbg_en_dis_pbreak(handle, enable))
    goto done;
#endif

  dbg_printf("Error: breakpoint %u not found.\n", handle);
  return;

done:
  bx_dbg_breakpoint_changed();
}

bx_bool bx_dbg_en_dis_pbreak(unsigned handle, bx_bool enable)
{
#if (BX_DBG_MAX_PHY_BPOINTS > 0)
  // see if breakpoint is a physical breakpoint
  for (unsigned i=0; i<bx_guard.iaddr.num_physical; i++) {
    if (bx_guard.iaddr.phy[i].bpoint_id == handle) {
      bx_guard.iaddr.phy[i].enabled=enable;
      return 1;
    }
  }
#endif
  return 0;
}

bx_bool bx_dbg_en_dis_lbreak(unsigned handle, bx_bool enable)
{
#if (BX_DBG_MAX_LIN_BPOINTS > 0)
  // see if breakpoint is a linear breakpoint
  for (unsigned i=0; i<bx_guard.iaddr.num_linear; i++) {
    if (bx_guard.iaddr.lin[i].bpoint_id == handle) {
      bx_guard.iaddr.lin[i].enabled=enable;
      return 1;
    }
  }
#endif
  return 0;
}

bx_bool bx_dbg_en_dis_vbreak(unsigned handle, bx_bool enable)
{
#if (BX_DBG_MAX_VIR_BPOINTS > 0)
  // see if breakpoint is a virtual breakpoint
  for (unsigned i=0; i<bx_guard.iaddr.num_virtual; i++) {
    if (bx_guard.iaddr.vir[i].bpoint_id == handle) {
      bx_guard.iaddr.vir[i].enabled=enable;
      return 1;
    }
  }
#endif
  return 0;
}

void bx_dbg_del_breakpoint_command(unsigned handle)
{
#if (BX_DBG_MAX_VIR_BPOINTS > 0)
  if (bx_dbg_del_vbreak(handle))
   goto done;
#endif

#if (BX_DBG_MAX_LIN_BPOINTS > 0)
  if (bx_dbg_del_lbreak(handle))
   goto done;
#endif

#if (BX_DBG_MAX_PHY_BPOINTS > 0)
  if (bx_dbg_del_pbreak(handle))
   goto done;
#endif

  dbg_printf("Error: breakpoint %u not found.\n", handle);
  return;

done:
  bx_dbg_breakpoint_changed();
}

bx_bool bx_dbg_del_pbreak(unsigned handle)
{
#if (BX_DBG_MAX_PHY_BPOINTS > 0)
  // see if breakpoint is a physical breakpoint
  for (int i=0; i<(int)bx_guard.iaddr.num_physical; i++) {
    if (bx_guard.iaddr.phy[i].bpoint_id == handle) {
      // found breakpoint, delete it by shifting remaining entries left
      if (bx_guard.iaddr.phy[i].condition) free(bx_guard.iaddr.phy[i].condition);
      for (int j=i; j<(int)(bx_guard.iaddr.num_physical-1); j++) {
        bx_guard.iaddr.phy[j] = bx_guard.iaddr.phy[j+1];
      }
      bx_guard.iaddr.num_physical--;
      return 1;
    }
  }
#endif
  return 0;
}

bx_bool bx_dbg_del_lbreak(unsigned handle)
{
#if (BX_DBG_MAX_LIN_BPOINTS > 0)
  // see if breakpoint is a linear breakpoint
  for (int i=0; i<(int)bx_guard.iaddr.num_linear; i++) {
    if (bx_guard.iaddr.lin[i].bpoint_id == handle) {
      // found breakpoint, delete it by shifting remaining entries left
      if (bx_guard.iaddr.lin[i].condition) free(bx_guard.iaddr.lin[i].condition);
      for (int j=i; j<(int)(bx_guard.iaddr.num_linear-1); j++) {
        bx_guard.iaddr.lin[j] = bx_guard.iaddr.lin[j+1];
      }
      bx_guard.iaddr.num_linear--;
      return 1;
    }
  }
#endif
  return 0;
}

bx_bool bx_dbg_del_vbreak(unsigned handle)
{
#if (BX_DBG_MAX_VIR_BPOINTS > 0)
  // see if breakpoint is a virtual breakpoint
  for (int i=0; i<(int)bx_guard.iaddr.num_virtual; i++) {
    if (bx_guard.iaddr.vir[i].bpoint_id == handle) {
      // found breakpoint, delete it by shifting remaining entries left
      if (bx_guard.iaddr.vir[i].condition) free(bx_guard.iaddr.vir[i].condition);
      for (int j=i; j<(int)(bx_guard.iaddr.num_virtual-1); j++) {
        bx_guard.iaddr.vir[j] = bx_guard.iaddr.vir[j+1];
      }
      bx_guard.iaddr.num_virtual--;
      return 1;
    }
  }
#endif
  return 0;
}

static char* prepare_condition(const char *str)
{
  if (str == NULL) return NULL;
  dbg_printf("condition set: <%s>\n", str);
  size_t len = strlen(str);
  char *condition = (char*) malloc(len+2);
  strcpy(condition, str);
  condition[len]='\n';
  condition[len+1]='\0';
  return condition;
}

int bx_dbg_vbreakpoint_command(BreakpointKind bk, Bit32u cs, bx_address eip, const char *condition)
{
#if (BX_DBG_MAX_VIR_BPOINTS > 0)
  if (bk != bkRegular) {
    dbg_printf("Error: vbreak of this kind not implemented yet.\n");
    return -1;
  }

  if (bx_guard.iaddr.num_virtual >= BX_DBG_MAX_VIR_BPOINTS) {
    dbg_printf("Error: no more virtual breakpoint slots left.\n");
    dbg_printf("Error: see BX_DBG_MAX_VIR_BPOINTS.\n");
    return -1;
  }

  struct bx_guard_t::ibreak::vbreak *bp = &bx_guard.iaddr.vir[bx_guard.iaddr.num_virtual];
  bp->cs  = cs;
  bp->eip = eip;
  bp->bpoint_id = next_bpoint_id++;
  bp->condition = prepare_condition(condition);
  bp->enabled=1;
  bx_guard.iaddr.num_virtual++;
  bx_guard.guard_for |= BX_DBG_GUARD_IADDR_VIR;
  return bp->bpoint_id;

#else
  dbg_printf("Error: virtual breakpoint support not compiled in.\n");
  dbg_printf("Error: make sure BX_DBG_MAX_VIR_BPOINTS > 0\n");
  return -1;
#endif
}

int bx_dbg_lbreakpoint_command(BreakpointKind bk, bx_address laddress, const char *condition)
{
#if (BX_DBG_MAX_LIN_BPOINTS > 0)
  if (bk == bkAtIP) {
    dbg_printf("Error: lbreak of this kind not implemented yet.\n");
    return -1;
  }

  if (bx_guard.iaddr.num_linear >= BX_DBG_MAX_LIN_BPOINTS) {
    dbg_printf("Error: no more linear breakpoint slots left.\n");
    dbg_printf("Error: see BX_DBG_MAX_LIN_BPOINTS.\n");
    return -1;
  }

  struct bx_guard_t::ibreak::lbreak *bp = &bx_guard.iaddr.lin[bx_guard.iaddr.num_linear];
  bp->addr = laddress;
  int BpId = (bk == bkStepOver) ? 0 : next_bpoint_id++;
  bp->bpoint_id = BpId;
  bp->condition = prepare_condition(condition);
  bp->enabled=1;
  bx_guard.iaddr.num_linear++;
  bx_guard.guard_for |= BX_DBG_GUARD_IADDR_LIN;
  return BpId;

#else
  dbg_printf("Error: linear breakpoint support not compiled in.\n");
  dbg_printf("Error: make sure BX_DBG_MAX_LIN_BPOINTS > 0\n");
  return -1;
#endif
}

int bx_dbg_pbreakpoint_command(BreakpointKind bk, bx_phy_address paddress, const char *condition)
{
#if (BX_DBG_MAX_PHY_BPOINTS > 0)
  if (bk != bkRegular) {
    dbg_printf("Error: pbreak of this kind not implemented yet.\n");
    return -1;
  }

  if (bx_guard.iaddr.num_physical >= BX_DBG_MAX_PHY_BPOINTS) {
    dbg_printf("Error: no more physical breakpoint slots left.\n");
    dbg_printf("Error: see BX_DBG_MAX_PHY_BPOINTS.\n");
    return -1;
  }

  struct bx_guard_t::ibreak::pbreak *bp = &bx_guard.iaddr.phy[bx_guard.iaddr.num_physical];
  bp->addr = paddress;
  bp->bpoint_id = next_bpoint_id++;
  bp->condition = prepare_condition(condition);
  bp->enabled=1;
  bx_guard.iaddr.num_physical++;
  bx_guard.guard_for |= BX_DBG_GUARD_IADDR_PHY;
  return bp->bpoint_id;
#else
  dbg_printf("Error: physical breakpoint support not compiled in.\n");
  dbg_printf("Error: make sure BX_DBG_MAX_PHY_BPOINTS > 0\n");
  return -1;
#endif
}

void bx_dbg_info_bpoints_command(void)
{
  unsigned i;
// Num Type           Disp Enb Address    
// 1   breakpoint     keep y   0x00010664 condition

  dbg_printf("Num Type           Disp Enb Address\n");
#if (BX_DBG_MAX_VIR_BPOINTS > 0)
  for (i=0; i<bx_guard.iaddr.num_virtual; i++) {
    struct bx_guard_t::ibreak::vbreak *bp = &bx_guard.iaddr.vir[i];
    dbg_printf("%3u ", bp->bpoint_id);
    dbg_printf("vbreakpoint    ");
    dbg_printf("keep ");
    dbg_printf(bp->enabled?"y   ":"n   ");
    dbg_printf("0x%04x:" FMT_ADDRX " %s", bp->cs, bp->eip,
                  (bp->condition != NULL) ? bp->condition : "\n");
  }
#endif

#if (BX_DBG_MAX_LIN_BPOINTS > 0)
  for (i=0; i<bx_guard.iaddr.num_linear; i++) {
    struct bx_guard_t::ibreak::lbreak *bp = &bx_guard.iaddr.lin[i];
    dbg_printf("%3u ", bp->bpoint_id);
    dbg_printf("lbreakpoint    ");
    dbg_printf("keep ");
    dbg_printf(bp->enabled?"y   ":"n   ");
    dbg_printf("0x" FMT_ADDRX " %s", bp->addr,
                  (bp->condition != NULL) ? bp->condition : "\n");
  }
#endif

#if (BX_DBG_MAX_PHY_BPOINTS > 0)
  for (i=0; i<bx_guard.iaddr.num_physical; i++) {
    struct bx_guard_t::ibreak::pbreak *bp = &bx_guard.iaddr.phy[i];
    dbg_printf("%3u ", bp->bpoint_id);
    dbg_printf("pbreakpoint    ");
    dbg_printf("keep ");
    dbg_printf(bp->enabled?"y   ":"n   ");
    dbg_printf("0x" FMT_PHY_ADDRX " %s", bp->addr,
                  (bp->condition != NULL) ? bp->condition : "\n");
  }
#endif
}

#endif /* if BX_DEBUGGER */
