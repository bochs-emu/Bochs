/////////////////////////////////////////////////////////////////////////
// $Id: carbon.cc,v 1.9.2.1 2002-03-17 09:05:55 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
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


// carbon.cc -- bochs GUI file for MacOS X with Carbon API
// written by David Batterham <drbatter@progsoc.uts.edu.au>
// with contributions from Tim Senecal
// port to Carbon API by Emmanuel Maillard <e.rsz@libertysurf.fr>

// BOCHS INCLUDES
#include "bochs.h"
#include "icon_bochs.h"
#include "font/vga.bitmap.h"

// MAC OS INCLUDES
#include <Carbon/Carbon.h>
#include <ApplicationServices/ApplicationServices.h>

// CONSTANTS

#define rMBarID 128
#define mApple 128
#define iAbout 1
#define mFile 129
#define iQuit 1
#define mEdit 130
#define mBochs 131
#define iFloppy 1
#define iCursor 3
#define iTool 4
#define iMenuBar 5
#define iFullScreen 6
#define iConsole 7
#define iSnapshot 9
#define iReset 10

const MenuCommand kCommandFloppy = FOUR_CHAR_CODE ('FLPY');
const MenuCommand kCommandCursor = FOUR_CHAR_CODE ('CRSR');
const MenuCommand kCommandTool = FOUR_CHAR_CODE ('TOOL');
const MenuCommand kCommandMenuBar = FOUR_CHAR_CODE ('MENU');
const MenuCommand kCommandFullScreen = FOUR_CHAR_CODE ('SCRN');
const MenuCommand kCommandSnapshot = FOUR_CHAR_CODE ('SNAP');
const MenuCommand kCommandReset = FOUR_CHAR_CODE ('RSET');

#define SLEEP_TIME	0 // Number of ticks to surrender the processor during a WaitNextEvent()
// Change this to 15 or higher if you don't want Bochs to hog the processor!

#define FONT_WIDTH		8
#define FONT_HEIGHT	16

#define WINBITMAP(w)	GetPortBitMapForCopyBits(GetWindowPort(w))  //	(((GrafPtr)(w))->portBits)

#define ASCII_1_MASK	0x00FF0000
#define ASCII_2_MASK	0x000000FF

const RGBColor	black =		{0, 0, 0};
const RGBColor	white = 	{0xFFFF, 0xFFFF, 0xFFFF};
const RGBColor	medGrey = {0xCCCC, 0xCCCC, 0xCCCC};
const RGBColor	ltGrey = 	{0xEEEE, 0xEEEE, 0xEEEE};					 

// GLOBALS

WindowPtr			win, toolwin, fullwin, backdrop, hidden, SouixWin;
WindowGroupRef			fullwinGroup;
bx_gui_c			*thisGUI;
SInt16				gOldMBarHeight;
Boolean				menubarVisible = true, cursorVisible = true;
Boolean				windowUpdatesPending = true, mouseMoved = false;
RgnHandle			mBarRgn, cnrRgn;
unsigned			mouse_button_state = 0;
CTabHandle		gCTable;
PixMapHandle	gTile;
BitMap 				*vgafont[256];
Rect					srcTextRect, srcTileRect;
Point					scrCenter = {300, 240};
Ptr						KCHR;
short					gheaderbar_y;
Point					prevPt;
unsigned			width, height, gMinTop, gMaxTop, gLeft;
GWorldPtr			gOffWorld;
ProcessSerialNumber		gProcessSerNum;

// HEADERBAR STUFF
int 					numPixMaps = 0, toolPixMaps = 0;
unsigned			bx_bitmap_left_xorigin = 2; // pixels from left
unsigned			bx_bitmap_right_xorigin = 2; // pixels from right
//PixMapHandle	bx_pixmap[BX_MAX_PIXMAPS];
CIconHandle bx_cicn[BX_MAX_PIXMAPS];

struct {
	CIconHandle cicn;
//	PixMapHandle pm;
	unsigned xdim;
	unsigned ydim;
	unsigned xorigin;
	unsigned yorigin;
	unsigned alignment;
	void (*f)(void);
} bx_tool_pixmap[BX_MAX_PIXMAPS];

// Carbon Event Handlers
pascal OSStatus CEvtHandleWindowToolClick       (EventHandlerCallRef nextHandler, EventRef theEvent, void* userData);
pascal OSStatus CEvtHandleWindowToolUpdate      (EventHandlerCallRef nextHandler, EventRef theEvent, void* userData);
pascal OSStatus CEvtHandleWindowBackdropUpdate  (EventHandlerCallRef nextHandler, EventRef theEvent, void* userData);
pascal OSStatus CEvtHandleWindowEmulatorClick   (EventHandlerCallRef nextHandler, EventRef theEvent, void* userData);
pascal OSStatus CEvtHandleWindowEmulatorUpdate  (EventHandlerCallRef nextHandler, EventRef theEvent, void* userData);
pascal OSStatus CEvtHandleWindowEmulatorKeys    (EventHandlerCallRef nextHandler, EventRef theEvent, void* userData);
pascal OSStatus CEvtHandleApplicationAppleEvent (EventHandlerCallRef nextHandler, EventRef theEvent, void* userData);
pascal OSStatus CEvtHandleApplicationMouseMoved (EventHandlerCallRef nextHandler, EventRef theEvent, void* userData);
pascal OSStatus CEvtHandleApplicationMouseUp    (EventHandlerCallRef nextHandler, EventRef theEvent, void* userData);
pascal OSStatus CEvtHandleApplicationMenuClick  (EventHandlerCallRef nextHandler, EventRef theEvent, void* userData);
pascal OSStatus CEvtHandleApplicationMenus      (EventHandlerCallRef nextHandler, EventRef theEvent, void* userData);

// Event handlers
BX_CPP_INLINE void HandleKey(EventRecord *event, Bit32u keyState);
BX_CPP_INLINE void HandleToolClick(Point where);

// Show/hide UI elements
void HidePointer(void);
void ShowPointer(void);
void HideTools(void);
void ShowTools(void);
void HideMenubar(void);
void ShowMenubar(void);
// void HideConsole(void);
// void ShowConsole(void);

// Initialisation
void FixWindow(void);
void MacPanic(void);
void InitToolbox(void);
void CreateTile(void);
void CreateMenus(void);
void CreateWindows(void);
void CreateKeyMap(void);
void CreateVGAFont(void);
BitMap *CreateBitMap(unsigned width,	unsigned height);
PixMapHandle CreatePixMap(unsigned left, unsigned top, unsigned width,
	unsigned height, unsigned depth, CTabHandle clut);
unsigned char reverse_bitorder(unsigned char);

static pascal OSErr QuitAppleEventHandler(const AppleEvent *appleEvt, AppleEvent* reply, SInt32 refcon);

extern bx_gui_c   bx_gui;

#define BX_GUI_THIS bx_gui.
#define LOG_THIS BX_GUI_THIS

// Carbon Event Handlers

pascal OSStatus CEvtHandleWindowToolClick (EventHandlerCallRef nextHandler,
    EventRef theEvent,
    void* userData)
{
	Point wheresMyMouse;
	GetEventParameter (theEvent, kEventParamMouseLocation, typeQDPoint,
            NULL, sizeof(Point), NULL, &wheresMyMouse);

	HiliteWindow(win, true);
	HandleToolClick(wheresMyMouse);

	return noErr; // Report success
}

