/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2015  The Bochs Project
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

// This file (SOUNDWIN.CC) written and donated by Josef Drexler

// Define BX_PLUGGABLE in files that can be compiled into plugins.  For
// platforms that require a special tag on exported symbols, BX_PLUGGABLE
// is used to know when we are exporting symbols and when we are importing.
#define BX_PLUGGABLE

#include "iodev.h"
#include "soundlow.h"
#include "soundwin.h"

#if defined(WIN32) && BX_SUPPORT_SOUNDLOW

#define LOG_THIS

bx_sound_windows_c::bx_sound_windows_c()
  :bx_sound_lowlevel_c()
{
  MidiOpen = 0;
  WaveOutOpen = 0;
  WaveInOpen = 0;

  ismidiready = 1;

  // size is the total size of the midi header and buffer and the
  // BX_SOUND_WINDOWS_NBUF wave header and buffers, all aligned
  // on a 16-byte boundary

#define ALIGN(size) ((size + 15) & ~15)

#define size   ALIGN(sizeof(MIDIHDR)) \
             + ALIGN(sizeof(WAVEHDR)) * (BX_SOUND_WINDOWS_NBUF + 1) \
             + ALIGN(BX_SOUND_WINDOWS_MAXSYSEXLEN) \
             + ALIGN(BX_SOUNDLOW_WAVEPACKETSIZE + 64) * (BX_SOUND_WINDOWS_NBUF + 1)

  DataHandle = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE, size);
  DataPointer = (Bit8u*) GlobalLock(DataHandle);

  if (DataPointer == NULL)
    BX_PANIC(("GlobalLock returned NULL-pointer"));

#define NEWBUFFER(size) &(DataPointer[offset]); offset += ALIGN(size)

  unsigned offset = 0;
  MidiHeader = (LPMIDIHDR) NEWBUFFER(sizeof(MIDIHDR));
  MidiData = (LPSTR) NEWBUFFER(BX_SOUND_WINDOWS_MAXSYSEXLEN);

  WaveOutHdr = (LPWAVEHDR) NEWBUFFER(sizeof(WAVEHDR));
  WaveInHdr = (LPWAVEHDR) NEWBUFFER(sizeof(WAVEHDR));
  WaveInData = (LPSTR) NEWBUFFER(BX_SOUNDLOW_WAVEPACKETSIZE+64);

  if (offset > size)
    BX_PANIC(("Allocated memory was too small!"));

#undef size
#undef ALIGN
#undef NEWBUFFER

  BX_INFO(("Sound lowlevel module 'win' initialized"));
}

bx_sound_windows_c::~bx_sound_windows_c()
{
  GlobalUnlock(DataHandle);
  GlobalFree(DataHandle);
}

int bx_sound_windows_c::midiready()
{
  if (ismidiready == 0)
    checkmidiready();

  if (ismidiready == 1)
    return BX_SOUNDLOW_OK;
  else
    return BX_SOUNDLOW_ERR;
}

int bx_sound_windows_c::openmidioutput(const char *mididev)
{
  UINT deviceid;

  if (strlen(mididev) == 0) {
    deviceid = (UINT) MIDIMAPPER;
  } else {
    deviceid = atoi(mididev);
    if (((deviceid < 0) || (deviceid >= midiOutGetNumDevs())) &&
        (deviceid != (UINT) MIDIMAPPER)) {
      BX_ERROR(("MIDI device ID out of range - using default MIDI mapper"));
      deviceid = (UINT) MIDIMAPPER;
    }
  }

  MidiOpen = 0;

  UINT ret = midiOutOpen(&MidiOut, deviceid, 0, 0, CALLBACK_NULL);
  if (ret == 0)
    MidiOpen = 1;

  BX_DEBUG(("midiOutOpen() = %d, MidiOpen: %d", ret, MidiOpen));

  return (MidiOpen == 1) ? BX_SOUNDLOW_OK : BX_SOUNDLOW_ERR;
}

