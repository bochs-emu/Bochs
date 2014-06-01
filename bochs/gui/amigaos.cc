/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2000-2013  The Bochs Project
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

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "bochs.h"
#include "bxversion.h"
#include "param_names.h"
#include "iodev.h"
#if BX_WITH_AMIGAOS
#include "icon_bochs.h"
#include "amigagui.h"

unsigned long __stack = 100000;
static unsigned int text_rows=25, text_cols=80;

class bx_amigaos_gui_c : public bx_gui_c {
public:
  bx_amigaos_gui_c (void) {}
  DECLARE_GUI_VIRTUAL_METHODS()
};

// declare one instance of the gui object and call macro to insert the
// plugin code
static bx_amigaos_gui_c *theGui = NULL;
IMPLEMENT_GUI_PLUGIN_CODE(amigaos)

#define LOG_THIS theGui->

static void hide_pointer();
static void show_pointer();
static void freeiff(struct IFFHandle *iff);


static ULONG screenreqfunc(struct Hook *hook, struct ScreenModeRequester *smr, ULONG id)
{
  return IsCyberModeID(id);
}

LONG DispatcherFunc(void)
{
  struct Hook *hook = (Hook *)REG_A0;
  return (*(LONG(*)(struct Hook *,LONG,LONG))hook->h_SubEntry)(hook,REG_A2,REG_A1);
}

struct InputEvent *MyInputHandler(void)
{
  struct InputEvent *event = (struct InputEvent *)REG_A0;

  if (SIM->get_param_bool(BXPN_MOUSE_ENABLED)->get())
  {
    switch(event->ie_Code) {
      case IECODE_LBUTTON:
      {
        mouse_button_state |= 0x01;
        DEV_mouse_motion(event->ie_position.ie_xy.ie_x, -event->ie_position.ie_xy.ie_y, 0, mouse_button_state, 0);
        return NULL;
      }

      case (IECODE_LBUTTON | IECODE_UP_PREFIX):
      {
        mouse_button_state &= ~0x01;
        DEV_mouse_motion(event->ie_position.ie_xy.ie_x, -event->ie_position.ie_xy.ie_y, 0, mouse_button_state, 0);
        return NULL;
      }

      case IECODE_RBUTTON:
      {
        mouse_button_state |= 0x02;
        DEV_mouse_motion(event->ie_position.ie_xy.ie_x, -event->ie_position.ie_xy.ie_y, 0, mouse_button_state, 0);
        return NULL;
      }

      case (IECODE_RBUTTON | IECODE_UP_PREFIX):
      {
        mouse_button_state &= 0x01;
        DEV_mouse_motion(event->ie_position.ie_xy.ie_x, -event->ie_position.ie_xy.ie_y, 0, mouse_button_state, 0);
        return NULL;
      }
    }

    if (event->ie_Class == IECLASS_RAWMOUSE)
    {
      DEV_mouse_motion(event->ie_position.ie_xy.ie_x, -event->ie_position.ie_xy.ie_y, 0, mouse_button_state, 0);
      return NULL;
    }

    return (event);
  }
  return (event);
}

void setup_inputhandler(void)
{
  static struct EmulLibEntry GATEMyInputHandler =
  {
    TRAP_LIB, 0, (void (*)(void))MyInputHandler
  };


  if (inputPort=CreateMsgPort())
  {
    if (inputHandler=(struct Interrupt *)AllocMem(sizeof(struct Interrupt), MEMF_PUBLIC|MEMF_CLEAR))
    {
      if (inputReqBlk=(struct IOStdReq *)CreateIORequest(inputPort, sizeof(struct IOStdReq)))
      {
        if (!(input_error = OpenDevice("input.device",NULL, (struct IORequest *)inputReqBlk, NULL)))
        {
          inputHandler->is_Code=(void(*)())&GATEMyInputHandler;
          inputHandler->is_Data=NULL;
          inputHandler->is_Node.ln_Pri=100;
          inputHandler->is_Node.ln_Name=HandlerName;
          inputReqBlk->io_Data=(APTR)inputHandler;
          inputReqBlk->io_Command=IND_ADDHANDLER;
          DoIO((struct IORequest *)inputReqBlk);
        }
        else
          BX_PANIC(("Amiga: Could not open input.device"));
      }
      else
        BX_PANIC(("Amiga: Could not create I/O request"));
    }
    else
      BX_PANIC(("Amiga: Could not allocate interrupt struct memory"));
  }
  else
    printf(("Amiga: Could not create message port"));
}