pascal OSStatus CEvtHandleWindowToolUpdate (EventHandlerCallRef nextHandler,
    EventRef theEvent,
    void* userData)
{
	thisGUI->show_headerbar();

	return noErr; // Report success
}

pascal OSStatus CEvtHandleWindowBackdropUpdate (EventHandlerCallRef nextHandler,
    EventRef theEvent,
    void* userData)
{
	Rect	box;
	Pattern qdBlackPattern;
        
        WindowRef myWindow;
	GetEventParameter (theEvent, kEventParamDirectObject, typeWindowRef,
            NULL, sizeof(WindowRef), NULL, &myWindow);

        GetQDGlobalsBlack(&qdBlackPattern);
	GetWindowPortBounds(myWindow, &box);
        FillRect(&box, &qdBlackPattern);

	return noErr; // Report success
}

// Translate MouseDowns in a handled window into Bochs events
// Main ::HANDLE_EVENTS will feed all mouse updates to Bochs
pascal OSStatus CEvtHandleWindowEmulatorClick (EventHandlerCallRef nextHandler,
    EventRef theEvent,
    void* userData)
{
	UInt32 keyModifiers;
	GetEventParameter (theEvent, kEventParamKeyModifiers, typeUInt32,
            NULL, sizeof(UInt32), NULL, &keyModifiers);

//	if (!IsWindowActive(win))
//	{
 //           SelectWindow(win);
//	}
	if (keyModifiers & cmdKey)
            mouse_button_state |= 0x02;
	else
            mouse_button_state |= 0x01;

	return noErr; // Report success
}

pascal OSStatus CEvtHandleWindowEmulatorUpdate (EventHandlerCallRef nextHandler,
    EventRef theEvent,
    void* userData)
{
	Rect	box;
	Pattern qdBlackPattern;
        
	WindowRef myWindow;
	GetEventParameter (theEvent, kEventParamDirectObject, typeWindowRef,
            NULL, sizeof(WindowRef), NULL, &myWindow);

	GetWindowPortBounds(myWindow, &box);
	bx_vga.redraw_area(box.left, box.top, box.right, box.bottom);

	return noErr; // Report success
}

pascal OSStatus CEvtHandleWindowEmulatorKeys (EventHandlerCallRef nextHandler,
    EventRef theEvent,
    void* userData)
{
	EventRecord event;
	int oldMods=0;

	if(ConvertEventRefToEventRecord(theEvent, &event))
	{
            int key = event.message & charCodeMask;

            switch(event.what)
            {
                case keyDown:
                case autoKey:
                        oldMods = event.modifiers;
                        HandleKey(&event, BX_KEY_PRESSED);
                        break;
                        
                case keyUp:
                        event.modifiers = oldMods;
                        HandleKey(&event, BX_KEY_RELEASED);
                        break;
            }
	}
	else
            BX_PANIC(("Can't convert keyboard event"));

	return noErr; 
}

#if 0
// This stuff does work... it gets called, but converting the record
// and then calling AEProcessAppleEvent consistently results in noOutstandingHLE(err result -608)
// And its going to take more work to get RunApplicationLoop to work...
pascal OSStatus CEvtHandleApplicationAppleEvent (EventHandlerCallRef nextHandler,
    EventRef theEvent,
    void* userData)
{
	EventRecord eventRec;

	fprintf(stderr, "# Carbon apple event handler called\n");
	if(ConvertEventRefToEventRecord(theEvent, &eventRec))
	{
            fprintf(stderr, "# Calling AEProcessAppleEvent\n");
            OSStatus result = AEProcessAppleEvent(&eventRec);
            fprintf(stderr, "# Received AE result: %i\n", result);
            returm result;
	}
	else
            BX_PANIC(("Can't convert apple event"));

	return noErr; // Report success
}
#endif

// Only have our application deal with mouseEvents when we catch the movement
pascal OSStatus CEvtHandleApplicationMouseMoved (EventHandlerCallRef nextHandler,
    EventRef theEvent,
    void* userData)
{
	mouseMoved = true;
        
        return eventNotHandledErr;
}

pascal OSStatus CEvtHandleApplicationMouseUp (EventHandlerCallRef nextHandler,
    EventRef theEvent,
    void* userData)
{
	UInt32 keyModifiers;
	GetEventParameter (theEvent, kEventParamKeyModifiers, typeUInt32,
            NULL, sizeof(UInt32), NULL, &keyModifiers);

	if (keyModifiers & cmdKey)
            mouse_button_state &= ~0x02;
	else
            mouse_button_state &= ~0x01;

	return eventNotHandledErr; // Don't want to eat all the mouseups
}

// Catch MouseDown's in the menubar, trigger menu browsing
pascal OSStatus CEvtHandleApplicationMenuClick (EventHandlerCallRef nextHandler,
    EventRef theEvent,
    void* userData)
{
	short part;
	WindowPtr whichWindow;

	Point wheresMyMouse;
	GetEventParameter (theEvent, kEventParamMouseLocation, typeQDPoint,
            NULL, sizeof(Point), NULL, &wheresMyMouse);

	part = FindWindow(wheresMyMouse, &whichWindow);

	if(part == inMenuBar)
	{
            // MenuSelect will actually trigger an event cascade, 
            // Triggering command events for any selected menu item
            MenuSelect(wheresMyMouse);
            return noErr;
	}

	return eventNotHandledErr; // Don't want to eat all the clicks
}

pascal OSStatus CEvtHandleApplicationMenus (EventHandlerCallRef nextHandler,
    EventRef theEvent,
    void* userData)
{
	HICommand commandStruct;

	OSErr		err = noErr;
	short		i;
	DialogPtr	theDlog;

	GetEventParameter (theEvent, kEventParamDirectObject,
            typeHICommand, NULL, sizeof(HICommand),
            NULL, &commandStruct);

	switch(commandStruct.commandID)
	{
            case kHICommandAbout:
                theDlog = GetNewDialog(128, NULL, (WindowPtr)-1);
                ModalDialog(NULL, &i);
                DisposeDialog(theDlog);
                break;

            case kHICommandQuit:
                BX_PANIC(("User terminated"));
                break;

            case kCommandFloppy:
                //DiskEject(1);
                break;

            case kCommandCursor:
                if (cursorVisible)
                        HidePointer();
                else
                        ShowPointer();
                break;

            case kCommandTool:
                if (IsWindowVisible(toolwin))
                        HideTools();
                else
                        ShowTools();
                break;

            case kCommandMenuBar:
                    if (menubarVisible)
                            HideMenubar();
                    else
                            ShowMenubar();
                    break;

            case kCommandFullScreen:
                    if (IsWindowVisible(toolwin) || menubarVisible)
                    {
                            if (menubarVisible)
                                    HideMenubar();
                            if (IsWindowVisible(toolwin))
                                    HideTools();
                    }
                    else
                    {
                            if (!menubarVisible)
                                    ShowMenubar();
                            if (!IsWindowVisible(toolwin))
                                    ShowTools();
                    }
                    break;
                    
/*
            // Codewarrior programatic console that isn't available under Carbon without Codewarrior
            case iConsole:
                    if (IsWindowVisible(SouixWin))
                            HideConsole();
                    else
                            ShowConsole();
                    break;
*/
            case kCommandSnapshot:
                    //the following will break if snapshot is not last bitmap button instantiated
                    bx_tool_pixmap[toolPixMaps-1].f();
                    break;
        }
        
	return noErr; // Report success
}

