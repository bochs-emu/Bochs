/////////////////////////////////////////////////////////////////////////
// $Id: amigaos.cc,v 1.5 2001-12-13 18:36:29 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2000  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
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
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA


#include "bochs.h"
#include "icon_bochs.h"
#include "amigagui.h"

#define LOG_THIS bx_gui.

static void hide_pointer();
static void show_pointer();


static ULONG screenreqfunc(struct Hook *hook, struct ScreenModeRequester *smr, ULONG id)
{
  return IsCyberModeID(id);
}

LONG DispatcherFunc(void) {
    struct Hook *hook = (Hook *)REG_A0;
    return (*(LONG(*)(struct Hook *,LONG,LONG))hook->h_SubEntry)(hook,REG_A2,REG_A1);
}

struct InputEvent *
MyInputHandler(void)
{
    struct InputEvent *event = (struct InputEvent *)REG_A0;

    if(bx_options.Omouse_enabled->get ())
    {
    	switch(event->ie_Code)
        {
        case IECODE_LBUTTON:
    	{
        	mouse_button_state |= 0x01;
        	bx_devices.keyboard->mouse_motion(event->ie_position.ie_xy.ie_x, -event->ie_position.ie_xy.ie_y, mouse_button_state);
        	
			return NULL;
    	}

    	case (IECODE_LBUTTON | IECODE_UP_PREFIX):
    	{
        	mouse_button_state &= ~0x01;
        	bx_devices.keyboard->mouse_motion(event->ie_position.ie_xy.ie_x, -event->ie_position.ie_xy.ie_y, mouse_button_state);
			return NULL;
    	}

        case IECODE_RBUTTON:
    	{
            mouse_button_state |= 0x02;
        	bx_devices.keyboard->mouse_motion(event->ie_position.ie_xy.ie_x, -event->ie_position.ie_xy.ie_y, mouse_button_state);
			return NULL;
    	}

    	case (IECODE_RBUTTON | IECODE_UP_PREFIX):
    	{
            mouse_button_state &= 0x01;
        	bx_devices.keyboard->mouse_motion(event->ie_position.ie_xy.ie_x, -event->ie_position.ie_xy.ie_y, mouse_button_state);
			return NULL;
    	}
        }

        if (event->ie_Class == IECLASS_RAWMOUSE)
    	{
        	bx_devices.keyboard->mouse_motion(event->ie_position.ie_xy.ie_x, -event->ie_position.ie_xy.ie_y, mouse_button_state);
        	return NULL;
    	}

        return (event);
    }
    return (event);
}

