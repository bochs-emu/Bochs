/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
/*
 *  Portion of this software comes with the following license
 */

/***************************************************************************

    Copyright Aaron Giles
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in
          the documentation and/or other materials provided with the
          distribution.
        * Neither the name 'MAME' nor the names of its contributors may be
          used to endorse or promote products derived from this software
          without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY AARON GILES ''AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL AARON GILES BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/

/*************************************************************************

    3dfx Voodoo Graphics SST-1/2 emulator

    emulator by Aaron Giles

**************************************************************************/


/*************************************
 *
 *  Misc. constants
 *
 *************************************/

/* enumeration describing reasons we might be stalled */
enum
{
  NOT_STALLED = 0,
  STALLED_UNTIL_FIFO_LWM,
  STALLED_UNTIL_FIFO_EMPTY
};

/* maximum number of TMUs */
#define MAX_TMU         2

/* accumulate operations less than this number of clocks */
#define ACCUMULATE_THRESHOLD  0

/* number of clocks to set up a triangle (just a guess) */
#define TRIANGLE_SETUP_CLOCKS 100

/* maximum number of rasterizers */
#define MAX_RASTERIZERS     1024

/* size of the rasterizer hash table */
#define RASTER_HASH_SIZE    97

/* flags for LFB writes */
#define LFB_RGB_PRESENT     1
#define LFB_ALPHA_PRESENT   2
#define LFB_DEPTH_PRESENT   4
#define LFB_DEPTH_PRESENT_MSW 8

/* flags for the register access array */
#define REGISTER_READ     0x01    /* reads are allowed */
#define REGISTER_WRITE      0x02    /* writes are allowed */
#define REGISTER_PIPELINED    0x04    /* writes are pipelined */
#define REGISTER_FIFO     0x08    /* writes go to FIFO */
#define REGISTER_WRITETHRU    0x10    /* writes are valid even for CMDFIFO */

/* shorter combinations to make the table smaller */
#define REG_R         (REGISTER_READ)
#define REG_W         (REGISTER_WRITE)
#define REG_WT          (REGISTER_WRITE | REGISTER_WRITETHRU)
#define REG_RW          (REGISTER_READ | REGISTER_WRITE)
#define REG_RWT         (REGISTER_READ | REGISTER_WRITE | REGISTER_WRITETHRU)
#define REG_RP          (REGISTER_READ | REGISTER_PIPELINED)
#define REG_WP          (REGISTER_WRITE | REGISTER_PIPELINED)
#define REG_RWP         (REGISTER_READ | REGISTER_WRITE | REGISTER_PIPELINED)
#define REG_RWPT        (REGISTER_READ | REGISTER_WRITE | REGISTER_PIPELINED | REGISTER_WRITETHRU)
#define REG_RF          (REGISTER_READ | REGISTER_FIFO)
#define REG_WF          (REGISTER_WRITE | REGISTER_FIFO)
#define REG_RWF         (REGISTER_READ | REGISTER_WRITE | REGISTER_FIFO)
#define REG_RPF         (REGISTER_READ | REGISTER_PIPELINED | REGISTER_FIFO)
#define REG_WPF         (REGISTER_WRITE | REGISTER_PIPELINED | REGISTER_FIFO)
#define REG_RWPF        (REGISTER_READ | REGISTER_WRITE | REGISTER_PIPELINED | REGISTER_FIFO)

/* lookup bits is the log2 of the size of the log table */
#define LOG_LOOKUP_BITS     7

/* output precision is how many fraction bits the result should have */
#define LOG_OUTPUT_PREC     8



/*************************************
 *
 *  Register constants
 *
 *************************************/

/* Codes to the right:
    R = readable
    W = writeable
    P = pipelined
    F = goes to FIFO
*/

/* 0x000 */
#define status      (0x000/4) /* R  P  */
#define intrCtrl    (0x004/4) /* RW P   -- Voodoo2/Banshee only */
#define vertexAx    (0x008/4) /*  W PF */
#define vertexAy    (0x00c/4) /*  W PF */
#define vertexBx    (0x010/4) /*  W PF */
#define vertexBy    (0x014/4) /*  W PF */
#define vertexCx    (0x018/4) /*  W PF */
#define vertexCy    (0x01c/4) /*  W PF */
#define startR      (0x020/4) /*  W PF */
#define startG      (0x024/4) /*  W PF */
#define startB      (0x028/4) /*  W PF */
#define startZ      (0x02c/4) /*  W PF */
#define startA      (0x030/4) /*  W PF */
#define startS      (0x034/4) /*  W PF */
#define startT      (0x038/4) /*  W PF */
#define startW      (0x03c/4) /*  W PF */

/* 0x040 */
#define dRdX      (0x040/4) /*  W PF */
#define dGdX      (0x044/4) /*  W PF */
#define dBdX      (0x048/4) /*  W PF */
#define dZdX      (0x04c/4) /*  W PF */
#define dAdX      (0x050/4) /*  W PF */
#define dSdX      (0x054/4) /*  W PF */
#define dTdX      (0x058/4) /*  W PF */
#define dWdX      (0x05c/4) /*  W PF */
#define dRdY      (0x060/4) /*  W PF */
#define dGdY      (0x064/4) /*  W PF */
#define dBdY      (0x068/4) /*  W PF */
#define dZdY      (0x06c/4) /*  W PF */
#define dAdY      (0x070/4) /*  W PF */
#define dSdY      (0x074/4) /*  W PF */
#define dTdY      (0x078/4) /*  W PF */
#define dWdY      (0x07c/4) /*  W PF */

/* 0x080 */
#define triangleCMD   (0x080/4) /*  W PF */
#define fvertexAx   (0x088/4) /*  W PF */
#define fvertexAy   (0x08c/4) /*  W PF */
#define fvertexBx   (0x090/4) /*  W PF */
#define fvertexBy   (0x094/4) /*  W PF */
#define fvertexCx   (0x098/4) /*  W PF */
#define fvertexCy   (0x09c/4) /*  W PF */
#define fstartR     (0x0a0/4) /*  W PF */
#define fstartG     (0x0a4/4) /*  W PF */
#define fstartB     (0x0a8/4) /*  W PF */
#define fstartZ     (0x0ac/4) /*  W PF */
#define fstartA     (0x0b0/4) /*  W PF */
#define fstartS     (0x0b4/4) /*  W PF */
#define fstartT     (0x0b8/4) /*  W PF */
#define fstartW     (0x0bc/4) /*  W PF */

/* 0x0c0 */
#define fdRdX     (0x0c0/4) /*  W PF */
#define fdGdX     (0x0c4/4) /*  W PF */
#define fdBdX     (0x0c8/4) /*  W PF */
#define fdZdX     (0x0cc/4) /*  W PF */
#define fdAdX     (0x0d0/4) /*  W PF */
#define fdSdX     (0x0d4/4) /*  W PF */
#define fdTdX     (0x0d8/4) /*  W PF */
#define fdWdX     (0x0dc/4) /*  W PF */
#define fdRdY     (0x0e0/4) /*  W PF */
#define fdGdY     (0x0e4/4) /*  W PF */
#define fdBdY     (0x0e8/4) /*  W PF */
#define fdZdY     (0x0ec/4) /*  W PF */
#define fdAdY     (0x0f0/4) /*  W PF */
#define fdSdY     (0x0f4/4) /*  W PF */
#define fdTdY     (0x0f8/4) /*  W PF */
#define fdWdY     (0x0fc/4) /*  W PF */

/* 0x100 */
#define ftriangleCMD  (0x100/4) /*  W PF */
#define fbzColorPath  (0x104/4) /* RW PF */
#define fogMode     (0x108/4) /* RW PF */
#define alphaMode   (0x10c/4) /* RW PF */
#define fbzMode     (0x110/4) /* RW  F */
#define lfbMode     (0x114/4) /* RW  F */
#define clipLeftRight (0x118/4) /* RW  F */
#define clipLowYHighY (0x11c/4) /* RW  F */
#define nopCMD      (0x120/4) /*  W  F */
#define fastfillCMD   (0x124/4) /*  W  F */
#define swapbufferCMD (0x128/4) /*  W  F */
#define fogColor    (0x12c/4) /*  W  F */
#define zaColor     (0x130/4) /*  W  F */
#define chromaKey   (0x134/4) /*  W  F */
#define chromaRange   (0x138/4) /*  W  F  -- Voodoo2/Banshee only */
#define userIntrCMD   (0x13c/4) /*  W  F  -- Voodoo2/Banshee only */

/* 0x140 */
#define stipple     (0x140/4) /* RW  F */
#define color0      (0x144/4) /* RW  F */
#define color1      (0x148/4) /* RW  F */
#define fbiPixelsIn   (0x14c/4) /* R     */
#define fbiChromaFail (0x150/4) /* R     */
#define fbiZfuncFail  (0x154/4) /* R     */
#define fbiAfuncFail  (0x158/4) /* R     */
#define fbiPixelsOut  (0x15c/4) /* R     */
#define fogTable    (0x160/4) /*  W  F */

/* 0x1c0 */
#define cmdFifoBaseAddr (0x1e0/4) /* RW     -- Voodoo2 only */
#define cmdFifoBump   (0x1e4/4) /* RW     -- Voodoo2 only */
#define cmdFifoRdPtr  (0x1e8/4) /* RW     -- Voodoo2 only */
#define cmdFifoAMin   (0x1ec/4) /* RW     -- Voodoo2 only */
#define colBufferAddr (0x1ec/4) /* RW     -- Banshee only */
#define cmdFifoAMax   (0x1f0/4) /* RW     -- Voodoo2 only */
#define colBufferStride (0x1f0/4) /* RW     -- Banshee only */
#define cmdFifoDepth  (0x1f4/4) /* RW     -- Voodoo2 only */
#define auxBufferAddr (0x1f4/4) /* RW     -- Banshee only */
#define cmdFifoHoles  (0x1f8/4) /* RW     -- Voodoo2 only */
#define auxBufferStride (0x1f8/4) /* RW     -- Banshee only */

/* 0x200 */
#define fbiInit4    (0x200/4) /* RW     -- Voodoo/Voodoo2 only */
#define clipLeftRight1  (0x200/4) /* RW     -- Banshee only */
#define vRetrace    (0x204/4) /* R      -- Voodoo/Voodoo2 only */
#define clipTopBottom1  (0x204/4) /* RW     -- Banshee only */
#define backPorch   (0x208/4) /* RW     -- Voodoo/Voodoo2 only */
#define videoDimensions (0x20c/4) /* RW     -- Voodoo/Voodoo2 only */
#define fbiInit0    (0x210/4) /* RW     -- Voodoo/Voodoo2 only */
#define fbiInit1    (0x214/4) /* RW     -- Voodoo/Voodoo2 only */
#define fbiInit2    (0x218/4) /* RW     -- Voodoo/Voodoo2 only */
#define fbiInit3    (0x21c/4) /* RW     -- Voodoo/Voodoo2 only */
#define hSync     (0x220/4) /*  W     -- Voodoo/Voodoo2 only */
#define vSync     (0x224/4) /*  W     -- Voodoo/Voodoo2 only */
#define clutData    (0x228/4) /*  W  F  -- Voodoo/Voodoo2 only */
#define dacData     (0x22c/4) /*  W     -- Voodoo/Voodoo2 only */
#define maxRgbDelta   (0x230/4) /*  W     -- Voodoo/Voodoo2 only */
#define hBorder     (0x234/4) /*  W     -- Voodoo2 only */
#define vBorder     (0x238/4) /*  W     -- Voodoo2 only */
#define borderColor   (0x23c/4) /*  W     -- Voodoo2 only */

/* 0x240 */
#define hvRetrace   (0x240/4) /* R      -- Voodoo2 only */
#define fbiInit5    (0x244/4) /* RW     -- Voodoo2 only */
#define fbiInit6    (0x248/4) /* RW     -- Voodoo2 only */
#define fbiInit7    (0x24c/4) /* RW     -- Voodoo2 only */
#define swapPending   (0x24c/4) /*  W     -- Banshee only */
#define leftOverlayBuf  (0x250/4) /*  W     -- Banshee only */
#define rightOverlayBuf (0x254/4) /*  W     -- Banshee only */
#define fbiSwapHistory  (0x258/4) /* R      -- Voodoo2/Banshee only */
#define fbiTrianglesOut (0x25c/4) /* R      -- Voodoo2/Banshee only */
#define sSetupMode    (0x260/4) /*  W PF  -- Voodoo2/Banshee only */
#define sVx       (0x264/4) /*  W PF  -- Voodoo2/Banshee only */
#define sVy       (0x268/4) /*  W PF  -- Voodoo2/Banshee only */
#define sARGB     (0x26c/4) /*  W PF  -- Voodoo2/Banshee only */
#define sRed      (0x270/4) /*  W PF  -- Voodoo2/Banshee only */
#define sGreen      (0x274/4) /*  W PF  -- Voodoo2/Banshee only */
#define sBlue     (0x278/4) /*  W PF  -- Voodoo2/Banshee only */
#define sAlpha      (0x27c/4) /*  W PF  -- Voodoo2/Banshee only */

/* 0x280 */
#define sVz       (0x280/4) /*  W PF  -- Voodoo2/Banshee only */
#define sWb       (0x284/4) /*  W PF  -- Voodoo2/Banshee only */
#define sWtmu0      (0x288/4) /*  W PF  -- Voodoo2/Banshee only */
#define sS_W0     (0x28c/4) /*  W PF  -- Voodoo2/Banshee only */
#define sT_W0     (0x290/4) /*  W PF  -- Voodoo2/Banshee only */
#define sWtmu1      (0x294/4) /*  W PF  -- Voodoo2/Banshee only */
#define sS_Wtmu1    (0x298/4) /*  W PF  -- Voodoo2/Banshee only */
#define sT_Wtmu1    (0x29c/4) /*  W PF  -- Voodoo2/Banshee only */
#define sDrawTriCMD   (0x2a0/4) /*  W PF  -- Voodoo2/Banshee only */
#define sBeginTriCMD  (0x2a4/4) /*  W PF  -- Voodoo2/Banshee only */

/* 0x2c0 */
#define bltSrcBaseAddr  (0x2c0/4) /* RW PF  -- Voodoo2 only */
#define bltDstBaseAddr  (0x2c4/4) /* RW PF  -- Voodoo2 only */
#define bltXYStrides  (0x2c8/4) /* RW PF  -- Voodoo2 only */
#define bltSrcChromaRange (0x2cc/4) /* RW PF  -- Voodoo2 only */
#define bltDstChromaRange (0x2d0/4) /* RW PF  -- Voodoo2 only */
#define bltClipX    (0x2d4/4) /* RW PF  -- Voodoo2 only */
#define bltClipY    (0x2d8/4) /* RW PF  -- Voodoo2 only */
#define bltSrcXY    (0x2e0/4) /* RW PF  -- Voodoo2 only */
#define bltDstXY    (0x2e4/4) /* RW PF  -- Voodoo2 only */
#define bltSize     (0x2e8/4) /* RW PF  -- Voodoo2 only */
#define bltRop      (0x2ec/4) /* RW PF  -- Voodoo2 only */
#define bltColor    (0x2f0/4) /* RW PF  -- Voodoo2 only */
#define bltCommand    (0x2f8/4) /* RW PF  -- Voodoo2 only */
#define bltData     (0x2fc/4) /*  W PF  -- Voodoo2 only */

/* 0x300 */
#define textureMode   (0x300/4) /*  W PF */
#define tLOD      (0x304/4) /*  W PF */
#define tDetail     (0x308/4) /*  W PF */
#define texBaseAddr   (0x30c/4) /*  W PF */
#define texBaseAddr_1 (0x310/4) /*  W PF */
#define texBaseAddr_2 (0x314/4) /*  W PF */
#define texBaseAddr_3_8 (0x318/4) /*  W PF */
#define trexInit0   (0x31c/4) /*  W  F  -- Voodoo/Voodoo2 only */
#define trexInit1   (0x320/4) /*  W  F */
#define nccTable    (0x324/4) /*  W  F */



/*************************************
 *
 *  Alias map of the first 64
 *  registers when remapped
 *
 *************************************/

static const Bit8u register_alias_map[0x40] =
{
  status,   0x004/4,  vertexAx, vertexAy,
  vertexBx, vertexBy, vertexCx, vertexCy,
  startR,   dRdX,   dRdY,   startG,
  dGdX,   dGdY,   startB,   dBdX,
  dBdY,   startZ,   dZdX,   dZdY,
  startA,   dAdX,   dAdY,   startS,
  dSdX,   dSdY,   startT,   dTdX,
  dTdY,   startW,   dWdX,   dWdY,

  triangleCMD,0x084/4,  fvertexAx,  fvertexAy,
  fvertexBx,  fvertexBy,  fvertexCx,  fvertexCy,
  fstartR,  fdRdX,    fdRdY,    fstartG,
  fdGdX,    fdGdY,    fstartB,  fdBdX,
  fdBdY,    fstartZ,  fdZdX,    fdZdY,
  fstartA,  fdAdX,    fdAdY,    fstartS,
  fdSdX,    fdSdY,    fstartT,  fdTdX,
  fdTdY,    fstartW,  fdWdX,    fdWdY
};


/*************************************
 *
 *  Table of per-register access rights
 *
 *************************************/

static const Bit8u voodoo_register_access[0x100] =
{
  /* 0x000 */
  REG_RP,   0,        REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,

  /* 0x040 */
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,

  /* 0x080 */
  REG_WPF,  0,        REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,

  /* 0x0c0 */
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,

  /* 0x100 */
  REG_WPF,  REG_RWPF, REG_RWPF, REG_RWPF,
  REG_RWF,  REG_RWF,  REG_RWF,  REG_RWF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   0,        0,

  /* 0x140 */
  REG_RWF,  REG_RWF,  REG_RWF,  REG_R,
  REG_R,    REG_R,    REG_R,    REG_R,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,

  /* 0x180 */
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,

  /* 0x1c0 */
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  0,        0,        0,        0,
  0,        0,        0,        0,

  /* 0x200 */
  REG_RW,   REG_R,    REG_RW,   REG_RW,
  REG_RW,   REG_RW,   REG_RW,   REG_RW,
  REG_W,    REG_W,    REG_W,    REG_W,
  REG_W,    0,        0,        0,

  /* 0x240 */
  0,      0,      0,      0,
  0,      0,      0,      0,
  0,      0,      0,      0,
  0,      0,      0,      0,

  /* 0x280 */
  0,      0,      0,      0,
  0,      0,      0,      0,
  0,      0,      0,      0,
  0,      0,      0,      0,

  /* 0x2c0 */
  0,      0,      0,      0,
  0,      0,      0,      0,
  0,      0,      0,      0,
  0,      0,      0,      0,

  /* 0x300 */
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,

  /* 0x340 */
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,

  /* 0x380 */
  REG_WF
};


static const Bit8u voodoo2_register_access[0x100] =
{
  /* 0x000 */
  REG_RP,   REG_RWPT, REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,

  /* 0x040 */
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,

  /* 0x080 */
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,

  /* 0x0c0 */
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,

  /* 0x100 */
  REG_WPF,  REG_RWPF, REG_RWPF, REG_RWPF,
  REG_RWF,  REG_RWF,  REG_RWF,  REG_RWF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,

  /* 0x140 */
  REG_RWF,  REG_RWF,  REG_RWF,  REG_R,
  REG_R,    REG_R,    REG_R,    REG_R,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,

  /* 0x180 */
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,

  /* 0x1c0 */
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_RWT,  REG_RWT,  REG_RWT,  REG_RWT,
  REG_RWT,  REG_RWT,  REG_RWT,  REG_RW,

  /* 0x200 */
  REG_RWT,  REG_R,    REG_RWT,  REG_RWT,
  REG_RWT,  REG_RWT,  REG_RWT,  REG_RWT,
  REG_WT,   REG_WT,   REG_WF,   REG_WT,
  REG_WT,   REG_WT,   REG_WT,   REG_WT,

  /* 0x240 */
  REG_R,    REG_RWT,  REG_RWT,  REG_RWT,
  0,        0,        REG_R,    REG_R,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,

  /* 0x280 */
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  0,        0,
  0,        0,        0,        0,

  /* 0x2c0 */
  REG_RWPF, REG_RWPF, REG_RWPF, REG_RWPF,
  REG_RWPF, REG_RWPF, REG_RWPF, REG_RWPF,
  REG_RWPF, REG_RWPF, REG_RWPF, REG_RWPF,
  REG_RWPF, REG_RWPF, REG_RWPF, REG_WPF,

  /* 0x300 */
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,

  /* 0x340 */
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,

  /* 0x380 */
  REG_WF
};