//this routine moves the initial window position so that it is entirely onscreen
//it is needed for os 8.x with appearance managaer
void FixWindow(void)
{
  RgnHandle wStruct;
  Rect wRect;
  RgnHandle tStruct;
  Rect tRect;
  short MinVal;

  wStruct = NewRgn();
  tStruct = NewRgn();
  
  GetWindowRegion(win, kWindowStructureRgn, wStruct);
  GetRegionBounds(wStruct, &wRect);
  GetWindowRegion(toolwin, kWindowStructureRgn, tStruct);
  GetRegionBounds(tStruct, &tRect);
  
  if (wRect.left < 2)
  {
    gLeft = gLeft + (2 - wRect.left);
  }
  
  MinVal = tRect.bottom+2;
//MinVal = MinVal + GetMBarHeight();
  
  if (wRect.top < MinVal)
  {
//  gMinTop = gMinTop + (MinVal - wRect.top);
    gMaxTop = gMaxTop + (MinVal - wRect.top);
  }

  MoveWindow(win, gLeft, gMaxTop, false);
  DisposeRgn(wStruct);
  DisposeRgn(tStruct);
}

void MacPanic(void)
{
  StopAlert(200, NULL);
}

void InitToolbox(void)
{
    OSErr	err;
  //  gQuitFlag = false;
    
    InitCursor();

#if 0
    // Our handler gets called... but I can't AEProcesAppleEvent successfully upon it?
    EventTypeSpec appleEvent = { kEventClassAppleEvent, kEventAppleEvent };
    InstallApplicationEventHandler(NewEventHandlerUPP(CEvtHandleApplicationAppleEvent),
        1, &appleEvent, 0, NULL);
#endif

    err = AEInstallEventHandler( kCoreEventClass, kAEQuitApplication,
        NewAEEventHandlerUPP(QuitAppleEventHandler), 0, false);
    if (err != noErr)
        ExitToShell();
}

static pascal OSErr QuitAppleEventHandler(const AppleEvent *appleEvt, AppleEvent* reply, SInt32 refcon)
{
    //gQuitFlag =  true;
    BX_PANIC(("User terminated"));
    return (noErr);
}

void CreateTile(void)
{
	GDHandle	saveDevice;
	CGrafPtr	savePort;
	OSErr			err;
	
	if (bx_options.Oprivate_colormap->get ())
	{
		GetGWorld(&savePort, &saveDevice);
	
		err = NewGWorld(&gOffWorld, 8, 
			&srcTileRect, gCTable, NULL, useTempMem);
		if (err != noErr)
			BX_PANIC(("mac: can't create gOffWorld"));

		SetGWorld(gOffWorld, NULL);
	
		gTile = GetGWorldPixMap(gOffWorld);
		
		if (gTile != NULL)
		{
			NoPurgePixels(gTile);
			LockPixels(gTile);
			(**gTile).rowBytes = 0x8000 | srcTileRect.right;
			(**gCTable).ctFlags = (**gCTable).ctFlags | 0x4000;		//use palette manager indexes
			(**gTile).pmTable = gCTable;
		}
		else
			BX_PANIC(("mac: can't create gTile"));
	
		SetGWorld(savePort, saveDevice);
	}
	else
	{
		gTile = CreatePixMap(0, 0, srcTileRect.right, srcTileRect.bottom, 8, gCTable);
		if (gTile == NULL)
			BX_PANIC(("mac: can't create gTile"));
	}
}

void CreateMenus(void)
{
	Handle menu;
	
	menu = GetNewMBar(rMBarID);		//	get our menus from resource
        if (menu != nil) 
        {
            SetMenuBar(menu);
            DrawMenuBar();
        }
        else
            BX_PANIC(("can't create menu"));
        
        SetMenuItemCommandID (GetMenuRef(mApple), iAbout,      kHICommandAbout);
        SetMenuItemCommandID (GetMenuRef(mFile),  iQuit,       kHICommandQuit);
        SetMenuItemCommandID (GetMenuRef(mBochs), iFloppy,     kCommandFloppy);
        SetMenuItemCommandID (GetMenuRef(mBochs), iCursor,     kCommandCursor);
        SetMenuItemCommandID (GetMenuRef(mBochs), iTool,       kCommandTool);
        SetMenuItemCommandID (GetMenuRef(mBochs), iMenuBar,    kCommandMenuBar);
        SetMenuItemCommandID (GetMenuRef(mBochs), iFullScreen, kCommandFullScreen);
        SetMenuItemCommandID (GetMenuRef(mBochs), iSnapshot,   kCommandSnapshot);
        SetMenuItemCommandID (GetMenuRef(mBochs), iReset,      kCommandReset);
        
        EventTypeSpec commandEvents = {kEventClassCommand, kEventCommandProcess};
        EventTypeSpec menuEvents = {kEventClassMouse, kEventMouseDown};
        InstallApplicationEventHandler(NewEventHandlerUPP(CEvtHandleApplicationMenus),
            1, &commandEvents, 0, NULL);
        InstallApplicationEventHandler(NewEventHandlerUPP(CEvtHandleApplicationMenuClick),
            1, &menuEvents, 0, NULL);
}

