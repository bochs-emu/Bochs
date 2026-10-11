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

/*************************************
 *
 *  Macros for extracting pixels
 *
 *************************************/

#define EXTRACT_565_TO_888(val, a, b, c)          \
  (a) = (((val) >> 8) & 0xf8) | (((val) >> 13) & 0x07); \
  (b) = (((val) >> 3) & 0xfc) | (((val) >> 9) & 0x03);  \
  (c) = (((val) << 3) & 0xf8) | (((val) >> 2) & 0x07);

#define EXTRACT_x555_TO_888(val, a, b, c)         \
  (a) = (((val) >> 7) & 0xf8) | (((val) >> 12) & 0x07); \
  (b) = (((val) >> 2) & 0xf8) | (((val) >> 7) & 0x07);  \
  (c) = (((val) << 3) & 0xf8) | (((val) >> 2) & 0x07);

#define EXTRACT_555x_TO_888(val, a, b, c)         \
  (a) = (((val) >> 8) & 0xf8) | (((val) >> 13) & 0x07); \
  (b) = (((val) >> 3) & 0xf8) | (((val) >> 8) & 0x07);  \
  (c) = (((val) << 2) & 0xf8) | (((val) >> 3) & 0x07);

#define EXTRACT_1555_TO_8888(val, a, b, c, d)     \
  (a) = ((Bit16s)(val) >> 15) & 0xff;             \
  EXTRACT_x555_TO_888(val, b, c, d)

#define EXTRACT_5551_TO_8888(val, a, b, c, d)     \
  EXTRACT_555x_TO_888(val, a, b, c)               \
  (d) = ((val) & 0x0001) ? 0xff : 0x00;

#define EXTRACT_x888_TO_888(val, a, b, c)         \
  (a) = ((val) >> 16) & 0xff;               \
  (b) = ((val) >> 8) & 0xff;                \
  (c) = ((val) >> 0) & 0xff;

#define EXTRACT_888x_TO_888(val, a, b, c)         \
  (a) = ((val) >> 24) & 0xff;               \
  (b) = ((val) >> 16) & 0xff;               \
  (c) = ((val) >> 8) & 0xff;

#define EXTRACT_8888_TO_8888(val, a, b, c, d)     \
  (a) = ((val) >> 24) & 0xff;               \
  (b) = ((val) >> 16) & 0xff;               \
  (c) = ((val) >> 8) & 0xff;                \
  (d) = ((val) >> 0) & 0xff;

#define EXTRACT_4444_TO_8888(val, a, b, c, d)     \
  (a) = (((val) >> 8) & 0xf0) | (((val) >> 12) & 0x0f); \
  (b) = (((val) >> 4) & 0xf0) | (((val) >> 8) & 0x0f);  \
  (c) = (((val) >> 0) & 0xf0) | (((val) >> 4) & 0x0f);  \
  (d) = (((val) << 4) & 0xf0) | (((val) >> 0) & 0x0f);

#define EXTRACT_332_TO_888(val, a, b, c)          \
  (a) = (((val) >> 0) & 0xe0) | (((val) >> 3) & 0x1c) | (((val) >> 6) & 0x03); \
  (b) = (((val) << 3) & 0xe0) | (((val) >> 0) & 0x1c) | (((val) >> 3) & 0x03); \
  (c) = (((val) << 6) & 0xc0) | (((val) << 4) & 0x30) | (((val) << 2) & 0xc0) | (((val) << 0) & 0x03);
