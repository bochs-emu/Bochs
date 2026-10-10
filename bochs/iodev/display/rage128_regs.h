/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2025-2026  The Bochs Project
//
//  ATI Rage 128 PRO -- register and identity constants.
//
//  Register offsets and field layouts follow the Rage 128 Pro Register
//  Reference Guide (RRG), the XFree86 / Linux DRM r128 sources and the
//  behaviour documented by the 86Box Rage 128 emulation.
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

#ifndef BX_IODEV_RAGE128_REGS_H
#define BX_IODEV_RAGE128_REGS_H

// PCI identity
#define RAGE128_PCI_VENDOR      0x1002
#define RAGE128_PCI_DEVICE_PF   0x5046  // Rage 128 PRO GL (AGP)
#define RAGE128_PCI_DEVICE_TR   0x5452  // Rage 128 PRO Ultra / II (PCI)
#define RAGE128_SUBSYS_VENDOR   0x1002
#define RAGE128_SUBSYS_ID_PF    0x0018
#define RAGE128_SUBSYS_ID_TR    0x001c

// PCI capability chain: AGP capability at 0x50, PMI at 0x5c
#define RAGE128_PCI_CAP_PTR        0x50
#define RAGE128_AGP_CAP_ID         0x02
#define RAGE128_AGP_NEXT_PTR       0x5c
#define RAGE128_AGP_REV            0x20
#define RAGE128_AGP_STATUS         0x1f000207
#define RAGE128_AGP_COMMAND_SBA_EN 0x00000200
#define RAGE128_AGP_COMMAND_MASK   0xff000107
#define RAGE128_PMI_CAP_ID         0x01
#define RAGE128_PMI_PMC            0x0202

// BAR layout: BAR0 = 64 MB linear framebuffer aperture (lower 32 MB local
// VRAM, upper 32 MB = AGP system memory image), BAR1 = 256 byte IO block,
// BAR2 = 16 KB register aperture (two identical 8 KB copies).
#define RAGE128_LFB_SIZE  0x04000000
#define RAGE128_IO_SIZE   0x00000100
#define RAGE128_MMIO_SIZE 0x00004000
#define RAGE128_REG_APER_MASK 0x00001fff

// Card address space: bit 25 splits local VRAM / AGP image
#define R128_CARD_AGP_HALF 0x02000000

// Reference oscillator (XTALIN)
#define RAGE128_REF_FREQ_AGP_HZ 27000000.0
#define RAGE128_REF_FREQ_PCI_HZ 29500000.0

// ------------------------------------------------------------------
// Register file offsets (non-GUI registers 0x0000-0x00ff are also
// reachable through the IO BAR)
// ------------------------------------------------------------------
#define RAGE128_MM_INDEX 0x0000
#define RAGE128_MM_DATA  0x0004
#define RAGE128_MM_INDEX_MM_APER 0x80000000
#define RAGE128_MM_INDEX_MM_ADDR 0x07fffffc

#define RAGE128_CLOCK_CNTL_INDEX 0x0008
#define RAGE128_CLOCK_CNTL_DATA  0x000c
#define RAGE128_PLL_ADDR_MASK      0x1f
#define RAGE128_PLL_WR_EN          (1 << 7)
#define RAGE128_PPLL_DIV_SEL_SHIFT 8

#define RAGE128_BIOS_0_SCRATCH 0x0010
#define RAGE128_BIOS_1_SCRATCH 0x0014
#define RAGE128_BIOS_2_SCRATCH 0x0018
#define RAGE128_BIOS_3_SCRATCH 0x001c

#define RAGE128_GEN_INT_CNTL    0x0040
#define RAGE128_GEN_INT_STATUS  0x0044
#define RAGE128_GIC_VBLANK_EN   (1u << 0)
#define RAGE128_GIS_VBLANK      (1u << 0)
#define RAGE128_GIC_VLINE_EN    (1u << 1)
#define RAGE128_GIS_VLINE       (1u << 1)
#define RAGE128_GIC_VSYNC_EN    (1u << 2)
#define RAGE128_GIS_VSYNC       (1u << 2)
#define RAGE128_GIC_SNAPSHOT_EN (1u << 3)
#define RAGE128_GIS_SNAPSHOT    (1u << 3)
#define RAGE128_GIC_GUI_IDLE_EN (1u << 19)
#define RAGE128_GIS_GUI_IDLE    (1u << 19)
#define RAGE128_GIC_SUPPORTED   (RAGE128_GIC_VBLANK_EN | RAGE128_GIC_VLINE_EN | \
                                 RAGE128_GIC_VSYNC_EN | RAGE128_GIC_SNAPSHOT_EN | \
                                 RAGE128_GIC_GUI_IDLE_EN)
#define RAGE128_GIS_ACK_MASK    (RAGE128_GIS_VBLANK | RAGE128_GIS_VLINE | \
                                 RAGE128_GIS_VSYNC | RAGE128_GIS_SNAPSHOT | \
                                 RAGE128_GIS_GUI_IDLE)