static const Bit8u banshee_register_access[0x100] =
{
  /* 0x000 */
  REG_RP,   REG_RWPT, REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,

  /* 0x040 */
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,

  /* 0x080 */
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,

  /* 0x0c0 */
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,

  /* 0x100 */
  REG_WPF,  REG_RWPF, REG_RWPF, REG_RWPF,
  REG_RWF,  REG_RWF,  REG_RWF,  REG_RWF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,

  /* 0x140 */
  REG_RWF,  REG_RWF,  REG_RWF,  REG_R,
  REG_R,    REG_R,    REG_R,    REG_R,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,

  /* 0x180 */
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,

  /* 0x1c0 */
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  0,        0,        0,        REG_RWF,
  REG_RWF,  REG_RWF,  REG_RWF,  0,

  /* 0x200 */
  REG_RWF,  REG_RWF,  0,        0,
  0,        0,        0,        0,
  0,        0,        0,        0,
  0,        0,        0,        0,

  /* 0x240 */
  0,        0,        0,        REG_WT,
  REG_RWF,  REG_RWF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_R,    REG_R,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,

  /* 0x280 */
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  0,        0,
  0,        0,        0,        0,

  /* 0x2c0 */
  0,      0,      0,      0,
  0,      0,      0,      0,
  0,      0,      0,      0,
  0,      0,      0,      0,

  /* 0x300 */
  REG_WPF,  REG_WPF,  REG_WPF,  REG_WPF,
  REG_WPF,  REG_WPF,  REG_WPF,  0,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,

  /* 0x340 */
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,
  REG_WF,   REG_WF,   REG_WF,   REG_WF,

  /* 0x380 */
  REG_WF
};


/*************************************
 *
 *  Register string table for debug
 *
 *************************************/

static const char *const voodoo_reg_name[0x100] =
{
  /* 0x000 */
  "status",   "{intrCtrl}", "vertexAx",   "vertexAy",
  "vertexBx",   "vertexBy",   "vertexCx",   "vertexCy",
  "startR",   "startG",   "startB",   "startZ",
  "startA",   "startS",   "startT",   "startW",
  /* 0x040 */
  "dRdX",     "dGdX",     "dBdX",     "dZdX",
  "dAdX",     "dSdX",     "dTdX",     "dWdX",
  "dRdY",     "dGdY",     "dBdY",     "dZdY",
  "dAdY",     "dSdY",     "dTdY",     "dWdY",
  /* 0x080 */
  "triangleCMD",  "reserved084",  "fvertexAx",  "fvertexAy",
  "fvertexBx",  "fvertexBy",  "fvertexCx",  "fvertexCy",
  "fstartR",    "fstartG",    "fstartB",    "fstartZ",
  "fstartA",    "fstartS",    "fstartT",    "fstartW",
  /* 0x0c0 */
  "fdRdX",    "fdGdX",    "fdBdX",    "fdZdX",
  "fdAdX",    "fdSdX",    "fdTdX",    "fdWdX",
  "fdRdY",    "fdGdY",    "fdBdY",    "fdZdY",
  "fdAdY",    "fdSdY",    "fdTdY",    "fdWdY",
  /* 0x100 */
  "ftriangleCMD", "fbzColorPath", "fogMode",    "alphaMode",
  "fbzMode",    "lfbMode",    "clipLeftRight","clipLowYHighY",
  "nopCMD",   "fastfillCMD",  "swapbufferCMD","fogColor",
  "zaColor",    "chromaKey",  "{chromaRange}","{userIntrCMD}",
  /* 0x140 */
  "stipple",    "color0",   "color1",   "fbiPixelsIn",
  "fbiChromaFail","fbiZfuncFail", "fbiAfuncFail", "fbiPixelsOut",
  "fogTable160",  "fogTable164",  "fogTable168",  "fogTable16c",
  "fogTable170",  "fogTable174",  "fogTable178",  "fogTable17c",
  /* 0x180 */
  "fogTable180",  "fogTable184",  "fogTable188",  "fogTable18c",
  "fogTable190",  "fogTable194",  "fogTable198",  "fogTable19c",
  "fogTable1a0",  "fogTable1a4",  "fogTable1a8",  "fogTable1ac",
  "fogTable1b0",  "fogTable1b4",  "fogTable1b8",  "fogTable1bc",
  /* 0x1c0 */
  "fogTable1c0",  "fogTable1c4",  "fogTable1c8",  "fogTable1cc",
  "fogTable1d0",  "fogTable1d4",  "fogTable1d8",  "fogTable1dc",
  "{cmdFifoBaseAddr}","{cmdFifoBump}","{cmdFifoRdPtr}","{cmdFifoAMin}",
  "{cmdFifoAMax}","{cmdFifoDepth}","{cmdFifoHoles}","reserved1fc",
  /* 0x200 */
  "fbiInit4",   "vRetrace",   "backPorch",  "videoDimensions",
  "fbiInit0",   "fbiInit1",   "fbiInit2",   "fbiInit3",
  "hSync",    "vSync",    "clutData",   "dacData",
  "maxRgbDelta",  "{hBorder}",  "{vBorder}",  "{borderColor}",
  /* 0x240 */
  "{hvRetrace}",  "{fbiInit5}", "{fbiInit6}", "{fbiInit7}",
  "reserved250",  "reserved254",  "{fbiSwapHistory}","{fbiTrianglesOut}",
  "{sSetupMode}", "{sVx}",    "{sVy}",    "{sARGB}",
  "{sRed}",   "{sGreen}",   "{sBlue}",    "{sAlpha}",
  /* 0x280 */
  "{sVz}",    "{sWb}",    "{sWtmu0}",   "{sS/Wtmu0}",
  "{sT/Wtmu0}", "{sWtmu1}",   "{sS/Wtmu1}", "{sT/Wtmu1}",
  "{sDrawTriCMD}","{sBeginTriCMD}","reserved2a8", "reserved2ac",
  "reserved2b0",  "reserved2b4",  "reserved2b8",  "reserved2bc",
  /* 0x2c0 */
  "{bltSrcBaseAddr}","{bltDstBaseAddr}","{bltXYStrides}","{bltSrcChromaRange}",
  "{bltDstChromaRange}","{bltClipX}","{bltClipY}","reserved2dc",
  "{bltSrcXY}", "{bltDstXY}", "{bltSize}",  "{bltRop}",
  "{bltColor}", "reserved2f4",  "{bltCommand}", "{bltData}",
  /* 0x300 */
  "textureMode",  "tLOD",     "tDetail",    "texBaseAddr",
  "texBaseAddr_1","texBaseAddr_2","texBaseAddr_3_8","trexInit0",
  "trexInit1",  "nccTable0.0",  "nccTable0.1",  "nccTable0.2",
  "nccTable0.3",  "nccTable0.4",  "nccTable0.5",  "nccTable0.6",
  /* 0x340 */
  "nccTable0.7",  "nccTable0.8",  "nccTable0.9",  "nccTable0.A",
  "nccTable0.B",  "nccTable1.0",  "nccTable1.1",  "nccTable1.2",
  "nccTable1.3",  "nccTable1.4",  "nccTable1.5",  "nccTable1.6",
  "nccTable1.7",  "nccTable1.8",  "nccTable1.9",  "nccTable1.A",
  /* 0x380 */
  "nccTable1.B",  "reserved384",  "reserved388",  "reserved38c"
};


static const char *const banshee_reg_name[] =
{
  /* 0x000 */
  "status",   "intrCtrl",   "vertexAx",   "vertexAy",
  "vertexBx",   "vertexBy",   "vertexCx",   "vertexCy",
  "startR",   "startG",   "startB",   "startZ",
  "startA",   "startS",   "startT",   "startW",
  /* 0x040 */
  "dRdX",     "dGdX",     "dBdX",     "dZdX",
  "dAdX",     "dSdX",     "dTdX",     "dWdX",
  "dRdY",     "dGdY",     "dBdY",     "dZdY",
  "dAdY",     "dSdY",     "dTdY",     "dWdY",
  /* 0x080 */
  "triangleCMD",  "reserved084",  "fvertexAx",  "fvertexAy",
  "fvertexBx",  "fvertexBy",  "fvertexCx",  "fvertexCy",
  "fstartR",    "fstartG",    "fstartB",    "fstartZ",
  "fstartA",    "fstartS",    "fstartT",    "fstartW",
  /* 0x0c0 */
  "fdRdX",    "fdGdX",    "fdBdX",    "fdZdX",
  "fdAdX",    "fdSdX",    "fdTdX",    "fdWdX",
  "fdRdY",    "fdGdY",    "fdBdY",    "fdZdY",
  "fdAdY",    "fdSdY",    "fdTdY",    "fdWdY",
  /* 0x100 */
  "ftriangleCMD", "fbzColorPath", "fogMode",    "alphaMode",
  "fbzMode",    "lfbMode",    "clipLeftRight","clipLowYHighY",
  "nopCMD",   "fastfillCMD",  "swapbufferCMD","fogColor",
  "zaColor",    "chromaKey",  "chromaRange",  "userIntrCMD",
  /* 0x140 */
  "stipple",    "color0",   "color1",   "fbiPixelsIn",
  "fbiChromaFail","fbiZfuncFail", "fbiAfuncFail", "fbiPixelsOut",
  "fogTable160",  "fogTable164",  "fogTable168",  "fogTable16c",
  "fogTable170",  "fogTable174",  "fogTable178",  "fogTable17c",
  /* 0x180 */
  "fogTable180",  "fogTable184",  "fogTable188",  "fogTable18c",
  "fogTable190",  "fogTable194",  "fogTable198",  "fogTable19c",
  "fogTable1a0",  "fogTable1a4",  "fogTable1a8",  "fogTable1ac",
  "fogTable1b0",  "fogTable1b4",  "fogTable1b8",  "fogTable1bc",
  /* 0x1c0 */
  "fogTable1c0",  "fogTable1c4",  "fogTable1c8",  "fogTable1cc",
  "fogTable1d0",  "fogTable1d4",  "fogTable1d8",  "fogTable1dc",
  "reserved1e0",  "reserved1e4",  "reserved1e8",  "colBufferAddr",
  "colBufferStride","auxBufferAddr","auxBufferStride","reserved1fc",
  /* 0x200 */
  "clipLeftRight1","clipTopBottom1","reserved208","reserved20c",
  "reserved210",  "reserved214",  "reserved218",  "reserved21c",
  "reserved220",  "reserved224",  "reserved228",  "reserved22c",
  "reserved230",  "reserved234",  "reserved238",  "reserved23c",
  /* 0x240 */
  "reserved240",  "reserved244",  "reserved248",  "swapPending",
  "leftOverlayBuf","rightOverlayBuf","fbiSwapHistory","fbiTrianglesOut",
  "sSetupMode", "sVx",      "sVy",      "sARGB",
  "sRed",     "sGreen",   "sBlue",    "sAlpha",
  /* 0x280 */
  "sVz",      "sWb",      "sWtmu0",   "sS/Wtmu0",
  "sT/Wtmu0",   "sWtmu1",   "sS/Wtmu1",   "sT/Wtmu1",
  "sDrawTriCMD",  "sBeginTriCMD", "reserved2a8",  "reserved2ac",
  "reserved2b0",  "reserved2b4",  "reserved2b8",  "reserved2bc",
  /* 0x2c0 */
  "reserved2c0",  "reserved2c4",  "reserved2c8",  "reserved2cc",
  "reserved2d0",  "reserved2d4",  "reserved2d8",  "reserved2dc",
  "reserved2e0",  "reserved2e4",  "reserved2e8",  "reserved2ec",
  "reserved2f0",  "reserved2f4",  "reserved2f8",  "reserved2fc",
  /* 0x300 */
  "textureMode",  "tLOD",     "tDetail",    "texBaseAddr",
  "texBaseAddr_1","texBaseAddr_2","texBaseAddr_3_8","reserved31c",
  "trexInit1",  "nccTable0.0",  "nccTable0.1",  "nccTable0.2",
  "nccTable0.3",  "nccTable0.4",  "nccTable0.5",  "nccTable0.6",
  /* 0x340 */
  "nccTable0.7",  "nccTable0.8",  "nccTable0.9",  "nccTable0.A",
  "nccTable0.B",  "nccTable1.0",  "nccTable1.1",  "nccTable1.2",
  "nccTable1.3",  "nccTable1.4",  "nccTable1.5",  "nccTable1.6",
  "nccTable1.7",  "nccTable1.8",  "nccTable1.9",  "nccTable1.A",
  /* 0x380 */
  "nccTable1.B"
};



/*************************************
 *
 *  Voodoo Banshee I/O space registers
 *
 *************************************/

/* 0x000 */
#define io_status             (0x000/4) /*  */
#define io_pciInit0           (0x004/4) /*  */
#define io_sipMonitor         (0x008/4) /*  */
#define io_lfbMemoryConfig    (0x00c/4) /*  */
#define io_miscInit0          (0x010/4) /*  */
#define io_miscInit1          (0x014/4) /*  */
#define io_dramInit0          (0x018/4) /*  */
#define io_dramInit1          (0x01c/4) /*  */
#define io_agpInit            (0x020/4) /*  */
#define io_tmuGbeInit         (0x024/4) /*  */
#define io_vgaInit0           (0x028/4) /*  */
#define io_vgaInit1           (0x02c/4) /*  */
#define io_dramCommand        (0x030/4) /*  */
#define io_dramData           (0x034/4) /*  */
#define io_strapInfo          (0x038/4) /*  */

/* 0x040 */
#define io_pllCtrl0           (0x040/4) /*  */
#define io_pllCtrl1           (0x044/4) /*  */
#define io_pllCtrl2           (0x048/4) /*  */
#define io_dacMode            (0x04c/4) /*  */
#define io_dacAddr            (0x050/4) /*  */
#define io_dacData            (0x054/4) /*  */
#define io_rgbMaxDelta        (0x058/4) /*  */
#define io_vidProcCfg         (0x05c/4) /*  */
#define io_hwCurPatAddr       (0x060/4) /*  */
#define io_hwCurLoc           (0x064/4) /*  */
#define io_hwCurC0            (0x068/4) /*  */
#define io_hwCurC1            (0x06c/4) /*  */
#define io_vidInFormat        (0x070/4) /*  */
#define io_vidInStatus        (0x074/4) /*  */
#define io_vidSerialParallelPort    (0x078/4) /*  */
#define io_vidInXDecimDeltas  (0x07c/4) /*  */

/* 0x080 */
#define io_vidInDecimInitErrs (0x080/4) /*  */
#define io_vidInYDecimDeltas  (0x084/4) /*  */
#define io_vidPixelBufThold   (0x088/4) /*  */
#define io_vidChromaMin       (0x08c/4) /*  */
#define io_vidChromaMax       (0x090/4) /*  */
#define io_vidCurrentLine     (0x094/4) /*  */
#define io_vidScreenSize      (0x098/4) /*  */
#define io_vidOverlayStartCoord (0x09c/4) /*  */
#define io_vidOverlayEndScreen (0x0a0/4) /*  */
#define io_vidOverlayDudx     (0x0a4/4) /*  */
#define io_vidOverlayDudxOffsetSrcWidth (0x0a8/4) /*  */
#define io_vidOverlayDvdy     (0x0ac/4) /*  */
#define io_vgab0              (0x0b0/4) /*  */
#define io_vgab4              (0x0b4/4) /*  */
#define io_vgab8              (0x0b8/4) /*  */
#define io_vgabc              (0x0bc/4) /*  */

/* 0x0c0 */
#define io_vgac0              (0x0c0/4) /*  */
#define io_vgac4              (0x0c4/4) /*  */
#define io_vgac8              (0x0c8/4) /*  */
#define io_vgacc              (0x0cc/4) /*  */
#define io_vgad0              (0x0d0/4) /*  */
#define io_vgad4              (0x0d4/4) /*  */
#define io_vgad8              (0x0d8/4) /*  */
#define io_vgadc              (0x0dc/4) /*  */
#define io_vidOverlayDvdyOffset (0x0e0/4) /*  */
#define io_vidDesktopStartAddr (0x0e4/4) /*  */
#define io_vidDesktopOverlayStride (0x0e8/4) /*  */
#define io_vidInAddr0         (0x0ec/4) /*  */
#define io_vidInAddr1         (0x0f0/4) /*  */
#define io_vidInAddr2         (0x0f4/4) /*  */
#define io_vidInStride        (0x0f8/4) /*  */
#define io_vidCurrOverlayStartAddr (0x0fc/4) /*  */



/*************************************
 *
 *  Register string table for debug
 *
 *************************************/

static const char *const banshee_io_reg_name[] =
{
  /* 0x000 */
  "status",   "pciInit0",   "sipMonitor", "lfbMemoryConfig",
  "miscInit0",  "miscInit1",  "dramInit0",  "dramInit1",
  "agpInit",    "tmuGbeInit", "vgaInit0",   "vgaInit1",
  "dramCommand",  "dramData",   "strapInfo", "reserved3c",

  /* 0x040 */
  "pllCtrl0",   "pllCtrl1",   "pllCtrl2",   "dacMode",
  "dacAddr",    "dacData",    "rgbMaxDelta",  "vidProcCfg",
  "hwCurPatAddr", "hwCurLoc",   "hwCurC0",    "hwCurC1",
  "vidInFormat",  "vidInStatus",  "vidSerialParallelPort","vidInXDecimDeltas",

  /* 0x080 */
  "vidInDecimInitErrs","vidInYDecimDeltas","vidPixelBufThold","vidChromaMin",
  "vidChromaMax", "vidCurrentLine","vidScreenSize","vidOverlayStartCoord",
  "vidOverlayEndScreen","vidOverlayDudx","vidOverlayDudxOffsetSrcWidth","vidOverlayDvdy",
  "vga[b0]",    "vga[b4]",    "vga[b8]",    "vga[bc]",

  /* 0x0c0 */
  "vga[c0]",    "vga[c4]",    "vga[c8]",    "vga[cc]",
  "vga[d0]",    "vga[d4]",    "vga[d8]",    "vga[dc]",
  "vidOverlayDvdyOffset","vidDesktopStartAddr","vidDesktopOverlayStride","vidInAddr0",
  "vidInAddr1", "vidInAddr2", "vidInStride",  "vidCurrOverlayStartAddr"
};



/*************************************
 *
 *  Voodoo Banshee AGP space registers
 *
 *************************************/

/* 0x000 */
#define agpReqSize        (0x000/4) /*  */
#define agpHostAddressLow   (0x004/4) /*  */
#define agpHostAddressHigh    (0x008/4) /*  */
#define agpGraphicsAddress    (0x00c/4) /*  */
#define agpGraphicsStride   (0x010/4) /*  */
#define agpMoveCMD        (0x014/4) /*  */
#define cmdBaseAddr0      (0x020/4) /*  */
#define cmdBaseSize0      (0x024/4) /*  */
#define cmdBump0        (0x028/4) /*  */
#define cmdRdPtrL0        (0x02c/4) /*  */
#define cmdRdPtrH0        (0x030/4) /*  */
#define cmdAMin0        (0x034/4) /*  */
#define cmdAMax0        (0x03c/4) /*  */

/* 0x040 */
#define cmdStatus0        (0x040/4) /*  */
#define cmdFifoDepth0     (0x044/4) /*  */
#define cmdHoleCnt0       (0x048/4) /*  */
#define cmdBaseAddr1      (0x050/4) /*  */
#define cmdBaseSize1      (0x054/4) /*  */
#define cmdBump1          (0x058/4) /*  */
#define cmdRdPtrL1        (0x05c/4) /*  */
#define cmdRdPtrH1        (0x060/4) /*  */
#define cmdAMin1          (0x064/4) /*  */
#define cmdAMax1          (0x06c/4) /*  */
#define cmdStatus1        (0x070/4) /*  */
#define cmdFifoDepth1     (0x074/4) /*  */
#define cmdHoleCnt1       (0x078/4) /*  */

/* 0x080 */
#define cmdFifoThresh     (0x080/4) /*  */
#define cmdHoleInt        (0x084/4) /*  */

/* 0x100 */
#define yuvBaseAddress      (0x100/4) /*  */
#define yuvStride       (0x104/4) /*  */
#define crc1          (0x120/4) /*  */
#define crc2          (0x130/4) /*  */