bx_bool open_screen(void)
{
  ULONG id = INVALID_ID;

  char *scrmode;
  struct DrawInfo *screen_drawinfo = NULL;

  struct ScreenModeRequester *smr;

  static struct EmulLibEntry GATEDispatcherFunc =
  {
    TRAP_LIB, 0, (void (*)(void))DispatcherFunc
  };

  struct Hook screenreqhook = { 0, 0, (ULONG(*)())&GATEDispatcherFunc, (ULONG(*)())screenreqfunc, 0 };

  sprintf (verstr, "Bochs x86 Emulator %s", VER_STRING);

  if (SIM->get_param_bool(BXPN_FULLSCREEN)->get())
  {
    if((scrmode = SIM->get_param_string(BXPN_SCREENMODE)->getptr()))
    {
      id = strtoul(scrmode, NULL, 0);
      if (!IsCyberModeID(id)) id = INVALID_ID;
    }

    if (id == INVALID_ID)
    {
      if (smr = (ScreenModeRequester *)AllocAslRequestTags(ASL_ScreenModeRequest,
                                ASLSM_DoWidth, TRUE,
                                ASLSM_DoHeight, TRUE,
                                ASLSM_MinDepth, 8,
                                ASLSM_MaxDepth, 32,
                                ASLSM_PropertyFlags, DIPF_IS_WB,
                                ASLSM_PropertyMask, DIPF_IS_WB,
                                ASLSM_FilterFunc, (ULONG) &screenreqhook,
                                TAG_DONE))
      {
        if (AslRequest(smr, NULL))
        {
          id = smr->sm_DisplayID;
          FreeAslRequest(smr);
        }
        else
        {
          FreeAslRequest(smr);
          BX_PANIC(("Amiga: Can't start without a screen"));
        }
      }
    }

    h = GetCyberIDAttr(CYBRIDATTR_HEIGHT, id);
    w = GetCyberIDAttr(CYBRIDATTR_WIDTH, id);
    d = GetCyberIDAttr(CYBRIDATTR_DEPTH, id);
    
    //sprintf(scrmode, "%d", id);
    //setenv("env:bochs/screenmode", scrmode, 1);


    screen = OpenScreenTags(NULL,
          SA_Width, w,
          SA_Height, h,
          SA_Depth, d,
          SA_Title, verstr,
          SA_DisplayID, id,
          SA_ShowTitle, FALSE,
          SA_Type, PUBLICSCREEN,
          SA_SharePens, TRUE,
          TAG_DONE);

    if(!screen)
      BX_PANIC(("Amiga: Couldn't open screen"));

    window = OpenWindowTags(NULL,
                         WA_CustomScreen,(int)screen,
                         WA_Width,w,
                         WA_Height,h,
                         WA_Title, verstr,
                         WA_IDCMP,        IDCMP_RAWKEY | IDCMP_GADGETUP | IDCMP_INACTIVEWINDOW,
                         WA_ReportMouse, TRUE,
                         WA_RMBTrap, TRUE,
                         WA_Backdrop,TRUE,
                         WA_Borderless,TRUE,
                         WA_Activate,TRUE,
                         TAG_DONE);
  }
  else
  {
    pub_screen = LockPubScreen(NULL);
    if (pub_screen != NULL)
    {
      screen_drawinfo = GetScreenDrawInfo(pub_screen);
      if (screen_drawinfo != NULL)
      {
        id = GetVPModeID(&pub_screen->ViewPort);
        d = GetCyberIDAttr(CYBRIDATTR_DEPTH, id);
      } else {
        UnlockPubScreen(NULL,pub_screen);
        BX_PANIC(("Amiga: Couldn't get ScreenDrawInfo"));
      }

      window = OpenWindowTags(NULL,
                         WA_Width,w,
                         WA_Height,h,
                         WA_Title, verstr,
                         WA_IDCMP,        IDCMP_RAWKEY | IDCMP_GADGETUP | IDCMP_CHANGEWINDOW | IDCMP_INACTIVEWINDOW,
                         WA_RMBTrap, TRUE,
                         WA_DepthGadget, TRUE,
                         WA_ReportMouse, TRUE,
                         WA_DragBar, TRUE,
                         WA_Activate,TRUE,
                         TAG_DONE);

      UnlockPubScreen(NULL,pub_screen);
    }
    else
      BX_PANIC(("Amiga: Couldn't lock the public screen"));
  }

  if (!window)
    BX_PANIC(("Amiga: Couldn't open the window"));

  if ((emptypointer = (UWORD *)AllocVec (16, MEMF_CLEAR)) == NULL)
    BX_PANIC(("Amiga: Couldn't allocate memory"));

  vgafont = OpenDiskFont(&vgata);

  if (SIM->get_param_bool(BXPN_MOUSE_ENABLED)->get())
    hide_pointer();

  if(!vgafont)
    BX_PANIC(("Amiga: Couldn't open the vga font"));

  SetFont(window->RPort, vgafont);

  if (NULL == (vi = GetVisualInfo(window->WScreen, TAG_END)))
    BX_PANIC(("Amiga: GetVisualInfo() failed"));

  bx_gadget_handle = CreateContext(&bx_glistptr);

  bx_bordertop = window->BorderTop;
  bx_borderleft = window->BorderLeft;
  bx_borderright = window->BorderRight;
  bx_borderbottom = window->BorderBottom;
  bx_headernext_left = bx_borderleft;
  bx_headernext_right += bx_borderright;

  for (apen = 0; apen < 256; apen++) /*fill the pen map with -1 so we can know which pens to free at exit*/
    pmap[apen] = -1;

  white = ObtainBestPen(window->WScreen->ViewPort.ColorMap, 0xffffffff, 0xffffffff, 0xffffffff, NULL);
  black = ObtainBestPen(window->WScreen->ViewPort.ColorMap, 0x00000000, 0x00000000, 0x00000000, NULL);
}