void CreateWindows(void)
{
	int l, t, r, b;
	Rect winRect;
	Rect screenBitsBounds;
        
	EventTypeSpec eventClick =  { kEventClassWindow, kEventWindowHandleContentClick };
	EventTypeSpec eventUpdate = { kEventClassWindow, kEventWindowDrawContent };
	EventTypeSpec keyboardEvents[3] = {
            { kEventClassKeyboard, kEventRawKeyDown }, { kEventClassKeyboard, kEventRawKeyRepeat },
            { kEventClassKeyboard, kEventRawKeyUp }};

	// Create a backdrop window for fullscreen mode
        GetRegionBounds(GetGrayRgn(), &screenBitsBounds);
	SetRect(&winRect, 0, 0, screenBitsBounds.right, screenBitsBounds.bottom + GetMBarHeight());
	CreateNewWindow(kDocumentWindowClass, (kWindowStandardHandlerAttribute ), &winRect, &backdrop);
	if (backdrop == NULL)
		{BX_PANIC(("mac: can't create backdrop window"));}
	InstallWindowEventHandler(backdrop, NewEventHandlerUPP(CEvtHandleWindowBackdropUpdate), 1, &eventUpdate, NULL, NULL);
	InstallWindowEventHandler(backdrop, NewEventHandlerUPP(CEvtHandleWindowEmulatorClick), 1, &eventClick, NULL, NULL);
	InstallWindowEventHandler(backdrop, NewEventHandlerUPP(CEvtHandleWindowEmulatorKeys), 3, keyboardEvents, 0, NULL);
	
	width = 640;
	height = 480;
	gLeft = 4;
	gMinTop = 44;
	gMaxTop = 44 + gheaderbar_y;
	
	l = (screenBitsBounds.right - width) /2;  //(qd.screenBits.bounds.right - width)/2;
	r = l + width;
	t = (screenBitsBounds.bottom - height)/2;
	b = t + height;
	
	// Create a moveable tool window for the "headerbar"
	SetRect(&winRect, 0, 20, screenBitsBounds.right , 22+gheaderbar_y); //qd.screenBits.bounds.right, 22+gheaderbar_y);
	CreateNewWindow(kFloatingWindowClass, kWindowStandardHandlerAttribute ,&winRect, &toolwin);
	if (toolwin == NULL)
		{BX_PANIC(("mac: can't create tool window"));}

	SetWindowTitleWithCFString (toolwin, CFSTR("MacBochs 586")); // Set title
	InstallWindowEventHandler(toolwin, NewEventHandlerUPP(CEvtHandleWindowToolClick),  1, &eventClick, NULL, NULL);
	InstallWindowEventHandler(toolwin, NewEventHandlerUPP(CEvtHandleWindowToolUpdate), 1, &eventUpdate, NULL, NULL);

	// Create the emulator window for full screen mode
	SetRect(&winRect, l, t, r, b);
	CreateNewWindow(kPlainWindowClass, (kWindowStandardHandlerAttribute), &winRect, &fullwin);
	if (fullwin == NULL)
		BX_PANIC(("mac: can't create fullscreen emulator window"));

	InstallWindowEventHandler(fullwin, NewEventHandlerUPP(CEvtHandleWindowEmulatorUpdate), 1, &eventUpdate, NULL, NULL);
	InstallWindowEventHandler(fullwin, NewEventHandlerUPP(CEvtHandleWindowEmulatorClick), 1, &eventClick, NULL, NULL);	InstallWindowEventHandler(fullwin, NewEventHandlerUPP(CEvtHandleWindowEmulatorKeys), 3, keyboardEvents, 0, NULL);

	// Create the regular emulator window
	SetRect(&winRect, gLeft, gMaxTop, gLeft+width, gMaxTop+height);
	CreateNewWindow(kDocumentWindowClass,
            (kWindowStandardHandlerAttribute | kWindowCollapseBoxAttribute),
            &winRect, &win);
	if (win == NULL)
		BX_PANIC(("mac: can't create emulator window"));
	
	SetWindowTitleWithCFString (win, CFSTR("MacBochs 586")); // Set title
	InstallWindowEventHandler(win, NewEventHandlerUPP(CEvtHandleWindowEmulatorUpdate), 1, &eventUpdate, NULL, NULL);
	InstallWindowEventHandler(win, NewEventHandlerUPP(CEvtHandleWindowEmulatorClick), 1, &eventClick, NULL, NULL);
	InstallWindowEventHandler(win, NewEventHandlerUPP(CEvtHandleWindowEmulatorKeys), 3, keyboardEvents, 0, NULL);

	// Group the fullscreen and backdrop windows together, since they also share the same click
	// event handler they will effectively act as a single window for layering and events

	CreateWindowGroup((kWindowGroupAttrLayerTogether | kWindowGroupAttrSharedActivation), &fullwinGroup);
	SetWindowGroupName(fullwinGroup, CFSTR("net.sourceforge.bochs.windowgroups.fullscreen"));

	// This *can't* be the right way, then again groups aren't yet the right way
	// For the life of me I couldn't find a right way of making sure my created group stayed
	// below the layer of Floating Windows. But with the windows we have there's no current
	// harm from making it part of the same group.
	SetWindowGroup(toolwin, fullwinGroup);
	SetWindowGroup(fullwin, fullwinGroup);
	SetWindowGroup(backdrop, fullwinGroup);

	FixWindow();

	hidden = fullwin;
	
	ShowWindow(toolwin);
	ShowWindow(win);
	HiliteWindow(win, true);
	
	SetPort(GetWindowPort(win));
}

// ::SPECIFIC_INIT()
//
// Called from gui.cc, once upon program startup, to allow for the
// specific GUI code (X11, BeOS, ...) to be initialized.
//
// th: a 'this' pointer to the gui class.  If a function external to the
//     class needs access, store this pointer and use later.
// argc, argv: not used right now, but the intention is to pass native GUI
//     specific options from the command line.  (X11 options, BeOS options,...)
//
// tilewidth, tileheight: for optimization, graphics_tile_update() passes
//     only updated regions of the screen to the gui code to be redrawn.
//     These define the dimensions of a region (tile).
// headerbar_y:  A headerbar (toolbar) is display on the top of the
//     VGA window, showing floppy status, and other information.  It
//     always assumes the width of the current VGA mode width, but
//     it's height is defined by this parameter.

void bx_gui_c::specific_init(bx_gui_c *th, int argc, char **argv, unsigned tilewidth, unsigned tileheight,
										 unsigned headerbar_y)
{	
	th->put("MGUI");
	InitToolbox();
	
	//SouixWin = FrontWindow();
	
	atexit(MacPanic);
	
	thisGUI = th;
	gheaderbar_y = headerbar_y;
	
	CreateKeyMap();

	gCTable = GetCTable(128);
	BX_ASSERT (gCTable != NULL);
	(*gCTable)->ctSeed = GetCTSeed();	
	SetRect(&srcTextRect, 0, 0, FONT_WIDTH, FONT_HEIGHT);
	SetRect(&srcTileRect, 0, 0, tilewidth, tileheight);
	
	CreateMenus();
	CreateVGAFont();
	CreateTile();
	CreateWindows();
	
	EventTypeSpec mouseUpEvent = { kEventClassMouse, kEventMouseUp };
	EventTypeSpec mouseMoved[2] = { { kEventClassMouse, kEventMouseMoved },
            { kEventClassMouse, kEventMouseDragged } };
	InstallApplicationEventHandler(NewEventHandlerUPP(CEvtHandleApplicationMouseUp),
            1, &mouseUpEvent, 0, NULL);
	InstallApplicationEventHandler(NewEventHandlerUPP(CEvtHandleApplicationMouseMoved),
            2, mouseMoved, 0, NULL);

        GetCurrentProcess(&gProcessSerNum);
        
	GetMouse(&prevPt);
	
	UNUSED(argc);
	UNUSED(argv);

	//HideWindow(SouixWin);
	
}

// HandleKey()
//
// Handles keyboard-related events.

BX_CPP_INLINE void HandleKey(EventRecord *event, Bit32u keyState)
{
	int	key;
	UInt32 trans;
	static UInt32 transState = 0;
	
	key = event->message & charCodeMask;
	
//	if (event->modifiers & cmdKey)
//	{
//                // Like MenuSelect, MenuKey also triggers a cascade of
//                // events that results in sending a command event
//                MenuKey(key);
//	}	
//	else if (FrontWindow() == SouixWin)
//	{
//		SIOUXHandleOneEvent(event);
//	}
//	else
//	{		
		if (event->modifiers & shiftKey)
			bx_devices.keyboard->gen_scancode(BX_KEY_SHIFT_L | keyState);
		if (event->modifiers & controlKey)
			bx_devices.keyboard->gen_scancode(BX_KEY_CTRL_L | keyState);
		if (event->modifiers & optionKey)
			bx_devices.keyboard->gen_scancode(BX_KEY_ALT_L | keyState);
		
		key = (event->message & keyCodeMask) >> 8;
		
		trans = KeyTranslate(KCHR, key, &transState);
		
		// KeyTranslate maps Mac virtual key codes to any type of character code
		// you like (in this case, Bochs key codes). Much nicer than a huge switch
		// statement!
		
		if (trans > 0)
			bx_devices.keyboard->gen_scancode(trans | keyState);

		if (event->modifiers & shiftKey)
			bx_devices.keyboard->gen_scancode(BX_KEY_SHIFT_L | BX_KEY_RELEASED);
		if (event->modifiers & controlKey)
			bx_devices.keyboard->gen_scancode(BX_KEY_CTRL_L | BX_KEY_RELEASED);
		if (event->modifiers & optionKey)
			bx_devices.keyboard->gen_scancode(BX_KEY_ALT_L | BX_KEY_RELEASED);
//	}		
}