/*************************************
 *
 *  Register string table for debug
 *
 *************************************/

static const char *const banshee_agp_reg_name[] =
{
  /* 0x000 */
  "agpReqSize", "agpHostAddressLow","agpHostAddressHigh","agpGraphicsAddress",
  "agpGraphicsStride","agpMoveCMD","reserved18",  "reserved1c",
  "cmdBaseAddr0", "cmdBaseSize0", "cmdBump0",   "cmdRdPtrL0",
  "cmdRdPtrH0", "cmdAMin0",   "reserved38", "cmdAMax0",

  /* 0x040 */
  "reserved40", "cmdFifoDepth0","cmdHoleCnt0",  "reserved4c",
  "cmdBaseAddr1", "cmdBaseSize1", "cmdBump1",   "cmdRdPtrL1",
  "cmdRdPtrH1", "cmdAMin1",   "reserved68", "cmdAMax1",
  "reserved70", "cmdFifoDepth1","cmdHoleCnt1",  "reserved7c",

  /* 0x080 */
  "cmdFifoThresh","cmdHoleInt", "reserved88", "reserved8c",
  "reserved90", "reserved94", "reserved98", "reserved9c",
  "reserveda0", "reserveda4", "reserveda8", "reservedac",
  "reservedb0", "reservedb4", "reservedb8", "reservedbc",

  /* 0x0c0 */
  "reservedc0", "reservedc4", "reservedc8", "reservedcc",
  "reservedd0", "reservedd4", "reservedd8", "reserveddc",
  "reservede0", "reservede4", "reservede8", "reservedec",
  "reservedf0", "reservedf4", "reservedf8", "reservedfc",

  /* 0x100 */
  "yuvBaseAddress","yuvStride", "reserved108",  "reserved10c",
  "reserved110",  "reserved114",  "reserved118",  "reserved11c",
  "crc1",     "reserved124",  "reserved128",  "reserved12c",
  "crc2",     "reserved134",  "reserved138",  "reserved13c"
};



/*************************************
 *
 *  Voodoo Banshee 2D registers
 *
 *************************************/

/* 0x000 */
#define blt_status         (0x000/4) /*  */
#define blt_intrCtrl       (0x004/4) /*  */
#define blt_clip0Min       (0x008/4) /*  */
#define blt_clip0Max       (0x00c/4) /*  */
#define blt_dstBaseAddr    (0x010/4) /*  */
#define blt_dstFormat      (0x014/4) /*  */
#define blt_srcColorkeyMin (0x018/4) /*  */
#define blt_srcColorkeyMax (0x01c/4) /*  */
#define blt_dstColorkeyMin (0x020/4) /*  */
#define blt_dstColorkeyMax (0x024/4) /*  */
#define blt_bresError0     (0x028/4) /*  */
#define blt_bresError1     (0x02c/4) /*  */
#define blt_rop            (0x030/4) /*  */
#define blt_srcBaseAddr    (0x034/4) /*  */
#define blt_commandExtra   (0x038/4) /*  */
#define blt_lineStipple    (0x03c/4) /*  */

/* 0x040 */
#define blt_lineStyle      (0x040/4) /*  */
#define blt_pattern0Alias  (0x044/4) /*  */
#define blt_pattern1Alias  (0x048/4) /*  */
#define blt_clip1Min       (0x04c/4) /*  */
#define blt_clip1Max       (0x050/4) /*  */
#define blt_srcFormat      (0x054/4) /*  */
#define blt_srcSize        (0x058/4) /*  */
#define blt_srcXY          (0x05c/4) /*  */
#define blt_colorBack      (0x060/4) /*  */
#define blt_colorFore      (0x064/4) /*  */
#define blt_dstSize        (0x068/4) /*  */
#define blt_dstXY          (0x06c/4) /*  */
#define blt_command        (0x070/4) /*  */



/*************************************
 *
 *  Register string table for debug
 *
 *************************************/

static const char *const banshee_blt_reg_name[] =
{
  /* 0x000 */
  "status", "intrCtrl", "clip0Min", "clip0Max",
  "dstBaseAddr", "dstFormat", "srcColorkeyMin", "srcColorkeyMax",
  "dstColorkeyMin", "dstColorkeyMax", "bresError0", "bresError1",
  "rop", "srcBaseAddr", "commandExtra", "lineStipple",

  /* 0x040 */
 "lineStyle", "pattern0Alias", "pattern1Alias", "clip1Min",
 "clip1Max", "srcFormat", "srcSize", "srcXY",
 "colorBack", "colorFore", "dstSize", "dstXY",
 "command", "RESERVED", "RESERVED", "RESERVED"
};



/*************************************
 *
 *  Dithering tables
 *
 *************************************/

static const Bit8u dither_matrix_4x4[16] =
{
   0,  8,  2, 10,
  12,  4, 14,  6,
   3, 11,  1,  9,
  15,  7, 13,  5
};

static const Bit8u dither_matrix_2x2[16] =
{
   2, 10,  2, 10,
  14,  6, 14,  6,
   2, 10,  2, 10,
  14,  6, 14,  6
};



/*************************************
 *
 *  Macros for extracting pixels
 *
 *************************************/

#define EXTRACT_565_TO_888(val, a, b, c)          \
  (a) = (((val) >> 8) & 0xf8) | (((val) >> 13) & 0x07); \
  (b) = (((val) >> 3) & 0xfc) | (((val) >> 9) & 0x03);  \
  (c) = (((val) << 3) & 0xf8) | (((val) >> 2) & 0x07);  \

#define EXTRACT_x555_TO_888(val, a, b, c)         \
  (a) = (((val) >> 7) & 0xf8) | (((val) >> 12) & 0x07); \
  (b) = (((val) >> 2) & 0xf8) | (((val) >> 7) & 0x07);  \
  (c) = (((val) << 3) & 0xf8) | (((val) >> 2) & 0x07);  \

#define EXTRACT_555x_TO_888(val, a, b, c)         \
  (a) = (((val) >> 8) & 0xf8) | (((val) >> 13) & 0x07); \
  (b) = (((val) >> 3) & 0xf8) | (((val) >> 8) & 0x07);  \
  (c) = (((val) << 2) & 0xf8) | (((val) >> 3) & 0x07);  \

#define EXTRACT_1555_TO_8888(val, a, b, c, d)       \
  (a) = ((Bit16s)(val) >> 15) & 0xff;           \
  EXTRACT_x555_TO_888(val, b, c, d)           \

#define EXTRACT_5551_TO_8888(val, a, b, c, d)       \
  EXTRACT_555x_TO_888(val, a, b, c)           \
  (d) = ((val) & 0x0001) ? 0xff : 0x00;         \

#define EXTRACT_x888_TO_888(val, a, b, c)         \
  (a) = ((val) >> 16) & 0xff;               \
  (b) = ((val) >> 8) & 0xff;                \
  (c) = ((val) >> 0) & 0xff;                \

#define EXTRACT_888x_TO_888(val, a, b, c)         \
  (a) = ((val) >> 24) & 0xff;               \
  (b) = ((val) >> 16) & 0xff;               \
  (c) = ((val) >> 8) & 0xff;                \

#define EXTRACT_8888_TO_8888(val, a, b, c, d)       \
  (a) = ((val) >> 24) & 0xff;               \
  (b) = ((val) >> 16) & 0xff;               \
  (c) = ((val) >> 8) & 0xff;                \
  (d) = ((val) >> 0) & 0xff;                \

#define EXTRACT_4444_TO_8888(val, a, b, c, d)       \
  (a) = (((val) >> 8) & 0xf0) | (((val) >> 12) & 0x0f); \
  (b) = (((val) >> 4) & 0xf0) | (((val) >> 8) & 0x0f);  \
  (c) = (((val) >> 0) & 0xf0) | (((val) >> 4) & 0x0f);  \
  (d) = (((val) << 4) & 0xf0) | (((val) >> 0) & 0x0f);  \

#define EXTRACT_332_TO_888(val, a, b, c)          \
  (a) = (((val) >> 0) & 0xe0) | (((val) >> 3) & 0x1c) | (((val) >> 6) & 0x03); \
  (b) = (((val) << 3) & 0xe0) | (((val) >> 0) & 0x1c) | (((val) >> 3) & 0x03); \
  (c) = (((val) << 6) & 0xc0) | (((val) << 4) & 0x30) | (((val) << 2) & 0xc0) | (((val) << 0) & 0x03); \



/*************************************
 *
 *  Misc. macros
 *
 *************************************/

/* macro for clamping a value between minimum and maximum values */
#define CLAMP(val,min,max)    do { if ((val) < (min)) { (val) = (min); } else if ((val) > (max)) { (val) = (max); } } while (0)

/* macro to compute the base 2 log for LOD calculations */
#define LOGB2(x)        (log((double)(x)) / log(2.0))



/*************************************
 *
 *  Macros for extracting bitfields
 *
 *************************************/

#define INITEN_ENABLE_HW_INIT(val)      (((val) >> 0) & 1)
#define INITEN_ENABLE_PCI_FIFO(val)     (((val) >> 1) & 1)
#define INITEN_REMAP_INIT_TO_DAC(val)   (((val) >> 2) & 1)
#define INITEN_ENABLE_SNOOP0(val)     (((val) >> 4) & 1)
#define INITEN_SNOOP0_MEMORY_MATCH(val)   (((val) >> 5) & 1)
#define INITEN_SNOOP0_READWRITE_MATCH(val)  (((val) >> 6) & 1)
#define INITEN_ENABLE_SNOOP1(val)     (((val) >> 7) & 1)
#define INITEN_SNOOP1_MEMORY_MATCH(val)   (((val) >> 8) & 1)
#define INITEN_SNOOP1_READWRITE_MATCH(val)  (((val) >> 9) & 1)
#define INITEN_SLI_BUS_OWNER(val)     (((val) >> 10) & 1)
#define INITEN_SLI_ODD_EVEN(val)      (((val) >> 11) & 1)
#define INITEN_SECONDARY_REV_ID(val)    (((val) >> 12) & 0xf) /* voodoo 2 only */
#define INITEN_MFCTR_FAB_ID(val)      (((val) >> 16) & 0xf) /* voodoo 2 only */
#define INITEN_ENABLE_PCI_INTERRUPT(val)  (((val) >> 20) & 1)   /* voodoo 2 only */
#define INITEN_PCI_INTERRUPT_TIMEOUT(val) (((val) >> 21) & 1)   /* voodoo 2 only */
#define INITEN_ENABLE_NAND_TREE_TEST(val) (((val) >> 22) & 1)   /* voodoo 2 only */
#define INITEN_ENABLE_SLI_ADDRESS_SNOOP(val) (((val) >> 23) & 1)  /* voodoo 2 only */
#define INITEN_SLI_SNOOP_ADDRESS(val)   (((val) >> 24) & 0xff)  /* voodoo 2 only */

#define FBZCP_CC_RGBSELECT(val)       (((val) >> 0) & 3)
#define FBZCP_CC_ASELECT(val)       (((val) >> 2) & 3)
#define FBZCP_CC_LOCALSELECT(val)     (((val) >> 4) & 1)
#define FBZCP_CCA_LOCALSELECT(val)      (((val) >> 5) & 3)
#define FBZCP_CC_LOCALSELECT_OVERRIDE(val)  (((val) >> 7) & 1)
#define FBZCP_CC_ZERO_OTHER(val)      (((val) >> 8) & 1)
#define FBZCP_CC_SUB_CLOCAL(val)      (((val) >> 9) & 1)
#define FBZCP_CC_MSELECT(val)       (((val) >> 10) & 7)
#define FBZCP_CC_REVERSE_BLEND(val)     (((val) >> 13) & 1)
#define FBZCP_CC_ADD_ACLOCAL(val)     (((val) >> 14) & 3)
#define FBZCP_CC_INVERT_OUTPUT(val)     (((val) >> 16) & 1)
#define FBZCP_CCA_ZERO_OTHER(val)     (((val) >> 17) & 1)
#define FBZCP_CCA_SUB_CLOCAL(val)     (((val) >> 18) & 1)
#define FBZCP_CCA_MSELECT(val)        (((val) >> 19) & 7)
#define FBZCP_CCA_REVERSE_BLEND(val)    (((val) >> 22) & 1)
#define FBZCP_CCA_ADD_ACLOCAL(val)      (((val) >> 23) & 3)
#define FBZCP_CCA_INVERT_OUTPUT(val)    (((val) >> 25) & 1)
#define FBZCP_CCA_SUBPIXEL_ADJUST(val)    (((val) >> 26) & 1)
#define FBZCP_TEXTURE_ENABLE(val)     (((val) >> 27) & 1)
#define FBZCP_RGBZW_CLAMP(val)        (((val) >> 28) & 1)   /* voodoo 2 only */
#define FBZCP_ANTI_ALIAS(val)       (((val) >> 29) & 1)   /* voodoo 2 only */

#define ALPHAMODE_ALPHATEST(val)      (((val) >> 0) & 1)
#define ALPHAMODE_ALPHAFUNCTION(val)    (((val) >> 1) & 7)
#define ALPHAMODE_ALPHABLEND(val)     (((val) >> 4) & 1)
#define ALPHAMODE_ANTIALIAS(val)      (((val) >> 5) & 1)
#define ALPHAMODE_SRCRGBBLEND(val)      (((val) >> 8) & 15)
#define ALPHAMODE_DSTRGBBLEND(val)      (((val) >> 12) & 15)
#define ALPHAMODE_SRCALPHABLEND(val)    (((val) >> 16) & 15)
#define ALPHAMODE_DSTALPHABLEND(val)    (((val) >> 20) & 15)
#define ALPHAMODE_ALPHAREF(val)       (((val) >> 24) & 0xff)

#define FOGMODE_ENABLE_FOG(val)       (((val) >> 0) & 1)
#define FOGMODE_FOG_ADD(val)        (((val) >> 1) & 1)
#define FOGMODE_FOG_MULT(val)       (((val) >> 2) & 1)
#define FOGMODE_FOG_ZALPHA(val)       (((val) >> 3) & 3)
#define FOGMODE_FOG_CONSTANT(val)     (((val) >> 5) & 1)
#define FOGMODE_FOG_DITHER(val)       (((val) >> 6) & 1)    /* voodoo 2 only */
#define FOGMODE_FOG_ZONES(val)        (((val) >> 7) & 1)    /* voodoo 2 only */

#define FBZMODE_ENABLE_CLIPPING(val)    (((val) >> 0) & 1)
#define FBZMODE_ENABLE_CHROMAKEY(val)   (((val) >> 1) & 1)
#define FBZMODE_ENABLE_STIPPLE(val)     (((val) >> 2) & 1)
#define FBZMODE_WBUFFER_SELECT(val)     (((val) >> 3) & 1)
#define FBZMODE_ENABLE_DEPTHBUF(val)    (((val) >> 4) & 1)
#define FBZMODE_DEPTH_FUNCTION(val)     (((val) >> 5) & 7)
#define FBZMODE_ENABLE_DITHERING(val)   (((val) >> 8) & 1)
#define FBZMODE_RGB_BUFFER_MASK(val)    (((val) >> 9) & 1)
#define FBZMODE_AUX_BUFFER_MASK(val)    (((val) >> 10) & 1)
#define FBZMODE_DITHER_TYPE(val)      (((val) >> 11) & 1)
#define FBZMODE_STIPPLE_PATTERN(val)    (((val) >> 12) & 1)
#define FBZMODE_ENABLE_ALPHA_MASK(val)    (((val) >> 13) & 1)
#define FBZMODE_DRAW_BUFFER(val)      (((val) >> 14) & 3)
#define FBZMODE_ENABLE_DEPTH_BIAS(val)    (((val) >> 16) & 1)
#define FBZMODE_Y_ORIGIN(val)       (((val) >> 17) & 1)
#define FBZMODE_ENABLE_ALPHA_PLANES(val)  (((val) >> 18) & 1)
#define FBZMODE_ALPHA_DITHER_SUBTRACT(val)  (((val) >> 19) & 1)
#define FBZMODE_DEPTH_SOURCE_COMPARE(val) (((val) >> 20) & 1)
#define FBZMODE_DEPTH_FLOAT_SELECT(val)   (((val) >> 21) & 1)   /* voodoo 2 only */

#define LFBMODE_WRITE_FORMAT(val)     (((val) >> 0) & 0xf)
#define LFBMODE_WRITE_BUFFER_SELECT(val)  (((val) >> 4) & 3)
#define LFBMODE_READ_BUFFER_SELECT(val)   (((val) >> 6) & 3)
#define LFBMODE_ENABLE_PIXEL_PIPELINE(val)  (((val) >> 8) & 1)
#define LFBMODE_RGBA_LANES(val)       (((val) >> 9) & 3)
#define LFBMODE_WORD_SWAP_WRITES(val)   (((val) >> 11) & 1)
#define LFBMODE_BYTE_SWIZZLE_WRITES(val)  (((val) >> 12) & 1)
#define LFBMODE_Y_ORIGIN(val)       (((val) >> 13) & 1)
#define LFBMODE_WRITE_W_SELECT(val)     (((val) >> 14) & 1)
#define LFBMODE_WORD_SWAP_READS(val)    (((val) >> 15) & 1)
#define LFBMODE_BYTE_SWIZZLE_READS(val)   (((val) >> 16) & 1)

#define CHROMARANGE_BLUE_EXCLUSIVE(val)   (((val) >> 24) & 1)
#define CHROMARANGE_GREEN_EXCLUSIVE(val)  (((val) >> 25) & 1)
#define CHROMARANGE_RED_EXCLUSIVE(val)    (((val) >> 26) & 1)
#define CHROMARANGE_UNION_MODE(val)     (((val) >> 27) & 1)
#define CHROMARANGE_ENABLE(val)       (((val) >> 28) & 1)

#define FBIINIT0_VGA_PASSTHRU(val)      (((val) >> 0) & 1)
#define FBIINIT0_GRAPHICS_RESET(val)    (((val) >> 1) & 1)
#define FBIINIT0_FIFO_RESET(val)      (((val) >> 2) & 1)
#define FBIINIT0_SWIZZLE_REG_WRITES(val)  (((val) >> 3) & 1)
#define FBIINIT0_STALL_PCIE_FOR_HWM(val)  (((val) >> 4) & 1)
#define FBIINIT0_PCI_FIFO_LWM(val)      (((val) >> 6) & 0x1f)
#define FBIINIT0_LFB_TO_MEMORY_FIFO(val)  (((val) >> 11) & 1)
#define FBIINIT0_TEXMEM_TO_MEMORY_FIFO(val) (((val) >> 12) & 1)
#define FBIINIT0_ENABLE_MEMORY_FIFO(val)  (((val) >> 13) & 1)
#define FBIINIT0_MEMORY_FIFO_HWM(val)   (((val) >> 14) & 0x7ff)
#define FBIINIT0_MEMORY_FIFO_BURST(val)   (((val) >> 25) & 0x3f)

#define FBIINIT1_PCI_DEV_FUNCTION(val)    (((val) >> 0) & 1)
#define FBIINIT1_PCI_WRITE_WAIT_STATES(val) (((val) >> 1) & 1)
#define FBIINIT1_MULTI_SST1(val)      (((val) >> 2) & 1)    /* not on voodoo 2 */
#define FBIINIT1_ENABLE_LFB(val)      (((val) >> 3) & 1)
#define FBIINIT1_X_VIDEO_TILES(val)     (((val) >> 4) & 0xf)
#define FBIINIT1_VIDEO_TIMING_RESET(val)  (((val) >> 8) & 1)
#define FBIINIT1_SOFTWARE_OVERRIDE(val)   (((val) >> 9) & 1)
#define FBIINIT1_SOFTWARE_HSYNC(val)    (((val) >> 10) & 1)
#define FBIINIT1_SOFTWARE_VSYNC(val)    (((val) >> 11) & 1)
#define FBIINIT1_SOFTWARE_BLANK(val)    (((val) >> 12) & 1)
#define FBIINIT1_DRIVE_VIDEO_TIMING(val)  (((val) >> 13) & 1)
#define FBIINIT1_DRIVE_VIDEO_BLANK(val)   (((val) >> 14) & 1)
#define FBIINIT1_DRIVE_VIDEO_SYNC(val)    (((val) >> 15) & 1)
#define FBIINIT1_DRIVE_VIDEO_DCLK(val)    (((val) >> 16) & 1)
#define FBIINIT1_VIDEO_TIMING_VCLK(val)   (((val) >> 17) & 1)
#define FBIINIT1_VIDEO_CLK_2X_DELAY(val)  (((val) >> 18) & 3)
#define FBIINIT1_VIDEO_TIMING_SOURCE(val) (((val) >> 20) & 3)
#define FBIINIT1_ENABLE_24BPP_OUTPUT(val) (((val) >> 22) & 1)
#define FBIINIT1_ENABLE_SLI(val)      (((val) >> 23) & 1)
#define FBIINIT1_X_VIDEO_TILES_BIT5(val)  (((val) >> 24) & 1)   /* voodoo 2 only */
#define FBIINIT1_ENABLE_EDGE_FILTER(val)  (((val) >> 25) & 1)
#define FBIINIT1_INVERT_VID_CLK_2X(val)   (((val) >> 26) & 1)
#define FBIINIT1_VID_CLK_2X_SEL_DELAY(val)  (((val) >> 27) & 3)
#define FBIINIT1_VID_CLK_DELAY(val)     (((val) >> 29) & 3)
#define FBIINIT1_DISABLE_FAST_READAHEAD(val) (((val) >> 31) & 1)