#define RAGE128_BUS_CNTL  0x0030
#define RAGE128_BUS_CNTL1 0x0034
#define RAGE128_MEM_VGA_WP_SEL 0x0038
#define RAGE128_MEM_VGA_RP_SEL 0x003c
#define RAGE128_VGA_PAGE_SIZE  0x2000
#define RAGE128_BUS_CNTL_DEFAULT  0x880f4f41
#define RAGE128_BUS_CNTL_WO_MASK  0x00000006
#define RAGE128_BUS_CNTL_BUS_MASTER_DIS 0x00000040

#define RAGE128_GPIO_MONID            0x0068
#define RAGE128_GPIO_MONID_A_DDC_CLK  (1 << 2)
#define RAGE128_GPIO_MONID_A_DDC_DAT  (1 << 1)
#define RAGE128_GPIO_MONID_Y_DDC_CLK  (1 << 10)
#define RAGE128_GPIO_MONID_Y_DDC_DAT  (1 << 9)
#define RAGE128_GPIO_MONID_EN_DDC_CLK (1 << 18)
#define RAGE128_GPIO_MONID_EN_DDC_DAT (1 << 17)

#define RAGE128_CRTC_GEN_CNTL 0x0050
#define RAGE128_CRTC_EXT_CNTL 0x0054
#define RAGE128_DAC_CNTL      0x0058
#define RAGE128_CRTC_STATUS   0x005c

#define RAGE128_CRTC_GEN_CNTL_DEFAULT 0x04000000
#define RAGE128_CRTC_DBL_SCAN_EN      (1 << 0)
#define RAGE128_CRTC_INTERLACE_EN     (1 << 1)
#define RAGE128_CRTC_PIX_WIDTH_SHIFT  8
#define RAGE128_CRTC_CUR_EN           (1 << 16)
#define RAGE128_CRTC_EXT_DISP_EN      (1 << 24)
#define RAGE128_CRTC_EN               (1 << 25)
#define RAGE128_CRTC_DISP_REQ_EN_B    (1 << 26)

#define RAGE128_CRTC_EXT_CNTL_DEFAULT 0x00200000
#define RAGE128_CRTC_VGA_ATI_LINEAR   (1 << 3)
#define RAGE128_VGA_BLINK_RATE_SHIFT  1
#define RAGE128_CRTC_XCRT_CNT_EN      (1 << 6)
#define RAGE128_CRTC_HSYNC_DIS        (1 << 8)
#define RAGE128_CRTC_VSYNC_DIS        (1 << 9)
#define RAGE128_CRTC_DISPLAY_DIS      (1 << 10)
#define RAGE128_VGA_MEM_PS_EN         (1 << 19)

#define RAGE128_DAC_CNTL_DEFAULT 0xff00000a
#define RAGE128_DAC_CMP_OUTPUT   (1 << 7)
#define RAGE128_DAC_8BIT_EN      (1 << 8)
#define RAGE128_DAC_4BPP_PIX_ORDER (1 << 9)
#define RAGE128_DAC_VGA_ADR_EN   (1 << 13)
#define RAGE128_DAC_PDWN         (1 << 15)
#define RAGE128_DAC_CRC_EN       (1 << 19)

#define RAGE128_CRTC_STATUS_DEFAULT 0x80000000

#define RAGE128_PALETTE_INDEX 0x00b0
#define RAGE128_PALETTE_DATA  0x00b4

#define RAGE128_CONFIG_CNTL    0x00e0
#define RAGE128_CFG_ATI_REV_PF 0x00010000
#define RAGE128_CFG_ATI_REV_TR 0x00040000
#define RAGE128_CFG_VGA_IO_DIS 0x00000200
#define RAGE128_CONFIG_XSTRAP  0x00e4
#define RAGE128_CONFIG_BONDS   0x00e8
#define RAGE128_GEN_RESET_CNTL 0x00f0
#define RAGE128_GEN_SOFT_RESET_GUI 0x00000001
#define RAGE128_GEN_STATUS     0x00f4
#define RAGE128_CONFIG_MEMSIZE 0x00f8
#define RAGE128_CONFIG_APER_0_BASE     0x0100
#define RAGE128_CONFIG_APER_1_BASE     0x0104
#define RAGE128_CONFIG_APER_SIZE       0x0108
#define RAGE128_CONFIG_REG_1_BASE      0x010c
#define RAGE128_CONFIG_REG_APER_SIZE   0x0110
#define RAGE128_CONFIG_APER_SIZE_VAL   0x02000000
#define RAGE128_APER_0_BASE_MASK       0xfc000000
#define RAGE128_APER_1_BASE_MASK       0xfe000000
#define RAGE128_REG_1_BASE_MASK        0xffffe000
#define RAGE128_CONFIG_MEMSIZE_EMB     0x0114
#define RAGE128_CONFIG_MEMSIZE_MASK    0x03e00000
#define RAGE128_BM_CHUNK_0_VAL 0x0a18
#define RAGE128_BM_CHUNK_1_VAL 0x0a1c
#define RAGE128_BM_QUEUE_FREE_STATUS 0x0a14
#define RAGE128_BM_QUEUE_FREE_IDLE   0x00802222