int bx_sound_windows_c::sendmidicommand(int delta, int command, int length, Bit8u data[])
{
  UINT ret;

  if (MidiOpen != 1)
    return BX_SOUNDLOW_ERR;

  if ((command == 0xf0) || (command == 0xf7) || (length > 3))
  {
    BX_DEBUG(("SYSEX started, length %d", length));
    ismidiready = 0;   // until the buffer is done
    memcpy(MidiData, data, length);
    MidiHeader->lpData = MidiData;
    MidiHeader->dwBufferLength = BX_SOUND_WINDOWS_MAXSYSEXLEN;
    MidiHeader->dwBytesRecorded = 0;
    MidiHeader->dwUser = 0;
    MidiHeader->dwFlags = 0;
    ret = midiOutPrepareHeader(MidiOut, MidiHeader, sizeof(*MidiHeader));
    if (ret != 0)
      BX_ERROR(("midiOutPrepareHeader(): error = %d", ret));
    ret = midiOutLongMsg(MidiOut, MidiHeader, sizeof(*MidiHeader));
    if (ret != 0)
      BX_ERROR(("midiOutLongMsg(): error = %d", ret));
  }
  else
  {
    DWORD msg = command;

    for (int i = 0; i<length; i++)
      msg |= (data[i] << (8 * (i + 1)));

    ret = midiOutShortMsg(MidiOut, msg);
    BX_DEBUG(("midiOutShortMsg(%x) = %d", msg, ret));
  }

  return (ret == 0) ? BX_SOUNDLOW_OK : BX_SOUNDLOW_ERR;
}

int bx_sound_windows_c::closemidioutput()
{
  UINT ret;

  if (MidiOpen != 1)
    return BX_SOUNDLOW_ERR;

  ret = midiOutReset(MidiOut);
  if (ismidiready == 0)
    checkmidiready();   // to clear any pending SYSEX

  ret = midiOutClose(MidiOut);
  BX_DEBUG(("midiOutClose() = %d", ret));
  MidiOpen = 0;

  return (ret == 0) ? BX_SOUNDLOW_OK : BX_SOUNDLOW_ERR;
}

int bx_sound_windows_c::openwaveoutput(const char *wavedev)
{
  // could make the output device selectable,
  // but currently only the wave mapper is supported
  UNUSED(wavedev);

  BX_DEBUG(("openwaveoutput(%s)", wavedev));

  WaveDevice = (UINT) WAVEMAPPER;

  set_pcm_params(real_pcm_param);
  pcm_callback_id = register_wave_callback(this, pcm_callback);
  BX_INIT_MUTEX(mixer_mutex);
  start_mixer_thread();
  return BX_SOUNDLOW_OK;
}