#define FBIINIT2_DISABLE_DITHER_SUB(val)  (((val) >> 0) & 1)
#define FBIINIT2_DRAM_BANKING(val)      (((val) >> 1) & 1)
#define FBIINIT2_ENABLE_TRIPLE_BUF(val)   (((val) >> 4) & 1)
#define FBIINIT2_ENABLE_FAST_RAS_READ(val)  (((val) >> 5) & 1)
#define FBIINIT2_ENABLE_GEN_DRAM_OE(val)  (((val) >> 6) & 1)
#define FBIINIT2_ENABLE_FAST_READWRITE(val) (((val) >> 7) & 1)
#define FBIINIT2_ENABLE_PASSTHRU_DITHER(val) (((val) >> 8) & 1)
#define FBIINIT2_SWAP_BUFFER_ALGORITHM(val) (((val) >> 9) & 3)
#define FBIINIT2_VIDEO_BUFFER_OFFSET(val) (((val) >> 11) & 0x1ff)
#define FBIINIT2_ENABLE_DRAM_BANKING(val) (((val) >> 20) & 1)
#define FBIINIT2_ENABLE_DRAM_READ_FIFO(val) (((val) >> 21) & 1)
#define FBIINIT2_ENABLE_DRAM_REFRESH(val) (((val) >> 22) & 1)
#define FBIINIT2_REFRESH_LOAD_VALUE(val)  (((val) >> 23) & 0x1ff)

#define FBIINIT3_TRI_REGISTER_REMAP(val)  (((val) >> 0) & 1)
#define FBIINIT3_VIDEO_FIFO_THRESH(val)   (((val) >> 1) & 0x1f)
#define FBIINIT3_DISABLE_TMUS(val)      (((val) >> 6) & 1)
#define FBIINIT3_FBI_MEMORY_TYPE(val)   (((val) >> 8) & 7)
#define FBIINIT3_VGA_PASS_RESET_VAL(val)  (((val) >> 11) & 1)
#define FBIINIT3_HARDCODE_PCI_BASE(val)   (((val) >> 12) & 1)
#define FBIINIT3_FBI2TREX_DELAY(val)    (((val) >> 13) & 0xf)
#define FBIINIT3_TREX2FBI_DELAY(val)    (((val) >> 17) & 0x1f)
#define FBIINIT3_YORIGIN_SUBTRACT(val)    (((val) >> 22) & 0x3ff)

#define FBIINIT4_PCI_READ_WAITS(val)    (((val) >> 0) & 1)
#define FBIINIT4_ENABLE_LFB_READAHEAD(val)  (((val) >> 1) & 1)
#define FBIINIT4_MEMORY_FIFO_LWM(val)   (((val) >> 2) & 0x3f)
#define FBIINIT4_MEMORY_FIFO_START_ROW(val) (((val) >> 8) & 0x3ff)
#define FBIINIT4_MEMORY_FIFO_STOP_ROW(val)  (((val) >> 18) & 0x3ff)
#define FBIINIT4_VIDEO_CLOCKING_DELAY(val)  (((val) >> 29) & 7)   /* voodoo 2 only */

#define FBIINIT5_DISABLE_PCI_STOP(val)    (((val) >> 0) & 1)    /* voodoo 2 only */
#define FBIINIT5_PCI_SLAVE_SPEED(val)   (((val) >> 1) & 1)    /* voodoo 2 only */
#define FBIINIT5_DAC_DATA_OUTPUT_WIDTH(val) (((val) >> 2) & 1)    /* voodoo 2 only */
#define FBIINIT5_DAC_DATA_17_OUTPUT(val)  (((val) >> 3) & 1)    /* voodoo 2 only */
#define FBIINIT5_DAC_DATA_18_OUTPUT(val)  (((val) >> 4) & 1)    /* voodoo 2 only */
#define FBIINIT5_GENERIC_STRAPPING(val)   (((val) >> 5) & 0xf)  /* voodoo 2 only */
#define FBIINIT5_BUFFER_ALLOCATION(val)   (((val) >> 9) & 3)    /* voodoo 2 only */
#define FBIINIT5_DRIVE_VID_CLK_SLAVE(val) (((val) >> 11) & 1)   /* voodoo 2 only */
#define FBIINIT5_DRIVE_DAC_DATA_16(val)   (((val) >> 12) & 1)   /* voodoo 2 only */
#define FBIINIT5_VCLK_INPUT_SELECT(val)   (((val) >> 13) & 1)   /* voodoo 2 only */
#define FBIINIT5_MULTI_CVG_DETECT(val)    (((val) >> 14) & 1)   /* voodoo 2 only */
#define FBIINIT5_SYNC_RETRACE_READS(val)  (((val) >> 15) & 1)   /* voodoo 2 only */
#define FBIINIT5_ENABLE_RHBORDER_COLOR(val) (((val) >> 16) & 1)   /* voodoo 2 only */
#define FBIINIT5_ENABLE_LHBORDER_COLOR(val) (((val) >> 17) & 1)   /* voodoo 2 only */
#define FBIINIT5_ENABLE_BVBORDER_COLOR(val) (((val) >> 18) & 1)   /* voodoo 2 only */
#define FBIINIT5_ENABLE_TVBORDER_COLOR(val) (((val) >> 19) & 1)   /* voodoo 2 only */
#define FBIINIT5_DOUBLE_HORIZ(val)      (((val) >> 20) & 1)   /* voodoo 2 only */
#define FBIINIT5_DOUBLE_VERT(val)     (((val) >> 21) & 1)   /* voodoo 2 only */
#define FBIINIT5_ENABLE_16BIT_GAMMA(val)  (((val) >> 22) & 1)   /* voodoo 2 only */
#define FBIINIT5_INVERT_DAC_HSYNC(val)    (((val) >> 23) & 1)   /* voodoo 2 only */
#define FBIINIT5_INVERT_DAC_VSYNC(val)    (((val) >> 24) & 1)   /* voodoo 2 only */
#define FBIINIT5_ENABLE_24BIT_DACDATA(val)  (((val) >> 25) & 1)   /* voodoo 2 only */
#define FBIINIT5_ENABLE_INTERLACING(val)  (((val) >> 26) & 1)   /* voodoo 2 only */
#define FBIINIT5_DAC_DATA_18_CONTROL(val) (((val) >> 27) & 1)   /* voodoo 2 only */
#define FBIINIT5_RASTERIZER_UNIT_MODE(val)  (((val) >> 30) & 3)   /* voodoo 2 only */

#define FBIINIT6_WINDOW_ACTIVE_COUNTER(val) (((val) >> 0) & 7)    /* voodoo 2 only */
#define FBIINIT6_WINDOW_DRAG_COUNTER(val) (((val) >> 3) & 0x1f) /* voodoo 2 only */
#define FBIINIT6_SLI_SYNC_MASTER(val)   (((val) >> 8) & 1)    /* voodoo 2 only */
#define FBIINIT6_DAC_DATA_22_OUTPUT(val)  (((val) >> 9) & 3)    /* voodoo 2 only */
#define FBIINIT6_DAC_DATA_23_OUTPUT(val)  (((val) >> 11) & 3)   /* voodoo 2 only */
#define FBIINIT6_SLI_SYNCIN_OUTPUT(val)   (((val) >> 13) & 3)   /* voodoo 2 only */
#define FBIINIT6_SLI_SYNCOUT_OUTPUT(val)  (((val) >> 15) & 3)   /* voodoo 2 only */
#define FBIINIT6_DAC_RD_OUTPUT(val)     (((val) >> 17) & 3)   /* voodoo 2 only */
#define FBIINIT6_DAC_WR_OUTPUT(val)     (((val) >> 19) & 3)   /* voodoo 2 only */
#define FBIINIT6_PCI_FIFO_LWM_RDY(val)    (((val) >> 21) & 0x7f)  /* voodoo 2 only */
#define FBIINIT6_VGA_PASS_N_OUTPUT(val)   (((val) >> 28) & 3)   /* voodoo 2 only */
#define FBIINIT6_X_VIDEO_TILES_BIT0(val)  (((val) >> 30) & 1)   /* voodoo 2 only */

#define FBIINIT7_GENERIC_STRAPPING(val)   (((val) >> 0) & 0xff) /* voodoo 2 only */
#define FBIINIT7_CMDFIFO_ENABLE(val)    (((val) >> 8) & 1)    /* voodoo 2 only */
#define FBIINIT7_CMDFIFO_MEMORY_STORE(val)  (((val) >> 9) & 1)    /* voodoo 2 only */
#define FBIINIT7_DISABLE_CMDFIFO_HOLES(val) (((val) >> 10) & 1)   /* voodoo 2 only */
#define FBIINIT7_CMDFIFO_READ_THRESH(val) (((val) >> 11) & 0x1f)  /* voodoo 2 only */
#define FBIINIT7_SYNC_CMDFIFO_WRITES(val) (((val) >> 16) & 1)   /* voodoo 2 only */
#define FBIINIT7_SYNC_CMDFIFO_READS(val)  (((val) >> 17) & 1)   /* voodoo 2 only */
#define FBIINIT7_RESET_PCI_PACKER(val)    (((val) >> 18) & 1)   /* voodoo 2 only */
#define FBIINIT7_ENABLE_CHROMA_STUFF(val) (((val) >> 19) & 1)   /* voodoo 2 only */
#define FBIINIT7_CMDFIFO_PCI_TIMEOUT(val) (((val) >> 20) & 0x7f)  /* voodoo 2 only */
#define FBIINIT7_ENABLE_TEXTURE_BURST(val)  (((val) >> 27) & 1)   /* voodoo 2 only */

#define TEXMODE_ENABLE_PERSPECTIVE(val)   (((val) >> 0) & 1)
#define TEXMODE_MINIFICATION_FILTER(val)  (((val) >> 1) & 1)
#define TEXMODE_MAGNIFICATION_FILTER(val) (((val) >> 2) & 1)
#define TEXMODE_CLAMP_NEG_W(val)      (((val) >> 3) & 1)
#define TEXMODE_ENABLE_LOD_DITHER(val)    (((val) >> 4) & 1)
#define TEXMODE_NCC_TABLE_SELECT(val)   (((val) >> 5) & 1)
#define TEXMODE_CLAMP_S(val)        (((val) >> 6) & 1)
#define TEXMODE_CLAMP_T(val)        (((val) >> 7) & 1)
#define TEXMODE_FORMAT(val)         (((val) >> 8) & 0xf)
#define TEXMODE_TC_ZERO_OTHER(val)      (((val) >> 12) & 1)
#define TEXMODE_TC_SUB_CLOCAL(val)      (((val) >> 13) & 1)
#define TEXMODE_TC_MSELECT(val)       (((val) >> 14) & 7)
#define TEXMODE_TC_REVERSE_BLEND(val)   (((val) >> 17) & 1)
#define TEXMODE_TC_ADD_ACLOCAL(val)     (((val) >> 18) & 3)
#define TEXMODE_TC_INVERT_OUTPUT(val)   (((val) >> 20) & 1)
#define TEXMODE_TCA_ZERO_OTHER(val)     (((val) >> 21) & 1)
#define TEXMODE_TCA_SUB_CLOCAL(val)     (((val) >> 22) & 1)
#define TEXMODE_TCA_MSELECT(val)      (((val) >> 23) & 7)
#define TEXMODE_TCA_REVERSE_BLEND(val)    (((val) >> 26) & 1)
#define TEXMODE_TCA_ADD_ACLOCAL(val)    (((val) >> 27) & 3)
#define TEXMODE_TCA_INVERT_OUTPUT(val)    (((val) >> 29) & 1)
#define TEXMODE_TRILINEAR(val)        (((val) >> 30) & 1)
#define TEXMODE_SEQ_8_DOWNLD(val)     (((val) >> 31) & 1)

#define TEXLOD_LODMIN(val)          (((val) >> 0) & 0x3f)
#define TEXLOD_LODMAX(val)          (((val) >> 6) & 0x3f)
#define TEXLOD_LODBIAS(val)         (((val) >> 12) & 0x3f)
#define TEXLOD_LOD_ODD(val)         (((val) >> 18) & 1)
#define TEXLOD_LOD_TSPLIT(val)        (((val) >> 19) & 1)
#define TEXLOD_LOD_S_IS_WIDER(val)      (((val) >> 20) & 1)
#define TEXLOD_LOD_ASPECT(val)        (((val) >> 21) & 3)
#define TEXLOD_LOD_ZEROFRAC(val)      (((val) >> 23) & 1)
#define TEXLOD_TMULTIBASEADDR(val)      (((val) >> 24) & 1)
#define TEXLOD_TDATA_SWIZZLE(val)     (((val) >> 25) & 1)
#define TEXLOD_TDATA_SWAP(val)        (((val) >> 26) & 1)
#define TEXLOD_TDIRECT_WRITE(val)     (((val) >> 27) & 1)   /* Voodoo 2 only */
#define TEXLOD_TMIRROR_S(val)         (((val) >> 28) & 1)   /* Banshee only */
#define TEXLOD_TMIRROR_T(val)         (((val) >> 29) & 1)   /* Banshee only */

#define TEXDETAIL_DETAIL_MAX(val)     (((val) >> 0) & 0xff)
#define TEXDETAIL_DETAIL_BIAS(val)      (((val) >> 8) & 0x3f)
#define TEXDETAIL_DETAIL_SCALE(val)     (((val) >> 14) & 7)
#define TEXDETAIL_RGB_MIN_FILTER(val)   (((val) >> 17) & 1)   /* Voodoo 2 only */
#define TEXDETAIL_RGB_MAG_FILTER(val)   (((val) >> 18) & 1)   /* Voodoo 2 only */
#define TEXDETAIL_ALPHA_MIN_FILTER(val)   (((val) >> 19) & 1)   /* Voodoo 2 only */
#define TEXDETAIL_ALPHA_MAG_FILTER(val)   (((val) >> 20) & 1)   /* Voodoo 2 only */
#define TEXDETAIL_SEPARATE_RGBA_FILTER(val) (((val) >> 21) & 1)   /* Voodoo 2 only */

#define TREXINIT_SEND_TMU_CONFIG(val)   (((val) >> 18) & 1)


/*************************************
 *
 *  Core types
 *
 *************************************/

typedef struct _voodoo_state voodoo_state;
typedef struct _poly_extra_data poly_extra_data;

typedef Bit32u rgb_t;

typedef struct _rgba rgba;
#define LSB_FIRST
struct _rgba
{
#ifdef LSB_FIRST
  Bit8u b, g, r, a;
#else
  Bit8u a, r, g, b;
#endif
};


typedef union _voodoo_reg voodoo_reg;
union _voodoo_reg
{
  Bit32s  i;
  Bit32u  u;
  float   f;
  rgba    rgb;
};


typedef voodoo_reg rgb_union;


/* note that this structure is an even 64 bytes long */
typedef struct _stats_block stats_block;
struct _stats_block
{
  Bit32s  pixels_in;        /* pixels in statistic */
  Bit32s  pixels_out;       /* pixels out statistic */
  Bit32s  chroma_fail;      /* chroma test fail statistic */
  Bit32s  zfunc_fail;       /* z function test fail statistic */
  Bit32s  afunc_fail;       /* alpha function test fail statistic */
  Bit32s  clip_fail;        /* clipping fail statistic */
  Bit32s  stipple_count;    /* stipple statistic */
  Bit32s  filler[64/4 - 7]; /* pad this structure to 64 bytes */
};


typedef struct _fifo_state fifo_state;
struct _fifo_state
{
  bool enabled; /* enabled? */
  Bit32u* base; /* base of the FIFO */
  Bit32s  size; /* size of the FIFO */
  Bit32s  in;   /* input pointer */
  Bit32s  out;  /* output pointer */
};


typedef struct _cmdfifo_info cmdfifo_info;
struct _cmdfifo_info
{
  bool    enabled;      /* enabled? */
  bool    count_holes;  /* count holes? */
  Bit32u  base;         /* base address in framebuffer RAM */
  Bit32u  end;          /* end address in framebuffer RAM */
  Bit32u  rdptr;        /* current read pointer */
  Bit32u  amin;         /* minimum address */
  Bit32u  amax;         /* maximum address */
  Bit32u  depth;        /* current depth */
  Bit32u  depth_needed; /* depth needed for command */
  Bit32u  holes;        /* number of holes */
  Bit32u  retAddr;
  bool    cmd_ready;
  bool    jsr;
};


typedef struct _pci_state pci_state;
struct _pci_state
{
  fifo_state  fifo;           /* PCI FIFO */
  Bit32u      init_enable;    /* initEnable value */
  Bit8u       stall_state;    /* state of the system if we're stalled */
  Bit16u      op_pending;     /* number of operations pending */
  Bit32u      fifo_mem[64*2]; /* memory backing the PCI FIFO */
};


typedef struct _ncc_table ncc_table;
struct _ncc_table
{
  bool        dirty;        /* is the texel lookup dirty? */
  voodoo_reg* reg;          /* pointer to our registers */
  Bit32s      ir[4], ig[4], ib[4];  /* I values for R,G,B */
  Bit32s      qr[4], qg[4], qb[4];  /* Q values for R,G,B */
  Bit32s      y[16];        /* Y values */
  rgb_t *     palette;      /* pointer to associated RGB palette */
  rgb_t *     palettea;     /* pointer to associated ARGB palette */
  rgb_t       texel[256];   /* texel lookup */
};


typedef struct _tmu_state tmu_state;
struct _tmu_state
{
  Bit8u*      ram;            /* pointer to our RAM */
  Bit32u      mask;           /* mask to apply to pointers */
  voodoo_reg* reg;            /* pointer to our register base */
  bool        regdirty;       /* true if the LOD/mode/base registers have changed */

  Bit32u      texaddr_mask;   /* mask for texture address */
  Bit8u       texaddr_shift;  /* shift for texture address */

  Bit64s      starts, startt; /* starting S,T (14.18) */
  Bit64s      startw;         /* starting W (2.30) */
  Bit64s      dsdx, dtdx;     /* delta S,T per X */
  Bit64s      dwdx;           /* delta W per X */
  Bit64s      dsdy, dtdy;     /* delta S,T per Y */
  Bit64s      dwdy;           /* delta W per Y */

  Bit32s      lodmin, lodmax; /* min, max LOD values */
  Bit32s      lodbias;        /* LOD bias */
  Bit32u      lodmask;        /* mask of available LODs */
  Bit32u      lodoffset[9];   /* offset of texture base for each LOD */
  Bit32s      detailmax;      /* detail clamp */
  Bit32s      detailbias;     /* detail bias */
  Bit8u       detailscale;    /* detail scale */

  Bit32u      wmask;          /* mask for the current texture width */
  Bit32u      hmask;          /* mask for the current texture height */

  Bit32u      bilinear_mask;  /* mask for bilinear resolution (0xf0 for V1, 0xff for V2) */

  ncc_table   ncc[2];         /* two NCC tables */

  rgb_t *     lookup;         /* currently selected lookup */
  rgb_t *     texel[16];      /* texel lookups for each format */

  rgb_t       palette[256];   /* palette lookup table */
  rgb_t       palettea[256];  /* palette+alpha lookup table */
};


typedef struct _tmu_shared_state tmu_shared_state;
struct _tmu_shared_state
{
  rgb_t rgb332[256];     /* RGB 3-3-2 lookup table */
  rgb_t alpha8[256];     /* alpha 8-bit lookup table */
  rgb_t int8[256];       /* intensity 8-bit lookup table */
  rgb_t ai44[256];       /* alpha, intensity 4-4 lookup table */

