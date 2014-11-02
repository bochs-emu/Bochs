/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2009-2013  The Bochs Project
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

// This file includes the rfb key definitions and keyboard mapping stuff

#ifdef HAVE_LIBVNCSERVER

#include <rfb/keysym.h>

#define XK_Oslash           0x00d8
#define XK_ooblique         0x00f8

#else

enum {
  XK_space = 0x020,
  XK_exclam,
  XK_quotedbl,
  XK_numbersign,
  XK_dollar,
  XK_percent,
  XK_ampersand,
  XK_apostrophe,
  XK_parenleft,
  XK_parenright,
  XK_asterisk,
  XK_plus,
  XK_comma,
  XK_minus,
  XK_period,
  XK_slash,
  XK_0,
  XK_1,
  XK_2,
  XK_3,
  XK_4,
  XK_5,
  XK_6,
  XK_7,
  XK_8,
  XK_9,
  XK_colon,
  XK_semicolon,
  XK_less,
  XK_equal,
  XK_greater,
  XK_question,
  XK_at,
  XK_A,
  XK_B,
  XK_C,
  XK_D,
  XK_E,
  XK_F,
  XK_G,
  XK_H,
  XK_I,
  XK_J,
  XK_K,
  XK_L,
  XK_M,
  XK_N,
  XK_O,
  XK_P,
  XK_Q,
  XK_R,
  XK_S,
  XK_T,
  XK_U,
  XK_V,
  XK_W,
  XK_X,
  XK_Y,
  XK_Z,
  XK_bracketleft,
  XK_backslash,
  XK_bracketright,
  XK_asciicircum,
  XK_underscore,
  XK_grave,
  XK_a,
  XK_b,
  XK_c,
  XK_d,
  XK_e,
  XK_f,
  XK_g,
  XK_h,
  XK_i,
  XK_j,
  XK_k,
  XK_l,
  XK_m,
  XK_n,
  XK_o,
  XK_p,
  XK_q,
  XK_r,
  XK_s,
  XK_t,
  XK_u,
  XK_v,
  XK_w,
  XK_x,
  XK_y,
  XK_z,
  XK_braceleft,
  XK_bar,
  XK_braceright,
  XK_asciitilde
};

#define XK_nobreakspace     0x00a0
#define XK_exclamdown       0x00a1
#define XK_cent             0x00a2
#define XK_sterling         0x00a3
#define XK_currency         0x00a4
#define XK_yen              0x00a5
#define XK_brokenbar        0x00a6
#define XK_section          0x00a7
#define XK_diaeresis        0x00a8
#define XK_copyright        0x00a9
#define XK_ordfeminine      0x00aa
#define XK_guillemotleft    0x00ab
#define XK_notsign          0x00ac
#define XK_hyphen           0x00ad
#define XK_registered       0x00ae
#define XK_macron           0x00af
#define XK_degree           0x00b0
#define XK_plusminus        0x00b1
#define XK_twosuperior      0x00b2
#define XK_threesuperior    0x00b3
#define XK_acute            0x00b4
#define XK_mu               0x00b5
#define XK_paragraph        0x00b6
#define XK_periodcentered   0x00b7
#define XK_cedilla          0x00b8
#define XK_onesuperior      0x00b9
#define XK_masculine        0x00ba
#define XK_guillemotright   0x00bb
#define XK_onequarter       0x00bc
#define XK_onehalf          0x00bd
#define XK_threequarters    0x00be
#define XK_questiondown     0x00bf
#define XK_Agrave           0x00c0
#define XK_Aacute           0x00c1
#define XK_Acircumflex      0x00c2
#define XK_Atilde           0x00c3
#define XK_Adiaeresis       0x00c4
#define XK_Aring            0x00c5
#define XK_AE               0x00c6
#define XK_Ccedilla         0x00c7
#define XK_Egrave           0x00c8
#define XK_Eacute           0x00c9
#define XK_Ecircumflex      0x00ca
#define XK_Ediaeresis       0x00cb
#define XK_Igrave           0x00cc
#define XK_Iacute           0x00cd
#define XK_Icircumflex      0x00ce
#define XK_Idiaeresis       0x00cf
#define XK_ETH              0x00d0
#define XK_Ntilde           0x00d1
#define XK_Ograve           0x00d2
#define XK_Oacute           0x00d3
#define XK_Ocircumflex      0x00d4
#define XK_Otilde           0x00d5
#define XK_Odiaeresis       0x00d6
#define XK_multiply         0x00d7
#define XK_Oslash           0x00d8
#define XK_Ooblique         0x00d8
#define XK_Ugrave           0x00d9
#define XK_Uacute           0x00da
#define XK_Ucircumflex      0x00db
#define XK_Udiaeresis       0x00dc
#define XK_Yacute           0x00dd
#define XK_THORN            0x00de
#define XK_ssharp           0x00df
#define XK_agrave           0x00e0
#define XK_aacute           0x00e1
#define XK_acircumflex      0x00e2
#define XK_atilde           0x00e3
#define XK_adiaeresis       0x00e4
#define XK_aring            0x00e5
#define XK_ae               0x00e6
#define XK_ccedilla         0x00e7
#define XK_egrave           0x00e8
#define XK_eacute           0x00e9
#define XK_ecircumflex      0x00ea
#define XK_ediaeresis       0x00eb
#define XK_igrave           0x00ec
#define XK_iacute           0x00ed
#define XK_icircumflex      0x00ee
#define XK_idiaeresis       0x00ef
#define XK_eth              0x00f0
#define XK_ntilde           0x00f1
#define XK_ograve           0x00f2
#define XK_oacute           0x00f3
#define XK_ocircumflex      0x00f4
#define XK_otilde           0x00f5
#define XK_odiaeresis       0x00f6
#define XK_division         0x00f7
#define XK_oslash           0x00f8
#define XK_ooblique         0x00f8
#define XK_ugrave           0x00f9
#define XK_uacute           0x00fa
#define XK_ucircumflex      0x00fb
#define XK_udiaeresis       0x00fc
#define XK_yacute           0x00fd
#define XK_thorn            0x00fe
#define XK_ydiaeresis       0x00ff