void
setup_inputhandler(void)
{

	static struct EmulLibEntry    GATEMyInputHandler=
	{
		TRAP_LIB, 0, (void (*)(void))MyInputHandler
	};


    if (inputPort=CreateMsgPort())
    {
    if (inputHandler=(struct Interrupt *)AllocMem(sizeof(struct Interrupt),
                               MEMF_PUBLIC|MEMF_CLEAR))
        {
        if (inputReqBlk=(struct IOStdReq *)CreateIORequest(inputPort,
                                                 sizeof(struct IOStdReq)))
            {
            if (!(input_error = OpenDevice("input.device",NULL,
                             (struct IORequest *)inputReqBlk,NULL)))
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

Boolean
open_screen(void)
{

  	int id = INVALID_ID;

    char *scrmode;
    struct DrawInfo *screen_drawinfo = NULL;

    struct ScreenModeRequester *smr;
    
    static struct EmulLibEntry    GATEDispatcherFunc=
	{
		TRAP_LIB, 0, (void (*)(void))DispatcherFunc
	};

    struct Hook screenreqhook = { 0, 0, (ULONG(*)())&GATEDispatcherFunc, (ULONG(*)())screenreqfunc, 0 };


    if(bx_options.Ofullscreen->get ())
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
      if (AslRequest(smr, NULL))
      {
        id = smr->sm_DisplayID;
        FreeAslRequest(smr);
      }
    	else
        	BX_PANIC(("Amiga: Can't start without a screen"));

  		h = GetCyberIDAttr(CYBRIDATTR_HEIGHT, id);
        w = GetCyberIDAttr(CYBRIDATTR_WIDTH, id);
        d = GetCyberIDAttr(CYBRIDATTR_DEPTH, id);

    //sprintf(scrmode, "%d", id);
    //setenv("env:bochs/screenmode", scrmode, 1);
          

    screen = OpenScreenTags(NULL,
      SA_Width, w,
      SA_Height, h,
      SA_Depth, d,
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
             WA_IDCMP,	IDCMP_RAWKEY | IDCMP_GADGETUP,
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
    if (pub_screen != NULL )
    {
    	screen_drawinfo = GetScreenDrawInfo(pub_screen);
    	if ( screen_drawinfo != NULL)
        {
            id = GetVPModeID(&pub_screen->ViewPort);
            d = GetCyberIDAttr(CYBRIDATTR_DEPTH, id);
        }
    	else
            BX_PANIC(("Amiga: Couldn't get ScreenDrawInfo"));

      window = OpenWindowTags(NULL,
             WA_Width,w,
             WA_Height,h,
             WA_IDCMP,	IDCMP_RAWKEY | IDCMP_GADGETUP,
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
         bx_gui_c::exit();

    if ((emptypointer = (UWORD *)AllocVec (16, MEMF_CLEAR)) == NULL)
        BX_PANIC(("Amiga: Couldn't allocate memory"));

    vgafont = OpenDiskFont(&vgata);

    if (bx_options.Omouse_enabled->get ())
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

     white = ObtainBestPen(window->WScreen->ViewPort.ColorMap, 0xffffffff, 0xffffffff, 0xffffffff);
     black = ObtainBestPen(window->WScreen->ViewPort.ColorMap, 0x00000000, 0x00000000, 0x00000000);
     
}

  void
bx_gui_c::specific_init(bx_gui_c *th, int argc, char **argv, unsigned tilewidth, unsigned tileheight,
                     unsigned headerbar_y)
{
  
  x_tilesize = tilewidth;
  y_tilesize = tileheight;

  bx_headerbar_y = headerbar_y;

  IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 39);
  if (IntuitionBase == NULL)
    BX_PANIC(("Amiga: Failed to open intuition.library v39 or later!"));


  GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 39);
  if (GfxBase == NULL)
    BX_PANIC(("Amiga: Failed to open graphics.library v39 or later!"));

  GadToolsBase = OpenLibrary("gadtools.library", 37);
    if (GadToolsBase == NULL)
    BX_PANIC(("Amiga: Failed to open gadtools.library v37 or later!"));

  CyberGfxBase = OpenLibrary("cybergraphics.library", 40);
  if (CyberGfxBase == NULL)
    BX_PANIC(("Amiga: Failed to open cybergraphics.library v40 or later!"));

  AslBase = OpenLibrary("asl.library", 38);
  if (AslBase == NULL)
    BX_PANIC(("Amiga: Failed to open asl.library v38 or later!"));

  DiskfontBase = OpenLibrary("diskfont.library", 38);
  if (DiskfontBase == NULL)
   	BX_PANIC(("Amiga: Failed to open diskfont.library v38 or later!"));

    open_screen();
    setup_inputhandler();
  /*
  if (bx_options.private_colormap) {
    fprintf(stderr, "# WARNING: Amiga: private_colormap option ignored.\n");
    }*/
}


  void
bx_gui_c::handle_events(void)
{
    void (*func) (void);
    struct IntuiMessage *imsg = NULL;
  	struct Gadget *gad;
    ULONG imCode,imClass;
    Bit32u key_event;

    while ((imsg = (struct IntuiMessage *)GetMsg(window->UserPort)))
    {
      gad = (struct Gadget *)imsg->IAddress;
      key_event= 0;

      imClass = imsg->Class;
      imCode = imsg->Code;

      ReplyMsg((struct Message *)imsg);

      switch (imClass)
        {
        case IDCMP_RAWKEY:
            if (imsg->Qualifier & IEQUALIFIER_LSHIFT && imsg->Qualifier & IEQUALIFIER_CONTROL && imsg->Qualifier & IEQUALIFIER_LCOMMAND)
            {
                toggle_mouse_enable();
                break;
            }
            if(imCode <= 101)
        		key_event = raw_to_bochs[imCode];
            if(imCode >= 128)
             	key_event = raw_to_bochs[imCode-128] | BX_KEY_RELEASED;
            if(key_event)
        		bx_devices.keyboard->gen_scancode(key_event);
     		break;

        case GADGETUP:
        	((void (*)()) bx_header_gadget[gad->GadgetID]->UserData)();
            break;
     	}
	}
}


  void
bx_gui_c::flush(void)
{
}

  void
bx_gui_c::clear_screen(void)
{
    if(d > 8 || !bx_options.Ofullscreen->get ())
    	SetAPen(window->RPort, black);
    else
    	SetAPen(window->RPort, 0); /*should be ok to clear with the first pen in the map*/
    RectFill(window->RPort, bx_borderleft, bx_bordertop + bx_headerbar_y, window->Width - bx_borderright - 1, window->Height - bx_borderbottom - 1);
}

  void
bx_gui_c::text_update(Bit8u *old_text, Bit8u *new_text,
                      unsigned long cursor_x, unsigned long cursor_y,
                      Bit16u cursor_state, unsigned nrows)
{
int i;
int	cursori;
unsigned nchars;
unsigned char achar;
char string[80];
int	x, y;
static int previ;
unsigned int fgcolor, bgcolor;

//current cursor position
	cursori = (cursor_y*80 + cursor_x)*2;

	// Number of characters on screen, variable number of rows
	nchars = 80*nrows;

    
for (i=0; i<nchars*2; i+=2)
	{
		if ( i == cursori || i == previ || new_text[i] != old_text[i] || new_text[i+1] != old_text[i+1])
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


            x = ((i/2) % 80)*window->RPort->TxWidth;
			y = ((i/2) / 80)*window->RPort->TxHeight;

            Move(window->RPort, bx_borderleft + x, bx_bordertop + bx_headerbar_y + y + window->RPort->TxBaseline);
            Text(window->RPort, &achar, 1);
            }
    }

    previ = cursori;
}


  Boolean
bx_gui_c::palette_change(unsigned index, unsigned red, unsigned green, unsigned blue)
{

  Bit8u *ptr;
                    
  	ptr = (Bit8u *)(cmap+index);

  	ptr++; /*first 8bits are not defined in the XRGB8 entry*/
  	*ptr = red; ptr++;
  	*ptr = green; ptr++;
  	*ptr = blue;

  if(d > 8 || !bx_options.Ofullscreen->get ())
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


  void
bx_gui_c::graphics_tile_update(Bit8u *tile, unsigned x0, unsigned y0)
{
 if (d == 8)
 {
   WritePixelArray(tile, 0, 0, x_tilesize, window->RPort, bx_borderleft + x0, bx_bordertop + bx_headerbar_y + y0, x_tilesize, y_tilesize, RECTFMT_LUT8);
 }
 else
 	WriteLUTPixelArray(tile, 0, 0, x_tilesize, window->RPort, cmap, bx_borderleft + x0, bx_bordertop + bx_headerbar_y + y0, x_tilesize, y_tilesize, CTABFMT_XRGB8);
}


  void
bx_gui_c::dimension_update(unsigned x, unsigned y)
{
        if(!bx_options.Ofullscreen->get () && (x != w || y != h))
        {
            ChangeWindowBox(window, window->LeftEdge, window->TopEdge, x + bx_borderleft + bx_borderright, y + bx_bordertop + bx_borderbottom + bx_headerbar_y);
        	 w = x;
        	 h = y;
        }
    /* todo: I should also realign the rightaligned gadgets*/
}

 

  unsigned
bx_gui_c::create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim)
{
    int i = 0;
    Bit8u *a;

    if (bx_image_entries >= BX_MAX_PIXMAPS) {
    BX_PANIC(("amiga: too many pixmaps, increase BX_MAX_PIXMAPS"));
    }

	bx_header_image[bx_headerbar_entries].LeftEdge    = 0;
    bx_header_image[bx_image_entries].TopEdge     = 0;
    bx_header_image[bx_image_entries].Width       = xdim;
    bx_header_image[bx_image_entries].Height      = ydim;
    bx_header_image[bx_image_entries].Depth       = 2;
    bx_header_image[bx_image_entries].ImageData   = (UWORD *)bmap;
    bx_header_image[bx_image_entries].NextImage	   = NULL;
    bx_header_image[bx_image_entries].PlanePick   = 0x1;
    if(d > 8 || !bx_options.Ofullscreen->get ())
    	bx_header_image[bx_image_entries].PlaneOnOff  = 0x2;

    /*we need to reverse the bitorder for this to work*/

    a = (Bit8u *) bx_header_image[bx_image_entries].ImageData;

    for(i = 0; i <= xdim*ydim/8; i++, a++)
    {
    	*a = ((*a & 0xf0) >> 4) | ((*a & 0x0f) << 4);
		*a = ((*a & 0xcc) >> 2) | ((*a & 0x33) << 2);
		*a = ((*a & 0xaa) >> 1) | ((*a & 0x55) << 1);
    }

  	bx_image_entries++;
  	return(bx_image_entries - 1); // return index as handle
}

  unsigned
bx_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment, void (*f)(void))
{
struct NewGadget ng;

ng.ng_TopEdge    = bx_bordertop;
ng.ng_Width      = bx_header_image[bmap_id].Width;
ng.ng_Height     = bx_header_image[bmap_id].Height;
ng.ng_VisualInfo = vi;
ng.ng_TextAttr   = &vgata;
ng.ng_GadgetID   = bx_headerbar_entries;
ng.ng_GadgetText = (UBYTE *)"";

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
	CreateGadget(BUTTON_KIND, bx_gadget_handle, &ng,
                    GT_Underscore, '_',
                    TAG_END);
bx_gadget_handle->GadgetType |= GTYP_BOOLGADGET;
bx_gadget_handle->Flags |= GFLG_GADGIMAGE | GFLG_GADGHNONE;
bx_gadget_handle->GadgetRender = &bx_header_image[bmap_id];
bx_gadget_handle->UserData = f;

bx_headerbar_entries++;
return(bx_headerbar_entries - 1);
}

 
  void
bx_gui_c::show_headerbar(void)
{
    if(d > 8 || !bx_options.Ofullscreen->get ())
    	SetAPen(window->RPort, white);
    else
        SetAPen(window->RPort, 0);
    RectFill(window->RPort, bx_borderleft, bx_bordertop, window->Width - bx_borderright - 1, bx_headerbar_y + bx_bordertop - 1);
    RemoveGList(window, bx_glistptr, bx_headerbar_entries);
    AddGList(window, bx_glistptr, ~0, bx_headerbar_entries + 1, NULL);
    RefreshGList(bx_glistptr, window, NULL, bx_headerbar_entries + 1);
}


  void
bx_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id)
{
	bx_header_gadget[hbar_id]->GadgetRender = &bx_header_image[bmap_id];
    RefreshGList(bx_glistptr, window, NULL, bx_headerbar_entries + 1);
}

  void
bx_gui_c::exit(void)
{
	if(window)
  	{

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
  		CloseLibrary(GfxBase);
  	if(IntuitionBase)
  		CloseLibrary(IntuitionBase);
  	if(DiskfontBase)
  		CloseLibrary(DiskfontBase);
  	if(AslBase)
  		CloseLibrary(AslBase);

	if(!input_error)
    {
    	printf("closing device\n");
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

void
show_pointer(void)
{
	ClearPointer(window);
}

void
hide_pointer(void)
{
	SetPointer(window, emptypointer, 1, 16, 0, 0);
}

  void
bx_gui_c::mouse_enabled_changed_specific (Boolean val)
{
  BX_INFO (("mouse_enabled=%d, x11 specific code", val?1:0));
  if (val) {
    BX_INFO(("[AmigaOS] Mouse on"));
    hide_pointer();
  } else {
    BX_INFO(("[AmigaOS] Mouse off"));
    show_pointer();
  }
}