  rgb_t rgb565[65536];   /* RGB 5-6-5 lookup table */
  rgb_t argb1555[65536]; /* ARGB 1-5-5-5 lookup table */
  rgb_t argb4444[65536]; /* ARGB 4-4-4-4 lookup table */
};


typedef struct _setup_vertex setup_vertex;
struct _setup_vertex
{
  float x, y;       /* X, Y coordinates */
  float a, r, g, b; /* A, R, G, B values */
  float z, wb;      /* Z and broadcast W values */
  float w0, s0, t0; /* W, S, T for TMU 0 */
  float w1, s1, t1; /* W, S, T for TMU 1 */
};


typedef struct _fbi_state fbi_state;
struct _fbi_state
{
  Bit8u *       ram;            /* pointer to frame buffer RAM */
  Bit32u        mask;           /* mask to apply to pointers */
  Bit32u        rgboffs[3];     /* word offset to 3 RGB buffers */
  Bit32u        auxoffs;        /* word offset to 1 aux buffer */

  Bit8u         frontbuf;       /* front buffer index */
  Bit8u         backbuf;        /* back buffer index */
  Bit8u         swaps_pending;  /* number of pending swaps */
  bool          video_changed;  /* did the frontbuffer video change? */

  Bit32u        yorigin;        /* Y origin subtract value */
  Bit32u        lfb_base;       /* base of LFB in memory */
  Bit8u         lfb_stride;     /* stride of LFB accesses in bits */
  Bit16u        clip_mask;      /* non-reserved bits in clipLeftRight register */

  Bit32u        width;          /* width of current frame buffer */
  Bit32u        height;         /* height of current frame buffer */
  Bit32u        xoffs;          /* horizontal offset (back porch) */
  Bit32u        yoffs;          /* vertical offset (back porch) */
  Bit32u        vsyncscan;      /* vertical sync scanline */
  Bit32u        rowpixels;      /* pixels per row */
  Bit32u        tile_width;     /* width of video tiles */
  Bit32u        tile_height;    /* height of video tiles */
  Bit32u        x_tiles;        /* number of tiles in the X direction */

  Bit8u         vblank_count;   /* number of VBLANKs since last swap */
  bool          vblank_swap_pending; /* a swap is pending, waiting for a vblank */
  Bit8u         vblank_swap;    /* swap when we hit this count */
  Bit8u         vblank_dont_swap; /* don't actually swap when we hit this point */

  /* triangle setup info */
  bool          cheating_allowed; /* allow cheating? */
  Bit32s        sign;           /* triangle sign */
  Bit16s        ax, ay;         /* vertex A x,y (12.4) */
  Bit16s        bx, by;         /* vertex B x,y (12.4) */
  Bit16s        cx, cy;         /* vertex C x,y (12.4) */
  Bit32s        startr, startg, startb, starta; /* starting R,G,B,A (12.12) */
  Bit32s        startz;         /* starting Z (20.12) */
  Bit64s        startw;         /* starting W (16.32) */
  Bit32s        drdx, dgdx, dbdx, dadx; /* delta R,G,B,A per X */
  Bit32s        dzdx;           /* delta Z per X */
  Bit64s        dwdx;           /* delta W per X */
  Bit32s        drdy, dgdy, dbdy, dady; /* delta R,G,B,A per Y */
  Bit32s        dzdy;           /* delta Z per Y */
  Bit64s        dwdy;           /* delta W per Y */

  stats_block   lfb_stats;      /* LFB-access statistics */

  Bit8u         sverts;         /* number of vertices ready */
  setup_vertex  svert[3];       /* 3 setup vertices */

  fifo_state    fifo;           /* framebuffer memory fifo */
  cmdfifo_info  cmdfifo[2];     /* command FIFOs */

  Bit8u         fogblend[64];   /* 64-entry fog table */
  Bit8u         fogdelta[64];   /* 64-entry fog table */
  Bit8u         fogdelta_mask;  /* mask for for delta (0xff for V1, 0xfc for V2) */

  rgb_t         pen[65536];     /* mapping from pixels to pens */
  rgb_t         clut[512];      /* clut gamma data */
  bool          clut_dirty;     /* do we need to recompute? */
};


typedef struct _dac_state dac_state;
struct _dac_state
{
  Bit8u reg[8];       /* 8 registers */
  Bit8u read_result;  /* pending read result */
  Bit8u data_size;
  Bit8u clk0_m;
  Bit8u clk0_n;
  Bit8u clk0_p;
};


struct _poly_extra_data
{
  voodoo_state* state;        /* pointer back to the voodoo state */

  Bit16s        ax, ay;       /* vertex A x,y (12.4) */
  Bit32s        startr, startg, startb, starta; /* starting R,G,B,A (12.12) */
  Bit32s        startz;       /* starting Z (20.12) */
  Bit64s        startw;       /* starting W (16.32) */
  Bit32s        drdx, dgdx, dbdx, dadx; /* delta R,G,B,A per X */
  Bit32s        dzdx;         /* delta Z per X */
  Bit64s        dwdx;         /* delta W per X */
  Bit32s        drdy, dgdy, dbdy, dady; /* delta R,G,B,A per Y */
  Bit32s        dzdy;         /* delta Z per Y */
  Bit64s        dwdy;         /* delta W per Y */

  Bit64s        starts0, startt0; /* starting S,T (14.18) */
  Bit64s        startw0;      /* starting W (2.30) */
  Bit64s        ds0dx, dt0dx; /* delta S,T per X */
  Bit64s        dw0dx;        /* delta W per X */
  Bit64s        ds0dy, dt0dy; /* delta S,T per Y */
  Bit64s        dw0dy;        /* delta W per Y */
  Bit32s        lodbase0;     /* used during rasterization */

  Bit64s        starts1, startt1; /* starting S,T (14.18) */
  Bit64s        startw1;      /* starting W (2.30) */
  Bit64s        ds1dx, dt1dx; /* delta S,T per X */
  Bit64s        dw1dx;        /* delta W per X */
  Bit64s        ds1dy, dt1dy; /* delta S,T per Y */
  Bit64s        dw1dy;        /* delta W per Y */
  Bit32s        lodbase1;     /* used during rasterization */

  Bit16u        dither[16];   /* dither matrix, for fastfill */
};


#define BX_ROP_PATTERN 0x01

typedef struct _banshee_info banshee_info;
struct _banshee_info
{
  Bit32u io[0x40];   /* I/O registers */
  Bit32u agp[0x80];  /* AGP registers */
  Bit8u  crtc[0x27]; /* VGA CRTC registers */
  Bit8u  disp_bpp;
  bool half_mode;
  bool double_width;
  bool dac_8bit;
  bool desktop_tiled;
  bool overlay_tiled;
  struct {
    bool enabled;
    bool mode;
    Bit32u addr;
    Bit16u x;
    Bit16u y;
    Bit32u color[2];
  } hwcursor;
  struct {
    Bit32u reg[0x20];  /* 2D registers */
    Bit8u  cpat[0x40][4];
    bool   busy;
    Bit8u  cmd;
    bool   immed;
    bool   x_dir;
    bool   y_dir;
    bool   transp;
    Bit8u  patsx;
    Bit8u  patsy;
    bool   clip_sel;
    Bit8u  rop[4];
    bx_bitblt_rop_t rop_fn[4];
    bx_bitblt_rop_t rop_handler[2][0x100];
    Bit8u  rop_flags[0x100];
    bool   pattern_blt;
    Bit32u src_base;
    bool   src_tiled;
    Bit8u  src_fmt;
    Bit16u src_pitch;
    Bit8u  src_swizzle;
    Bit16s src_x;
    Bit16s src_y;
    Bit16u src_w;
    Bit16u src_h;
    Bit32u dst_base;
    bool   dst_tiled;
    Bit8u  dst_fmt;
    Bit16u dst_pitch;
    Bit16s dst_x;
    Bit16s dst_y;
    Bit16u dst_w;
    Bit16u dst_h;
    Bit8u  fgcolor[4];
    Bit8u  bgcolor[4];
    Bit16u clipx0[2];
    Bit16u clipy0[2];
    Bit16u clipx1[2];
    Bit16u clipy1[2];
    Bit16u h2s_pitch;
    Bit8u  h2s_pxstart;
    bool   pgn_init;
    Bit32u pgn_val;
    Bit16u pgn_l0x;
    Bit16u pgn_l0y;
    Bit16u pgn_l1x;
    Bit16u pgn_l1y;
    Bit16u pgn_r0x;
    Bit16u pgn_r0y;
    Bit16u pgn_r1x;
    Bit16u pgn_r1y;
    Bit32u lacnt;
    Bit32u laidx;
    Bit8u *lamem;
  } blt;
  struct {
    bool   enabled;
    bool   redraw;
    Bit8u  format;
    Bit32u start;
    Bit16u pitch;
    Bit16u x0;
    Bit16u y0;
    Bit16u x1;
    Bit16u y1;
    double fx;
    double fy;
  } overlay;
};


/* typedef struct _voodoo_state voodoo_state; -- declared above */
struct _voodoo_state
{
  Bit8u       index;         /* index of board */
  Bit8u       type;         /* type of system */
  Bit8u       chipmask;     /* mask for which chips are available */
  Bit32u      freq;         /* operating frequency */
  Bit32u      extra_cycles; /* extra cycles not yet accounted for */
  int         trigger;      /* trigger used for stalling */

  voodoo_reg  reg[0x400];   /* raw registers */
  const Bit8u *regaccess;   /* register access array */
  const char *const *regnames; /* register names array */
  Bit8u       alt_regmap;   /* enable alternate register map? */

  pci_state   pci;           /* PCI state */
  dac_state   dac;           /* DAC state */
  float       vidclk;        /* video clock */
  float       vertfreq;      /* vertical frequency */
  bool        vtimer_running; /* vertical timer running */

  fbi_state   fbi;           /* FBI states */
  tmu_state   tmu[MAX_TMU];  /* TMU states */
  tmu_shared_state tmushare; /* TMU shared state */
  banshee_info banshee;      /* Banshee state */

  struct {
    Bit8u   bgcolor[2];
    Bit8u   chroma_en;
    bool    clip_en;
    Bit16u  clipx0;
    Bit16u  clipx1;
    Bit16u  clipy0;
    Bit16u  clipy1;
    Bit16u  cur_x;
    Bit32u  dst_base;
    Bit16u  dst_h;
    Bit16u  dst_pitch;
    Bit16u  dst_w;
    Bit16u  dst_x;
    Bit16u  dst_y;
    Bit16u  dst_col_min;
    Bit16u  dst_col_max;
    Bit8u   fgcolor[2];
    bool    h2s_mode;
    Bit8u   rop[4];
    Bit16u  src_col_min;
    Bit16u  src_col_max;
    Bit8u   src_fmt;
    Bit8u   src_swizzle;
    bool    transp;
  } blt;

  Bit32u      send_config;
  Bit32u      tmu_config;

  stats_block *   thread_stats; /* per-thread statistics */

  Bit32u      last_status_pc;   /* PC of last status description (for logging) */
  Bit32u      last_status_value; /* value of last status read (for logging) */

};


// FIFO event handling

extern BX_MUTEX(fifo_mutex);
extern bx_thread_sem_t fifo_wakeup;
extern bx_thread_sem_t fifo_not_full;


/*************************************
 *
 *  Inline FIFO management
 *
 *************************************/

/* fifo content defines */
#define FIFO_TYPES  (7 << 29)
#define FIFO_WR_REG     (1U << 29)
#define FIFO_WR_TEX     (2U << 29)
#define FIFO_WR_FBI_32  (3U << 29)
#define FIFO_WR_FBI_16L (4U << 29)
#define FIFO_WR_FBI_16H (5U << 29)

BX_CPP_INLINE void fifo_reset(fifo_state *f)
{
  BX_LOCK(fifo_mutex);
  f->in = f->out = 0;
  bx_set_sem(&fifo_not_full);
  BX_UNLOCK(fifo_mutex);
}


BX_CPP_INLINE bool fifo_full(fifo_state *f)
{
  return (f->in + 2 == f->out || (f->in == f->size - 2 && f->out == 0));
}


BX_CPP_INLINE Bit32s fifo_items(fifo_state *f)
{
  Bit32s items = f->in - f->out;
  if (items < 0)
    items += f->size;
  return items;
}


BX_CPP_INLINE Bit32s fifo_space(fifo_state *f)
{
  Bit32s items = f->in - f->out;
  if (items < 0)
    items += f->size;
  Bit32s fspace = f->size - 1 - items;
  return fspace;
}


BX_CPP_INLINE void fifo_add(fifo_state *f, Bit32u offset, Bit32u data)
{
  Bit32s next_in;

  if (fifo_full(f)) {
    bx_set_sem(&fifo_wakeup);
    BX_UNLOCK(fifo_mutex);
    bx_wait_sem(&fifo_not_full);
    BX_LOCK(fifo_mutex);
  }
  /* compute the value of 'in' after we add this item */
  next_in = f->in + 2;
  if (next_in >= f->size)
    next_in = 0;

  /* as long as it's not equal to the output pointer, we can do it */
  if (next_in != f->out)
  {
    f->base[f->in] = offset;
    f->base[f->in + 1] = data;
    f->in = next_in;
  }
}


BX_CPP_INLINE Bit32u fifo_remove(fifo_state *f, Bit32u *offset, Bit32u *data)
{
  Bit32u type = 0;

  /* as long as we have data, we can do it */
  if (f->out != f->in)
  {
    Bit32s next_out;

    /* fetch the data */
    *offset = f->base[f->out];
    type = *offset & FIFO_TYPES;
    *offset &= 0xffffff;
    *data = f->base[f->out + 1];

    /* advance the output pointer */
    next_out = f->out + 2;
    if (next_out >= f->size)
      next_out = 0;
    f->out = next_out;
  }
  if (fifo_space(f) > 15) {
    bx_set_sem(&fifo_not_full);
  }
  return type;
}


BX_CPP_INLINE bool fifo_empty(fifo_state *f)
{
  return (f->in == f->out);
}


BX_CPP_INLINE bool fifo_empty_locked(fifo_state *f)
{
  BX_LOCK(fifo_mutex);
  bool ret = (f->in == f->out);
  BX_UNLOCK(fifo_mutex);
  return ret;
}


BX_CPP_INLINE void fifo_move(fifo_state *f1, fifo_state *f2)
{
  if (fifo_full(f2)) {
    bx_set_sem(&fifo_wakeup);
    BX_UNLOCK(fifo_mutex);
    bx_wait_sem(&fifo_not_full);
    BX_LOCK(fifo_mutex);
  }
  Bit32s items1 = fifo_items(f1);
  Bit32s space2 = fifo_space(f2);
  while ((items1 > 0) && (space2 > 0)) {
    f2->base[f2->in++] = f1->base[f1->out++];
    if (f2->in >= f2->size) f2->in = 0;
    if (f1->out >= f1->size) f1->out = 0;
    items1--;
    space2--;
  }
}


#ifndef PTR64
#define count_leading_zeros _count_leading_zeros
BX_CPP_INLINE Bit8u _count_leading_zeros(Bit32u value)
{
  Bit32s result = 32;

  while(value > 0) {
    result--;
    value >>= 1;
  }

  return result;
}
#endif

//-------------------------------------------------
//  fast_log2 - computes the log2 of a double-
//  precision value as a 24.8 value; if the double
//  was converted from a fixed-point integer, the
//  number of fractional bits should be specified
//  by fracbits
//-------------------------------------------------

BX_CPP_INLINE Bit32s fast_log2(double value, int fracbits = 0)
{
  extern Bit8u voodoo_log[];

  // negative values return 0
  if (unlikely(value < 0))
    return 0;

  // convert the value to a raw integer
  union { double d; Bit64u i; } temp;
  temp.d = value;

  // we only care about the 11-bit exponent and top 7 bits of mantissa
  // (sign is already assured to be 0)
  Bit32u ival = temp.i >> 45;

  // extract exponent, unbias, and adjust for fixed-point fraction
  Bit32s exp = (ival >> 7) - 1023 - fracbits;

  // use top 7 bits of mantissa to look up fractional log
  // combine the integral and fractional parts
  return (exp << 8) | voodoo_log[ival & 127];
}



/*************************************
 *
 *  Float-to-int conversions
 *
 *************************************/

BX_CPP_INLINE Bit32s float_to_Bit32s(Bit32u data, int fixedbits)
{
  int exponent = ((data >> 23) & 0xff) - 127 - 23 + fixedbits;
  Bit32s result = (data & 0x7fffff) | 0x800000;
  if (exponent < 0)
  {
    if (exponent > -32)
      result >>= -exponent;
    else
      result = 0;
  }
  else
  {
    if (exponent < 32)
      result <<= exponent;
    else
      result = 0x7fffffff;
  }
  if (data & 0x80000000)
    result = -result;
  return result;
}


BX_CPP_INLINE Bit64s float_to_Bit64s(Bit32u data, int fixedbits)
{
  int exponent = ((data >> 23) & 0xff) - 127 - 23 + fixedbits;
  Bit64s result = (data & 0x7fffff) | 0x800000;
  if (exponent < 0)
  {
    if (exponent > -64)
      result >>= -exponent;
    else
      result = 0;
  }
  else
  {
    if (exponent < 64)
      result <<= exponent;
    else
      result = BX_CONST64(0x7fffffffffffffff);
  }
  if (data & 0x80000000)
    result = -result;
  return result;
}



/*************************************
 *
 *  Rasterizer inlines
 *
 *************************************/

BX_CPP_INLINE Bit32u normalize_color_path(Bit32u eff_color_path)
{
  /* ignore the subpixel adjust and texture enable flags */
  eff_color_path &= ~((1 << 26) | (1 << 27));

  return eff_color_path;
}


BX_CPP_INLINE Bit32u normalize_alpha_mode(Bit32u eff_alpha_mode)
{
  /* always ignore alpha ref value */
  eff_alpha_mode &= ~(0xff << 24);

  /* if not doing alpha testing, ignore the alpha function and ref value */
  if (!ALPHAMODE_ALPHATEST(eff_alpha_mode))
    eff_alpha_mode &= ~(7 << 1);

  /* if not doing alpha blending, ignore the source and dest blending factors */
  if (!ALPHAMODE_ALPHABLEND(eff_alpha_mode))
    eff_alpha_mode &= ~((15 << 8) | (15 << 12) | (15 << 16) | (15 << 20));

  return eff_alpha_mode;
}


BX_CPP_INLINE Bit32u normalize_fog_mode(Bit32u eff_fog_mode)
{
  /* if not doing fogging, ignore all the other fog bits */
  if (!FOGMODE_ENABLE_FOG(eff_fog_mode))
    eff_fog_mode = 0;

  return eff_fog_mode;
}


BX_CPP_INLINE Bit32u normalize_fbz_mode(Bit32u eff_fbz_mode)
{
  /* ignore the draw buffer */
  eff_fbz_mode &= ~(3 << 14);

  return eff_fbz_mode;
}


BX_CPP_INLINE Bit32u normalize_tex_mode(Bit32u eff_tex_mode)
{
  /* ignore the NCC table and seq_8_downld flags */
  eff_tex_mode &= ~((1 << 5) | (1 << 31));

  /* classify texture formats into 3 format categories */
  if (TEXMODE_FORMAT(eff_tex_mode) < 8)
    eff_tex_mode = (eff_tex_mode & ~(0xf << 8)) | (0 << 8);
  else if (TEXMODE_FORMAT(eff_tex_mode) >= 10 && TEXMODE_FORMAT(eff_tex_mode) <= 12)
    eff_tex_mode = (eff_tex_mode & ~(0xf << 8)) | (10 << 8);
  else
    eff_tex_mode = (eff_tex_mode & ~(0xf << 8)) | (8 << 8);

  return eff_tex_mode;
}


/*************************************
 *
 *  Dithering macros
 *
 *************************************/

/* note that these equations and the dither matrixes have
   been confirmed to be exact matches to the real hardware */
#define DITHER_RB(val,dith) ((((val) << 1) - ((val) >> 4) + ((val) >> 7) + (dith)) >> 1)
#define DITHER_G(val,dith)  ((((val) << 2) - ((val) >> 4) + ((val) >> 6) + (dith)) >> 2)

