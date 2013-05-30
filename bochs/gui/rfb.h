/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2013  The Bochs Project
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

// This file includes the rfb spec header and the port numbers

#ifndef BX_RFB_GUI
#define BX_RFB_GUI

#ifdef HAVE_LIBVNCSERVER
#include <rfb/rfb.h>
#endif

// Define the RFB types
typedef Bit32u         U32;
typedef Bit16u         U16;
typedef Bit8u          U8;
typedef Bit32s         S32;
typedef Bit16s         S16;
typedef Bit8s          S8;


#ifndef HAVE_LIBVNCSERVER

// Port range
#define BX_RFB_PORT_MIN 5900
#define BX_RFB_PORT_MAX 5949


// RFB Protocol definitions.
// Please refer to the specification document
// http://www.realvnc.com/docs/rfbproto.pdf

typedef struct {
    U16 xPosition;
    U16 yPosition;
    U16 width;
    U16 height;
    U32 encodingType;
} rfbRectangle;

#define rfbRectangleSize (sizeof(rfbRectangle))


typedef struct {
    U8  bitsPerPixel;
    U8  depth;
    U8  bigEndianFlag;
    U8  trueColourFlag;
    U16 redMax;
    U16 greenMax;
    U16 blueMax;
    U8  redShift;
    U8  greenShift;
    U8  blueShift;
    U8  padding[3];
} rfbPixelFormat;

#define rfbPixelFormatSize (sizeof(rfbPixelFormat))


#define rfbProtocolVersionFormat "RFB %03d.%03d\n"
#define rfbServerProtocolMajorVersion 3
#define rfbServerProtocolMinorVersion 3

#define rfbProtocolVersionMessageSize 12
typedef char rfbProtocolVersionMessage[rfbProtocolVersionMessageSize + 1]; // Add 1 for null byte


#define rfbSecurityConnFailed 0
#define rfbSecurityNone 1
#define rfbSecurityVncAuth 2

#define rfbSecurityVncAuthOK 0
#define rfbSecurityVncAuthFailed 1
#define rfbSecurityVncAuthTooMany 2 // No longer used (3.7)


typedef struct {
    U8 sharedFlag;
} rfbClientInitMessage;

#define rfbClientInitMessageSize (sizeof(rfbClientInitMessage))


typedef struct {
    U16            framebufferWidth;
    U16            framebufferHeight;
    rfbPixelFormat serverPixelFormat;
    U32            nameLength;
    // U8[]        nameString;
} rfbServerInitMessage;

#define rfbServerInitMessageSize (sizeof(rfbServerInitMessage))

// Client to Server message types
#define rfbSetPixelFormat 0
#define rfbFixColourMapEntries 1
#define rfbSetEncodings 2
#define rfbFramebufferUpdateRequest 3
#define rfbKeyEvent 4
#define rfbPointerEvent 5
#define rfbClientCutText 6


// Client to Server messages

typedef struct {
	U8             messageType;
	U8             padding[3];
	rfbPixelFormat pixelFormat;
} rfbSetPixelFormatMessage;

#define rfbSetPixelFormatMessageSize (sizeof(rfbSetPixelFormatMessage))


typedef struct {
    U8  messageType;
    U8  padding;
    U16 firstColour;
    U16 numberOfColours;
} rfbFixColourMapEntriesMessage;

#define rfbFixColourMapEntriesMessageSize (sizeof(rfbFixColourMapEntriesMessage))


typedef struct {
	U8  messageType;
	U8  padding;
	U16 numberOfEncodings;
} rfbSetEncodingsMessage;

#define rfbSetEncodingsMessageSize (sizeof(rfbSetEncodingsMessage))


typedef struct {
	U8  messageType;
	U8  incremental;
	U16 xPosition;
	U16 yPosition;
	U16 width;
	U16 height;
} rfbFramebufferUpdateRequestMessage;

#define rfbFramebufferUpdateRequestMessageSize (sizeof(rfbFramebufferUpdateRequestMessage))


typedef struct {
	U8  messageType;
	U8  downFlag;
	U8  padding[2];
	U32 key;
} rfbKeyEventMessage;

#define rfbKeyEventMessageSize (sizeof(rfbKeyEventMessage))


typedef struct {
	U8  messageType;
	U8  buttonMask;
	U16 xPosition;
	U16 yPosition;
} rfbPointerEventMessage;

#define rfbPointerEventMessageSize (sizeof(rfbPointerEventMessage))