#define XK_EuroSign         0x20ac

#define XK_ISO_Level3_Shift 0xFE03
#define XK_ISO_Left_Tab     0xFE20

#define XK_dead_grave       0xFE50
#define XK_dead_acute       0xFE51
#define XK_dead_circumflex  0xFE52
#define XK_dead_tilde       0xFE53

#define XK_BackSpace        0xFF08
#define XK_Tab              0xFF09
#define XK_Linefeed         0xFF0A
#define XK_Clear            0xFF0B
#define XK_Return           0xFF0D
#define XK_Pause            0xFF13
#define XK_Scroll_Lock      0xFF14
#define XK_Sys_Req          0xFF15
#define XK_Escape           0xFF1B

#define XK_Delete           0xFFFF

#define XK_Home             0xFF50
#define XK_Left             0xFF51
#define XK_Up               0xFF52
#define XK_Right            0xFF53
#define XK_Down             0xFF54
#define XK_Page_Up          0xFF55
#define XK_Page_Down        0xFF56
#define XK_End              0xFF57
#define XK_Begin            0xFF58

#define XK_Select           0xFF60
#define XK_Print            0xFF61
#define XK_Execute          0xFF62
#define XK_Insert           0xFF63
#define XK_Menu             0xff67
#define XK_Cancel           0xFF69
#define XK_Help             0xFF6A
#define XK_Break            0xFF6B
#define XK_Num_Lock         0xFF7F

#define XK_KP_Space         0xFF80
#define XK_KP_Tab           0xFF89
#define XK_KP_Enter         0xFF8D

#define XK_KP_Home          0xFF95
#define XK_KP_Left          0xFF96
#define XK_KP_Up            0xFF97
#define XK_KP_Right         0xFF98
#define XK_KP_Down          0xFF99
#define XK_KP_Prior         0xFF9A
#define XK_KP_Page_Up       0xFF9A
#define XK_KP_Next          0xFF9B
#define XK_KP_Page_Down     0xFF9B
#define XK_KP_End           0xFF9C
#define XK_KP_Begin         0xFF9D
#define XK_KP_Insert        0xFF9E
#define XK_KP_Delete        0xFF9F
#define XK_KP_Equal         0xFFBD
#define XK_KP_Multiply      0xFFAA
#define XK_KP_Add           0xFFAB
#define XK_KP_Separator     0xFFAC
#define XK_KP_Subtract      0xFFAD
#define XK_KP_Decimal       0xFFAE
#define XK_KP_Divide        0xFFAF

#define XK_KP_F1            0xFF91
#define XK_KP_F2            0xFF92
#define XK_KP_F3            0xFF93
#define XK_KP_F4            0xFF94

