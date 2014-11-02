/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2014  The Bochs Project
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

// This file (SOUNDWIN.H) written and donated by Josef Drexler

#if defined(WIN32)

#include <mmsystem.h>

// uncomment one of the following two #defines
//#define usesndPlaySnd
#define usewaveOut

#define BX_SOUND_WINDOWS_MAXSYSEXLEN  256    // maximum supported length of a sysex message

#define BX_SOUND_WINDOWS_NBUF   64  // number of buffers for the output, must be power of 2 and >= 4
#define BX_SOUND_WINDOWS_NMASK  (BX_SOUND_WINDOWS_NBUF - 1)

#ifndef WAVEMAPPER
#define WAVEMAPPER -1
#endif

// Definitions for WINMM.DLL, if not defined already
#ifndef MMSYSERR_NOERROR

#pragma pack(1)

typedef UINT HMIDIOUT;
typedef	HMIDIOUT *LPHMIDIOUT;
typedef struct midihdr_tag {
    LPSTR lpData;
    DWORD dwBufferLength;
    DWORD dwBytesRecorded;
    DWORD dwUser;
    DWORD dwFlags;
    struct midihdr_tag *lpNext;
    DWORD reserved;
} MIDIHDR, *LPMIDIHDR;

typedef UINT HWAVEOUT;
typedef HWAVEOUT *LPHWAVEOUT;

typedef struct wavehdr_tag {
    LPSTR lpData;
    DWORD dwBufferLength;
    DWORD dwBytesRecorded;
    DWORD dwUser;
    DWORD dwFlags;
    DWORD dwLoops;
    struct wavehdr_tag *lpNext;
    DWORD reserved;
} WAVEHDR, *LPWAVEHDR;

#define WHDR_DONE         0x00000001
#define WHDR_PREPARED     0x00000002
#define WHDR_BEGINLOOP    0x00000004
#define WHDR_ENDLOOP      0x00000008
#define WHDR_INQUEUE      0x00000010


typedef struct waveformat_tag {
    WORD wFormatTag;
    WORD nChannels;
    DWORD nSamplesPerSec;
    DWORD nAvgBytesPerSec;
    WORD nBlockAlign;
} WAVEFORMAT, *LPWAVEFORMAT;

#define WAVE_FORMAT_PCM 	1

typedef struct pcmwaveformat_tag {
    WAVEFORMAT wf;
    WORD wBitsPerSample;
} PCMWAVEFORMAT, *LPPCMWAVEFORMAT;

#define MIDIMAPPER		-1

#define CALLBACK_NULL		0x00000000
#define CALLBACK_WINDOW		0x00010000
#define CALLBACK_TASK		0x00020000
#define CALLBACK_FUNCTION	0x00030000

#define MMSYSERR_NOERROR	0
#define MMSYSERR_ERROR		1
#define MMSYSERR_BADDEVICEID	2
#define MMSYSERR_NOTENABLED	3
#define MMSYSERR_ALLOCATED      4
#define MMSYSERR_INVALHANDLE	5
#define MMSYSERR_NODRIVER	6
#define MMSYSERR_NOMEM		7
#define MMSYSERR_NOTSUPPORTED	8
#define MMSYSERR_NOMAP		7

#define MIDIERR_UNPREPARED      64
#define MIDIERR_STILLPLAYING    65
#define MIDIERR_NOTREADY	66
#define MIDIERR_NODEVICE        67

#define WAVERR_BADFORMAT        32
#define WAVERR_STILLPLAYING     33
#define WAVERR_UNPREPARED       34
#define WAVERR_SYNC             35

#define MAXERRORLENGTH		128