#define rfbButton1Mask 1
#define rfbButton2Mask 2
#define rfbButton3Mask 4


typedef struct {
	U8  messageType;
	U8  padding[3];
	U32 length;
	// U8[]  text;
} rfbClientCutTextMessage;

#define rfbClientCutTextMessageSize (sizeof(rfbClientCutTextMessage))

// Fill in correct names
typedef union {
	rfbSetPixelFormatMessage           spf;
	rfbFixColourMapEntriesMessage      fcme;
	rfbSetEncodingsMessage             se;
	rfbFramebufferUpdateRequestMessage fur;
	rfbKeyEventMessage                 ke;
	rfbPointerEventMessage             pe;
	rfbClientCutTextMessage            cct;
} rfbClientToServerMessage;


// Server to Client message types
#define rfbFramebufferUpdate   0
#define rfbSetColourMapEntries 1
#define rfbBell                2
#define rfbServerCutText       3

// Encoding types
#define rfbEncodingRaw         0
#define rfbEncodingCopyRect    1
#define rfbEncodingRRE         2
#define rfbEncodingCoRRE       4
#define rfbEncodingHextile     5
#define rfbEncodingZRLE       16
#define rfbEncodingCursor      0xffffff11
#define rfbEncodingDesktopSize 0xffffff21
#define rfbEncodingZlib        6
#define rfbEncodingTight       7
#define rfbEncodingZlibHex     8
#define rfbEncodingTightOption00 0xffffff00  // to 0xffffffff except 0xffffff11 and 0xffffff21
#define rfbEncodingTightOption01 0xffffff01
#define rfbEncodingTightOption02 0xffffff02
#define rfbEncodingTightOption03 0xffffff03
#define rfbEncodingTightOption04 0xffffff04
#define rfbEncodingTightOption05 0xffffff05
#define rfbEncodingTightOption06 0xffffff06
#define rfbEncodingTightOption07 0xffffff07
#define rfbEncodingTightOption08 0xffffff08
#define rfbEncodingTightOption09 0xffffff09
#define rfbEncodingTightOption0a 0xffffff0a
#define rfbEncodingTightOption0b 0xffffff0b
#define rfbEncodingTightOption0c 0xffffff0c
#define rfbEncodingTightOption0d 0xffffff0d
#define rfbEncodingTightOption0e 0xffffff0e
#define rfbEncodingTightOption0f 0xffffff0f
#define rfbEncodingTightOption10 0xffffff10
#define rfbEncodingTightOption12 0xffffff12
#define rfbEncodingTightOption13 0xffffff13
#define rfbEncodingTightOption14 0xffffff14
#define rfbEncodingTightOption15 0xffffff15
#define rfbEncodingTightOption16 0xffffff16
#define rfbEncodingTightOption17 0xffffff17
#define rfbEncodingTightOption18 0xffffff18
#define rfbEncodingTightOption19 0xffffff19
#define rfbEncodingTightOption1a 0xffffff1a
#define rfbEncodingTightOption1b 0xffffff1b
#define rfbEncodingTightOption1c 0xffffff1c
#define rfbEncodingTightOption1d 0xffffff1d
#define rfbEncodingTightOption1e 0xffffff1e
#define rfbEncodingTightOption1f 0xffffff1f
#define rfbEncodingTightOption20 0xffffff20

typedef struct {
        U32 id;
        const char *name;
} rfbEncodingType;