void bx_amigaos_gui_c::specific_init(int argc, char **argv, unsigned headerbar_y)
{
  put("AMGUI");
  bx_headerbar_y = headerbar_y;

  IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 39);
  if (IntuitionBase == NULL)
    BX_PANIC(("Amiga: Failed to open intuition.library v39 or later!"));
  if (IntuitionBase->LibNode.lib_Version == 50 && IntuitionBase->LibNode.lib_Revision < 5)
    BX_PANIC(("Amiga: intuition.library v50 needs to be revision 5 or higher!"));

  GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 39);
  if (GfxBase == NULL)
    BX_PANIC(("Amiga: Failed to open graphics.library v39 or later!"));

  GadToolsBase = OpenLibrary("gadtools.library", 37);
  if (GadToolsBase == NULL)
    BX_PANIC(("Amiga: Failed to open gadtools.library v37 or later!"));
  if (GadToolsBase->lib_Version == 50 && GadToolsBase->lib_Revision < 3)
    BX_PANIC(("Amiga: gadtools.library v50 needs to be revision 3 or higher!"));

  CyberGfxBase = OpenLibrary("cybergraphics.library", 40);
  if (CyberGfxBase == NULL)
    BX_PANIC(("Amiga: Failed to open cybergraphics.library v40 or later!"));

  AslBase = OpenLibrary("asl.library", 38);
  if (AslBase == NULL)
    BX_PANIC(("Amiga: Failed to open asl.library v38 or later!"));

  DiskfontBase = OpenLibrary("diskfont.library", 38);
  if (DiskfontBase == NULL)
    BX_PANIC(("Amiga: Failed to open diskfont.library v38 or later!"));

  IFFParseBase = OpenLibrary("iffparse.library", 39);
  if (IFFParseBase == NULL)
    BX_PANIC(("Amiga: Failed to open iffparse.library v39 or later!"));

  open_screen();
  setup_inputhandler();

  if (SIM->get_param_bool(BXPN_PRIVATE_COLORMAP)->get()) {
    BX_ERROR(("Amiga: private_colormap option ignored."));
  }
}