#define RAGE128_TEST_DEBUG_CNTL 0x0120
#define RAGE128_TEST_DEBUG_MUX  0x0124
#define RAGE128_HW_DEBUG        0x0128
#define RAGE128_TEST_DEBUG_CLK_SHIFT 8

#define RAGE128_HOST_PATH_CNTL         0x0130
#define RAGE128_HOST_PATH_CNTL_DEFAULT 0x0000207f

#define RAGE128_MEM_CNTL           0x0140
#define RAGE128_EXT_MEM_CNTL       0x0144
#define RAGE128_MEM_ADDR_CONFIG    0x0148
#define RAGE128_MEM_INTF_CNTL      0x014c
#define RAGE128_MEM_STR_CNTL       0x0150
#define RAGE128_MEM_INIT_LAT_TIMER 0x0154
#define RAGE128_MEM_SDRAM_MODE_REG 0x0158
#define RAGE128_MEM_CNTL_DEFAULT      0x08000300
#define RAGE128_MEM_CNTL_RO_MASK      0x00700000
#define RAGE128_EXT_MEM_CNTL_DEFAULT  0x0000d67f
#define RAGE128_MEM_SDRAM_MODE_DEFAULT 0x00300000
#define RAGE128_MEM_INIT_LAT_DEFAULT  0x3fffffff

#define RAGE128_PAD_CTLR_STRENGTH 0x0168
#define RAGE128_PC_MISC_CTL       0x0188
#define RAGE128_VIDEOMUX_CNTL     0x0190
#define RAGE128_PAD_CTLR_STRENGTH_DEFAULT 0x00010000
#define RAGE128_VIDEOMUX_CNTL_DEFAULT     0x000507f3

#define RAGE128_AGP_BASE      0x0170
#define RAGE128_AGP_BASE_MASK 0xffc00000
#define RAGE128_AGP_CNTL      0x0174
#define RAGE128_AGP_CNTL_MASK    0x3fffff3f
#define RAGE128_AGP_CNTL_DEFAULT 0x00700000
#define RAGE128_AGP_APER_OFFSET  0x0178
#define RAGE128_PCI_GART_PAGE 0x017c

// CRTC timing / scanout
#define RAGE128_CRTC_H_TOTAL_DISP    0x0200
#define RAGE128_CRTC_H_SYNC_STRT_WID 0x0204
#define RAGE128_CRTC_V_TOTAL_DISP    0x0208
#define RAGE128_CRTC_V_SYNC_STRT_WID 0x020c
#define RAGE128_CRTC_VLINE_CRNT_VLINE 0x0210
#define RAGE128_CRTC_CRNT_FRAME      0x0214
#define RAGE128_CRTC_GUI_TRIG_VLINE  0x0218
#define RAGE128_CRTC_DEBUG           0x021c
#define RAGE128_CRTC_OFFSET          0x0224
#define RAGE128_CRTC_OFFSET_CNTL     0x0228
#define RAGE128_CRTC_TILE_EN         (1 << 15)
#define RAGE128_CRTC_OFFSET_FLIP_CNTL (1 << 16)
#define RAGE128_CRTC_PITCH           0x022c
#define RAGE128_OVR_CLR              0x0230
#define RAGE128_OVR_WID_LEFT_RIGHT   0x0234
#define RAGE128_OVR_WID_TOP_BOTTOM   0x0238
#define RAGE128_SNAPSHOT_VH_COUNTS   0x0240
#define RAGE128_SNAPSHOT_F_COUNT     0x0244
#define RAGE128_N_VIF_COUNT          0x0248
#define RAGE128_SNAPSHOT_VIF_COUNT   0x024c
#define RAGE128_CUR_OFFSET           0x0260
#define RAGE128_CUR_HORZ_VERT_POSN   0x0264
#define RAGE128_CUR_HORZ_VERT_OFF    0x0268
#define RAGE128_CUR_CLR0             0x026c
#define RAGE128_CUR_CLR1             0x0270

#define RAGE128_DAC_EXT_CNTL 0x0280
#define RAGE128_DAC_CRC_SIG  0x02cc
#define RAGE128_DAC_FORCE_BLANK_OFF_EN (1 << 4)
#define RAGE128_DAC_FORCE_DATA_EN      (1 << 5)
#define RAGE128_DAC_FORCE_DATA_SEL_SHIFT 6
#define RAGE128_DAC_FORCE_DATA_SHIFT     8

#define RAGE128_DDA_CONFIG     0x02e0
#define RAGE128_DDA_ON_OFF     0x02e4
#define RAGE128_VGA_DDA_CONFIG 0x02e8
#define RAGE128_VGA_DDA_ON_OFF 0x02ec