int bx_sound_windows_c::set_pcm_params(bx_pcm_param_t param)
{
  UINT ret;
  PCMWAVEFORMAT waveformat;

  BX_DEBUG(("set_pcm_params(): %u, %u, %u, %02x", param.samplerate, param.bits,
            param.channels, param.format));
  if (WaveOutOpen != 0) {
    ret = waveOutReset(hWaveOut);
    ret = waveOutClose(hWaveOut);
    WaveOutOpen = 0;
  }

  // try three times to find a suitable format
  for (int tries = 0; tries < 3; tries++) {
    int frequency = real_pcm_param.samplerate;
    bx_bool stereo = real_pcm_param.channels == 2;
    int bits = real_pcm_param.bits;
    int bps = (bits / 8) * (stereo + 1);

    waveformat.wf.wFormatTag = WAVE_FORMAT_PCM;
    waveformat.wf.nChannels = stereo + 1;
    waveformat.wf.nSamplesPerSec = frequency;
    waveformat.wf.nAvgBytesPerSec = frequency * bps;
    waveformat.wf.nBlockAlign = bps;
    waveformat.wBitsPerSample = bits;

    ret = waveOutOpen(&(hWaveOut), WaveDevice, (LPWAVEFORMATEX)&(waveformat.wf), 0, 0, CALLBACK_NULL);
    if (ret != 0) {
      char errormsg[4*MAXERRORLENGTH+1];
      waveOutGetErrorTextA(ret, errormsg, 4*MAXERRORLENGTH+1);
      BX_DEBUG(("waveOutOpen: %s", errormsg));
      switch (tries) {
        case 0:        // maybe try a different frequency
          if (frequency < 15600)
            frequency = 11025;
          else if (frequency < 31200)
            frequency = 22050;
          else
            frequency = 44100;

          BX_DEBUG(("Couldn't open wave device (error %d), trying frequency %d", ret, frequency));
          break;

        case 1:        // or something else
          frequency = 11025;
          stereo = 0;
          bits = 8;
          bps = 1;

          BX_DEBUG(("Couldn't open wave device again (error %d), trying 11KHz, mono, 8bit", ret));
          break;

        case 2:        // nope, doesn't work
          BX_ERROR(("Couldn't open wave output device (error = %d)!", ret));
          return BX_SOUNDLOW_ERR;
      }

      BX_DEBUG(("The format was: wFormatTag=%d, nChannels=%d, nSamplesPerSec=%d,",
                waveformat.wf.wFormatTag, waveformat.wf.nChannels, waveformat.wf.nSamplesPerSec));
      BX_DEBUG(("                nAvgBytesPerSec=%d, nBlockAlign=%d, wBitsPerSample=%d",
                waveformat.wf.nAvgBytesPerSec, waveformat.wf.nBlockAlign, waveformat.wBitsPerSample));
    } else {
      WaveOutOpen = 1;
      break;
    }
  }

  return BX_SOUNDLOW_OK;
}

int bx_sound_windows_c::waveout(int length, Bit8u data[])
{
  UINT ret;

  // prepare the wave header
  WaveOutHdr->lpData = (LPSTR)data;
  WaveOutHdr->dwBufferLength = length;
  WaveOutHdr->dwBytesRecorded = length;
  WaveOutHdr->dwUser = 0;
  WaveOutHdr->dwFlags = 0;
  WaveOutHdr->dwLoops = 1;

  ret = waveOutPrepareHeader(hWaveOut, WaveOutHdr, sizeof(*WaveOutHdr));
  if (ret != 0) {
    BX_ERROR(("waveOutPrepareHeader(): error = %d", ret));
    return BX_SOUNDLOW_ERR;
  }

  ret = waveOutWrite(hWaveOut, WaveOutHdr, sizeof(*WaveOutHdr));
  if (ret != 0) {
    char errormsg[4*MAXERRORLENGTH+1];
    waveOutGetErrorTextA(ret, errormsg, 4*MAXERRORLENGTH+1);
    BX_ERROR(("waveOutWrite(): %s", errormsg));
  }
  Sleep(100);

  return BX_SOUNDLOW_OK;
}

int bx_sound_windows_c::closewaveoutput()
{
  BX_DEBUG(("closewaveoutput"));

  if (WaveOutOpen == 1) {
    waveOutReset(hWaveOut);
    waveOutClose(hWaveOut);
  }

  return BX_SOUNDLOW_OK;
}

void bx_sound_windows_c::checkmidiready()
{
  if ((MidiHeader->dwFlags & MHDR_DONE) != 0)
  {
    BX_DEBUG(("SYSEX message done, midi ready again"));
    midiOutUnprepareHeader(MidiOut, MidiHeader, sizeof(*MidiHeader));
    ismidiready = 1;
  }
}

int bx_sound_windows_c::openwaveinput(const char *wavedev, sound_record_handler_t rh)
{
  UNUSED(wavedev);
  record_handler = rh;
  if (rh != NULL) {
    record_timer_index = bx_pc_system.register_timer(this, record_timer_handler, 1, 1, 0, "soundwin");
    // record timer: inactive, continuous, frequency variable
  }
  recording = 0;
  wavein_param.samplerate = 0;
  return BX_SOUNDLOW_OK;
}