#define DECLARE_DITHER_POINTERS                       \
  const Bit8u *dither_lookup = NULL;                  \
  const Bit8u *dither4 = NULL;                        \
  const Bit8u *dither = NULL                          \


#define COMPUTE_DITHER_POINTERS(FBZMODE, YY)              \
do                                                        \
{                                                         \
  /* compute the dithering pointers */                    \
  if (FBZMODE_ENABLE_DITHERING(FBZMODE))                  \
  {                                                       \
    dither4 = &dither_matrix_4x4[((YY) & 3) * 4];         \
    if (FBZMODE_DITHER_TYPE(FBZMODE) == 0)                \
    {                                                     \
      dither = dither4;                                   \
      dither_lookup = &dither4_lookup[(YY & 3) << 11];    \
    }                                                     \
    else                                                  \
    {                                                     \
      dither = &dither_matrix_2x2[((YY) & 3) * 4];        \
      dither_lookup = &dither2_lookup[(YY & 3) << 11];    \
    }                                                     \
  }                                                       \
}                                                         \
while (0)

#define APPLY_DITHER(FBZMODE, XX, DITHER_LOOKUP, RR, GG, BB)      \
do                                                                \
{                                                                 \
  /* apply dithering */                                           \
  if (FBZMODE_ENABLE_DITHERING(FBZMODE))                          \
  {                                                               \
    /* look up the dither value from the appropriate matrix */    \
    const Bit8u *dith = &DITHER_LOOKUP[((XX) & 3) << 1];          \
                                                                  \
    /* apply dithering to R,G,B */                                \
    (RR) = dith[((RR) << 3) + 0];                                 \
    (GG) = dith[((GG) << 3) + 1];                                 \
    (BB) = dith[((BB) << 3) + 0];                                 \
  }                                                               \
  else                                                            \
  {                                                               \
    (RR) >>= 3;                                                   \
    (GG) >>= 2;                                                   \
    (BB) >>= 3;                                                   \
  }                                                               \
}                                                                 \
while (0)



/*************************************
 *
 *  Clamping macros
 *
 *************************************/

#define CLAMPED_ARGB(ITERR, ITERG, ITERB, ITERA, FBZCP, RESULT)   \
do                                                                \
{                                                                 \
  Bit32s r = (Bit32s)(ITERR) >> 12;                               \
  Bit32s g = (Bit32s)(ITERG) >> 12;                               \
  Bit32s b = (Bit32s)(ITERB) >> 12;                               \
  Bit32s a = (Bit32s)(ITERA) >> 12;                               \
                                                                  \
  if (FBZCP_RGBZW_CLAMP(FBZCP) == 0)                              \
  {                                                               \
    r &= 0xfff;                                                   \
    RESULT.rgb.r = r;                                             \
    if (r == 0xfff)                                               \
      RESULT.rgb.r = 0;                                           \
    else if (r == 0x100)                                          \
      RESULT.rgb.r = 0xff;                                        \
                                                                  \
    g &= 0xfff;                                                   \
    RESULT.rgb.g = g;                                             \
    if (g == 0xfff)                                               \
      RESULT.rgb.g = 0;                                           \
    else if (g == 0x100)                                          \
      RESULT.rgb.g = 0xff;                                        \
                                                                  \
    b &= 0xfff;                                                   \
    RESULT.rgb.b = b;                                             \
    if (b == 0xfff)                                               \
      RESULT.rgb.b = 0;                                           \
    else if (b == 0x100)                                          \
      RESULT.rgb.b = 0xff;                                        \
                                                                  \
    a &= 0xfff;                                                   \
    RESULT.rgb.a = a;                                             \
    if (a == 0xfff)                                               \
      RESULT.rgb.a = 0;                                           \
    else if (a == 0x100)                                          \
      RESULT.rgb.a = 0xff;                                        \
  }                                                               \
  else                                                            \
  {                                                               \
    RESULT.rgb.r = (r < 0) ? 0 : (r > 0xff) ? 0xff : r;           \
    RESULT.rgb.g = (g < 0) ? 0 : (g > 0xff) ? 0xff : g;           \
    RESULT.rgb.b = (b < 0) ? 0 : (b > 0xff) ? 0xff : b;           \
    RESULT.rgb.a = (a < 0) ? 0 : (a > 0xff) ? 0xff : a;           \
  }                                                               \
}                                                                 \
while (0)


#define CLAMPED_Z(ITERZ, FBZCP, RESULT)                 \
do                                                      \
{                                                       \
  (RESULT) = (Bit32s)(ITERZ) >> 12;                     \
  if (FBZCP_RGBZW_CLAMP(FBZCP) == 0)                    \
  {                                                     \
    (RESULT) &= 0xfffff;                                \
    if ((RESULT) == 0xfffff)                            \
      (RESULT) = 0;                                     \
    else if ((RESULT) == 0x10000)                       \
      (RESULT) = 0xffff;                                \
    else                                                \
      (RESULT) &= 0xffff;                               \
  }                                                     \
  else                                                  \
  {                                                     \
    CLAMP((RESULT), 0, 0xffff);                         \
  }                                                     \
}                                                       \
while (0)


#define CLAMPED_W(ITERW, FBZCP, RESULT)                 \
do                                                      \
{                                                       \
  (RESULT) = (Bit16s)((ITERW) >> 32);                   \
  if (FBZCP_RGBZW_CLAMP(FBZCP) == 0)                    \
  {                                                     \
    (RESULT) &= 0xffff;                                 \
    if ((RESULT) == 0xffff)                             \
      (RESULT) = 0;                                     \
    else if ((RESULT) == 0x100)                         \
      (RESULT) = 0xff;                                  \
    (RESULT) &= 0xff;                                   \
  }                                                     \
  else                                                  \
  {                                                     \
    CLAMP((RESULT), 0, 0xff);                           \
  }                                                     \
}                                                       \
while (0)



/*************************************
 *
 *  Chroma keying macro
 *
 *************************************/

#define APPLY_CHROMAKEY(VV, STATS, FBZMODE, COLOR)                      \
do                                                                      \
{                                                                       \
  if (FBZMODE_ENABLE_CHROMAKEY(FBZMODE))                                \
  {                                                                     \
    /* non-range version */                                             \
    if (!CHROMARANGE_ENABLE((VV)->reg[chromaRange].u))                  \
    {                                                                   \
      if (((COLOR.u ^ (VV)->reg[chromaKey].u) & 0xffffff) == 0)         \
      {                                                                 \
        (STATS)->chroma_fail++;                                         \
        goto skipdrawdepth;                                             \
      }                                                                 \
    }                                                                   \
                                                                        \
    /* tricky range version */                                          \
    else                                                                \
    {                                                                   \
      Bit32s low, high, test;                                           \
      int results = 0;                                                  \
                                                                        \
      /* check blue */                                                  \
      low = (VV)->reg[chromaKey].rgb.b;                                 \
      high = (VV)->reg[chromaRange].rgb.b;                              \
      test = COLOR.rgb.b;                                               \
      results = (test >= low && test <= high);                          \
      results ^= CHROMARANGE_BLUE_EXCLUSIVE((VV)->reg[chromaRange].u);  \
      results <<= 1;                                                    \
                                                                        \
      /* check green */                                                 \
      low = (VV)->reg[chromaKey].rgb.g;                                 \
      high = (VV)->reg[chromaRange].rgb.g;                              \
      test = COLOR.rgb.g;                                               \
      results |= (test >= low && test <= high);                         \
      results ^= CHROMARANGE_GREEN_EXCLUSIVE((VV)->reg[chromaRange].u); \
      results <<= 1;                                                    \
                                                                        \
      /* check red */                                                   \
      low = (VV)->reg[chromaKey].rgb.r;                                 \
      high = (VV)->reg[chromaRange].rgb.r;                              \
      test = COLOR.rgb.r;                                               \
      results |= (test >= low && test <= high);                         \
      results ^= CHROMARANGE_RED_EXCLUSIVE((VV)->reg[chromaRange].u);   \
                                                                        \
      /* final result */                                                \
      if (CHROMARANGE_UNION_MODE((VV)->reg[chromaRange].u))             \
      {                                                                 \
        if (results != 0)                                               \
        {                                                               \
          (STATS)->chroma_fail++;                                       \
          goto skipdrawdepth;                                           \
        }                                                               \
      }                                                                 \
      else                                                              \
      {                                                                 \
        if (results == 7)                                               \
        {                                                               \
          (STATS)->chroma_fail++;                                       \
          goto skipdrawdepth;                                           \
        }                                                               \
      }                                                                 \
    }                                                                   \
  }                                                                     \
}                                                                       \
while (0)



/*************************************
 *
 *  Alpha masking macro
 *
 *************************************/

#define APPLY_ALPHAMASK(VV, STATS, FBZMODE, AA)                         \
do                                                                      \
{                                                                       \
  if (FBZMODE_ENABLE_ALPHA_MASK(FBZMODE))                               \
  {                                                                     \
    if (((AA) & 1) == 0)                                                \
    {                                                                   \
      (STATS)->afunc_fail++;                                            \
      goto skipdrawdepth;                                               \
    }                                                                   \
  }                                                                     \
}                                                                       \
while (0)



/*************************************
 *
 *  Alpha testing macro
 *
 *************************************/

#define APPLY_ALPHATEST(VV, STATS, ALPHAMODE, AA)                       \
do                                                                      \
{                                                                       \
  if (ALPHAMODE_ALPHATEST(ALPHAMODE))                                   \
  {                                                                     \
    Bit8u alpharef = (VV)->reg[alphaMode].rgb.a;                        \
    switch (ALPHAMODE_ALPHAFUNCTION(ALPHAMODE))                         \
    {                                                                   \
      case 0:   /* alphaOP = never */                                   \
        (STATS)->afunc_fail++;                                          \
        goto skipdrawdepth;                                             \
                                                                        \
      case 1:   /* alphaOP = less than */                               \
        if ((AA) >= alpharef)                                           \
        {                                                               \
          (STATS)->afunc_fail++;                                        \
          goto skipdrawdepth;                                           \
        }                                                               \
        break;                                                          \
                                                                        \
      case 2:   /* alphaOP = equal */                                   \
        if ((AA) != alpharef)                                           \
        {                                                               \
          (STATS)->afunc_fail++;                                        \
          goto skipdrawdepth;                                           \
        }                                                               \
        break;                                                          \
                                                                        \
      case 3:   /* alphaOP = less than or equal */                      \
        if ((AA) > alpharef)                                            \
        {                                                               \
          (STATS)->afunc_fail++;                                        \
          goto skipdrawdepth;                                           \
        }                                                               \
        break;                                                          \
                                                                        \
      case 4:   /* alphaOP = greater than */                            \
        if ((AA) <= alpharef)                                           \
        {                                                               \
          (STATS)->afunc_fail++;                                        \
          goto skipdrawdepth;                                           \
        }                                                               \
        break;                                                          \
                                                                        \
      case 5:   /* alphaOP = not equal */                               \
        if ((AA) == alpharef)                                           \
        {                                                               \
          (STATS)->afunc_fail++;                                        \
          goto skipdrawdepth;                                           \
        }                                                               \
        break;                                                          \
                                                                        \
      case 6:   /* alphaOP = greater than or equal */                   \
        if ((AA) < alpharef)                                            \
        {                                                               \
          (STATS)->afunc_fail++;                                        \
          goto skipdrawdepth;                                           \
        }                                                               \
        break;                                                          \
                                                                        \
      case 7:   /* alphaOP = always */                                  \
        break;                                                          \
    }                                                                   \
  }                                                                     \
}                                                                       \
while (0)



/*************************************
 *
 *  Alpha blending macro
 *
 *************************************/

#define APPLY_ALPHA_BLEND(FBZMODE, ALPHAMODE, XX, DITHER, RR, GG, BB, AA)        \
do                                                                               \
{                                                                                \
  if (ALPHAMODE_ALPHABLEND(ALPHAMODE))                                           \
  {                                                                              \
    int dpix = dest[XX];                                                         \
    int dr = (dpix >> 8) & 0xf8;                                                 \
    int dg = (dpix >> 3) & 0xfc;                                                 \
    int db = (dpix << 3) & 0xf8;                                                 \
    int da = FBZMODE_ENABLE_ALPHA_PLANES(FBZMODE) ? depth[XX] : 0xff;            \
    int sr = (RR);                                                               \
    int sg = (GG);                                                               \
    int sb = (BB);                                                               \
    int sa = (AA);                                                               \
    int ta;                                                                      \
                                                                                 \
    /* apply dither subtraction */                                               \
    if (FBZMODE_ALPHA_DITHER_SUBTRACT(FBZMODE))                                  \
    {                                                                            \
      /* look up the dither value from the appropriate matrix */                 \
      int dith = DITHER[(XX) & 3];                                               \
                                                                                 \
      /* subtract the dither value */                                            \
      dr = ((dr << 1) + 15 - dith) >> 1;                                         \
      dg = ((dg << 2) + 15 - dith) >> 2;                                         \
      db = ((db << 1) + 15 - dith) >> 1;                                         \
    }                                                                            \
                                                                                 \
    /* compute source portion */                                                 \
    switch (ALPHAMODE_SRCRGBBLEND(ALPHAMODE))                                    \
    {                                                                            \
      default:  /* reserved */                                                   \
      case 0:   /* AZERO */                                                      \
        (RR) = (GG) = (BB) = 0;                                                  \
        break;                                                                   \
                                                                                 \
      case 1:   /* ASRC_ALPHA */                                                 \
        (RR) = (sr * (sa + 1)) >> 8;                                             \
        (GG) = (sg * (sa + 1)) >> 8;                                             \
        (BB) = (sb * (sa + 1)) >> 8;                                             \
        break;                                                                   \
                                                                                 \
      case 2:   /* A_COLOR */                                                    \
        (RR) = (sr * (dr + 1)) >> 8;                                             \
        (GG) = (sg * (dg + 1)) >> 8;                                             \
        (BB) = (sb * (db + 1)) >> 8;                                             \
        break;                                                                   \
                                                                                 \
      case 3:   /* ADST_ALPHA */                                                 \
        (RR) = (sr * (da + 1)) >> 8;                                             \
        (GG) = (sg * (da + 1)) >> 8;                                             \
        (BB) = (sb * (da + 1)) >> 8;                                             \
        break;                                                                   \
                                                                                 \
      case 4:   /* AONE */                                                       \
        break;                                                                   \
                                                                                 \
      case 5:   /* AOMSRC_ALPHA */                                               \
        (RR) = (sr * (0x100 - sa)) >> 8;                                         \
        (GG) = (sg * (0x100 - sa)) >> 8;                                         \
        (BB) = (sb * (0x100 - sa)) >> 8;                                         \
        break;                                                                   \
                                                                                 \
      case 6:   /* AOM_COLOR */                                                  \
        (RR) = (sr * (0x100 - dr)) >> 8;                                         \
        (GG) = (sg * (0x100 - dg)) >> 8;                                         \
        (BB) = (sb * (0x100 - db)) >> 8;                                         \
        break;                                                                   \
                                                                                 \
      case 7:   /* AOMDST_ALPHA */                                               \
        (RR) = (sr * (0x100 - da)) >> 8;                                         \
        (GG) = (sg * (0x100 - da)) >> 8;                                         \
        (BB) = (sb * (0x100 - da)) >> 8;                                         \
        break;                                                                   \
                                                                                 \
      case 15:  /* ASATURATE */                                                  \
        ta = (sa < (0x100 - da)) ? sa : (0x100 - da);                            \
        (RR) = (sr * (ta + 1)) >> 8;                                             \
        (GG) = (sg * (ta + 1)) >> 8;                                             \
        (BB) = (sb * (ta + 1)) >> 8;                                             \
        break;                                                                   \
    }                                                                            \
                                                                                 \
    /* add in dest portion */                                                    \
    switch (ALPHAMODE_DSTRGBBLEND(ALPHAMODE))                                    \
    {                                                                            \
      default:  /* reserved */                                                   \
      case 0:   /* AZERO */                                                      \
        break;                                                                   \
                                                                                 \
      case 1:   /* ASRC_ALPHA */                                                 \
        (RR) += (dr * (sa + 1)) >> 8;                                            \
        (GG) += (dg * (sa + 1)) >> 8;                                            \
        (BB) += (db * (sa + 1)) >> 8;                                            \
        break;                                                                   \
                                                                                 \
      case 2:   /* A_COLOR */                                                    \
        (RR) += (dr * (sr + 1)) >> 8;                                            \
        (GG) += (dg * (sg + 1)) >> 8;                                            \
        (BB) += (db * (sb + 1)) >> 8;                                            \
        break;                                                                   \
                                                                                 \
      case 3:   /* ADST_ALPHA */                                                 \
        (RR) += (dr * (da + 1)) >> 8;                                            \
        (GG) += (dg * (da + 1)) >> 8;                                            \
        (BB) += (db * (da + 1)) >> 8;                                            \
        break;                                                                   \
                                                                                 \
      case 4:   /* AONE */                                                       \
        (RR) += dr;                                                              \
        (GG) += dg;                                                              \
        (BB) += db;                                                              \
        break;                                                                   \
                                                                                 \
      case 5:   /* AOMSRC_ALPHA */                                               \
        (RR) += (dr * (0x100 - sa)) >> 8;                                        \
        (GG) += (dg * (0x100 - sa)) >> 8;                                        \
        (BB) += (db * (0x100 - sa)) >> 8;                                        \
        break;                                                                   \
                                                                                 \
      case 6:   /* AOM_COLOR */                                                  \
        (RR) += (dr * (0x100 - sr)) >> 8;                                        \
        (GG) += (dg * (0x100 - sg)) >> 8;                                        \
        (BB) += (db * (0x100 - sb)) >> 8;                                        \
        break;                                                                   \
                                                                                 \
      case 7:   /* AOMDST_ALPHA */                                               \
        (RR) += (dr * (0x100 - da)) >> 8;                                        \
        (GG) += (dg * (0x100 - da)) >> 8;                                        \
        (BB) += (db * (0x100 - da)) >> 8;                                        \
        break;                                                                   \
                                                                                 \
      case 15:  /* A_COLORBEFOREFOG */                                           \
        (RR) += (dr * (prefogr + 1)) >> 8;                                       \
        (GG) += (dg * (prefogg + 1)) >> 8;                                       \
        (BB) += (db * (prefogb + 1)) >> 8;                                       \
        break;                                                                   \
    }                                                                            \
                                                                                 \
    /* blend the source alpha */                                                 \
    (AA) = 0;                                                                    \
    if (ALPHAMODE_SRCALPHABLEND(ALPHAMODE) == 4)                                 \
      (AA) = sa;                                                                 \
                                                                                 \
    /* blend the dest alpha */                                                   \
    if (ALPHAMODE_DSTALPHABLEND(ALPHAMODE) == 4)                                 \
      (AA) += da;                                                                \
                                                                                 \
    /* clamp */                                                                  \
    CLAMP((RR), 0x00, 0xff);                                                     \
    CLAMP((GG), 0x00, 0xff);                                                     \
    CLAMP((BB), 0x00, 0xff);                                                     \
    CLAMP((AA), 0x00, 0xff);                                                     \
  }                                                                              \
}                                                                                \
while (0)



/*************************************
 *
 *  Fogging macro
 *
 *************************************/