rfbEncodingType rfbEncodings[] = {
        {rfbEncodingRaw, "Raw"},
        {rfbEncodingCopyRect, "CopyRect"},
        {rfbEncodingRRE, "RRE"},
        {rfbEncodingCoRRE, "CoRRE"},
        {rfbEncodingHextile, "Hextile"},
        {rfbEncodingZRLE, "ZRLE"},
        {rfbEncodingCursor, "Cursor"},
        {rfbEncodingDesktopSize, "DesktopSize"},
        {rfbEncodingZlib, "Zlib"},
        {rfbEncodingTight, "Tight"},
        {rfbEncodingZlibHex, "ZlibHex"},
        {rfbEncodingTightOption00, "TightOption00"},
        {rfbEncodingTightOption01, "TightOption01"},
        {rfbEncodingTightOption02, "TightOption02"},
        {rfbEncodingTightOption03, "TightOption03"},
        {rfbEncodingTightOption04, "TightOption04"},
        {rfbEncodingTightOption05, "TightOption05"},
        {rfbEncodingTightOption06, "TightOption06"},
        {rfbEncodingTightOption07, "TightOption07"},
        {rfbEncodingTightOption08, "TightOption08"},
        {rfbEncodingTightOption09, "TightOption09"},
        {rfbEncodingTightOption0a, "TightOption0a"},
        {rfbEncodingTightOption0b, "TightOption0b"},
        {rfbEncodingTightOption0c, "TightOption0c"},
        {rfbEncodingTightOption0d, "TightOption0d"},
        {rfbEncodingTightOption0e, "TightOption0e"},
        {rfbEncodingTightOption0f, "TightOption0f"},
        {rfbEncodingTightOption10, "TightOption10"},
        {rfbEncodingTightOption12, "TightOption12"},
        {rfbEncodingTightOption13, "TightOption13"},
        {rfbEncodingTightOption14, "TightOption14"},
        {rfbEncodingTightOption15, "TightOption15"},
        {rfbEncodingTightOption16, "TightOption16"},
        {rfbEncodingTightOption17, "TightOption17"},
        {rfbEncodingTightOption18, "TightOption18"},
        {rfbEncodingTightOption19, "TightOption19"},
        {rfbEncodingTightOption1a, "TightOption1a"},
        {rfbEncodingTightOption1b, "TightOption1b"},
        {rfbEncodingTightOption1c, "TightOption1c"},
        {rfbEncodingTightOption1d, "TightOption1d"},
        {rfbEncodingTightOption1e, "TightOption1e"},
        {rfbEncodingTightOption1f, "TightOption1f"},
        {rfbEncodingTightOption20, "TightOption20"},
};

#define rfbEncodingsCount (sizeof(rfbEncodings) / sizeof(rfbEncodingType))

// Server To Client Messages

typedef struct {
	U8  messageType;
	U8  padding;
	U16 numberOfRectangles;
} rfbFramebufferUpdateMessage;

#define rfbFramebufferUpdateMessageSize (sizeof(rfbFramebufferUpdateMessage))

typedef struct {
	rfbRectangle r;
} rfbFramebufferUpdateRectHeader;

#define rfbFramebufferUpdateRectHeaderSize (sizeof(rfbFramebufferUpdateRectHeader))

typedef struct {
	U16 srcXPosition;
	U16 srcYPosition;
} rfbCopyRect;

#define rfbCopyRectSize (sizeof(rfbCopyRect))


/* typedef struct { */
/* 	U32   numberOfSubrectangles; */
/* 	PIXEL backgroundPixelValue; */
/* } rfbRREHeader; */

/* #define rfbRREHeaderSize (sizeof(rfbRREHeader)) */


/* typedef struct { */
/* 	U32   numberOfSubrectangles; */
/* 	PIXEL backgroundPixelValue; */
/* } rfbCoRRERectangle; */

/* #define rfbCoRRERectangleSize (sizeof(rfbCoRRERectangle)) */


#define rfbHextileRaw			(1 << 0)
#define rfbHextileBackgroundSpecified	(1 << 1)
#define rfbHextileForegroundSpecified	(1 << 2)
#define rfbHextileAnySubrects		(1 << 3)
#define rfbHextileSubrectsColoured	(1 << 4)

#define rfbHextilePackXY(x,y) (((x) << 4) | (y))
#define rfbHextilePackWH(w,h) ((((w)-1) << 4) | ((h)-1))
#define rfbHextileExtractX(byte) ((byte) >> 4)
#define rfbHextileExtractY(byte) ((byte) & 0xf)
#define rfbHextileExtractW(byte) (((byte) >> 4) + 1)
#define rfbHextileExtractH(byte) (((byte) & 0xf) + 1)


typedef struct {
	U8  messageType;
	U8  padding;
	U16 firstColour;
	U16 numberOfColours;
} rfbSetColourMapEntriesMessage;

#define rfbSetColourMapEntriesMessageSize (sizeof(rfbSetColourMapEntriesMessage))



typedef struct {
	U8 messageType;
} rfbBellMessage;

#define rfbBellMessageSize (sizeof(rfbBellMessage))



typedef struct {
	U8  messageType;
	U8  padding[3];
	U32 length;
	// U8[]  text;
} rfbServerCutTextMessage;

#define rfbServerCutTextMessageSize (sizeof(rfbServerCutTextMessage))

// Fill in correct names
typedef union {
	rfbFramebufferUpdateMessage fu;
	rfbBellMessage              b;
	rfbServerCutTextMessage     sct;
} rfbServerToClientMessage;

#endif // HAVE_LIBVNCSERVER

#endif