// HandleToolClick()
//
// Handles mouse clicks in the Bochs tool window

BX_CPP_INLINE void HandleToolClick(Point where)
{
	unsigned i;
	int xorigin;
	Rect bounds;
        Rect toolwinRect;
        
	GetWindowPortBounds (toolwin, &toolwinRect);
        
	SetPort(GetWindowPort(toolwin));
	GlobalToLocal(&where);
	for (i=0; i<toolPixMaps; i++)
	{
		if (bx_tool_pixmap[i].alignment == BX_GRAVITY_LEFT)
			xorigin = bx_tool_pixmap[i].xorigin;
		else
			xorigin = toolwinRect.right - bx_tool_pixmap[i].xorigin;
                        
		SetRect(&bounds, xorigin, 0, xorigin+32, 32);
		if (PtInRect(where, &bounds))
			bx_tool_pixmap[i].f();
	}
	thisGUI->show_headerbar();
}

BX_CPP_INLINE void ResetPointer(void)
{
    // this appears to work well, especially when combined with
    // mouse processing on the MouseMoved events
    if(CGWarpMouseCursorPosition(CGPointMake(scrCenter.h, scrCenter.v)))
    {
        fprintf(stderr, "# Failed to warp cursor");
    }
}

// ::HANDLE_EVENTS()
//
// Called periodically (vga_update_interval in .bochsrc) so the
// the gui code can poll for keyboard, mouse, and other
// relevant events.

void bx_gui_c::handle_events(void)
{
	EventRecord	event;
	Point	mousePt;
	int dx, dy;
	int oldMods=0;
	unsigned curstate;
	GrafPtr	oldport;

	curstate = mouse_button_state;			//so we can compare the old and the new mouse state

	if (WaitNextEvent(everyEvent, &event, SLEEP_TIME, NULL))
	{
		switch(event.what)
		{
/*
			// This event is just redundant
			case nullEvent:
                        
			// These events are all covered by installed carbon event handlers
			case mouseDown:
			case mouseUp:
			case keyDown:
			case autoKey:
			case keyUp:
			case updateEvt:
				break;
*/				
			case diskEvt:
			//	floppyA_handler();
                                break;
                                
                        case kHighLevelEvent:
                            fprintf(stderr, "# Classic apple event handler called\n");
                            AEProcessAppleEvent(&event);
				
			default:
				break;
		}
	}
	
        // Only update mouse if we're not in the dock
        // and we are the frontmost app.
        ProcessSerialNumber frontProcessSerNum;
        Boolean isSameProcess;
        
        GetFrontProcess(&frontProcessSerNum);
        SameProcess(&frontProcessSerNum, &gProcessSerNum, &isSameProcess);

        if(isSameProcess && !IsWindowCollapsed(win))
        {
            GetPort(&oldport);
            SetPort(GetWindowPort(win));
            
            GetMouse(&mousePt);
            
            if(menubarVisible && cursorVisible)
            {
                // Don't track the mouse if we're working with the main window
                // and we're outside the window
                if(mouseMoved &&
                    (mousePt.v < 0 || mousePt.v > height || mousePt.h < 0 || mousePt.h > width) &&
                    (prevPt.v < 0 || prevPt.v > height || prevPt.h < 0 || prevPt.h > width))
                {
                    mouseMoved = false;
                }
/*
                // Limit mouse action to window
                // Grr, any better ways to sync host and bochs cursor?
                if(mousePt.h < 0) { mousePt.h = 0; }
                else if(mousePt.h > width) { mousePt.h = width; }
                if(mousePt.v < 0) { mousePt.v = 0; }
                else if(mousePt.v > height) { mousePt.v = height; }
*/
            }
            
            //if mouse has moved, or button has changed state
            if (mouseMoved || (curstate != mouse_button_state))
            {
                    if(mouseMoved)
                    {
                            CGMouseDelta CGdX, CGdY;
                            CGGetLastMouseDelta( &CGdX, &CGdY );
                            dx = CGdX;
                            dy = - CGdY; // Windows has an opposing grid
                    }
                    else
                    {
                            dx = 0;
                            dy = 0;
                    }
                    
                    bx_devices.keyboard->mouse_motion(dx, dy, mouse_button_state);
                    
                    if (!cursorVisible && mouseMoved)
                    {
                            SetPt(&scrCenter, 300, 240);
                            LocalToGlobal(&scrCenter);
                            ResetPointer();		//next getmouse should be 300, 240
                            SetPt(&mousePt, 300, 240);
                    }
                    mouseMoved = false;
            }
    
            prevPt = mousePt;
            
            SetPort(oldport);
    }
}


// ::FLUSH()
//
// Called periodically, requesting that the gui code flush all pending
// screen update requests.

void bx_gui_c::flush(void)
{
	// an opportunity to make the Window Manager happy.
	// not needed on the macintosh....
        
	// Unless you don't want to needlessly update the dock icon 
	// umpteen zillion times a second for each tile.
        // A further note, UpdateCollapsedWindowDockTile is not
        // recommended for animation. Setup like this my performance
        // seems reasonable for little fuss.
	if(windowUpdatesPending && IsWindowCollapsed(win))
	{
            UpdateCollapsedWindowDockTile(win);
	}
	windowUpdatesPending = false;
}


// ::CLEAR_SCREEN()
//
// Called to request that the VGA region is cleared.	Don't
// clear the area that defines the headerbar.

void bx_gui_c::clear_screen(void)
{
        Rect r;
        Pattern qdBlackPattern;
        
        GetQDGlobalsBlack(&qdBlackPattern);
        
	SetPort(GetWindowPort(win));
	
	RGBForeColor(&black);
	RGBBackColor(&white);
	GetWindowPortBounds(win, &r);
        FillRect (&r, &qdBlackPattern);
        
	windowUpdatesPending = true;
}



// ::TEXT_UPDATE()
//
// Called in a VGA text mode, to update the screen with
// new content.
//
// old_text: array of character/attributes making up the contents
//           of the screen from the last call.  See below
// new_text: array of character/attributes making up the current
//           contents, which should now be displayed.  See below
//
// format of old_text & new_text: each is 80*nrows*2 bytes long.
//     This represents 80 characters wide by 'nrows' high, with
//     each character being 2 bytes.  The first by is the
//     character value, the second is the attribute byte.
//     I currently don't handle the attribute byte.
//
// cursor_x: new x location of cursor
// cursor_y: new y location of cursor