#define APPLY_FOGGING(VV, FOGMODE, FBZCP, XX, DITHER4, RR, GG, BB, ITERZ, ITERW, ITERAXXX) \
do                                                                               \
{                                                                                \
  if (FOGMODE_ENABLE_FOG(FOGMODE))                                               \
  {                                                                              \
    rgb_union fogcolor = (VV)->reg[fogColor];                                    \
    Bit32s fr, fg, fb;                                                           \
                                                                                 \
    /* constant fog bypasses everything else */                                  \
    if (FOGMODE_FOG_CONSTANT(FOGMODE))                                           \
    {                                                                            \
      fr = fogcolor.rgb.r;                                                       \
      fg = fogcolor.rgb.g;                                                       \
      fb = fogcolor.rgb.b;                                                       \
    }                                                                            \
    /* non-constant fog comes from several sources */                            \
    else                                                                         \
    {                                                                            \
      Bit32s fogblend = 0;                                                       \
                                                                                 \
      /* if fog_add is zero, we start with the fog color */                      \
      if (FOGMODE_FOG_ADD(FOGMODE) == 0)                                         \
      {                                                                          \
        fr = fogcolor.rgb.r;                                                     \
        fg = fogcolor.rgb.g;                                                     \
        fb = fogcolor.rgb.b;                                                     \
      }                                                                          \
      else                                                                       \
        fr = fg = fb = 0;                                                        \
                                                                                 \
      /* if fog_mult is zero, we subtract the incoming color */                  \
      if (FOGMODE_FOG_MULT(FOGMODE) == 0)                                        \
      {                                                                          \
        fr -= (RR);                                                              \
        fg -= (GG);                                                              \
        fb -= (BB);                                                              \
      }                                                                          \
                                                                                 \
      /* fog blending mode */                                                    \
      switch (FOGMODE_FOG_ZALPHA(FOGMODE))                                       \
      {                                                                          \
        case 0:   /* fog table */                                                \
        {                                                                        \
          Bit32s delta = (VV)->fbi.fogdelta[wfloat >> 10];                       \
          Bit32s deltaval;                                                       \
                                                                                 \
          /* perform the multiply against lower 8 bits of wfloat */              \
          deltaval = (delta & (VV)->fbi.fogdelta_mask) *                         \
                ((wfloat >> 2) & 0xff);                                          \
                                                                                 \
          /* fog zones allow for negating this value */                          \
          if (FOGMODE_FOG_ZONES(FOGMODE) && (delta & 2))                         \
            deltaval = -deltaval;                                                \
          deltaval >>= 6;                                                        \
                                                                                 \
          /* apply dither */                                                     \
          if (FOGMODE_FOG_DITHER(FOGMODE))                                       \
            deltaval += DITHER4[(XX) & 3];                                       \
          deltaval >>= 4;                                                        \
                                                                                 \
          /* add to the blending factor */                                       \
          fogblend = (VV)->fbi.fogblend[wfloat >> 10] + deltaval;                \
          break;                                                                 \
        }                                                                        \
                                                                                 \
        case 1:   /* iterated A */                                               \
          fogblend = ITERAXXX.rgb.a;                                             \
          break;                                                                 \
                                                                                 \
        case 2:   /* iterated Z */                                               \
          CLAMPED_Z((ITERZ), FBZCP, fogblend);                                   \
          fogblend >>= 8;                                                        \
          break;                                                                 \
                                                                                 \
        case 3:   /* iterated W - Voodoo 2 only */                               \
          CLAMPED_W((ITERW), FBZCP, fogblend);                                   \
          break;                                                                 \
      }                                                                          \
                                                                                 \
      /* perform the blend */                                                    \
      fogblend++;                                                                \
      fr = (fr * fogblend) >> 8;                                                 \
      fg = (fg * fogblend) >> 8;                                                 \
      fb = (fb * fogblend) >> 8;                                                 \
    }                                                                            \
                                                                                 \
    /* if fog_mult is 0, we add this to the original color */                    \
    if (FOGMODE_FOG_MULT(FOGMODE) == 0)                                          \
    {                                                                            \
      (RR) += fr;                                                                \
      (GG) += fg;                                                                \
      (BB) += fb;                                                                \
    }                                                                            \
    /* otherwise this just becomes the new color */                              \
    else                                                                         \
    {                                                                            \
      (RR) = fr;                                                                 \
      (GG) = fg;                                                                 \
      (BB) = fb;                                                                 \
    }                                                                            \
                                                                                 \
    /* clamp */                                                                  \
    CLAMP((RR), 0x00, 0xff);                                                     \
    CLAMP((GG), 0x00, 0xff);                                                     \
    CLAMP((BB), 0x00, 0xff);                                                     \
  }                                                                              \
}                                                                                \
while (0)



/*************************************
 *
 *  Texture pipeline macro
 *
 *************************************/

#define TEXTURE_PIPELINE(TT, XX, DITHER4, TEXMODE, COTHER, LOOKUP, LODBASE, ITERS, ITERT, ITERW, RESULT) \
do                                                                               \
{                                                                                \
  Bit32s blendr, blendg, blendb, blenda;                                         \
  Bit32s tr, tg, tb, ta;                                                         \
  Bit32s s, t, lod, ilod;                                                        \
  double sf, tf, wf;                                                             \
  double ds1f, dt1f, ds2f, dt2f, rho, rhox, rhoy;                                \
  Bit32s smax, tmax;                                                             \
  Bit32u texbase;                                                                \
  rgb_union c_local;                                                             \
                                                                                 \
  /* determine the S/T/LOD values for this texture */                            \
  if (TEXMODE_ENABLE_PERSPECTIVE(TEXMODE))                                       \
  {                                                                              \
    wf = 1.0 / (ITERW);                                                          \
    sf = wf * (ITERS);                                                           \
    tf = wf * (ITERT);                                                           \
    s = (Bit32s)(Bit64s)(sf * 262144);                                           \
    t = (Bit32s)(Bit64s)(tf * 262144);                                           \
    ds1f = wf * ((TT)->dsdx - sf * (TT)->dwdx);                                  \
    dt1f = wf * ((TT)->dtdx - tf * (TT)->dwdx);                                  \
    rhox = ds1f * ds1f + dt1f * dt1f;                                            \
    ds2f = wf * ((TT)->dsdy - sf * (TT)->dwdy);                                  \
    dt2f = wf * ((TT)->dtdy - tf * (TT)->dwdy);                                  \
    rhoy = ds2f * ds2f + dt2f * dt2f;                                            \
    rho = BX_MAX(rhox, rhoy);                                                    \
    lod = fast_log2(rho) / 2;                                                    \
  }                                                                              \
  else                                                                           \
  {                                                                              \
    s = (Bit32s)((ITERS) >> 14);                                                 \
    t = (Bit32s)((ITERT) >> 14);                                                 \
    lod = (LODBASE);                                                             \
  }                                                                              \
                                                                                 \
  /* clamp W */                                                                  \
  if (TEXMODE_CLAMP_NEG_W(TEXMODE) && (ITERW) < 0)                               \
    s = t = 0;                                                                   \
                                                                                 \
  /* clamp the LOD */                                                            \
  lod += (TT)->lodbias;                                                          \
  if (TEXMODE_ENABLE_LOD_DITHER(TEXMODE))                                        \
    lod += DITHER4[(XX) & 3] << 4;                                               \
  if (lod < (TT)->lodmin)                                                        \
    lod = (TT)->lodmin;                                                          \
  if (lod > (TT)->lodmax)                                                        \
    lod = (TT)->lodmax;                                                          \
                                                                                 \
  /* now the LOD is in range; if we don't own this LOD, take the next one */     \
  ilod = lod >> 8;                                                               \
  if (!(((TT)->lodmask >> ilod) & 1))                                            \
    ilod++;                                                                      \
                                                                                 \
  /* fetch the texture base */                                                   \
  texbase = (TT)->lodoffset[ilod];                                               \
                                                                                 \
  /* compute the maximum s and t values at this LOD */                           \
  smax = (TT)->wmask >> ilod;                                                    \
  tmax = (TT)->hmask >> ilod;                                                    \
                                                                                 \
  /* determine whether we are point-sampled or bilinear */                       \
  if ((lod == (TT)->lodmin && !TEXMODE_MAGNIFICATION_FILTER(TEXMODE)) ||         \
    (lod != (TT)->lodmin && !TEXMODE_MINIFICATION_FILTER(TEXMODE)))              \
  {                                                                              \
    /* point sampled */                                                          \
                                                                                 \
    Bit32u texel0;                                                               \
                                                                                 \
    /* adjust S/T for the LOD and strip off the fractions */                     \
    s >>= ilod + 18;                                                             \
    t >>= ilod + 18;                                                             \
                                                                                 \
    /* clamp/wrap S/T if necessary */                                            \
    if (TEXMODE_CLAMP_S(TEXMODE))                                                \
      CLAMP(s, 0, smax);                                                         \
    if (TEXMODE_CLAMP_T(TEXMODE))                                                \
      CLAMP(t, 0, tmax);                                                         \
    s &= smax;                                                                   \
    t &= tmax;                                                                   \
    t *= smax + 1;                                                               \
                                                                                 \
    /* fetch texel data */                                                       \
    if (TEXMODE_FORMAT(TEXMODE) < 8)                                             \
    {                                                                            \
      texel0 = *(Bit8u *)&(TT)->ram[(texbase + t + s) & (TT)->mask];             \
      c_local.u = (LOOKUP)[texel0];                                              \
    }                                                                            \
    else                                                                         \
    {                                                                            \
      texel0 = *(Bit16u *)&(TT)->ram[(texbase + 2*(t + s)) & (TT)->mask];        \
      if (TEXMODE_FORMAT(TEXMODE) >= 10 && TEXMODE_FORMAT(TEXMODE) <= 12)        \
        c_local.u = (LOOKUP)[texel0];                                            \
      else                                                                       \
        c_local.u = ((LOOKUP)[texel0 & 0xff] & 0xffffff) |                       \
              ((texel0 & 0xff00) << 16);                                         \
    }                                                                            \
  }                                                                              \
  else                                                                           \
  {                                                                              \
    /* bilinear filtered */                                                      \
                                                                                 \
    Bit32u texel0, texel1, texel2, texel3;                                       \
    Bit32u sfrac, tfrac;                                                         \
    Bit32s s1, t1;                                                               \
                                                                                 \
    /* adjust S/T for the LOD and strip off all but the low 8 bits of */         \
    /* the fraction */                                                           \
    s >>= ilod + 10;                                                             \
    t >>= ilod + 10;                                                             \
                                                                                 \
    /* also subtract 1/2 texel so that (0.5,0.5) = a full (0,0) texel */         \
    s -= 0x80;                                                                   \
    t -= 0x80;                                                                   \
                                                                                 \
    /* extract the fractions */                                                  \
    sfrac = s & (TT)->bilinear_mask;                                             \
    tfrac = t & (TT)->bilinear_mask;                                             \
                                                                                 \
    /* now toss the rest */                                                      \
    s >>= 8;                                                                     \
    t >>= 8;                                                                     \
    s1 = s + 1;                                                                  \
    t1 = t + 1;                                                                  \
                                                                                 \
    /* clamp/wrap S/T if necessary */                                            \
    if (TEXMODE_CLAMP_S(TEXMODE))                                                \
    {                                                                            \
      CLAMP(s, 0, smax);                                                         \
      CLAMP(s1, 0, smax);                                                        \
    }                                                                            \
    if (TEXMODE_CLAMP_T(TEXMODE))                                                \
    {                                                                            \
      CLAMP(t, 0, tmax);                                                         \
      CLAMP(t1, 0, tmax);                                                        \
    }                                                                            \
    s &= smax;                                                                   \
    s1 &= smax;                                                                  \
    t &= tmax;                                                                   \
    t1 &= tmax;                                                                  \
    t *= smax + 1;                                                               \
    t1 *= smax + 1;                                                              \
                                                                                 \
    /* fetch texel data */                                                       \
    if (TEXMODE_FORMAT(TEXMODE) < 8)                                             \
    {                                                                            \
      texel0 = *(Bit8u *)&(TT)->ram[(texbase + t + s) & (TT)->mask];             \
      texel1 = *(Bit8u *)&(TT)->ram[(texbase + t + s1) & (TT)->mask];            \
      texel2 = *(Bit8u *)&(TT)->ram[(texbase + t1 + s) & (TT)->mask];            \
      texel3 = *(Bit8u *)&(TT)->ram[(texbase + t1 + s1) & (TT)->mask];           \
      texel0 = (LOOKUP)[texel0];                                                 \
      texel1 = (LOOKUP)[texel1];                                                 \
      texel2 = (LOOKUP)[texel2];                                                 \
      texel3 = (LOOKUP)[texel3];                                                 \
    }                                                                            \
    else                                                                         \
    {                                                                            \
      texel0 = *(Bit16u *)&(TT)->ram[(texbase + 2*(t + s)) & (TT)->mask];        \
      texel1 = *(Bit16u *)&(TT)->ram[(texbase + 2*(t + s1)) & (TT)->mask];       \
      texel2 = *(Bit16u *)&(TT)->ram[(texbase + 2*(t1 + s)) & (TT)->mask];       \
      texel3 = *(Bit16u *)&(TT)->ram[(texbase + 2*(t1 + s1)) & (TT)->mask];      \
      if (TEXMODE_FORMAT(TEXMODE) >= 10 && TEXMODE_FORMAT(TEXMODE) <= 12)        \
      {                                                                          \
        texel0 = (LOOKUP)[texel0];                                               \
        texel1 = (LOOKUP)[texel1];                                               \
        texel2 = (LOOKUP)[texel2];                                               \
        texel3 = (LOOKUP)[texel3];                                               \
      }                                                                          \
      else                                                                       \
      {                                                                          \
        texel0 = ((LOOKUP)[texel0 & 0xff] & 0xffffff) |                          \
              ((texel0 & 0xff00) << 16);                                         \
        texel1 = ((LOOKUP)[texel1 & 0xff] & 0xffffff) |                          \
              ((texel1 & 0xff00) << 16);                                         \
        texel2 = ((LOOKUP)[texel2 & 0xff] & 0xffffff) |                          \
              ((texel2 & 0xff00) << 16);                                         \
        texel3 = ((LOOKUP)[texel3 & 0xff] & 0xffffff) |                          \
              ((texel3 & 0xff00) << 16);                                         \
      }                                                                          \
    }                                                                            \
                                                                                 \
    /* weigh in each texel */                                                    \
    c_local.u = rgba_bilinear_filter(texel0, texel1, texel2, texel3, sfrac, tfrac); \
  }                                                                              \
                                                                                 \
  /* select zero/other for RGB */                                                \
  if (!TEXMODE_TC_ZERO_OTHER(TEXMODE))                                           \
  {                                                                              \
    tr = COTHER.rgb.r;                                                           \
    tg = COTHER.rgb.g;                                                           \
    tb = COTHER.rgb.b;                                                           \
  }                                                                              \
  else                                                                           \
    tr = tg = tb = 0;                                                            \
                                                                                 \
  /* select zero/other for alpha */                                              \
  if (!TEXMODE_TCA_ZERO_OTHER(TEXMODE))                                          \
    ta = COTHER.rgb.a;                                                           \
  else                                                                           \
    ta = 0;                                                                      \
                                                                                 \
  /* potentially subtract c_local */                                             \
  if (TEXMODE_TC_SUB_CLOCAL(TEXMODE))                                            \
  {                                                                              \
    tr -= c_local.rgb.r;                                                         \
    tg -= c_local.rgb.g;                                                         \
    tb -= c_local.rgb.b;                                                         \
  }                                                                              \
  if (TEXMODE_TCA_SUB_CLOCAL(TEXMODE))                                           \
    ta -= c_local.rgb.a;                                                         \
                                                                                 \
  /* blend RGB */                                                                \
  switch (TEXMODE_TC_MSELECT(TEXMODE))                                           \
  {                                                                              \
    default:  /* reserved */                                                     \
    case 0:   /* zero */                                                         \
      blendr = blendg = blendb = 0;                                              \
      break;                                                                     \
                                                                                 \
    case 1:   /* c_local */                                                      \
      blendr = c_local.rgb.r;                                                    \
      blendg = c_local.rgb.g;                                                    \
      blendb = c_local.rgb.b;                                                    \
      break;                                                                     \
                                                                                 \
    case 2:   /* a_other */                                                      \
      blendr = blendg = blendb = COTHER.rgb.a;                                   \
      break;                                                                     \
                                                                                 \
    case 3:   /* a_local */                                                      \
      blendr = blendg = blendb = c_local.rgb.a;                                  \
      break;                                                                     \
                                                                                 \
    case 4:   /* LOD (detail factor) */                                          \
      if ((TT)->detailbias <= lod)                                               \
        blendr = blendg = blendb = 0;                                            \
      else                                                                       \
      {                                                                          \
        blendr = ((((TT)->detailbias - lod) << (TT)->detailscale) >> 8);         \
        if (blendr > (TT)->detailmax)                                            \
          blendr = (TT)->detailmax;                                              \
        blendg = blendb = blendr;                                                \
      }                                                                          \
      break;                                                                     \
                                                                                 \
    case 5:   /* LOD fraction */                                                 \
      blendr = blendg = blendb = lod & 0xff;                                     \
      break;                                                                     \
  }                                                                              \
                                                                                 \
  /* blend alpha */                                                              \
  switch (TEXMODE_TCA_MSELECT(TEXMODE))                                          \
  {                                                                              \
    default:  /* reserved */                                                     \
    case 0:   /* zero */                                                         \
      blenda = 0;                                                                \
      break;                                                                     \
                                                                                 \
    case 1:   /* c_local */                                                      \
      blenda = c_local.rgb.a;                                                    \
      break;                                                                     \
                                                                                 \
    case 2:   /* a_other */                                                      \
      blenda = COTHER.rgb.a;                                                     \
      break;                                                                     \
                                                                                 \
    case 3:   /* a_local */                                                      \
      blenda = c_local.rgb.a;                                                    \
      break;                                                                     \
                                                                                 \
    case 4:   /* LOD (detail factor) */                                          \
      if ((TT)->detailbias <= lod)                                               \
        blenda = 0;                                                              \
      else                                                                       \
      {                                                                          \
        blenda = ((((TT)->detailbias - lod) << (TT)->detailscale) >> 8);         \
        if (blenda > (TT)->detailmax)                                            \
          blenda = (TT)->detailmax;                                              \
      }                                                                          \
      break;                                                                     \
                                                                                 \
    case 5:   /* LOD fraction */                                                 \
      blenda = lod & 0xff;                                                       \
      break;                                                                     \
  }                                                                              \
                                                                                 \
  /* reverse the RGB blend */                                                    \
  if (!TEXMODE_TC_REVERSE_BLEND(TEXMODE))                                        \
  {                                                                              \
    blendr ^= 0xff;                                                              \
    blendg ^= 0xff;                                                              \
    blendb ^= 0xff;                                                              \
  }                                                                              \
                                                                                 \
  /* reverse the alpha blend */                                                  \
  if (!TEXMODE_TCA_REVERSE_BLEND(TEXMODE))                                       \
    blenda ^= 0xff;                                                              \
                                                                                 \
  /* do the blend */                                                             \
  tr = (tr * (blendr + 1)) >> 8;                                                 \
  tg = (tg * (blendg + 1)) >> 8;                                                 \
  tb = (tb * (blendb + 1)) >> 8;                                                 \
  ta = (ta * (blenda + 1)) >> 8;                                                 \
                                                                                 \
  /* add clocal or alocal to RGB */                                              \
  switch (TEXMODE_TC_ADD_ACLOCAL(TEXMODE))                                       \
  {                                                                              \
    case 3:   /* reserved */                                                     \
    case 0:   /* nothing */                                                      \
      break;                                                                     \
                                                                                 \
    case 1:   /* add c_local */                                                  \
      tr += c_local.rgb.r;                                                       \
      tg += c_local.rgb.g;                                                       \
      tb += c_local.rgb.b;                                                       \
      break;                                                                     \
                                                                                 \
    case 2:   /* add_alocal */                                                   \
      tr += c_local.rgb.a;                                                       \
      tg += c_local.rgb.a;                                                       \
      tb += c_local.rgb.a;                                                       \
      break;                                                                     \
  }                                                                              \
                                                                                 \
  /* add clocal or alocal to alpha */                                            \
  if (TEXMODE_TCA_ADD_ACLOCAL(TEXMODE))                                          \
    ta += c_local.rgb.a;                                                         \
                                                                                 \
  /* clamp */                                                                    \
  RESULT.rgb.r = (tr < 0) ? 0 : (tr > 0xff) ? 0xff : tr;                         \
  RESULT.rgb.g = (tg < 0) ? 0 : (tg > 0xff) ? 0xff : tg;                         \
  RESULT.rgb.b = (tb < 0) ? 0 : (tb > 0xff) ? 0xff : tb;                         \
  RESULT.rgb.a = (ta < 0) ? 0 : (ta > 0xff) ? 0xff : ta;                         \
                                                                                 \
  /* invert */                                                                   \
  if (TEXMODE_TC_INVERT_OUTPUT(TEXMODE))                                         \
    RESULT.u ^= 0x00ffffff;                                                      \
  if (TEXMODE_TCA_INVERT_OUTPUT(TEXMODE))                                        \
    RESULT.rgb.a ^= 0xff;                                                        \
}                                                                                \
while (0)



/*************************************
 *
 *  Pixel pipeline macros
 *
 *************************************/