// OV0 hardware video overlay
#define RAGE128_OV0_BLOCK_BASE                 0x0400
#define RAGE128_OV0_BLOCK_END                  0x04ff
#define RAGE128_OV0_REG(off)                   (((off) - RAGE128_OV0_BLOCK_BASE) >> 2)
#define RAGE128_OV0_Y_X_START                  0x0400
#define RAGE128_OV0_Y_X_END                    0x0404
#define RAGE128_OV0_EXCLUSIVE_HORZ             0x0408
#define RAGE128_OV0_EXCLUSIVE_VERT             0x040c
#define RAGE128_OV0_REG_LOAD_CNTL              0x0410
#define RAGE128_OV0_REG_LD_CTL_LOCK                  0x00000001
#define RAGE128_OV0_REG_LD_CTL_VBLANK_DURING_LOCK    0x00000002
#define RAGE128_OV0_REG_LD_CTL_STALL_GUI_UNTIL_FLIP  0x00000004
#define RAGE128_OV0_REG_LD_CTL_LOCK_READBACK         0x00000008
#define RAGE128_OV0_SCALE_CNTL                 0x0420
#define RAGE128_OV0_SCALER_SURFAC_FORMAT             0x00000f00
#define RAGE128_OV0_SCALER_FORMAT_SHIFT              8
#define RAGE128_OV0_SCALER_SOURCE_15BPP              0x3
#define RAGE128_OV0_SCALER_SOURCE_16BPP              0x4
#define RAGE128_OV0_SCALER_SOURCE_32BPP              0x6
#define RAGE128_OV0_SCALER_SOURCE_YUV9               0x9
#define RAGE128_OV0_SCALER_SOURCE_YUV12              0xa
#define RAGE128_OV0_SCALER_SOURCE_VYUY422            0xb
#define RAGE128_OV0_SCALER_SOURCE_YVYU422            0xc
#define RAGE128_OV0_SCALER_ENABLE                    0x40000000
#define RAGE128_OV0_SCALER_SOFT_RESET                0x80000000
#define RAGE128_OV0_V_INC                      0x0424
#define RAGE128_OV0_P1_V_ACCUM_INIT            0x0428
#define RAGE128_OV0_P23_V_ACCUM_INIT           0x042c
#define RAGE128_OV0_P1_BLANK_LINES_AT_TOP      0x0430
#define RAGE128_OV0_P23_BLANK_LINES_AT_TOP     0x0434
#define RAGE128_OV0_VID_BUF0_BASE_ADRS         0x0440
#define RAGE128_OV0_VID_BUF_PITCH0_VALUE       0x0460
#define RAGE128_OV0_VID_BUF_PITCH1_VALUE       0x0464
#define RAGE128_OV0_AUTO_FLIP_CNTL             0x0470
#define RAGE128_OV0_H_INC                      0x0480
#define RAGE128_OV0_STEP_BY                    0x0484
#define RAGE128_OV0_P1_H_ACCUM_INIT            0x0488
#define RAGE128_OV0_P23_H_ACCUM_INIT           0x048c
#define RAGE128_OV0_P1_X_START_END             0x0494
#define RAGE128_OV0_P2_X_START_END             0x0498
#define RAGE128_OV0_P3_X_START_END             0x049c
#define RAGE128_OV0_COLOUR_CNTL                0x04e0
#define RAGE128_OV0_VIDEO_KEY_CLR              0x04e4
#define RAGE128_OV0_VIDEO_KEY_MSK              0x04e8
#define RAGE128_OV0_GRAPHICS_KEY_CLR           0x04ec
#define RAGE128_OV0_GRAPHICS_KEY_MSK           0x04f0
#define RAGE128_OV0_KEY_CNTL                   0x04f4
#define RAGE128_OV0_VIDEO_KEY_FN_MASK                0x00000007
#define RAGE128_OV0_GRAPHIC_KEY_FN_MASK              0x00000070
#define RAGE128_OV0_GRAPHIC_KEY_FN_SHIFT             4
#define RAGE128_OV0_CMP_MIX_AND                      0x00000100
#define RAGE128_ECP_DIV_SHIFT 8

// DVD subpicture block
#define RAGE128_SUBPIC_BLOCK_BASE              0x0540
#define RAGE128_SUBPIC_BLOCK_END               0x0588
#define RAGE128_SUBPIC_REG(off)                (((off) - RAGE128_SUBPIC_BLOCK_BASE) >> 2)
#define RAGE128_SUBPIC_CNTL                    0x0540
#define RAGE128_SUBPIC_CNTL_DISPLAY_EN               0x00000001
#define RAGE128_SUBPIC_CNTL_NEW_FRAME                0x00000002
#define RAGE128_SUBPIC_COLOR_CONTRAST          0x0544
#define RAGE128_SUBPIC_DAREA_START             0x054c
#define RAGE128_SUBPIC_DAREA_END               0x0550
#define RAGE128_SUBPIC_V_STEP                  0x0554
#define RAGE128_SUBPIC_H_STEP                  0x0558
#define RAGE128_SUBPIC_PXD_A                   0x055c
#define RAGE128_SUBPIC_PITCH_LENGTH            0x056c
#define RAGE128_SUBPIC_HL_COLOR_CONTRAST       0x0570
#define RAGE128_SUBPIC_HL_TOP                  0x0574
#define RAGE128_SUBPIC_HL_BOTTOM               0x0578
#define RAGE128_SUBPIC_PALETTE_INDEX           0x057c
#define RAGE128_SUBPIC_PALETTE_DATA            0x0580
#define RAGE128_SUBPIC_H_ACC                   0x0584
#define RAGE128_SUBPIC_V_ACC                   0x0588