void bx_amigaos_gui_c::handle_events(void)
{
  void (*func) (void);
  struct IntuiMessage *imsg = NULL;
  struct Gadget *gad;
  ULONG imCode,imClass,imQualifier;
  Bit32u key_event;

  while ((imsg = (struct IntuiMessage *)GetMsg(window->UserPort)))
  {
    gad = (struct Gadget *)imsg->IAddress;
    key_event= 0;

    imClass = imsg->Class;
    imCode = imsg->Code;
    imQualifier = imsg->Qualifier;

    ReplyMsg((struct Message *)imsg);

    switch (imClass) {
      case IDCMP_RAWKEY:
        if (imQualifier & IEQUALIFIER_LSHIFT && imQualifier & IEQUALIFIER_CONTROL && imQualifier & IEQUALIFIER_LCOMMAND)
        {
          toggle_mouse_enable();
          break;
        }

        if(imCode <= 101)
          key_event = raw_to_bochs[imCode];
        if(imCode >= 128)
          key_event = raw_to_bochs[imCode-128] | BX_KEY_RELEASED;
        if(key_event)
          DEV_kbd_gen_scancode(key_event);
        break;

      case GADGETUP:
        ((void (*)()) bx_header_gadget[gad->GadgetID]->UserData)();
        break;

      case IDCMP_INACTIVEWINDOW:
        if (SIM->get_param_bool(BXPN_MOUSE_ENABLED)->get())
          toggle_mouse_enable();
        break;

      case IDCMP_CHANGEWINDOW:
        if(bx_xchanged)
        {
          bx_amigaos_gui_c::show_headerbar();
          bx_xchanged = FALSE;
        }
        break;
    }
  }
}


void bx_amigaos_gui_c::flush(void) { }

void bx_amigaos_gui_c::clear_screen(void)
{
  if (d > 8 || !SIM->get_param_bool(BXPN_FULLSCREEN)->get())
    SetAPen(window->RPort, black);
  else
    SetAPen(window->RPort, 0); /*should be ok to clear with the first pen in the map*/
  RectFill(window->RPort, bx_borderleft, bx_bordertop + bx_headerbar_y, window->Width - bx_borderright - 1, window->Height - bx_borderbottom - 1);
}

void bx_amigaos_gui_c::text_update(Bit8u *old_text, Bit8u *new_text,
                                          unsigned long cursor_x, unsigned long cursor_y,
                                          bx_vga_tminfo_t *tm_info)
{
  int i;
  int cursori;
  unsigned nchars;
  char achar;
  char string[80];
  int x, y;
  static int previ;
  unsigned int fgcolor, bgcolor;

  //current cursor position
  cursori = (cursor_y * text_cols + cursor_x) * 2;

  // Number of characters on screen, variable number of rows
  nchars = text_cols * text_rows;

  for (i=0; i<nchars*2; i+=2)
  {
    if (i == cursori || i == previ || new_text[i] != old_text[i] || new_text[i+1] != old_text[i+1])
    {
      achar = new_text[i];
      fgcolor = new_text[i+1] & 0x0F;
      bgcolor = (new_text[i+1] & 0xF0) >> 4;

      if (i == cursori)    /*invert the cursor block*/
      {
        SetAPen(window->RPort, pmap[bgcolor]);
        SetBPen(window->RPort, pmap[fgcolor]);
      }
      else
      {
        SetAPen(window->RPort, pmap[fgcolor]);
        SetBPen(window->RPort, pmap[bgcolor]);
      }

      x = ((i/2) % text_cols)*window->RPort->TxWidth;
      y = ((i/2) / text_cols)*window->RPort->TxHeight;

      Move(window->RPort, bx_borderleft + x, bx_bordertop + bx_headerbar_y + y + window->RPort->TxBaseline);
      Text(window->RPort, &achar, 1);
    }
  }

  previ = cursori;

}