extern "C" {
UINT STDCALL midiOutOpen(LPHMIDIOUT, UINT, DWORD, DWORD, DWORD);
UINT STDCALL midiOutShortMsg(HMIDIOUT, DWORD);
UINT STDCALL midiOutLongMsg(HMIDIOUT, LPMIDIHDR, UINT);
UINT STDCALL midiOutPrepareHeader(HMIDIOUT, LPMIDIHDR, UINT);
UINT STDCALL midiOutUnprepareHeader(HMIDIOUT, LPMIDIHDR, UINT);
UINT STDCALL midiOutReset(HMIDIOUT);
UINT STDCALL midiOutClose(HMIDIOUT);

UINT STDCALL waveOutOpen(LPHWAVEOUT, UINT, LPWAVEFORMAT, DWORD, DWORD, DWORD);
UINT STDCALL waveOutWrite(HWAVEOUT, LPWAVEHDR, UINT);
UINT STDCALL waveOutPrepareHeader(HWAVEOUT, LPWAVEHDR, UINT);
UINT STDCALL waveOutUnprepareHeader(HWAVEOUT, LPWAVEHDR, UINT);
UINT STDCALL waveOutReset(HWAVEOUT);
UINT STDCALL waveOutClose(HWAVEOUT);

UINT STDCALL waveOutGetErrorTextA(UINT, LPSTR, UINT);

BOOL STDCALL sndPlaySoundA(LPCSTR, UINT);
}
#pragma pack(0)

#endif  // MMSYSERR_NOERROR defined

#ifndef WAVEFILEHEADER

#pragma pack(push, 1)

typedef struct {
  char RIFF[4];
  Bit32u length;
  char TYPE[4];
  char chnk[4];
  Bit32u chnklen;
  PCMWAVEFORMAT waveformat;
  char chnk2[4];
  Bit32u chnk2len;
  char data[1];
} WAVEFILEHEADER, *LPWAVEFILEHEADER;
#pragma pack(pop)

#endif

class bx_sound_windows_c : public bx_sound_lowlevel_c {
public:
  bx_sound_windows_c();
  virtual ~bx_sound_windows_c();

  virtual int get_type() {return BX_SOUNDLOW_WIN;}

  virtual int    waveready();
  virtual int    midiready();

  virtual int    openmidioutput(const char *mididev);
  virtual int    sendmidicommand(int delta, int command, int length, Bit8u data[]);
  virtual int    closemidioutput();

  virtual int    openwaveoutput(const char *wavedev);
  virtual int    startwaveplayback(int frequency, int bits, bx_bool stereo, int format);
  virtual int    sendwavepacket(int length, Bit8u data[]);
  virtual int    stopwaveplayback();
  virtual int    closewaveoutput();

  virtual int openwaveinput(const char *wavedev, sound_record_handler_t rh);
  virtual int startwaverecord(int frequency, int bits, bx_bool stereo, int format);
  virtual int getwavepacket(int length, Bit8u data[]);
  virtual int stopwaverecord();
  virtual int closewaveinput();

  static void record_timer_handler(void *);
  void record_timer(void);

private:
  struct bx_sound_waveinfo_struct {
    int frequency;
    int bits;
    bx_bool stereo;
    int format;
  };

  HMIDIOUT MidiOut;       // Midi output device
  int MidiOpen;           // is it open?
  HWAVEOUT hWaveOut;      // Wave output device
  int WaveOutOpen;        // is it open?
  HWAVEIN hWaveIn;        // Wave input device
  int WaveInOpen;         // is it open?

  UINT WaveDevice;        // Wave device ID, for waveOutOpen

  // some data for the wave buffers
  HANDLE DataHandle;          // returned by GlobalAlloc()
  Bit8u *DataPointer;         // returned by GlobalLock()

  LPWAVEHDR WaveHeader[BX_SOUND_WINDOWS_NBUF];
  LPSTR WaveData[BX_SOUND_WINDOWS_NBUF];
  LPWAVEHDR WaveInHdr;
  LPSTR WaveInData;
  bx_bool recording;
  int length[BX_SOUND_WINDOWS_NBUF];                // length of the data in the buffer
  int needreopen;                                   // if the format has changed
  int head,tailfull,tailplay;       // These are for three states of the buffers: empty, full, played
  bx_sound_waveinfo_struct WaveInfo[2];             // format for the next buffer to be played
  int iswaveready;

  // and the midi buffer for the SYSEX messages
  LPMIDIHDR MidiHeader;
  LPSTR MidiData;
  int ismidiready;

  int playnextbuffer();
  int recordnextpacket();
  void checkmidiready();
  void checkwaveready();
};

#endif  // defined(WIN32)