// PM4 / CCE command processor
#define RAGE128_PM4_BUFFER_OFFSET   0x0700
#define RAGE128_PM4_BUFFER_CNTL     0x0704
#define RAGE128_PM4_BUFFER_WM_CNTL  0x0708
#define RAGE128_PM4_BUFFER_DL_RPTR_ADDR 0x070c
#define RAGE128_PM4_BUFFER_DL_RPTR  0x0710
#define RAGE128_PM4_BUFFER_DL_WPTR  0x0714
#define RAGE128_PM4_BUFFER_DL_WPTR_DELAY 0x0718
#define RAGE128_PM4_VC_FPU_SETUP    0x071c
#define RAGE128_FPU_FRONT_DIR_CCW      (1 << 0)
#define RAGE128_FPU_BACKFACE_SHIFT     1
#define RAGE128_FPU_FRONTFACE_SHIFT    3
#define RAGE128_FPU_FACE_MODE_MASK     3
#define RAGE128_FPU_FACE_CULL          0
#define RAGE128_FPU_BACKFACE_SOLID     (3 << 1)
#define RAGE128_FPU_FRONTFACE_SOLID    (3 << 3)
#define RAGE128_FPU_COLOR_SHIFT        5
#define RAGE128_FPU_COLOR_MASK         3
#define RAGE128_FPU_COLOR_GOURAUD      (2 << 5)
#define RAGE128_FPU_FLAT_VERTEX_OGL    (1 << 14)
#define RAGE128_PM4_IW_INDOFF       0x0738
#define RAGE128_PM4_IW_INDSIZE      0x073c
#define RAGE128_PM4_VC_DEBUG_CONFIG 0x07a4
#define RAGE128_VC_DEBUG_DONT_START   (1 << 0)
#define RAGE128_VC_DEBUG_NO_OUTPUT    (1 << 1)
#define RAGE128_PM4_VC_STAT         0x07a8
#define RAGE128_PM4_STAT            0x07b8
#define RAGE128_PM4_STAT_FIFOCNT_MASK 0x00000fff
#define RAGE128_PM4_STAT_BUSY         (1 << 16)
#define RAGE128_PM4_STAT_GUI_ACTIVE   (1u << 31)
#define RAGE128_PM4_MICROCODE_ADDR  0x07d4
#define RAGE128_PM4_MICROCODE_RADDR 0x07d8
#define RAGE128_PM4_MICROCODE_DATAH 0x07dc
#define RAGE128_PM4_MICROCODE_DATAL 0x07e0
#define RAGE128_PM4_CMDFIFO_ADDR    0x07e4
#define RAGE128_PM4_MICRO_CNTL      0x07fc

// PM4 packet headers
#define RAGE128_PM4_TYPE(h)      ((h) >> 30)
#define RAGE128_PM4_COUNT(h)     ((((h) >> 16) & 0x3fff) + 1)
#define RAGE128_PM4_T0_REG(h)    (((h) & 0x1fff) * 4)
#define RAGE128_PM4_T1_REG0(h)   (((h) & 0x7ff) * 4)
#define RAGE128_PM4_T1_REG1(h)   ((((h) >> 11) & 0x7ff) * 4)
#define RAGE128_PM4_T3_OPCODE(h) (((h) >> 8) & 0xff)
#define RAGE128_PM4_OP_NOP         0x10
#define RAGE128_PM4_OP_PAINT_NC    0x11
#define RAGE128_PM4_OP_HOSTROW     0x19
#define RAGE128_PM4_OP_BITBLT_NC   0x1b
#define RAGE128_PM4_OP_PLY_NEXTSCAN 0x1d
#define RAGE128_PM4_OP_SET_SCISSORS 0x1e
#define RAGE128_PM4_OP_3D_SAVE_CONTEXT       0x20
#define RAGE128_PM4_OP_3D_PLAY_CONTEXT       0x21
#define RAGE128_PM4_OP_3D_RNDR_GEN_INDX_PRIM 0x23
#define RAGE128_PM4_OP_3D_RNDR_GEN_PRIM      0x25
#define RAGE128_PM4_OP_SCALER_CFG  0x26
#define RAGE128_PM4_OP_BLIT_MULTI  0x28
#define RAGE128_PM4_OP_LOAD_PALETTE          0x2c
#define RAGE128_PM4_OP_PURGE                 0x2d
#define RAGE128_PM4_OP_NEXT_VERTEX_BUNDLE    0x2e
#define RAGE128_PM4_OP_MPEG_MB               0x31
#define RAGE128_PM4_OP_PAINT       0x91
#define RAGE128_PM4_OP_CNTL_BITBLT 0x92
#define RAGE128_PM4_OP_SMALLTEXT   0x93
#define RAGE128_PM4_OP_HOSTDATA_BLT 0x94
#define RAGE128_PM4_OP_POLYLINE    0x95
#define RAGE128_PM4_OP_SCALING     0x96
#define RAGE128_PM4_OP_TRANS_SCALING 0x97
#define RAGE128_PM4_OP_SPANLIST    0x98
#define RAGE128_PM4_OP_PAINT_MULTI 0x9a
#define RAGE128_PM4_OP_BITBLT_MULTI 0x9b
#define RAGE128_PM4_OP_TRANS_BITBLT 0x9c