int bx_amigaos_gui_c::get_clipboard_text(Bit8u **bytes, Bit32s *nbytes)
{
  struct IFFHandle *iff = NULL;
  long err = 0;
  struct ContextNode  *cn;
  
  if (!(iff = AllocIFF ()))
  {
    BX_INFO(("Amiga: Failed to allocate iff handle"));
    return 0;
  }

  if (!(iff->iff_Stream = (ULONG) OpenClipboard (0)))
  {
    BX_INFO(("Amiga: Failed to open clipboard device"));
    freeiff(iff);
    return 0;
  }

  InitIFFasClip (iff);

  if (err = OpenIFF (iff, IFFF_READ))
  {
    BX_INFO(("Amiga: Failed to open clipboard for reading"));
    freeiff(iff);
    return 0;
  }

  if (err = StopChunk(iff, ID_FTXT, ID_CHRS))
  {
    BX_INFO(("Amiga: Failed StopChunk()"));
    freeiff(iff);
    return 0;
  }

  while(1) {
    UBYTE readbuf[1024];
    int len = 0;

    err = ParseIFF(iff, IFFPARSE_SCAN);
    if(err == IFFERR_EOC) continue;
    else if(err) break;

    cn = CurrentChunk(iff);

    if((cn) && (cn->cn_Type == ID_FTXT) && (cn->cn_ID == ID_CHRS))
    {
      while((len = ReadChunkBytes(iff,readbuf, 1024)) > 0)
      {
        Bit8u *buf = new Bit8u[len];
        memcpy (buf, readbuf, len);
        *bytes = buf;
        *nbytes = len;
      }
      if(len < 0) err = len;
    }
  }

  if(err && (err != IFFERR_EOF))
  {
    BX_INFO(("Amiga: Failed to read from clipboard"));
    freeiff(iff);
    return 0;
  }

  freeiff(iff);

  return 1;
}

int bx_amigaos_gui_c::set_clipboard_text(char *text_snapshot, Bit32u len)
{
  struct IFFHandle *iff = NULL;
  long err = 0;

  BX_INFO(("Amiga: set_clipboard_text"));

  if (!(iff = AllocIFF ()))
  {
    BX_INFO(("Amiga: Failed to allocate iff handle"));
    return 0;
  }

  if (!(iff->iff_Stream = (ULONG) OpenClipboard (0)))
  {
    BX_INFO(("Amiga: Failed to open clipboard device"));
    freeiff(iff);
    return 0;
  }

  InitIFFasClip (iff);

  if (err = OpenIFF (iff, IFFF_WRITE))
  {
    BX_INFO(("Amiga: Failed to open clipboard for writing"));
    freeiff(iff);
    return 0;
  }

  if(!(err = PushChunk(iff, ID_FTXT, ID_FORM, IFFSIZE_UNKNOWN)))
  {
    if(!(err=PushChunk(iff, 0, ID_CHRS, IFFSIZE_UNKNOWN)))
    {
      if(WriteChunkBytes(iff, text_snapshot, len) != len)
        err = IFFERR_WRITE;
    }
    if(!err) err = PopChunk(iff);
  }
  if(!err) err = PopChunk(iff);

  if(err)
  {
    BX_INFO(("Amiga: Failed to write text to clipboard"));
    freeiff(iff);
    return 0;
  }

  freeiff(iff);

  return 1;
}

bx_bool bx_amigaos_gui_c::palette_change(Bit8u index, Bit8u red, Bit8u green, Bit8u blue)
{
  Bit8u *ptr = (Bit8u *)(cmap+index);

  ptr++; /*first 8bits are not defined in the XRGB8 entry*/
  *ptr = red; ptr++;
  *ptr = green; ptr++;
  *ptr = blue;

  if (d > 8 || !SIM->get_param_bool(BXPN_FULLSCREEN)->get())
  {
    if(pmap[index] != -1)
      ReleasePen(window->WScreen->ViewPort.ColorMap, pmap[index]);
    pmap[index] = ObtainBestPen(window->WScreen->ViewPort.ColorMap, FULL(red), FULL(green), FULL(blue), OBP_Precision, (ULONG) PRECISION_EXACT, TAG_DONE);
  }
  else
  {
    SetRGB32(&screen->ViewPort, index, red << 24, green << 24, blue << 24);
    pmap[index] = index;
  }

  //printf("%d, %d: [%d, %d, %d]\n", pmap[index], index, red, green, blue);

  return(1);
}

void bx_amigaos_gui_c::graphics_tile_update(Bit8u *tile, unsigned x0, unsigned y0)
{
  if (d == 8)
    WritePixelArray(tile, 0, 0, x_tilesize, window->RPort, bx_borderleft + x0, bx_bordertop + bx_headerbar_y + y0, x_tilesize, y_tilesize, RECTFMT_LUT8);
  else
    WriteLUTPixelArray(tile, 0, 0, x_tilesize, window->RPort, cmap, bx_borderleft + x0, bx_bordertop + bx_headerbar_y + y0, x_tilesize, y_tilesize, CTABFMT_XRGB8);
}

