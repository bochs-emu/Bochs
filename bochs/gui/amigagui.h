/////////////////////////////////////////////////////////////////////////
// $Id: amigagui.h,v 1.2.4.1 2002-03-17 08:50:19 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
#include <exec/types.h>
#include <exec/exec.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/asl.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>
#include <proto/diskfont.h>
#include <proto/gadtools.h>
#include <diskfont/diskfont.h>
#include <intuition/IntuitionBase.h>
#include <intuition/pointerclass.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <graphics/gfxbase.h>
#include <graphics/videocontrol.h>
#include <cybergraphics/cybergraphics.h>
#include <libraries/gadtools.h>
#define FULL(x) (x*0x01010101)

void check_toolbar(void);

struct IntuitionBase 	*IntuitionBase;
struct GfxBase 			*GfxBase;
struct Library          *KeymapBase;
struct Library       	*GadToolsBase;

struct Library 			*CyberGfxBase;
struct Library 			*AslBase;
struct Library 			*DiskfontBase;
struct Screen 			*screen = NULL, *pub_screen = NULL;
struct Window 			*window = NULL;
struct TextFont 		*vgafont;

struct IOStdReq  *inputReqBlk = NULL;
struct MsgPort   *inputPort = NULL;
struct Interrupt *inputHandler = NULL;

int input_error = -1;

LONG pmap[256];
ULONG cmap[256];
static UWORD *emptypointer;

struct TextAttr vgata = {
        "vga.font",
        16,
        NULL
    };

struct Image bx_header_image[BX_MAX_PIXMAPS];
struct Gadget *bx_header_gadget[BX_MAX_PIXMAPS], *bx_glistptr = NULL, *bx_gadget_handle;
static unsigned bx_image_entries = 0, bx_headerbar_entries = 0;
static unsigned bx_bordertop, bx_borderleft, bx_borderright, bx_borderbottom,
				bx_headerbar_y, mouse_button_state = 0, bx_headernext_left,
                bx_headernext_right, x_tilesize, y_tilesize, bx_mouseX, bx_mouseY;
static LONG apen = -1, black = -1, white = -1;
void *vi;

int w = 648, h = 480, d = 8;

char HandlerName[]="Bochs InputHandler";

const unsigned char raw_to_bochs [130] = {
    		BX_KEY_GRAVE,
            BX_KEY_1,	               /*1*/
        	BX_KEY_2,
        	BX_KEY_3,
        	BX_KEY_4,
        	BX_KEY_5,
        	BX_KEY_6,
        	BX_KEY_7,
        	BX_KEY_8,
        	BX_KEY_9,
        	BX_KEY_0,               	/*10*/
            BX_KEY_MINUS,
            BX_KEY_EQUALS,
            BX_KEY_BACKSLASH,
            0,
            BX_KEY_INSERT,
            BX_KEY_Q,
            BX_KEY_W,
            BX_KEY_E,
			BX_KEY_R,
			BX_KEY_T,            	  /*20*/
            BX_KEY_Y,
			BX_KEY_U,
            BX_KEY_I,
			BX_KEY_O,
			BX_KEY_P,
            BX_KEY_LEFT_BRACKET,
            BX_KEY_RIGHT_BRACKET,
            0,
            BX_KEY_KP_END,
            BX_KEY_KP_DOWN,      	 /*30*/
            BX_KEY_KP_PAGE_DOWN,
            BX_KEY_A,
			BX_KEY_S,
			BX_KEY_D,
			BX_KEY_F,
			BX_KEY_G,
			BX_KEY_H,
			BX_KEY_J,
			BX_KEY_K,
            BX_KEY_L,           	  /*40*/
        	BX_KEY_SEMICOLON,
            BX_KEY_SINGLE_QUOTE,
            0,
            0,
            BX_KEY_KP_LEFT,
            BX_KEY_KP_5,
            BX_KEY_KP_RIGHT,
            0,
            BX_KEY_Z,
        	BX_KEY_X,               /*50*/
        	BX_KEY_C,
        	BX_KEY_V,
        	BX_KEY_B,
        	BX_KEY_N,
        	BX_KEY_M,
            BX_KEY_COMMA,
            BX_KEY_PERIOD,
        	BX_KEY_SLASH,
            0,
            BX_KEY_KP_INSERT,          /*60*/
            BX_KEY_KP_HOME,
            BX_KEY_KP_UP,
            BX_KEY_KP_PAGE_UP,
            BX_KEY_SPACE,
            BX_KEY_BACKSPACE,
            BX_KEY_TAB,
            BX_KEY_KP_ENTER,
            BX_KEY_ENTER,
            BX_KEY_ESC,
        	BX_KEY_DELETE,          /*70*/
            0,
            0,
            0,
            BX_KEY_KP_SUBTRACT,
            0,
            BX_KEY_UP,
            BX_KEY_DOWN,
            BX_KEY_RIGHT,
            BX_KEY_LEFT,
            BX_KEY_F1,              /*80*/
            BX_KEY_F2,
            BX_KEY_F3,
            BX_KEY_F4,
            BX_KEY_F5,
            BX_KEY_F6,
            BX_KEY_F7,
            BX_KEY_F8,
            BX_KEY_F9,
            BX_KEY_F10,
            BX_KEY_NUM_LOCK,          /*90*/
            0,
            BX_KEY_KP_DIVIDE,
            BX_KEY_KP_MULTIPLY,
            BX_KEY_KP_ADD,
            0,
            BX_KEY_SHIFT_L,
            BX_KEY_SHIFT_R,
            BX_KEY_CAPS_LOCK,
            BX_KEY_CTRL_L,
            BX_KEY_ALT_L,           /*100*/
            BX_KEY_ALT_R
            /* missing:
            BX_KEY_HOME
			BX_KEY_END
            BX_KEY_PAGE_UP
			BX_KEY_PAGE_DOWN     */
            };
