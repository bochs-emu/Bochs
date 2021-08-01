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

#ifndef VOODOO_MAIN_H
#define VOODOO_MAIN_H

/* enumeration specifying which model of Voodoo we are emulating */
enum
{
  VOODOO_1,
  VOODOO_2,
  VOODOO_BANSHEE,
  VOODOO_3,
  MAX_VOODOO_TYPES
};


#define STD_VOODOO_1_CLOCK       50000000.0
#define STD_VOODOO_2_CLOCK       90000000.0
#define STD_VOODOO_BANSHEE_CLOCK 90000000.0
#define STD_VOODOO_3_CLOCK      132000000.0


/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

#define VOODOO_MEM 0x60000000
#define VOODOO_REG_PAGES 1024
#define VOODOO_LFB_PAGES 1024
#define VOODOO_TEX_PAGES 2048
#define VOODOO_PAGES (VOODOO_REG_PAGES+VOODOO_LFB_PAGES+VOODOO_TEX_PAGES)

#define Voodoo_UpdateScreenStart() theVoodooDevice->update_screen_start()
#define Voodoo_Output_Enable(x)    theVoodooDevice->output_enable(x)
#define Voodoo_get_retrace(a)      theVoodooDevice->get_retrace(a)
#define Voodoo_update_timing()     theVoodooDevice->update_timing()
#define Voodoo_reg_write(a,b)      theVoodooDevice->reg_write(a,b)
#define Banshee_2D_write(a,b)      theVoodooDevice->blt_reg_write(a,b)
#define Banshee_LFB_write(a,b,c)   theVoodooDevice->mem_write_linear(a,b,c)

#endif