#define XK_KP_0             0xFFB0
#define XK_KP_1             0xFFB1
#define XK_KP_2             0xFFB2
#define XK_KP_3             0xFFB3
#define XK_KP_4             0xFFB4
#define XK_KP_5             0xFFB5
#define XK_KP_6             0xFFB6
#define XK_KP_7             0xFFB7
#define XK_KP_8             0xFFB8
#define XK_KP_9             0xFFB9

#define XK_F1               0xFFBE
#define XK_F2               0xFFBF
#define XK_F3               0xFFC0
#define XK_F4               0xFFC1
#define XK_F5               0xFFC2
#define XK_F6               0xFFC3
#define XK_F7               0xFFC4
#define XK_F8               0xFFC5
#define XK_F9               0xFFC6
#define XK_F10              0xFFC7
#define XK_F11              0xFFC8
#define XK_F12              0xFFC9

#define XK_Shift_L          0xFFE1
#define XK_Shift_R          0xFFE2
#define XK_Control_L        0xFFE3
#define XK_Control_R        0xFFE4
#define XK_Caps_Lock        0xFFE5
#define XK_Shift_Lock       0xFFE6
#define XK_Meta_L           0xFFE7
#define XK_Meta_R           0xFFE8
#define XK_Alt_L            0xFFE9
#define XK_Alt_R            0xFFEA
#define XK_Super_L          0xFFEB
#define XK_Super_R          0xFFEC

#endif

/// key mapping for rfb
typedef struct {
  const char *name;
  Bit32u value;
} rfbKeyTabEntry;