void bx_gui_c::text_update(Bit8u *old_text, Bit8u *new_text,
											unsigned long cursor_x, unsigned long cursor_y,
         Bit16u cursor_state, unsigned nrows)
{
	int				i;
	unsigned char		achar;
	int				x, y;
	static int previ;
	int				cursori;
	Rect			destRect;
	RGBColor	fgColor, bgColor;
	GrafPtr		oldPort;
        GrafPtr winGrafPtr = GetWindowPort(win);
	unsigned nchars;
	
	GetPort(&oldPort);
	
	SetPort(GetWindowPort(win));

//current cursor position
	cursori = (cursor_y*80 + cursor_x)*2;

	// Number of characters on screen, variable number of rows
	nchars = 80*nrows;
	
	for (i=0; i<nchars*2; i+=2)
	{
		if ( i == cursori || i == previ || new_text[i] != old_text[i] || new_text[i+1] != old_text[i+1])
		{
			achar = new_text[i];
			
//			fgColor = (**gCTable).ctTable[new_text[i+1] & 0x0F].rgb;
//			bgColor = (**gCTable).ctTable[(new_text[i+1] & 0xF0) >> 4].rgb;
			
//			RGBForeColor(&fgColor);
//			RGBBackColor(&bgColor);
			
			if (bx_options.Oprivate_colormap->get ())
			{
				PmForeColor(new_text[i+1] & 0x0F);
				PmBackColor((new_text[i+1] & 0xF0) >> 4);
			}
			else
			{
				fgColor = (**gCTable).ctTable[new_text[i+1] & 0x0F].rgb;
				bgColor = (**gCTable).ctTable[(new_text[i+1] & 0xF0) >> 4].rgb;
			
				RGBForeColor(&fgColor);
				RGBBackColor(&bgColor);
			}
			
			x = ((i/2) % 80)*FONT_WIDTH;
			y = ((i/2) / 80)*FONT_HEIGHT;

			SetRect(&destRect, x, y,
				x+FONT_WIDTH, y+FONT_HEIGHT);
				
			CopyBits( vgafont[achar], WINBITMAP(win),
				&srcTextRect, &destRect, srcCopy, NULL);

			if (i == cursori)		//invert the current cursor block
			{
				InvertRect(&destRect);
			}
			
		}
	}
 
//previous cursor position
	previ = cursori;
	
	SetPort(oldPort);

	windowUpdatesPending = true;
}


// ::PALETTE_CHANGE()
//
// Allocate a color in the native GUI, for this color, and put
// it in the colormap location 'index'.
// returns: 0=no screen update needed (color map change has direct effect)
//          1=screen updated needed (redraw using current colormap)

Boolean bx_gui_c::palette_change(unsigned index, unsigned red, unsigned green, unsigned blue)
{
	PaletteHandle	thePal, oldpal;
	GDHandle	saveDevice;
	CGrafPtr	savePort;
	
	if (bx_options.Oprivate_colormap->get ())
	{
		GetGWorld(&savePort, &saveDevice);

		SetGWorld(gOffWorld, NULL);
	}

	(**gCTable).ctTable[index].value = index;
	(**gCTable).ctTable[index].rgb.red = (red << 8);
	(**gCTable).ctTable[index].rgb.green = (green << 8);
	(**gCTable).ctTable[index].rgb.blue = (blue << 8);

	SetEntries(index, index, (**gCTable).ctTable);
	
	CTabChanged(gCTable);
	
	if (bx_options.Oprivate_colormap->get ())
	{
		SetGWorld(savePort, saveDevice);
	
		thePal = NewPalette(index, gCTable, pmTolerant, 0x5000);
		oldpal = GetPalette(win);
	
		SetPalette(win, thePal, false);
		SetPalette(fullwin, thePal, false);
		SetPalette(hidden, thePal, false);
	}

	return(0);
}


// ::GRAPHICS_TILE_UPDATE()
//
// Called to request that a tile of graphics be drawn to the
// screen, since info in this region has changed.
//
// tile: array of 8bit values representing a block of pixels with
//       dimension equal to the 'tilewidth' & 'tileheight' parameters to
//       ::specific_init().  Each value specifies an index into the
//       array of colors you allocated for ::palette_change()
// x0: x origin of tile
// y0: y origin of tile
//
// note: origin of tile and of window based on (0,0) being in the upper
//       left of the window.

void bx_gui_c::graphics_tile_update(Bit8u *tile, unsigned x0, unsigned y0)
{
	Rect					destRect;
/*	GDHandle	saveDevice;
	CGrafPtr	savePort;
	
	GetGWorld(&savePort, &saveDevice);

	SetGWorld(gOffWorld, NULL);	*/
        
	// SetPort - Otherwise an update happens to the headerbar and ooomph, we're drawing weirdly on the screen
	SetPort(GetWindowPort(win));
	destRect = srcTileRect;
	OffsetRect(&destRect, x0, y0);
	
	(**gTile).baseAddr = (Ptr)tile;	
	
	CopyBits( & (** ((BitMapHandle)gTile) ), WINBITMAP(win),
						&srcTileRect, &destRect, srcCopy, NULL);

	windowUpdatesPending = true;
//	SetGWorld(savePort, saveDevice);
}



// ::DIMENSION_UPDATE()
//
// Called when the VGA mode changes it's X,Y dimensions.
// Resize the window to this size, but you need to add on
// the height of the headerbar to the Y value.
//
// x: new VGA x size
// y: new VGA y size (add headerbar_y parameter from ::specific_init().

void bx_gui_c::dimension_update(unsigned x, unsigned y)
{
	if (x != width || y != height)
	{
#if 1
		SizeWindow(win, x, y, false);
#endif

#if 0
		// Animates the resizing, cute, but gratuitous
		Rect box, frame;
		GetWindowBounds(win, kWindowStructureRgn, &frame);
		GetWindowPortBounds(win, &box);
		frame.right = frame.right - box.right + x;
		frame.bottom = frame.bottom - box.bottom + y;

		TransitionWindow(win, kWindowSlideTransitionEffect, kWindowResizeTransitionAction, &frame);
#endif
		SizeWindow(fullwin, x, y, false);
		SizeWindow(hidden, x, y, false);
		width = x;
		height = y;
	}
        
	windowUpdatesPending = true;
}


// ::CREATE_BITMAP()
//
// Create a monochrome bitmap of size 'xdim' by 'ydim', which will
// be drawn in the headerbar.  Return an integer ID to the bitmap,
// with which the bitmap can be referenced later.
//
// bmap: packed 8 pixels-per-byte bitmap.  The pixel order is:
//       bit0 is the left most pixel, bit7 is the right most pixel.
// xdim: x dimension of bitmap
// ydim: y dimension of bitmap

// rewritten by tim senecal to use the cicn (color icon) resources instead

unsigned bx_gui_c::create_bitmap(const unsigned char *bmap, unsigned xdim, unsigned ydim)
{
	unsigned i;
	unsigned char *data;
	long row_bytes, bytecount;
	
	bx_cicn[numPixMaps] = GetCIcon(numPixMaps+128);
	
	numPixMaps++;

	return(numPixMaps-1);
}


// ::HEADERBAR_BITMAP()
//
// Called to install a bitmap in the bochs headerbar (toolbar).
//
// bmap_id: will correspond to an ID returned from
//     ::create_bitmap().  'alignment' is either BX_GRAVITY_LEFT
//     or BX_GRAVITY_RIGHT, meaning install the bitmap in the next
//     available leftmost or rightmost space.
// f: a 'C' function pointer to callback when the mouse is clicked in
//     the boundaries of this bitmap.