#define PIXEL_PIPELINE_BEGIN(VV, STATS, XX, YY, FBZCOLORPATH, FBZMODE, ITERZ, ITERW)  \
do                                                                               \
{                                                                                \
  Bit32s depthval, wfloat;                                                       \
  Bit32s prefogr, prefogg, prefogb;                                              \
  Bit32s r, g, b, a;                                                             \
                                                                                 \
  (STATS)->pixels_in++;                                                          \
                                                                                 \
  /* apply clipping */                                                           \
  /* note that for perf reasons, we assume the caller has done clipping */       \
                                                                                 \
  /* handle stippling */                                                         \
  if (FBZMODE_ENABLE_STIPPLE(FBZMODE))                                           \
  {                                                                              \
    /* rotate mode */                                                            \
    if (FBZMODE_STIPPLE_PATTERN(FBZMODE) == 0)                                   \
    {                                                                            \
      (VV)->reg[stipple].u = ((VV)->reg[stipple].u << 1) | ((VV)->reg[stipple].u >> 31); \
      if (((VV)->reg[stipple].u & 0x80000000) == 0)                              \
      {                                                                          \
        goto skipdrawdepth;                                                      \
      }                                                                          \
    }                                                                            \
                                                                                 \
    /* pattern mode */                                                           \
    else                                                                         \
    {                                                                            \
      int stipple_index = (((YY) & 3) << 3) | (~(XX) & 7);                       \
      if ((((VV)->reg[stipple].u >> stipple_index) & 1) == 0)                    \
      {                                                                          \
        goto skipdrawdepth;                                                      \
      }                                                                          \
    }                                                                            \
  }                                                                              \
                                                                                 \
  /* compute "floating point" W value (used for depth and fog) */                \
  if ((ITERW) & BX_CONST64(0xffff00000000))                                      \
    wfloat = 0x0000;                                                             \
  else                                                                           \
  {                                                                              \
    Bit32u temp = (Bit32u)(ITERW);                                               \
    if ((temp & 0xffff0000) == 0)                                                \
      wfloat = 0xffff;                                                           \
    else                                                                         \
    {                                                                            \
      int exp = count_leading_zeros(temp);                                       \
      wfloat = ((exp << 12) | ((~temp >> (19 - exp)) & 0xfff)) + 1;              \
    }                                                                            \
  }                                                                              \
                                                                                 \
  /* compute depth value (W or Z) for this pixel */                              \
  if (FBZMODE_WBUFFER_SELECT(FBZMODE) == 0)                                      \
    CLAMPED_Z(ITERZ, FBZCOLORPATH, depthval);                                    \
  else if (FBZMODE_DEPTH_FLOAT_SELECT(FBZMODE) == 0)                             \
    depthval = wfloat;                                                           \
  else                                                                           \
  {                                                                              \
    if ((ITERZ) & 0xf0000000)                                                    \
      depthval = 0x0000;                                                         \
    else                                                                         \
    {                                                                            \
      Bit32u temp = (ITERZ) << 4;                                                \
      if ((temp & 0xffff0000) == 0)                                              \
        depthval = 0xffff;                                                       \
      else                                                                       \
      {                                                                          \
        int exp = count_leading_zeros(temp);                                     \
        depthval = ((exp << 12) | ((~temp >> (19 - exp)) & 0xfff)) + 1;          \
      }                                                                          \
    }                                                                            \
  }                                                                              \
                                                                                 \
  /* add the bias */                                                             \
  if (FBZMODE_ENABLE_DEPTH_BIAS(FBZMODE))                                        \
  {                                                                              \
    depthval += (Bit16s)(VV)->reg[zaColor].u;                                    \
    CLAMP(depthval, 0, 0xffff);                                                  \
  }                                                                              \
                                                                                 \
  /* handle depth buffer testing */                                              \
  if (FBZMODE_ENABLE_DEPTHBUF(FBZMODE))                                          \
  {                                                                              \
    Bit32s depthsource;                                                          \
                                                                                 \
    /* the source depth is either the iterated W/Z+bias or a */                  \
    /* constant value */                                                         \
    if (FBZMODE_DEPTH_SOURCE_COMPARE(FBZMODE) == 0)                              \
      depthsource = depthval;                                                    \
    else                                                                         \
      depthsource = (Bit16u)(VV)->reg[zaColor].u;                                \
                                                                                 \
    /* test against the depth buffer */                                          \
    switch (FBZMODE_DEPTH_FUNCTION(FBZMODE))                                     \
    {                                                                            \
      case 0:   /* depthOP = never */                                            \
        (STATS)->zfunc_fail++;                                                   \
        goto skipdrawdepth;                                                      \
                                                                                 \
      case 1:   /* depthOP = less than */                                        \
        if (depthsource >= depth[XX])                                            \
        {                                                                        \
          (STATS)->zfunc_fail++;                                                 \
          goto skipdrawdepth;                                                    \
        }                                                                        \
        break;                                                                   \
                                                                                 \
      case 2:   /* depthOP = equal */                                            \
        if (depthsource != depth[XX])                                            \
        {                                                                        \
          (STATS)->zfunc_fail++;                                                 \
          goto skipdrawdepth;                                                    \
        }                                                                        \
        break;                                                                   \
                                                                                 \
      case 3:   /* depthOP = less than or equal */                               \
        if (depthsource > depth[XX])                                             \
        {                                                                        \
          (STATS)->zfunc_fail++;                                                 \
          goto skipdrawdepth;                                                    \
        }                                                                        \
        break;                                                                   \
                                                                                 \
      case 4:   /* depthOP = greater than */                                     \
        if (depthsource <= depth[XX])                                            \
        {                                                                        \
          (STATS)->zfunc_fail++;                                                 \
          goto skipdrawdepth;                                                    \
        }                                                                        \
        break;                                                                   \
                                                                                 \
      case 5:   /* depthOP = not equal */                                        \
        if (depthsource == depth[XX])                                            \
        {                                                                        \
          (STATS)->zfunc_fail++;                                                 \
          goto skipdrawdepth;                                                    \
        }                                                                        \
        break;                                                                   \
                                                                                 \
      case 6:   /* depthOP = greater than or equal */                            \
        if (depthsource < depth[XX])                                             \
        {                                                                        \
          (STATS)->zfunc_fail++;                                                 \
          goto skipdrawdepth;                                                    \
        }                                                                        \
        break;                                                                   \
                                                                                 \
      case 7:   /* depthOP = always */                                           \
        break;                                                                   \
    }                                                                            \
  }


#define PIXEL_PIPELINE_END(VV, STATS, DITHER, DITHER4, DITHER_LOOKUP, XX, dest, depth, FBZMODE, FBZCOLORPATH, ALPHAMODE, FOGMODE, ITERZ, ITERW, ITERAXXX) \
                                                                                 \
  /* perform fogging */                                                          \
  prefogr = r;                                                                   \
  prefogg = g;                                                                   \
  prefogb = b;                                                                   \
  APPLY_FOGGING(VV, FOGMODE, FBZCOLORPATH, XX, DITHER4, r, g, b,                 \
          ITERZ, ITERW, ITERAXXX);                                               \
                                                                                 \
  /* perform alpha blending */                                                   \
  APPLY_ALPHA_BLEND(FBZMODE, ALPHAMODE, XX, DITHER, r, g, b, a);                 \
                                                                                 \
  /* modify the pixel for debugging purposes */                                  \
  MODIFY_PIXEL(VV);                                                              \
                                                                                 \
  /* write to framebuffer */                                                     \
  if (FBZMODE_RGB_BUFFER_MASK(FBZMODE))                                          \
  {                                                                              \
    /* apply dithering */                                                        \
    APPLY_DITHER(FBZMODE, XX, DITHER_LOOKUP, r, g, b);                           \
    dest[XX] = (r << 11) | (g << 5) | b;                                         \
  }                                                                              \
                                                                                 \
  /* write to aux buffer */                                                      \
  if (depth && FBZMODE_AUX_BUFFER_MASK(FBZMODE))                                 \
  {                                                                              \
    if (FBZMODE_ENABLE_ALPHA_PLANES(FBZMODE) == 0)                               \
      depth[XX] = depthval;                                                      \
    else                                                                         \
      depth[XX] = a;                                                             \
  }                                                                              \
                                                                                 \
  /* track pixel writes to the frame buffer regardless of mask */                \
  (STATS)->pixels_out++;                                                         \
                                                                                 \
skipdrawdepth:                                                                   \
  ;                                                                              \
}                                                                                \
while (0)



/*************************************
 *
 *  Colorpath pipeline macro
 *
 *************************************/

/*

    c_other_is_used:

        if (FBZMODE_ENABLE_CHROMAKEY(FBZMODE) ||
            FBZCP_CC_ZERO_OTHER(FBZCOLORPATH) == 0)

    c_local_is_used:

        if (FBZCP_CC_SUB_CLOCAL(FBZCOLORPATH) ||
            FBZCP_CC_MSELECT(FBZCOLORPATH) == 1 ||
            FBZCP_CC_ADD_ACLOCAL(FBZCOLORPATH) == 1)

    NEEDS_ITER_RGB:

        if ((c_other_is_used && FBZCP_CC_RGBSELECT(FBZCOLORPATH) == 0) ||
            (c_local_is_used && (FBZCP_CC_LOCALSELECT_OVERRIDE(FBZCOLORPATH) != 0 || FBZCP_CC_LOCALSELECT(FBZCOLORPATH) == 0))

    NEEDS_ITER_A:

        if ((a_other_is_used && FBZCP_CC_ASELECT(FBZCOLORPATH) == 0) ||
            (a_local_is_used && FBZCP_CCA_LOCALSELECT(FBZCOLORPATH) == 0))

    NEEDS_ITER_Z:

        if (FBZMODE_WBUFFER_SELECT(FBZMODE) == 0 ||
            FBZMODE_DEPTH_FLOAT_SELECT(FBZMODE) != 0 ||
            FBZCP_CCA_LOCALSELECT(FBZCOLORPATH) == 2)


*/

/*
    Expects the following declarations to be outside of this scope:

    Bit32s r, g, b, a;
*/
#define COLORPATH_PIPELINE(VV, STATS, FBZCOLORPATH, FBZMODE, ALPHAMODE, TEXELARGB, ITERZ, ITERW, ITERARGB) \
do                                        \
{                                                                                \
  Bit32s blendr, blendg, blendb, blenda;                                         \
  rgb_union c_other;                                                             \
  rgb_union c_local;                                                             \
                                                                                 \
  /* compute c_other */                                                          \
  switch (FBZCP_CC_RGBSELECT(FBZCOLORPATH))                                      \
  {                                                                              \
    case 0:   /* iterated RGB */                                                 \
      c_other.u = ITERARGB.u;                                                    \
      break;                                                                     \
                                                                                 \
    case 1:   /* texture RGB */                                                  \
      c_other.u = TEXELARGB.u;                                                   \
      break;                                                                     \
                                                                                 \
    case 2:   /* color1 RGB */                                                   \
      c_other.u = (VV)->reg[color1].u;                                           \
      break;                                                                     \
                                                                                 \
    default:  /* reserved */                                                     \
      c_other.u = 0;                                                             \
      break;                                                                     \
  }                                                                              \
                                                                                 \
  /* handle chroma key */                                                        \
  APPLY_CHROMAKEY(VV, STATS, FBZMODE, c_other);                                  \
                                                                                 \
  /* compute a_other */                                                          \
  switch (FBZCP_CC_ASELECT(FBZCOLORPATH))                                        \
  {                                                                              \
    case 0:   /* iterated alpha */                                               \
      c_other.rgb.a = ITERARGB.rgb.a;                                            \
      break;                                                                     \
                                                                                 \
    case 1:   /* texture alpha */                                                \
      c_other.rgb.a = TEXELARGB.rgb.a;                                           \
      break;                                                                     \
                                                                                 \
    case 2:   /* color1 alpha */                                                 \
      c_other.rgb.a = (VV)->reg[color1].rgb.a;                                   \
      break;                                                                     \
                                                                                 \
    default:  /* reserved */                                                     \
      c_other.rgb.a = 0;                                                         \
      break;                                                                     \
  }                                                                              \
                                                                                 \
  /* handle alpha mask */                                                        \
  APPLY_ALPHAMASK(VV, STATS, FBZMODE, c_other.rgb.a);                            \
                                                                                 \
  /* handle alpha test */                                                        \
  APPLY_ALPHATEST(VV, STATS, ALPHAMODE, c_other.rgb.a);                          \
                                                                                 \
  /* compute c_local */                                                          \
  if (FBZCP_CC_LOCALSELECT_OVERRIDE(FBZCOLORPATH) == 0)                          \
  {                                                                              \
    if (FBZCP_CC_LOCALSELECT(FBZCOLORPATH) == 0)  /* iterated RGB */             \
      c_local.u = ITERARGB.u;                                                    \
    else                      /* color0 RGB */                                   \
      c_local.u = (VV)->reg[color0].u;                                           \
  }                                                                              \
  else                                                                           \
  {                                                                              \
    if (!(TEXELARGB.rgb.a & 0x80))          /* iterated RGB */                   \
      c_local.u = ITERARGB.u;                                                    \
    else                      /* color0 RGB */                                   \
      c_local.u = (VV)->reg[color0].u;                                           \
  }                                                                              \
                                                                                 \
  /* compute a_local */                                                          \
  switch (FBZCP_CCA_LOCALSELECT(FBZCOLORPATH))                                   \
  {                                                                              \
    default:                                                                     \
    case 0:   /* iterated alpha */                                               \
      c_local.rgb.a = ITERARGB.rgb.a;                                            \
      break;                                                                     \
                                                                                 \
    case 1:   /* color0 alpha */                                                 \
      c_local.rgb.a = (VV)->reg[color0].rgb.a;                                   \
      break;                                                                     \
                                                                                 \
    case 2:   /* clamped iterated Z[27:20] */                                    \
    {                                                                            \
      int temp;                                                                  \
      CLAMPED_Z(ITERZ, FBZCOLORPATH, temp);                                      \
      c_local.rgb.a = (Bit8u)temp;                                               \
      break;                                                                     \
    }                                                                            \
                                                                                 \
    case 3:   /* clamped iterated W[39:32] */                                    \
    {                                                                            \
      int temp;                                                                  \
      CLAMPED_W(ITERW, FBZCOLORPATH, temp);     /* Voodoo 2 only */              \
      c_local.rgb.a = (Bit8u)temp;                                               \
      break;                                                                     \
    }                                                                            \
  }                                                                              \
                                                                                 \
  /* select zero or c_other */                                                   \
  if (FBZCP_CC_ZERO_OTHER(FBZCOLORPATH) == 0)                                    \
  {                                                                              \
    r = c_other.rgb.r;                                                           \
    g = c_other.rgb.g;                                                           \
    b = c_other.rgb.b;                                                           \
  }                                                                              \
  else                                                                           \
    r = g = b = 0;                                                               \
                                                                                 \
  /* select zero or a_other */                                                   \
  if (FBZCP_CCA_ZERO_OTHER(FBZCOLORPATH) == 0)                                   \
    a = c_other.rgb.a;                                                           \
  else                                                                           \
    a = 0;                                                                       \
                                                                                 \
  /* subtract c_local */                                                         \
  if (FBZCP_CC_SUB_CLOCAL(FBZCOLORPATH))                                         \
  {                                                                              \
    r -= c_local.rgb.r;                                                          \
    g -= c_local.rgb.g;                                                          \
    b -= c_local.rgb.b;                                                          \
  }                                                                              \
                                                                                 \
  /* subtract a_local */                                                         \
  if (FBZCP_CCA_SUB_CLOCAL(FBZCOLORPATH))                                        \
    a -= c_local.rgb.a;                                                          \
                                                                                 \
  /* blend RGB */                                                                \
  switch (FBZCP_CC_MSELECT(FBZCOLORPATH))                                        \
  {                                                                              \
    default:  /* reserved */                                                     \
    case 0:   /* 0 */                                                            \
      blendr = blendg = blendb = 0;                                              \
      break;                                                                     \
                                                                                 \
    case 1:   /* c_local */                                                      \
      blendr = c_local.rgb.r;                                                    \
      blendg = c_local.rgb.g;                                                    \
      blendb = c_local.rgb.b;                                                    \
      break;                                                                     \
                                                                                 \
    case 2:   /* a_other */                                                      \
      blendr = blendg = blendb = c_other.rgb.a;                                  \
      break;                                                                     \
                                                                                 \
    case 3:   /* a_local */                                                      \
      blendr = blendg = blendb = c_local.rgb.a;                                  \
      break;                                                                     \
                                                                                 \
    case 4:   /* texture alpha */                                                \
      blendr = blendg = blendb = TEXELARGB.rgb.a;                                \
      break;                                                                     \
                                                                                 \
    case 5:   /* texture RGB (Voodoo 2 only) */                                  \
      blendr = TEXELARGB.rgb.r;                                                  \
      blendg = TEXELARGB.rgb.g;                                                  \
      blendb = TEXELARGB.rgb.b;                                                  \
      break;                                                                     \
  }                                                                              \
                                                                                 \
  /* blend alpha */                                                              \
  switch (FBZCP_CCA_MSELECT(FBZCOLORPATH))                                       \
  {                                                                              \
    default:  /* reserved */                                                     \
    case 0:   /* 0 */                                                            \
      blenda = 0;                                                                \
      break;                                                                     \
                                                                                 \
    case 1:   /* a_local */                                                      \
      blenda = c_local.rgb.a;                                                    \
      break;                                                                     \
                                                                                 \
    case 2:   /* a_other */                                                      \
      blenda = c_other.rgb.a;                                                    \
      break;                                                                     \
                                                                                 \
    case 3:   /* a_local */                                                      \
      blenda = c_local.rgb.a;                                                    \
      break;                                                                     \
                                                                                 \
    case 4:   /* texture alpha */                                                \
      blenda = TEXELARGB.rgb.a;                                                  \
      break;                                                                     \
  }                                                                              \
                                                                                 \
  /* reverse the RGB blend */                                                    \
  if (!FBZCP_CC_REVERSE_BLEND(FBZCOLORPATH))                                     \
  {                                                                              \
    blendr ^= 0xff;                                                              \
    blendg ^= 0xff;                                                              \
    blendb ^= 0xff;                                                              \
  }                                                                              \
                                                                                 \
  /* reverse the alpha blend */                                                  \
  if (!FBZCP_CCA_REVERSE_BLEND(FBZCOLORPATH))                                    \
    blenda ^= 0xff;                                                              \
                                                                                 \
  /* do the blend */                                                             \
  r = (r * (blendr + 1)) >> 8;                                                   \
  g = (g * (blendg + 1)) >> 8;                                                   \
  b = (b * (blendb + 1)) >> 8;                                                   \
  a = (a * (blenda + 1)) >> 8;                                                   \
                                                                                 \
  /* add clocal or alocal to RGB */                                              \
  switch (FBZCP_CC_ADD_ACLOCAL(FBZCOLORPATH))                                    \
  {                                                                              \
    case 3:   /* reserved */                                                     \
    case 0:   /* nothing */                                                      \
      break;                                                                     \
                                                                                 \
    case 1:   /* add c_local */                                                  \
      r += c_local.rgb.r;                                                        \
      g += c_local.rgb.g;                                                        \
      b += c_local.rgb.b;                                                        \
      break;                                                                     \
                                                                                 \
    case 2:   /* add_alocal */                                                   \
      r += c_local.rgb.a;                                                        \
      g += c_local.rgb.a;                                                        \
      b += c_local.rgb.a;                                                        \
      break;                                                                     \
  }                                                                              \
                                                                                 \
  /* add clocal or alocal to alpha */                                            \
  if (FBZCP_CCA_ADD_ACLOCAL(FBZCOLORPATH))                                       \
    a += c_local.rgb.a;                                                          \
                                                                                 \
  /* clamp */                                                                    \
  CLAMP(r, 0x00, 0xff);                                                          \
  CLAMP(g, 0x00, 0xff);                                                          \
  CLAMP(b, 0x00, 0xff);                                                          \
  CLAMP(a, 0x00, 0xff);                                                          \
                                                                                 \
  /* invert */                                                                   \
  if (FBZCP_CC_INVERT_OUTPUT(FBZCOLORPATH))                                      \
  {                                                                              \
    r ^= 0xff;                                                                   \
    g ^= 0xff;                                                                   \
    b ^= 0xff;                                                                   \
  }                                                                              \
  if (FBZCP_CCA_INVERT_OUTPUT(FBZCOLORPATH))                                     \
    a ^= 0xff;                                                                   \
}                                                                                \
while (0)