#define DEF_RFB_KEY(key) \
  { #key, key },

#if defined(HAVE_LIBVNCSERVER) && BX_WITH_RFB && !BX_PLUGINS
extern rfbKeyTabEntry rfb_keytable[];
#else
rfbKeyTabEntry rfb_keytable[] = {
  // this include provides all the entries.
  DEF_RFB_KEY(XK_space)
  DEF_RFB_KEY(XK_exclam)
  DEF_RFB_KEY(XK_quotedbl)
  DEF_RFB_KEY(XK_numbersign)
  DEF_RFB_KEY(XK_dollar)
  DEF_RFB_KEY(XK_percent)
  DEF_RFB_KEY(XK_ampersand)
  DEF_RFB_KEY(XK_apostrophe)
  DEF_RFB_KEY(XK_parenleft)
  DEF_RFB_KEY(XK_parenright)
  DEF_RFB_KEY(XK_asterisk)
  DEF_RFB_KEY(XK_plus)
  DEF_RFB_KEY(XK_comma)
  DEF_RFB_KEY(XK_minus)
  DEF_RFB_KEY(XK_period)
  DEF_RFB_KEY(XK_slash)
  DEF_RFB_KEY(XK_0)
  DEF_RFB_KEY(XK_1)
  DEF_RFB_KEY(XK_2)
  DEF_RFB_KEY(XK_3)
  DEF_RFB_KEY(XK_4)
  DEF_RFB_KEY(XK_5)
  DEF_RFB_KEY(XK_6)
  DEF_RFB_KEY(XK_7)
  DEF_RFB_KEY(XK_8)
  DEF_RFB_KEY(XK_9)
  DEF_RFB_KEY(XK_colon)
  DEF_RFB_KEY(XK_semicolon)
  DEF_RFB_KEY(XK_less)
  DEF_RFB_KEY(XK_equal)
  DEF_RFB_KEY(XK_greater)
  DEF_RFB_KEY(XK_question)
  DEF_RFB_KEY(XK_at)
  DEF_RFB_KEY(XK_A)
  DEF_RFB_KEY(XK_B)
  DEF_RFB_KEY(XK_C)
  DEF_RFB_KEY(XK_D)
  DEF_RFB_KEY(XK_E)
  DEF_RFB_KEY(XK_F)
  DEF_RFB_KEY(XK_G)
  DEF_RFB_KEY(XK_H)
  DEF_RFB_KEY(XK_I)
  DEF_RFB_KEY(XK_J)
  DEF_RFB_KEY(XK_K)
  DEF_RFB_KEY(XK_L)
  DEF_RFB_KEY(XK_M)
  DEF_RFB_KEY(XK_N)
  DEF_RFB_KEY(XK_O)
  DEF_RFB_KEY(XK_P)
  DEF_RFB_KEY(XK_Q)
  DEF_RFB_KEY(XK_R)
  DEF_RFB_KEY(XK_S)
  DEF_RFB_KEY(XK_T)
  DEF_RFB_KEY(XK_U)
  DEF_RFB_KEY(XK_V)
  DEF_RFB_KEY(XK_W)
  DEF_RFB_KEY(XK_X)
  DEF_RFB_KEY(XK_Y)
  DEF_RFB_KEY(XK_Z)
  DEF_RFB_KEY(XK_bracketleft)
  DEF_RFB_KEY(XK_backslash)
  DEF_RFB_KEY(XK_bracketright)
  DEF_RFB_KEY(XK_asciicircum)
  DEF_RFB_KEY(XK_underscore)
  DEF_RFB_KEY(XK_grave)
  DEF_RFB_KEY(XK_a)
  DEF_RFB_KEY(XK_b)
  DEF_RFB_KEY(XK_c)
  DEF_RFB_KEY(XK_d)
  DEF_RFB_KEY(XK_e)
  DEF_RFB_KEY(XK_f)
  DEF_RFB_KEY(XK_g)
  DEF_RFB_KEY(XK_h)
  DEF_RFB_KEY(XK_i)
  DEF_RFB_KEY(XK_j)
  DEF_RFB_KEY(XK_k)
  DEF_RFB_KEY(XK_l)
  DEF_RFB_KEY(XK_m)
  DEF_RFB_KEY(XK_n)
  DEF_RFB_KEY(XK_o)
  DEF_RFB_KEY(XK_p)
  DEF_RFB_KEY(XK_q)
  DEF_RFB_KEY(XK_r)
  DEF_RFB_KEY(XK_s)
  DEF_RFB_KEY(XK_t)
  DEF_RFB_KEY(XK_u)
  DEF_RFB_KEY(XK_v)
  DEF_RFB_KEY(XK_w)
  DEF_RFB_KEY(XK_x)
  DEF_RFB_KEY(XK_y)
  DEF_RFB_KEY(XK_z)
  DEF_RFB_KEY(XK_braceleft)
  DEF_RFB_KEY(XK_bar)
  DEF_RFB_KEY(XK_braceright)
  DEF_RFB_KEY(XK_asciitilde)
  DEF_RFB_KEY(XK_nobreakspace)
  DEF_RFB_KEY(XK_exclamdown)
  DEF_RFB_KEY(XK_cent)
  DEF_RFB_KEY(XK_sterling)
  DEF_RFB_KEY(XK_currency)
  DEF_RFB_KEY(XK_yen)
  DEF_RFB_KEY(XK_brokenbar)
  DEF_RFB_KEY(XK_section)
  DEF_RFB_KEY(XK_diaeresis)
  DEF_RFB_KEY(XK_copyright)
  DEF_RFB_KEY(XK_ordfeminine)
  DEF_RFB_KEY(XK_guillemotleft)
  DEF_RFB_KEY(XK_notsign)
  DEF_RFB_KEY(XK_hyphen)
  DEF_RFB_KEY(XK_registered)
  DEF_RFB_KEY(XK_macron)
  DEF_RFB_KEY(XK_degree)
  DEF_RFB_KEY(XK_plusminus)
  DEF_RFB_KEY(XK_twosuperior)
  DEF_RFB_KEY(XK_threesuperior)
  DEF_RFB_KEY(XK_acute)
  DEF_RFB_KEY(XK_mu)
  DEF_RFB_KEY(XK_paragraph)
  DEF_RFB_KEY(XK_periodcentered)
  DEF_RFB_KEY(XK_cedilla)
  DEF_RFB_KEY(XK_onesuperior)
  DEF_RFB_KEY(XK_masculine)
  DEF_RFB_KEY(XK_guillemotright)
  DEF_RFB_KEY(XK_onequarter)
  DEF_RFB_KEY(XK_onehalf)
  DEF_RFB_KEY(XK_threequarters)
  DEF_RFB_KEY(XK_questiondown)
  DEF_RFB_KEY(XK_Agrave)
  DEF_RFB_KEY(XK_Aacute)
  DEF_RFB_KEY(XK_Acircumflex)
  DEF_RFB_KEY(XK_Atilde)
  DEF_RFB_KEY(XK_Adiaeresis)
  DEF_RFB_KEY(XK_Aring)
  DEF_RFB_KEY(XK_AE)
  DEF_RFB_KEY(XK_Ccedilla)
  DEF_RFB_KEY(XK_Egrave)
  DEF_RFB_KEY(XK_Eacute)
  DEF_RFB_KEY(XK_Ecircumflex)
  DEF_RFB_KEY(XK_Ediaeresis)
  DEF_RFB_KEY(XK_Igrave)
  DEF_RFB_KEY(XK_Iacute)
  DEF_RFB_KEY(XK_Icircumflex)
  DEF_RFB_KEY(XK_Idiaeresis)
  DEF_RFB_KEY(XK_ETH)
  DEF_RFB_KEY(XK_Ntilde)
  DEF_RFB_KEY(XK_Ograve)
  DEF_RFB_KEY(XK_Oacute)
  DEF_RFB_KEY(XK_Ocircumflex)
  DEF_RFB_KEY(XK_Otilde)
  DEF_RFB_KEY(XK_Odiaeresis)
  DEF_RFB_KEY(XK_multiply)
  DEF_RFB_KEY(XK_Oslash)
  DEF_RFB_KEY(XK_Ooblique)
  DEF_RFB_KEY(XK_Ugrave)
  DEF_RFB_KEY(XK_Uacute)
  DEF_RFB_KEY(XK_Ucircumflex)
  DEF_RFB_KEY(XK_Udiaeresis)
  DEF_RFB_KEY(XK_Yacute)
  DEF_RFB_KEY(XK_THORN)
  DEF_RFB_KEY(XK_ssharp)
  DEF_RFB_KEY(XK_agrave)
  DEF_RFB_KEY(XK_aacute)
  DEF_RFB_KEY(XK_acircumflex)
  DEF_RFB_KEY(XK_atilde)
  DEF_RFB_KEY(XK_adiaeresis)
  DEF_RFB_KEY(XK_aring)
  DEF_RFB_KEY(XK_ae)
  DEF_RFB_KEY(XK_ccedilla)
  DEF_RFB_KEY(XK_egrave)
  DEF_RFB_KEY(XK_eacute)
  DEF_RFB_KEY(XK_ecircumflex)
  DEF_RFB_KEY(XK_ediaeresis)
  DEF_RFB_KEY(XK_igrave)
  DEF_RFB_KEY(XK_iacute)
  DEF_RFB_KEY(XK_icircumflex)
  DEF_RFB_KEY(XK_idiaeresis)
  DEF_RFB_KEY(XK_eth)
  DEF_RFB_KEY(XK_ntilde)
  DEF_RFB_KEY(XK_ograve)
  DEF_RFB_KEY(XK_oacute)
  DEF_RFB_KEY(XK_ocircumflex)
  DEF_RFB_KEY(XK_otilde)
  DEF_RFB_KEY(XK_odiaeresis)
  DEF_RFB_KEY(XK_division)
  DEF_RFB_KEY(XK_oslash)
  DEF_RFB_KEY(XK_ooblique)
  DEF_RFB_KEY(XK_ugrave)
  DEF_RFB_KEY(XK_uacute)
  DEF_RFB_KEY(XK_ucircumflex)
  DEF_RFB_KEY(XK_udiaeresis)
  DEF_RFB_KEY(XK_yacute)
  DEF_RFB_KEY(XK_thorn)
  DEF_RFB_KEY(XK_ydiaeresis)
  DEF_RFB_KEY(XK_EuroSign)
  DEF_RFB_KEY(XK_ISO_Level3_Shift)
  DEF_RFB_KEY(XK_ISO_Left_Tab)
  DEF_RFB_KEY(XK_dead_grave)
  DEF_RFB_KEY(XK_dead_acute)
  DEF_RFB_KEY(XK_dead_circumflex)
  DEF_RFB_KEY(XK_dead_tilde)
  DEF_RFB_KEY(XK_BackSpace)
  DEF_RFB_KEY(XK_Tab)
  DEF_RFB_KEY(XK_Linefeed)
  DEF_RFB_KEY(XK_Clear)
  DEF_RFB_KEY(XK_Return)
  DEF_RFB_KEY(XK_Pause)
  DEF_RFB_KEY(XK_Scroll_Lock)
  DEF_RFB_KEY(XK_Sys_Req)
  DEF_RFB_KEY(XK_Escape)
  DEF_RFB_KEY(XK_Delete)
  DEF_RFB_KEY(XK_Home)
  DEF_RFB_KEY(XK_Left)
  DEF_RFB_KEY(XK_Up)
  DEF_RFB_KEY(XK_Right)
  DEF_RFB_KEY(XK_Down)
  DEF_RFB_KEY(XK_Page_Up)
  DEF_RFB_KEY(XK_Page_Down)
  DEF_RFB_KEY(XK_End)
  DEF_RFB_KEY(XK_Begin)
  DEF_RFB_KEY(XK_Select)
  DEF_RFB_KEY(XK_Print)
  DEF_RFB_KEY(XK_Execute)
  DEF_RFB_KEY(XK_Insert)
  DEF_RFB_KEY(XK_Menu)
  DEF_RFB_KEY(XK_Cancel)
  DEF_RFB_KEY(XK_Help)
  DEF_RFB_KEY(XK_Break)
  DEF_RFB_KEY(XK_Num_Lock)
  DEF_RFB_KEY(XK_KP_Space)
  DEF_RFB_KEY(XK_KP_Tab)
  DEF_RFB_KEY(XK_KP_Enter)
  DEF_RFB_KEY(XK_KP_Home)
  DEF_RFB_KEY(XK_KP_Left)
  DEF_RFB_KEY(XK_KP_Up)
  DEF_RFB_KEY(XK_KP_Right)
  DEF_RFB_KEY(XK_KP_Down)
  DEF_RFB_KEY(XK_KP_Prior)
  DEF_RFB_KEY(XK_KP_Page_Up)
  DEF_RFB_KEY(XK_KP_Next)
  DEF_RFB_KEY(XK_KP_Page_Down)
  DEF_RFB_KEY(XK_KP_End)
  DEF_RFB_KEY(XK_KP_Begin)
  DEF_RFB_KEY(XK_KP_Insert)
  DEF_RFB_KEY(XK_KP_Delete)
  DEF_RFB_KEY(XK_KP_Equal)
  DEF_RFB_KEY(XK_KP_Multiply)
  DEF_RFB_KEY(XK_KP_Add)
  DEF_RFB_KEY(XK_KP_Separator)
  DEF_RFB_KEY(XK_KP_Subtract)
  DEF_RFB_KEY(XK_KP_Decimal)
  DEF_RFB_KEY(XK_KP_Divide)
  DEF_RFB_KEY(XK_KP_F1)
  DEF_RFB_KEY(XK_KP_F2)
  DEF_RFB_KEY(XK_KP_F3)
  DEF_RFB_KEY(XK_KP_F4)
  DEF_RFB_KEY(XK_KP_0)
  DEF_RFB_KEY(XK_KP_1)
  DEF_RFB_KEY(XK_KP_2)
  DEF_RFB_KEY(XK_KP_3)
  DEF_RFB_KEY(XK_KP_4)
  DEF_RFB_KEY(XK_KP_5)
  DEF_RFB_KEY(XK_KP_6)
  DEF_RFB_KEY(XK_KP_7)
  DEF_RFB_KEY(XK_KP_8)
  DEF_RFB_KEY(XK_KP_9)
  DEF_RFB_KEY(XK_F1)
  DEF_RFB_KEY(XK_F2)
  DEF_RFB_KEY(XK_F3)
  DEF_RFB_KEY(XK_F4)
  DEF_RFB_KEY(XK_F5)
  DEF_RFB_KEY(XK_F6)
  DEF_RFB_KEY(XK_F7)
  DEF_RFB_KEY(XK_F8)
  DEF_RFB_KEY(XK_F9)
  DEF_RFB_KEY(XK_F10)
  DEF_RFB_KEY(XK_F11)
  DEF_RFB_KEY(XK_F12)
  DEF_RFB_KEY(XK_Shift_L)
  DEF_RFB_KEY(XK_Shift_R)
  DEF_RFB_KEY(XK_Control_L)
  DEF_RFB_KEY(XK_Control_R)
  DEF_RFB_KEY(XK_Caps_Lock)
  DEF_RFB_KEY(XK_Shift_Lock)
  DEF_RFB_KEY(XK_Meta_L)
  DEF_RFB_KEY(XK_Meta_R)
  DEF_RFB_KEY(XK_Alt_L)
  DEF_RFB_KEY(XK_Alt_R)
  DEF_RFB_KEY(XK_Super_L)
  DEF_RFB_KEY(XK_Super_R)
  // one final entry to mark the end
  { NULL, 0 }
};
#endif