unsigned bx_gui_c::headerbar_bitmap(unsigned bmap_id, unsigned alignment, void (*f)(void))
{
	unsigned hb_index;
	
	toolPixMaps++;
	hb_index = toolPixMaps-1;
//	bx_tool_pixmap[hb_index].pm = bx_pixmap[bmap_id];
	bx_tool_pixmap[hb_index].cicn = bx_cicn[bmap_id];
	bx_tool_pixmap[hb_index].alignment = alignment;
	bx_tool_pixmap[hb_index].f = f;

	if (alignment == BX_GRAVITY_LEFT)
	{
		bx_tool_pixmap[hb_index].xorigin = bx_bitmap_left_xorigin;
		bx_tool_pixmap[hb_index].yorigin = 0;
//		bx_bitmap_left_xorigin += (**bx_pixmap[bmap_id]).bounds.right;
		bx_bitmap_left_xorigin += 34;
	}
	else
	{
//		bx_bitmap_right_xorigin += (**bx_pixmap[bmap_id]).bounds.right;
		bx_bitmap_right_xorigin += 34;
		bx_tool_pixmap[hb_index].xorigin = bx_bitmap_right_xorigin;
		bx_tool_pixmap[hb_index].yorigin = 0;
	}
	return(hb_index);
}


// ::SHOW_HEADERBAR()
//
// Show (redraw) the current headerbar, which is composed of
// currently installed bitmaps.

void bx_gui_c::show_headerbar(void)
{
	Rect	destRect;
        Rect r;
        GetWindowPortBounds(toolwin, &r);
	int		i, xorigin;
	Pattern qdBlackPattern;
        
        GetQDGlobalsBlack(&qdBlackPattern);
        
	SetPort(GetWindowPort(toolwin));
	RGBForeColor(&medGrey);
	FillRect(&r, &qdBlackPattern); //&qd.black);
        
	for (i=0; i<toolPixMaps; i++)
	{
		if (bx_tool_pixmap[i].alignment == BX_GRAVITY_LEFT)
			xorigin = bx_tool_pixmap[i].xorigin;
		else
			xorigin = r.right - bx_tool_pixmap[i].xorigin;
			
		SetRect(&destRect, xorigin, 0, xorigin+32, 32);
		
		//changed, simply plot the cicn for that button, in the prescribed rectangle
		PlotCIcon(&destRect, bx_tool_pixmap[i].cicn);		
	}
	RGBForeColor(&black);

}


// ::REPLACE_BITMAP()
//
// Replace the bitmap installed in the headerbar ID slot 'hbar_id',
// with the one specified by 'bmap_id'.  'bmap_id' will have
// been generated by ::create_bitmap().  The old and new bitmap
// must be of the same size.  This allows the bitmap the user
// sees to change, when some action occurs.  For example when
// the user presses on the floppy icon, it then displays
// the ejected status.
//
// hbar_id: headerbar slot ID
// bmap_id: bitmap ID

void bx_gui_c::replace_bitmap(unsigned hbar_id, unsigned bmap_id)
{
//	bx_tool_pixmap[hbar_id].pm = bx_pixmap[bmap_id];
	bx_tool_pixmap[hbar_id].cicn = bx_cicn[bmap_id];
	show_headerbar();
}

 
// ::EXIT()
//
// Called before bochs terminates, to allow for a graceful
// exit from the native GUI mechanism.

void bx_gui_c::exit(void)
{
	if (!menubarVisible)
		ShowMenubar(); // Make the menubar visible again
	InitCursor();
}

#if 0
void bx_gui_c::snapshot_handler(void)
{
	PicHandle	ScreenShot;
	long val;
	
	SetPort(GetWindowPort(win));
	
	ScreenShot = OpenPicture(&win->portRect);
	
	CopyBits(&win->portBits, &win->portBits, &win->portRect, &win->portRect, srcCopy, NULL);
	
	ClosePicture();
	
	val = ZeroScrap();
	
	HLock((Handle)ScreenShot);
	PutScrap(GetHandleSize((Handle)ScreenShot), 'PICT', *ScreenShot);
	HUnlock((Handle)ScreenShot);
	
	KillPicture(ScreenShot);
}
#endif

// HidePointer()
//
// Hides the Mac mouse pointer

void HidePointer()
{
	HiliteMenu(0);
	HideCursor();
	SetPort(GetWindowPort(win));
	SetPt(&scrCenter, 300, 240);
	LocalToGlobal(&scrCenter);
	ResetPointer();
	GetMouse(&prevPt);
	cursorVisible = false;
	CheckMenuItem(GetMenuHandle(mBochs), iCursor, false); 
}

// ShowPointer()
//
// Shows the Mac mouse pointer

void ShowPointer()
{
	InitCursor();
	cursorVisible = true;
	CheckMenuItem(GetMenuHandle(mBochs), iCursor, true);
      //CheckItem(GetMenuHandle(mBochs), iCursor, true);
}

// HideTools()
//
// Hides the Bochs toolbar

void HideTools()
{
	HideWindow(toolwin);
	if (menubarVisible)
	{
		MoveWindow(win, gLeft, gMinTop, false);
	}
	else
	{
		MoveWindow(hidden, gLeft, gMinTop, false);
	}
	CheckMenuItem(GetMenuHandle(mBochs), iTool, false);
	HiliteWindow(win, true);
}

// ShowTools()
//
// Shows the Bochs toolbar

void ShowTools()
{
	if (menubarVisible)
	{
		MoveWindow(win, gLeft, gMaxTop, false);
	}
	else
	{
		MoveWindow(hidden, gLeft, gMaxTop, false);
	}
	ShowWindow(toolwin);
	BringToFront(toolwin);
	SelectWindow(toolwin);
	HiliteWindow(win, true);
//	thisGUI->show_headerbar();
	CheckMenuItem(GetMenuHandle(mBochs), iTool, true);
	HiliteWindow(win, true);
}

// HideMenubar()
//
// Hides the menubar (obviously)

void HideMenubar()
{
        HideMenuBar();
        
	HideWindow(win);
	ShowWindow(backdrop);
	hidden = win;
	win = fullwin;
	ShowWindow(win);

	SelectWindow(win);
	menubarVisible = false;
	CheckMenuItem(GetMenuHandle(mBochs), iMenuBar, false);
}

// ShowMenubar()
//
// Makes the menubar visible again so other programs will display correctly.

void ShowMenubar()
{
        HideWindow(backdrop);
	win = hidden;
	hidden = fullwin;
	HideWindow(hidden);
	ShowWindow(win);
	HiliteWindow(win, true);
	
        ShowMenuBar();
        
	menubarVisible = true;
	CheckMenuItem(GetMenuHandle(mBochs), iMenuBar, true);
}

void HideConsole()
{
	HideWindow(SouixWin);
	CheckMenuItem(GetMenuHandle(mBochs), iConsole, false);
}

void ShowConsole()
{
	ShowWindow(SouixWin);
	SelectWindow(SouixWin);
	CheckMenuItem(GetMenuHandle(mBochs), iConsole, true);
}

// CreateKeyMap()
//
// Create a KCHR data structure to map Mac virtual key codes to Bochs key codes

