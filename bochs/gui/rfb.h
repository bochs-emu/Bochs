/////////////////////////////////////////////////////////////////////////
// $Id: rfb.h,v 1.2 2001-10-03 13:10:37 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//
// rfb.h
// This includes the rfb spec header, the port numbers,
// the CARD type definitions and various useful macros.
//

#ifndef RFB_H__
#define RFB_H__

// Define the CARD* types as used in X11/Xmd.h

typedef unsigned long CARD32;
typedef unsigned short CARD16;
typedef short INT16;
typedef unsigned char  CARD8;

// Define the port number offsets
#define FLASH_PORT_OFFSET 5400
#define INCOMING_PORT_OFFSET 5500
#define HTTP_PORT_OFFSET 5800	// we don't use this in Venice
#define RFB_PORT_OFFSET 5900

#define _SIZEOF(x) sz_##x
#define SIZEOF(x) _SIZEOF(x)

#define PORT_TO_DISPLAY(p) ( (p) - RFB_PORT_OFFSET )
#define DISPLAY_TO_PORT(d) ( (d) + RFB_PORT_OFFSET )

#define EWOULDBLOCK WSAEWOULDBLOCK
#define ETIMEDOUT WSAETIMEDOUT

#endif