int bx_sound_windows_c::recordnextpacket()
{
  MMRESULT result;

  WaveInHdr->lpData = (LPSTR)WaveInData;
  WaveInHdr->dwBufferLength = record_packet_size;
  WaveInHdr->dwBytesRecorded = 0;
  WaveInHdr->dwUser = 0L;
  WaveInHdr->dwFlags = 0L;
  WaveInHdr->dwLoops = 0L;
  waveInPrepareHeader(hWaveIn, WaveInHdr, sizeof(WAVEHDR));
  result = waveInAddBuffer(hWaveIn, WaveInHdr, sizeof(WAVEHDR));
  if (result) {
    BX_ERROR(("Couldn't add buffer for recording (error = %d)", result));
    return BX_SOUNDLOW_ERR;
  } else {
    result = waveInStart(hWaveIn);
    if (result) {
      BX_ERROR(("Couldn't start recording (error = %d)", result));
      return BX_SOUNDLOW_ERR;
    } else {
      recording = 1;
      return BX_SOUNDLOW_OK;
    }
  }
}

int bx_sound_windows_c::startwaverecord(bx_pcm_param_t *param)
{
  Bit64u timer_val;
  Bit8u shift = 0;
  MMRESULT result;

  if (record_timer_index != BX_NULL_TIMER_HANDLE) {
    if (param->bits == 16) shift++;
    if (param->channels == 2) shift++;
    record_packet_size = (param->samplerate / 10) << shift; // 0.1 sec
    if (record_packet_size > BX_SOUNDLOW_WAVEPACKETSIZE) {
      record_packet_size = BX_SOUNDLOW_WAVEPACKETSIZE;
    }
    timer_val = (Bit64u)record_packet_size * 1000000 / (param->samplerate << shift);
    bx_pc_system.activate_timer(record_timer_index, (Bit32u)timer_val, 1);
  }
  // check if any of the properties have changed
  if (memcmp(param, &wavein_param, sizeof(bx_pcm_param_t)) != 0) {
    wavein_param = *param;

    if (WaveInOpen) {
      waveInClose(hWaveIn);
    }

    // Specify recording parameters
    WAVEFORMATEX pFormat;
    pFormat.wFormatTag = WAVE_FORMAT_PCM;
    pFormat.nChannels = param->channels;
    pFormat.nSamplesPerSec = param->samplerate;
    pFormat.nAvgBytesPerSec = param->samplerate << shift;
    pFormat.nBlockAlign = 1 << shift;
    pFormat.wBitsPerSample = param->bits;
    pFormat.cbSize = 0;
    result = waveInOpen(&hWaveIn, WAVEMAPPER, &pFormat, 0L, 0L, WAVE_FORMAT_DIRECT);
    if (result) {
      BX_ERROR(("Couldn't open wave device for recording (error = %d)", result));
      return BX_SOUNDLOW_ERR;
    } else {
      WaveInOpen = 1;
    }
  }
  return recordnextpacket();
}

int bx_sound_windows_c::getwavepacket(int length, Bit8u data[])
{
  if (WaveInOpen && recording) {
    do {} while (waveInUnprepareHeader(hWaveIn, WaveInHdr, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING);
    memcpy(data, WaveInData, length);
    return recordnextpacket();
  } else {
    memset(data, 0, length);
    return BX_SOUNDLOW_OK;
  }
}

int bx_sound_windows_c::stopwaverecord()
{
  if (record_timer_index != BX_NULL_TIMER_HANDLE) {
    bx_pc_system.deactivate_timer(record_timer_index);
  }
  if (WaveInOpen && recording) {
    do {} while (waveInUnprepareHeader(hWaveIn, WaveInHdr, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING);
    recording = 0;
  }
  return BX_SOUNDLOW_OK;
}

int bx_sound_windows_c::closewaveinput()
{
  stopwaverecord();
  if (WaveInOpen) {
    waveInClose(hWaveIn);
  }
  return BX_SOUNDLOW_OK;
}

void bx_sound_windows_c::record_timer_handler(void *this_ptr)
{
  bx_sound_windows_c *class_ptr = (bx_sound_windows_c *) this_ptr;

  class_ptr->record_timer();
}

void bx_sound_windows_c::record_timer(void)
{
  record_handler(this, record_packet_size);
}

#endif // defined(WIN32)