// VC_CNTL / VC_FORMAT (3D draw packets)
#define RAGE128_VC_PRIM_TYPE(v) ((v) & 0xf)
#define RAGE128_VC_PRIM_WALK(v) (((v) >> 4) & 0x3)
#define RAGE128_VC_NUM(v)       ((v) >> 16)
#define RAGE128_VC_WALK_IND  1
#define RAGE128_VC_WALK_LIST 2
#define RAGE128_VC_WALK_RING 3
#define RAGE128_VCF_RHW          0x001
#define RAGE128_VCF_DIFFUSE_BGR  0x002
#define RAGE128_VCF_DIFFUSE_A    0x004
#define RAGE128_VCF_DIFFUSE_ARGB 0x008
#define RAGE128_VCF_SPEC_BGR     0x010
#define RAGE128_VCF_SPEC_F       0x020
#define RAGE128_VCF_SPEC_FRGB    0x040
#define RAGE128_VCF_S_T          0x080
#define RAGE128_VCF_S2_T2        0x100
#define RAGE128_VCF_RHW2         0x200

// Surface translation windows / AGP
#define RAGE128_SURFACE_DELAY 0x0b00
#define RAGE128_SURFACE0_LOWER_BOUND 0x0b04
#define RAGE128_SURFACE3_INFO        0x0b3c
#define RAGE128_SURF_BOUND_MASK      0x03ffffc0
#define RAGE128_SURF_INFO_MASK       0x0000001f
#define RAGE128_AGP_CNTL_B    0x0b44
#define RAGE128_SURFACE_DELAY_DEFAULT 0x00000131

// PCI configuration space read-only mirror
#define RAGE128_CONFIG_MIRROR_BASE 0x0f00
#define RAGE128_CONFIG_MIRROR_END  0x0fff

// ------------------------------------------------------------------
// PLL register file (via CLOCK_CNTL_INDEX/DATA)
// ------------------------------------------------------------------
#define RAGE128_PLL_REGS 0x20
#define RAGE128_PLL_CLK_PIN_CNTL      0x01
#define RAGE128_PLL_PPLL_CNTL         0x02
#define RAGE128_PLL_PPLL_REF_DIV      0x03
#define RAGE128_PLL_PPLL_DIV_0        0x04
#define RAGE128_PLL_PPLL_DIV_1        0x05
#define RAGE128_PLL_PPLL_DIV_2        0x06
#define RAGE128_PLL_PPLL_DIV_3        0x07
#define RAGE128_PLL_VCLK_ECP_CNTL     0x08
#define RAGE128_PLL_HTOTAL_CNTL       0x09
#define RAGE128_PLL_X_MPLL_REF_FB_DIV 0x0a
#define RAGE128_PLL_XPLL_CNTL         0x0b
#define RAGE128_PLL_XDLL_CNTL         0x0c
#define RAGE128_PLL_XCLK_CNTL         0x0d
#define RAGE128_PLL_MPLL_CNTL         0x0e
#define RAGE128_PLL_MCLK_CNTL         0x0f
#define RAGE128_PLL_AGP_PLL_CNTL      0x10
#define RAGE128_PLL_FCP_CNTL          0x12
#define RAGE128_PLL_TEST_CNTL         0x13
#define RAGE128_PPLL_RESET             (1 << 0)
#define RAGE128_PPLL_SLEEP             (1 << 1)
#define RAGE128_PPLL_ATOMIC_UPDATE_EN  (1 << 16)
#define RAGE128_PPLL_VGA_ATOMIC_UPDATE_EN (1 << 17)
#define RAGE128_PPLL_ATOMIC_UPDATE_SYNC   (1 << 18)
#define RAGE128_PPLL_ATOMIC_UPDATE (1 << 15)
#define RAGE128_PPLL_REF_DIV_MASK  0x3ff
#define RAGE128_PPLL_FB_DIV_MASK   0x7ff
#define RAGE128_PPLL_POST_DIV_SHIFT 16
#define RAGE128_VCLK_SRC_SEL_MASK 0x3
#define RAGE128_VCLK_SRC_PPLL     3