void bx_amigaos_gui_c::dimension_update(unsigned x, unsigned y, unsigned fheight, unsigned fwidth, unsigned bpp)
{

  if (bpp > 8) {
    BX_PANIC(("%d bpp graphics mode not supported yet", bpp));
  }
  guest_textmode = (fheight > 0);
  guest_xres = x;
  guest_yres = y;
  guest_bpp = bpp;

  int xdiff = w - x;

  if (guest_textmode) {
    text_cols = x / fwidth;
    text_rows = y / fheight;
    if (fwidth != 8) {
      x = x * 8 / fwidth;
    }
    if (fheight != 16) {
      y = y * 16 / fheight;
    }
  }

  if (!SIM->get_param_bool(BXPN_FULLSCREEN)->get() && (x != w || y != h))
  {
    ChangeWindowBox(window, window->LeftEdge, window->TopEdge, x + bx_borderleft + bx_borderright, y + bx_bordertop + bx_borderbottom + bx_headerbar_y);
    w = x;
    h = y;
  }

  /* Now we need to realign the gadgets and refresh the title bar*/

  if(xdiff != 0)
  {
    int i;

    for(i = 0; i < bx_headerbar_entries; i++)
    {
      if(bx_header_gadget[i]->LeftEdge + bx_header_gadget[i]->Width > bx_headernext_left)
        bx_header_gadget[i]->LeftEdge -= xdiff;
    }

    bx_xchanged = TRUE;

  }
}

unsigned bx_amigaos_gui_c::create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim)
{
  int i = 0;
  Bit8u *a;

  if (bx_image_entries >= BX_MAX_PIXMAPS) {
    BX_PANIC(("amiga: too many pixmaps, increase BX_MAX_PIXMAPS"));
  }

  bx_header_image[bx_image_entries].LeftEdge    = 0;
  bx_header_image[bx_image_entries].TopEdge     = 0;
  bx_header_image[bx_image_entries].Width       = xdim;
  bx_header_image[bx_image_entries].Height      = ydim;
  bx_header_image[bx_image_entries].Depth       = 2;
  bx_header_image[bx_image_entries].ImageData   = (UWORD *)bmap;
  bx_header_image[bx_image_entries].NextImage           = NULL;
  bx_header_image[bx_image_entries].PlanePick   = 0x1;
  if (d > 8 || !SIM->get_param_bool(BXPN_FULLSCREEN)->get())
    bx_header_image[bx_image_entries].PlaneOnOff  = 0x2;

  /*we need to reverse the bitorder for this to work*/

  a = (Bit8u *) bx_header_image[bx_image_entries].ImageData;

  for(i = 0; i <= xdim*ydim/8; i++, a++)
  {
    *a = ((*a & 0xf0) >> 4) | ((*a & 0x0f) << 4);
    *a = ((*a & 0xcc) >> 2) | ((*a & 0x33) << 2);
    *a = ((*a & 0xaa) >> 1) | ((*a & 0x55) << 1);
  }

//dprintf("image data (%d), %lx\n", bx_image_entries, bmap);

  bx_image_entries++;
  return(bx_image_entries - 1); // return index as handle
}

unsigned bx_amigaos_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment, void (*f)(void))
{
  struct NewGadget ng;

  ng.ng_TopEdge    = bx_bordertop;
  ng.ng_Width      = bx_header_image[bmap_id].Width;
  ng.ng_Height     = bx_header_image[bmap_id].Height;
  ng.ng_VisualInfo = vi;
  ng.ng_TextAttr   = &vgata;
  ng.ng_GadgetID   = bx_headerbar_entries;
  ng.ng_GadgetText = (UBYTE *)"";
  ng.ng_Flags      = 0;
  ng.ng_UserData = f;

  if (alignment == BX_GRAVITY_LEFT)
  {
    ng.ng_LeftEdge   = bx_headernext_left;
    bx_headernext_left += ng.ng_Width;
  }
  else
  {
    ng.ng_LeftEdge   = window->Width - bx_headernext_right - ng.ng_Width;
    bx_headernext_right += ng.ng_Width;
  }

  bx_gadget_handle = bx_header_gadget[bx_headerbar_entries] =
    CreateGadget(BUTTON_KIND, bx_gadget_handle, &ng, GT_Underscore, '_', TAG_END);

  bx_gadget_handle->GadgetType |= GTYP_BOOLGADGET;
  bx_gadget_handle->Flags |= GFLG_GADGIMAGE | GFLG_GADGHNONE;
  bx_gadget_handle->GadgetRender = &bx_header_image[bmap_id];
  bx_gadget_handle->UserData = f;

  bx_headerbar_entries++;
  return(bx_headerbar_entries - 1);
}