void CreateKeyMap(void)
{
	const unsigned char KCHRHeader [258] = {
		0,
		1
	};

	const unsigned char KCHRTable [130] = {
		0,
		1,
		BX_KEY_A,
		BX_KEY_S,
		BX_KEY_D,
		BX_KEY_F,
		BX_KEY_H,
		BX_KEY_G,
		BX_KEY_Z,
		BX_KEY_X,
		BX_KEY_C,
		BX_KEY_V,
		0,
		BX_KEY_B,
		BX_KEY_Q,
		BX_KEY_W,
		BX_KEY_E,
		BX_KEY_R,
		BX_KEY_Y,
		BX_KEY_T,
		BX_KEY_1,
		BX_KEY_2,
		BX_KEY_3,
		BX_KEY_4,
		BX_KEY_6,
		BX_KEY_5,
		BX_KEY_EQUALS,
		BX_KEY_9,
		BX_KEY_7,
		BX_KEY_MINUS,
		BX_KEY_8,
		BX_KEY_0,
		BX_KEY_RIGHT_BRACKET,
		BX_KEY_O,
		BX_KEY_U,
		BX_KEY_LEFT_BRACKET,
		BX_KEY_I,
		BX_KEY_P,
		BX_KEY_ENTER,
		BX_KEY_L,
		BX_KEY_J,
		BX_KEY_SINGLE_QUOTE,
		BX_KEY_K,
		BX_KEY_SEMICOLON,
		BX_KEY_BACKSLASH,
		BX_KEY_COMMA,
		BX_KEY_SLASH,
		BX_KEY_N,
		BX_KEY_M,
		BX_KEY_PERIOD,
		BX_KEY_TAB,
		BX_KEY_SPACE,
		BX_KEY_GRAVE,
		BX_KEY_BACKSPACE,
		0,
		BX_KEY_ESC,
		0, // 0x36
		0, // 0x37
		0, // 0x38
		0, // 0x39
		0, // 0x3A
		0, // 0x3B
		0, // 0x3C
		0, // 0x3D
		0, // 0x3E
		0, // 0x3F
		0, // 0x40
		BX_KEY_PERIOD, // KP_PERIOD
		0, // 0x42
		BX_KEY_KP_MULTIPLY,
		0, // 0x44
		BX_KEY_KP_ADD,
		0, // 0x46
		BX_KEY_KP_DELETE,
		0, // 0x48
		0, // 0x49
		0, // 0x4A
		BX_KEY_KP_DIVIDE,
		BX_KEY_KP_ENTER,
		0, // 0x4D
		BX_KEY_KP_SUBTRACT,
		0, // 0x4F
		0, // 0x50
		0, // 0x51 (kp equals)
		0, // 0x52 (kp 0)
		0, // 0x53 (kp 1)
		BX_KEY_KP_DOWN, // 0x54 (kp 2)
		0, // 0x55 (kp 3)
		BX_KEY_KP_LEFT, // 0x56 (kp 4)
		BX_KEY_KP_5,
		BX_KEY_KP_RIGHT, // 0x58 (kp 6)
		0, // 0x59 (kp 7)
		0, // 0x5A
		BX_KEY_KP_UP, // 0x5B (kp 8)
		0, // 0x5C (kp 9)
		0, // 0x5D
		0, // 0x5E
		0, // 0x5F
		BX_KEY_F5,
		BX_KEY_F6,
		BX_KEY_F7,
		BX_KEY_F3,
		BX_KEY_F8,
		BX_KEY_F9,
		0, // 0x66
		BX_KEY_F11,
		0, // 0x68
		0, // 0x69 (print screen)
		0, // 0x6A
		0, // 0x6B (scroll lock)
		0, // 0x6C
		BX_KEY_F10,
		0, // 0x6E
		BX_KEY_F12,
		0, // 0x70
		0, // 0x71 (pause)
		BX_KEY_INSERT,
		BX_KEY_HOME,
		BX_KEY_PAGE_UP,
		BX_KEY_DELETE,
		BX_KEY_F4,
		BX_KEY_END,
		BX_KEY_F2,
		BX_KEY_PAGE_DOWN,
		BX_KEY_F1,
		BX_KEY_LEFT,
		BX_KEY_RIGHT,
		BX_KEY_DOWN,
		BX_KEY_UP
	};

	KCHR = NewPtrClear(390);
	if (KCHR == NULL)
            BX_PANIC(("mac: can't allocate memory for key map"));
                
	BlockMove(KCHRHeader, KCHR, sizeof(KCHRHeader));
	BlockMove(KCHRTable, Ptr(KCHR + sizeof(KCHRHeader)), sizeof(KCHRTable));
}

// CreateVGAFont()
//
// Create an array of PixMaps for the PC screen font

void CreateVGAFont(void)
{
	int i, x;
	unsigned char *fontData, curPixel;
	long row_bytes, bytecount;
	
	for (i=0; i<256; i++)
	{
		vgafont[i] = CreateBitMap(FONT_WIDTH, FONT_HEIGHT);
		row_bytes = (*(vgafont[i])).rowBytes;
		bytecount = row_bytes * FONT_HEIGHT;
		fontData = (unsigned char *)NewPtrClear(bytecount);

		for (x=0; x<16; x++)
		{
			//curPixel = ~(bx_vgafont[i].data[x]);
			curPixel = (bx_vgafont[i].data[x]);
			fontData[x*row_bytes] = reverse_bitorder(curPixel);
		}
		vgafont[i]->baseAddr = Ptr(fontData);
	}
}

// CreateBitMap()
// Allocate a new bitmap and fill in the fields with appropriate
// values.

BitMap *CreateBitMap(unsigned width,	unsigned height)
{
	BitMap		*bm;
	long			row_bytes;
	
	row_bytes = (( width + 31) >> 5) << 2;
	bm = (BitMap *)calloc(1, sizeof(BitMap));
	if (bm == NULL)
		BX_PANIC(("mac: can't allocate memory for pixmap"));
	SetRect(&bm->bounds, 0, 0, width, height);
	bm->rowBytes = row_bytes;
	// Quickdraw allocates a new color table by default, but we want to
	// use one we created earlier.
		
	return bm;
}

// CreatePixMap()
// Allocate a new pixmap handle and fill in the fields with appropriate
// values.

PixMapHandle CreatePixMap(unsigned left, unsigned top, unsigned width,
	unsigned height, unsigned depth, CTabHandle clut)
{
	PixMapHandle	pm;
	long					row_bytes;
	
	row_bytes = (((long) depth * ((long) width) + 31) >> 5) << 2;
	pm = NewPixMap();
	if (pm == NULL)
		BX_PANIC(("mac: can't allocate memory for pixmap"));
	(**pm).bounds.left = left;
	(**pm).bounds.top = top;
	(**pm).bounds.right = left+width;
	(**pm).bounds.bottom = top+height;
	(**pm).pixelSize = depth;
	(**pm).rowBytes = row_bytes | 0x8000;
	
  DisposeCTable((**pm).pmTable);
  (**pm).pmTable = clut;
  // Quickdraw allocates a new color table by default, but we want to
  // use one we created earlier.
  	
	return pm;
}

unsigned char reverse_bitorder(unsigned char b)
{
	unsigned char ret=0;
	
	for (unsigned i=0; i<8; i++)
	{
		ret |= (b & 0x01) << (7-i);
		b >>= 1;
	}
	
	return(ret);
}

  void
bx_gui_c::mouse_enabled_changed_specific (Boolean val)
{
}