// ------------------------------------------------------------------
// 2D GUI engine
// ------------------------------------------------------------------
#define RAGE128_DST_OFFSET         0x1404
#define RAGE128_DST_PITCH          0x1408
#define RAGE128_DST_WIDTH          0x140c
#define RAGE128_DST_HEIGHT         0x1410
#define RAGE128_SRC_X              0x1414
#define RAGE128_SRC_Y              0x1418
#define RAGE128_DST_X              0x141c
#define RAGE128_DST_Y              0x1420
#define RAGE128_SRC_PITCH_OFFSET   0x1428
#define RAGE128_DST_PITCH_OFFSET   0x142c
#define RAGE128_SRC_Y_X            0x1434
#define RAGE128_DST_Y_X            0x1438
#define RAGE128_DST_HEIGHT_WIDTH   0x143c
#define RAGE128_DP_GUI_MASTER_CNTL 0x146c
#define RAGE128_BRUSH_Y_X          0x1474
#define RAGE128_DP_BRUSH_BKGD_CLR  0x1478
#define RAGE128_DP_BRUSH_FRGD_CLR  0x147c
#define RAGE128_BRUSH_DATA0        0x1480
#define RAGE128_DST_WIDTH_X        0x1588
#define RAGE128_SRC_X_Y            0x1590
#define RAGE128_DST_X_Y            0x1594
#define RAGE128_DST_WIDTH_HEIGHT   0x1598
#define RAGE128_DST_WIDTH_X_INCY   0x159c
#define RAGE128_DST_HEIGHT_Y       0x15a0
#define RAGE128_SRC_OFFSET         0x15ac
#define RAGE128_SRC_PITCH          0x15b0
#define RAGE128_DST_WIDTH_BW       0x15b4
#define RAGE128_CLR_CMP_CNTL       0x15c0
#define RAGE128_CLR_CMP_CLR_SRC    0x15c4
#define RAGE128_CLR_CMP_CLR_DST    0x15c8
#define RAGE128_CLR_CMP_MASK       0x15cc
#define RAGE128_DP_SRC_FRGD_CLR    0x15d8
#define RAGE128_DP_SRC_BKGD_CLR    0x15dc
#define RAGE128_GUI_SCRATCH_REG0   0x15e0
#define RAGE128_DST_BRES_ERR       0x1628
#define RAGE128_DST_BRES_INC       0x162c
#define RAGE128_DST_BRES_DEC       0x1630
#define RAGE128_DST_BRES_LNTH      0x1634
#define RAGE128_SC_LEFT            0x1640
#define RAGE128_SC_RIGHT           0x1644
#define RAGE128_SC_TOP             0x1648
#define RAGE128_SC_BOTTOM          0x164c
#define RAGE128_SRC_SC_RIGHT       0x1654
#define RAGE128_SRC_SC_BOTTOM      0x165c
#define RAGE128_AUX_SC_CNTL        0x1660
#define RAGE128_AUX_SC_ENB_MASK    0x15u
#define RAGE128_AUX1_SC_LEFT       0x1664
#define RAGE128_AUX3_SC_BOTTOM     0x1690
#define RAGE128_GUI_DEBUG0         0x16a0
#define RAGE128_DP_CNTL            0x16c0
#define RAGE128_DP_CNTL_DST_X_DIR  (1 << 0)
#define RAGE128_DP_CNTL_DST_Y_DIR  (1 << 1)
#define RAGE128_DP_CNTL_DST_LAST_PEL (1 << 5)
#define RAGE128_DP_CNTL_POLY_LINE  (1 << 15)
#define RAGE128_DP_DATATYPE        0x16c4
#define RAGE128_DP_MIX             0x16c8
#define RAGE128_DP_WRITE_MASK      0x16cc
#define RAGE128_DP_CNTL_XDIR_YDIR_YMAJOR 0x16d0
#define RAGE128_DP_LINE_Y_MAJOR    (1u << 2)
#define RAGE128_DP_LINE_Y_DIR      (1u << 15)
#define RAGE128_DP_LINE_X_DIR      (1u << 31)
#define RAGE128_DEFAULT_OFFSET     0x16e0
#define RAGE128_DEFAULT_PITCH      0x16e4
#define RAGE128_DEFAULT_SC_BOTTOM_RIGHT 0x16e8
#define RAGE128_SC_TOP_LEFT        0x16ec
#define RAGE128_SC_BOTTOM_RIGHT    0x16f0
#define RAGE128_SRC_SC_BOTTOM_RIGHT 0x16f4
#define RAGE128_FLUSH_1     0x1704
#define RAGE128_FLUSH_7     0x171c
#define RAGE128_WAIT_UNTIL  0x1720
#define RAGE128_GUI_STAT    0x1740
#define RAGE128_GUI_STAT_BUSY 0x80010040
#define RAGE128_GUI_STAT_IDLE 0x00000040
#define RAGE128_PC_GUI_MODE 0x1744
#define RAGE128_PC_GUI_CTLSTAT     0x1748
#define RAGE128_HOST_DATA0         0x17c0
#define RAGE128_HOST_DATA7         0x17dc
#define RAGE128_HOST_DATA_LAST     0x17e0

// DP_GUI_MASTER_CNTL fields
#define RAGE128_GMC_SRC_PITCH_OFFSET_LEAVE (1 << 0)
#define RAGE128_GMC_DST_PITCH_OFFSET_LEAVE (1 << 1)
#define RAGE128_GMC_SRC_CLIP_LEAVE         (1 << 2)
#define RAGE128_GMC_DST_CLIP_LEAVE         (1 << 3)
#define RAGE128_GMC_BRUSH_TYPE(g)   (((g) >> 4) & 0xf)
#define RAGE128_GMC_DST_DATATYPE(g) (((g) >> 8) & 0xf)
#define RAGE128_GMC_SRC_DATATYPE(g) (((g) >> 12) & 0x3)
#define RAGE128_GMC_ROP3(g)         (((g) >> 16) & 0xff)
#define RAGE128_GMC_SRC_SOURCE(g)   (((g) >> 24) & 0x7)
#define RAGE128_GMC_3D_FCN_EN        (1u << 27)
#define RAGE128_GMC_CLR_CMP_CNTL_DIS (1 << 28)
#define RAGE128_GMC_AUX_CLIP_DIS     (1 << 29)
#define RAGE128_GMC_WR_MSK_DIS       (1 << 30)
#define RAGE128_GMC_LD_BRUSH_Y_X     (1u << 31)