void bx_amigaos_gui_c::show_headerbar(void)
{
  RemoveGList(window, bx_glistptr, bx_headerbar_entries);

  if (d > 8 || !SIM->get_param_bool(BXPN_FULLSCREEN)->get())
    SetAPen(window->RPort, white);
  else
    SetAPen(window->RPort, 0);
  RectFill(window->RPort, bx_borderleft, bx_bordertop, window->Width - bx_borderright - 1, bx_headerbar_y + bx_bordertop - 1);

  AddGList(window, bx_glistptr, ~0, bx_headerbar_entries, NULL);
  RefreshGList(bx_glistptr, window, NULL, bx_headerbar_entries + 1);

  GT_RefreshWindow(window,NULL);
}

void bx_amigaos_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id)
{
  bx_header_gadget[hbar_id]->GadgetRender = &bx_header_image[bmap_id];

  RefreshGList(bx_glistptr, window, NULL, bx_headerbar_entries + 1);
}

void bx_amigaos_gui_c::exit(void)
{
  if(window)
  {
    RemoveGList(window, bx_glistptr, bx_headerbar_entries);
    FreeGadgets(bx_glistptr);
    FreeVec(emptypointer);

    /*Release the pens*/
    while (apen >= 0)
    {
      if (pmap[apen] == black)
        black = -1;
      if (pmap[apen] == white)
        white = -1;
      ReleasePen(window->WScreen->ViewPort.ColorMap, pmap[apen]);
      apen--;
    }
    if (black != -1)
      ReleasePen(window->WScreen->ViewPort.ColorMap, black);
    if(white != -1)
      ReleasePen(window->WScreen->ViewPort.ColorMap, white);
    CloseWindow(window);
  }

  if(screen)
    CloseScreen(screen);
  if(CyberGfxBase)
    CloseLibrary(CyberGfxBase);
  if(GadToolsBase)
    CloseLibrary(GadToolsBase);
  if(GfxBase)
    CloseLibrary((struct Library *)GfxBase);
  if(IntuitionBase)
    CloseLibrary((struct Library *)IntuitionBase);
  if(DiskfontBase)
    CloseLibrary(DiskfontBase);
  if(AslBase)
    CloseLibrary(AslBase);
  if(IFFParseBase)
    CloseLibrary(IFFParseBase);

  if(!input_error)
  {
    inputReqBlk->io_Data=(APTR)inputHandler;
    inputReqBlk->io_Command=IND_REMHANDLER;
    DoIO((struct IORequest *)inputReqBlk);
    CloseDevice((struct IORequest *)inputReqBlk);
  }

  if(inputReqBlk)
    DeleteIORequest((struct IORequest *)inputReqBlk);
  if(inputHandler)
    FreeMem(inputHandler,sizeof(struct Interrupt));
  if(inputPort)
    DeleteMsgPort(inputPort);
}

void show_pointer(void)
{
  ClearPointer(window);
}

void hide_pointer(void)
{
  SetPointer(window, emptypointer, 1, 16, 0, 0);
}

void bx_amigaos_gui_c::mouse_enabled_changed_specific (bx_bool val)
{
  if (val) {
    BX_INFO(("[AmigaOS] Mouse on"));
    hide_pointer();
  } else {
    BX_INFO(("[AmigaOS] Mouse off"));
    show_pointer();
  }
}

void freeiff(struct IFFHandle *iff)
{
  if(iff) {
    CloseIFF (iff);
    if (iff->iff_Stream)
       CloseClipboard((struct ClipboardHandle *) iff->iff_Stream);
    FreeIFF(iff);
  }
}

#endif /* if BX_WITH_AMIGAOS */