// ------------------------------------------------------------------
// 3D engine
// ------------------------------------------------------------------
#define RAGE128_TEX_CNTL             0x1800
#define RAGE128_PRIM_TEX_OFFSET(n)   (0x1840 + (n) * 4)
#define RAGE128_SEC_TEX_OFFSET(n)    (0x1880 + (n) * 4)
#define RAGE128_TEX_PALETTE_INDEX    0x1968
#define RAGE128_TEX_PALETTE_DATA     0x196c
#define RAGE128_SCALE_SCR_HEIGHT_WIDTH 0x1994
#define RAGE128_SCALE_3D_CNTL        0x1a00
#define RAGE128_COMPOSITE_SHADOW_ID  0x1a0c
#define RAGE128_SHADOW_ID_MASK          0x00ffffff
#define RAGE128_SHADOW_AUTO_INC_DIS     (1 << 27)
#define RAGE128_SHADOW_ID_WMASK         0x08ffffff
#define RAGE128_FOG_TABLE_INDEX      0x1a14
#define RAGE128_FOG_TABLE_DATA       0x1a18
#define RAGE128_SCALE_3D_DATATYPE    0x1a20
#define RAGE128_CLR_CMP_CLR_3D       0x1a24
#define RAGE128_CLR_CMP_MSK_3D       0x1a28
#define RAGE128_SECONDARY_TEXTURE_COMBINE_CNTL 0x1a34
#define RAGE128_GRAD_BLOCK_BASE      0x1a40
#define RAGE128_GRAD_BLOCK_END       0x1a60
#define RAGE128_STEN_REF_MASK_LEGACY 0x1ad0
#define RAGE128_SETUP_CNTL           0x1bc4
#define RAGE128_WINDOW_XY_OFFSET     0x1bcc
#define RAGE128_DRAW_LINE_POINT      0x1bd0
#define RAGE128_SETUP_CNTL_PM4       0x1bd4
#define RAGE128_DST_PITCH_OFFSET_C   0x1c80
#define RAGE128_DP_GUI_MASTER_CNTL_C 0x1c84
#define RAGE128_SC_TOP_LEFT_C        0x1c88
#define RAGE128_SC_BOTTOM_RIGHT_C    0x1c8c
#define RAGE128_Z_OFFSET_C           0x1c90
#define RAGE128_Z_PITCH_C            0x1c94
#define RAGE128_Z_STEN_CNTL_C        0x1c98
#define RAGE128_TEX_CNTL_C           0x1c9c
#define RAGE128_MISC_3D_STATE_CNTL   0x1ca0
#define RAGE128_TEX_CLR_CMP_CLR_C    0x1ca4
#define RAGE128_TEX_CLR_CMP_MSK_C    0x1ca8
#define RAGE128_FOG_COLOR_C          0x1cac
#define RAGE128_PRIM_TEX_CNTL_C      0x1cb0
#define RAGE128_PRIM_TEX_COMBINE_CNTL_C 0x1cb4
#define RAGE128_TEX_SIZE_PITCH_C     0x1cb8
#define RAGE128_PRIM_TEX_OFFSET_C(n) (0x1cbc + (n) * 4)
#define RAGE128_SEC_TEX_CNTL_C       0x1d00
#define RAGE128_SEC_TEX_COMBINE_CNTL_C 0x1d04
#define RAGE128_SEC_TEX_OFFSET_C(n)  (0x1d08 + (n) * 4)
#define RAGE128_CONSTANT_COLOR_C     0x1d34
#define RAGE128_PRIM_TEX_BORDER_COLOR_C 0x1d38
#define RAGE128_SEC_TEX_BORDER_COLOR_C  0x1d3c
#define RAGE128_STEN_REF_MASK_C      0x1d40
#define RAGE128_PLANE_3D_MASK_C      0x1d44
#define RAGE128_M2IA_STATUS          0x1f88
#define RAGE128_M2IA_DATA            0x1f8c

// TEX_CNTL_C master enables
#define R3D_TC_Z_EN     (1 << 0)
#define R3D_TC_Z_WR     (1 << 1)
#define R3D_TC_STEN_EN  (1 << 3)
#define R3D_TC_TEX_EN   (1 << 4)
#define R3D_TC_SEC_EN   (1 << 5)
#define R3D_TC_FOG_EN   (1 << 7)
#define R3D_TC_ALPHA_EN (1 << 9)
#define R3D_TC_ATEST_EN (1 << 10)
#define R3D_TC_SPEC_EN  (1 << 11)

#endif // BX_IODEV_RAGE128_REGS_H
