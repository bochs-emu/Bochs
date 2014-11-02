/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2004-2013  The Bochs Project
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

// This file (SOUNDOSX.CC) written and donated by Brian Huffman

#ifdef PARANOID
#include <MacTypes.h>
#endif

#include "iodev.h"
#include "soundmod.h"
#include "soundosx.h"

#if defined(macintosh) && BX_SUPPORT_SOUNDLOW

#define LOG_THIS

#if BX_WITH_MACOS
#include <QuickTimeMusic.h>
#else
#include <CoreServices/CoreServices.h>
#include <CoreAudio/CoreAudio.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/DefaultAudioOutput.h>
#include <AudioToolbox/AudioConverter.h>
#include <AudioToolbox/AUGraph.h>
#include <QuickTime/QuickTimeMusic.h>
#endif
#include <string.h>

#if (MAC_OS_X_VERSION_MAX_ALLOWED >= 1030)
#define BX_SOUND_OSX_CONVERTER_NEW_API 1
#endif

#ifdef BX_SOUND_OSX_use_converter
#ifndef BX_SOUND_OSX_CONVERTER_NEW_API
OSStatus MyRenderer (void *inRefCon, AudioUnitRenderActionFlags inActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, AudioBuffer *ioData);
OSStatus MyACInputProc (AudioConverterRef inAudioConverter, UInt32* outDataSize, void** outData, void* inUserData);
#else
OSStatus MyRenderer (void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData);
OSStatus MyACInputProc (AudioConverterRef inAudioConverter, UInt32 *ioNumberDataPackets, AudioBufferList *ioData, AudioStreamPacketDescription **outDataPacketDescription, void *inUserData);
#endif
#endif

// Global variables
#ifdef BX_SOUND_OSX_use_converter
AUGraph MidiGraph;
AudioUnit synthUnit;
#endif

#ifdef BX_SOUND_OSX_use_quicktime
SndChannelPtr WaveChannel;
ExtSoundHeader WaveInfo;
ExtSoundHeader WaveHeader[BX_SOUND_OSX_NBUF];
#endif

#ifdef BX_SOUND_OSX_use_converter
AudioUnit WaveOutputUnit = NULL;
AudioConverterRef WaveConverter = NULL;
#endif

bx_sound_osx_c::bx_sound_osx_c()
    :bx_sound_lowlevel_c()
{
    MidiOpen = 0;
    WaveOpen = 0;
    head = 0;
    tail = 0;
    for (int i=0; i<BX_SOUND_OSX_NBUF; i++)
        WaveLength[i] = 0;
    BX_INFO(("Sound lowlevel module 'osx' initialized"));
}

bx_sound_osx_c::~bx_sound_osx_c()
{
    // nothing for now
}


int bx_sound_osx_c::midiready()
{
    return BX_SOUNDLOW_OK;
}

int bx_sound_osx_c::openmidioutput(const char *mididev)
{
#ifdef BX_SOUND_OSX_use_converter
    ComponentDescription description;
    AUNode synthNode, outputNode;

    // Create the graph
    NewAUGraph (&MidiGraph);

    // Open the DLS Synth
    description.componentType           = kAudioUnitType_MusicDevice;
    description.componentSubType        = kAudioUnitSubType_DLSSynth;
    description.componentManufacturer   = kAudioUnitManufacturer_Apple;
    description.componentFlags          = 0;
    description.componentFlagsMask      = 0;
    AUGraphNewNode (MidiGraph, &description, 0, NULL, &synthNode);

    // Open the output device
    description.componentType           = kAudioUnitType_Output;
    description.componentSubType        = kAudioUnitSubType_DefaultOutput;
    description.componentManufacturer   = kAudioUnitManufacturer_Apple;
    description.componentFlags          = 0;
    description.componentFlagsMask      = 0;
    AUGraphNewNode (MidiGraph, &description, 0, NULL, &outputNode);

    // Connect the devices up
    AUGraphConnectNodeInput (MidiGraph, synthNode, 1, outputNode, 0);
    AUGraphUpdate (MidiGraph, NULL);

    // Open and initialize the audio units
    AUGraphOpen (MidiGraph);
    AUGraphInitialize (MidiGraph);

    // Turn off the reverb on the synth
    AUGraphGetNodeInfo (MidiGraph, synthNode, NULL, NULL, NULL, &synthUnit);
    UInt32 usesReverb = 0;
    AudioUnitSetProperty (synthUnit, kMusicDeviceProperty_UsesInternalReverb,
        kAudioUnitScope_Global,    0, &usesReverb, sizeof (usesReverb));

    // Start playing
    AUGraphStart (MidiGraph);
#endif
    BX_DEBUG(("openmidioutput(%s)", mididev));
    MidiOpen = 1;
    return BX_SOUNDLOW_OK;
}

int bx_sound_osx_c::sendmidicommand(int delta, int command, int length, Bit8u data[])
{
    BX_DEBUG(("sendmidicommand(%i,%02x,%i)", delta, command, length));
    if (!MidiOpen) return BX_SOUNDLOW_ERR;

#ifdef BX_SOUND_OSX_use_converter
    if (length <= 2) {
        Bit8u arg1 = (length >=1) ? data[0] : 0;
        Bit8u arg2 = (length >=2) ? data[1] : 0;
        MusicDeviceMIDIEvent (synthUnit, command, arg1, arg2, delta);
    }
    else {
        MusicDeviceSysEx (synthUnit, data, length);
    }
#endif
    return BX_SOUNDLOW_OK;
}

int bx_sound_osx_c::closemidioutput()
{
    BX_DEBUG(("closemidioutput()"));
    MidiOpen = 0;
#ifdef BX_SOUND_OSX_use_converter
    AUGraphStop (MidiGraph);
    AUGraphClose (MidiGraph);
#endif
    return BX_SOUNDLOW_OK;
}

#ifdef BX_SOUND_OSX_use_quicktime
#if BX_WITH_MACOS
pascal
#endif
void WaveCallbackProc (SndChannelPtr chan, SndCommand *cmd)
{
    // a new buffer is available, so increment tail pointer
    int *tail = (int *) (cmd->param2);
    (*tail)++;
}
#endif

int bx_sound_osx_c::openwaveoutput(const char *wavedev)
{
    OSStatus err;

    BX_DEBUG(("openwaveoutput(%s)", wavedev));

    // open the default output unit
#ifdef BX_SOUND_OSX_use_quicktime
    err = SndNewChannel (&WaveChannel, sampledSynth, 0, NewSndCallBackUPP(WaveCallbackProc));
    if (err != noErr) return BX_SOUNDLOW_ERR;
#endif
#ifdef BX_SOUND_OSX_use_converter
#ifndef BX_SOUND_OSX_CONVERTER_NEW_API
    err = OpenDefaultAudioOutput (&WaveOutputUnit);
    if (err != noErr) return BX_SOUNDLOW_ERR;

    AudioUnitInputCallback input;
    input.inputProc = MyRenderer;
    input.inputProcRefCon = (void *) this;
    AudioUnitSetProperty (WaveOutputUnit, kAudioUnitProperty_SetInputCallback,
        kAudioUnitScope_Global, 0, &input, sizeof(input));
#else
    ComponentDescription desc;
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_DefaultOutput;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;

    Component c = FindNextComponent(NULL, &desc);
    if (c == NULL) {
      BX_ERROR(("Core Audio: Unable to find default audio output component\n"));
      return BX_SOUNDLOW_ERR;
    }
    err = OpenAComponent(c, &WaveOutputUnit);
    if (err) {
      BX_ERROR(("Core Audio: Unable to open audio output (err=%X)\n", (unsigned int)err));
      return BX_SOUNDLOW_ERR;
    } 

    AURenderCallbackStruct input;
    input.inputProc = MyRenderer;
    input.inputProcRefCon = (void *) this;
    err = AudioUnitSetProperty (WaveOutputUnit,
        kAudioUnitProperty_SetRenderCallback,
        kAudioUnitScope_Input, 0, &input, sizeof(input));
    if (err) {
      BX_ERROR(("Core Audio: AudioUnitSetProperty error (err=%X)\n", (unsigned int)err));
      return BX_SOUNDLOW_ERR;
    }
#endif
    err = AudioUnitInitialize (WaveOutputUnit);
    if (err) {
      BX_ERROR(("Core Audio: AudioUnitInitialize error (err=%X)\n", (unsigned int)err));
      return BX_SOUNDLOW_ERR;
    } 
#endif

    WaveOpen = 1;
    return BX_SOUNDLOW_OK;
}

#ifdef BX_SOUND_OSX_use_converter
OSStatus bx_sound_osx_c::core_audio_pause() {
  OSStatus err = noErr;

  if (WaveOutputUnit) {
    err = AudioOutputUnitStop (WaveOutputUnit);
    if (err) {
      BX_ERROR(("Core Audio: nextbuffer(): AudioOutputUnitStop (err=%X)\n", (unsigned int)err)); 
    }
    WavePlaying = 0;
  }

  return err;
}

OSStatus bx_sound_osx_c::core_audio_resume() {
  OSStatus err = noErr;

  if (WaveConverter) {
    err = AudioConverterReset (WaveConverter);
    if (err) {
      BX_ERROR(("Core Audio: core_audio_resume(): AudioConverterReset (err=%X)\n", (unsigned int)err)); 
      return err;
    }
  }
  if (WaveOutputUnit) {
    err = AudioOutputUnitStart (WaveOutputUnit);
    if (err) {
      BX_ERROR(("Core Audio: core_audio_resume(): AudioOutputUnitStart (err=%X)\n", (unsigned int)err)); 
      return err;
    }
    WavePlaying = 1;
  }

  return err;
}
#endif

int bx_sound_osx_c::startwaveplayback(int frequency, int bits, bx_bool stereo, int format)
{
#ifdef BX_SOUND_OSX_use_converter
    static int oldfreq, oldbits, oldformat;
    static bx_bool oldstereo;
    AudioStreamBasicDescription srcFormat, dstFormat;
    UInt32 formatSize = sizeof(AudioStreamBasicDescription);
    OSStatus err;
#endif

    BX_DEBUG(("startwaveplayback(%d, %d, %d, %x)", frequency, bits, stereo, format));

#ifdef BX_SOUND_OSX_use_quicktime
    WaveInfo.samplePtr = NULL;
    WaveInfo.numChannels = stereo ? 2 : 1;
    WaveInfo.sampleRate = frequency << 16;  // sampleRate is a 16.16 fixed-point value
    WaveInfo.loopStart = 0;
    WaveInfo.loopEnd = 0;
    WaveInfo.encode = extSH;  // WaveInfo has type ExtSoundHeader
    WaveInfo.baseFrequency = 1;  // not sure what means. It's only a Uint8.
    WaveInfo.numFrames = 0;
    //WaveInfo.AIFFSampleRate = frequency;  // frequency as float80
    WaveInfo.markerChunk = NULL;

    WaveInfo.instrumentChunks = NULL;
    WaveInfo.AESRecording = NULL;
    WaveInfo.sampleSize = bits * WaveInfo.numChannels;
#endif

#ifdef BX_SOUND_OSX_use_converter
    if ((frequency == oldfreq) &&
        (bits == oldbits) &&
        (stereo == oldstereo) &&
        (format == oldformat))
      return BX_SOUNDLOW_OK;    // nothing to do

    oldfreq = frequency;
    oldbits = bits;
    oldstereo = stereo;
    oldformat = format;

    // update the source audio format
    UInt32 bytes = bits / 8;
    UInt32 channels = stereo ? 2 : 1;
    srcFormat.mSampleRate = (Float64) frequency;
    srcFormat.mFormatID = kAudioFormatLinearPCM;
    srcFormat.mFormatFlags = kLinearPCMFormatFlagIsPacked;
    if (format & 1) srcFormat.mFormatFlags |= kLinearPCMFormatFlagIsSignedInteger;
    srcFormat.mBytesPerPacket = channels * bytes;
    srcFormat.mFramesPerPacket = 1;
    srcFormat.mBytesPerFrame = channels * bytes;
    srcFormat.mChannelsPerFrame = channels;
    srcFormat.mBitsPerChannel = bytes * 8;

    if (WavePlaying) {
      err = AudioOutputUnitStop (WaveOutputUnit);
      if (err)
        BX_ERROR(("Core Audio: startwaveplayback(): AudioOutputUnitStop (err=%X)\n", (unsigned int)err)); 
    }
    if (WaveConverter) {
      err = AudioConverterDispose (WaveConverter);
      if (err)
        BX_ERROR(("Core Audio: startwaveplayback(): AudioConverterDispose (err=%X)\n", (unsigned int)err)); 
    }

    err = AudioUnitGetProperty (WaveOutputUnit, kAudioUnitProperty_StreamFormat,
        kAudioUnitScope_Output, 0, &dstFormat, &formatSize);
    if (err) {
      BX_ERROR(("Core Audio: startwaveplayback(): AudioUnitGetProperty (err=%X)\n", (unsigned int)err)); 
      return BX_SOUNDLOW_ERR;
    }

#ifdef BX_SOUND_OSX_CONVERTER_NEW_API
    // force interleaved mode
    dstFormat.mFormatFlags |= kAudioFormatFlagIsNonInterleaved;
    dstFormat.mBytesPerPacket = dstFormat.mBytesPerFrame = (dstFormat.mBitsPerChannel + 7) / 8;
    err = AudioUnitSetProperty (WaveOutputUnit, kAudioUnitProperty_StreamFormat,
        kAudioUnitScope_Input, 0, &dstFormat, sizeof(dstFormat));
    if (err) {
      BX_ERROR(("Core Audio: startwaveplayback(): AudioUnitSetProperty (err=%X)\n", (unsigned int)err)); 
      return BX_SOUNDLOW_ERR;
    }
#endif

    err = AudioConverterNew (&srcFormat, &dstFormat, &WaveConverter);
    if (err) {
      BX_ERROR(("Core Audio: startwaveplayback(): AudioConverterNew (err=%X)\n", (unsigned int)err)); 
      return BX_SOUNDLOW_ERR;
    }

    if (srcFormat.mChannelsPerFrame == 1 && dstFormat.mChannelsPerFrame == 2) {
        // map single-channel input to both output channels
        SInt32 map[2] = {0,0};
        err = AudioConverterSetProperty (WaveConverter,
                            kAudioConverterChannelMap, 
                            sizeof(map), (void*) map);
        if (err) {
          BX_ERROR(("Core Audio: startwaveplayback(): AudioConverterSetProperty (err=%X)\n", (unsigned int)err)); 
          return BX_SOUNDLOW_ERR;
        }
    }

    if (WavePlaying) {
      if (core_audio_resume() != noErr)
        return BX_SOUNDLOW_ERR;
    }
#endif

    return BX_SOUNDLOW_OK;
}

int bx_sound_osx_c::waveready()
{
    // HACK: the -4 is to keep from overwriting buffers that
    // have been sent, but possibly not yet played. There
    // should be a better way of doing this.
    if (WaveOpen && (head - tail < BX_SOUND_OSX_NBUF-4)) {
        return BX_SOUNDLOW_OK;
    }
    else {
#ifdef BX_SOUND_OSX_use_converter
        // If buffer is full, make sure sound is playing
        if (!WavePlaying) {
          if (core_audio_resume() != noErr)
            return BX_SOUNDLOW_ERR;
        }
#endif
        return BX_SOUNDLOW_ERR;
    }
}

int bx_sound_osx_c::sendwavepacket(int length, Bit8u data[])
{
#ifdef BX_SOUND_OSX_use_quicktime
    SndCommand mySndCommand;
#endif

    BX_DEBUG(("sendwavepacket(%d, %p), head=%u", length, data, head));

    // sanity check
    if ((!WaveOpen) || (head - tail >= BX_SOUND_OSX_NBUF))
        return BX_SOUNDLOW_ERR;

    // find next available buffer
    int n = head++ % BX_SOUND_OSX_NBUF;

    // put data in buffer
    memcpy(WaveData[n], data, length);
    WaveLength[n] = length;

#ifdef BX_SOUND_OSX_use_quicktime
    memcpy(&WaveHeader[n], &WaveInfo, sizeof(WaveInfo));
    WaveHeader[n].samplePtr = (char *) (WaveData[n]);
    WaveHeader[n].numFrames = length * 8 / WaveInfo.sampleSize;
#endif
#ifdef BX_SOUND_OSX_use_converter
    // make sure that the sound is playing
    if (!WavePlaying) {
      if (core_audio_resume() != noErr)
        return BX_SOUNDLOW_ERR;
    }
#endif

#ifdef BX_SOUND_OSX_use_quicktime
    // queue buffer to play
    mySndCommand.cmd = bufferCmd;
    mySndCommand.param1 = 0;
    mySndCommand.param2 = (long)(&WaveHeader[n]);
    SndDoCommand(WaveChannel, &mySndCommand, TRUE);

    // queue callback for when buffer finishes
    mySndCommand.cmd = callBackCmd;
    mySndCommand.param1 = 0;
    mySndCommand.param2 = (long)(&tail);
    SndDoCommand(WaveChannel, &mySndCommand, TRUE);
#endif

    return BX_SOUNDLOW_OK;
}

int bx_sound_osx_c::stopwaveplayback()
{
    return BX_SOUNDLOW_OK;
}

int bx_sound_osx_c::closewaveoutput()
{
#ifdef BX_SOUND_OSX_use_converter
    if (WavePlaying) AudioOutputUnitStop (WaveOutputUnit);
    if (WaveConverter) AudioConverterDispose (WaveConverter);
    if (WaveOutputUnit) CloseComponent (WaveOutputUnit);
    WavePlaying = 0;
    WaveOpen = 0;
    WaveConverter = NULL;
    WaveOutputUnit = NULL;
#endif
    return BX_SOUNDLOW_OK;
}

#ifdef BX_SOUND_OSX_use_converter
#ifndef BX_SOUND_OSX_CONVERTER_NEW_API
OSStatus MyRenderer (void *inRefCon, AudioUnitRenderActionFlags inActionFlags,
    const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, AudioBuffer *ioData)
{
    OSStatus err;
    UInt32 size = ioData->mDataByteSize;
    err = AudioConverterFillBuffer (WaveConverter, MyACInputProc, inRefCon, &size, ioData->mData);
    return err;
}

OSStatus MyACInputProc (AudioConverterRef inAudioConverter,
    UInt32* outDataSize, void** outData, void* inUserData)
{
    bx_sound_osx_c *self = (bx_sound_osx_c*) inUserData;
    self->nextbuffer ((int*) outDataSize, outData);
    return noErr;
}
#else
OSStatus MyRenderer (void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData)
{
    UInt32 packets;
    AudioStreamBasicDescription dstFormat;
    UInt32 formatSize = sizeof(AudioStreamBasicDescription);
    OSStatus err = noErr;

    err = AudioConverterGetProperty (WaveConverter,
      kAudioConverterCurrentOutputStreamDescription,
      &formatSize, &dstFormat);
    if (err) {
      return err;
    }    

    packets = inNumberFrames / dstFormat.mFramesPerPacket; 
    err = AudioConverterFillComplexBuffer(WaveConverter,
      MyACInputProc, inRefCon, &packets, ioData, NULL);
     
    return err;
}

OSStatus MyACInputProc (AudioConverterRef inAudioConverter, UInt32 *ioNumberDataPackets, AudioBufferList *ioData, AudioStreamPacketDescription **outDataPacketDescription, void *inUserData)
{
    OSStatus err;
    bx_sound_osx_c *self = (bx_sound_osx_c*) inUserData;
    AudioStreamBasicDescription srcFormat;
    UInt32 formatSize = sizeof(AudioStreamBasicDescription);

    err = AudioConverterGetProperty (inAudioConverter,
      kAudioConverterCurrentInputStreamDescription,
      &formatSize, &srcFormat);
    if (err) {
      *ioNumberDataPackets = 0;
      return err;
    }    
    int outDataSize = *ioNumberDataPackets * srcFormat.mBytesPerPacket;
    void *outData = ioData->mBuffers[0].mData; 
    self->nextbuffer ((int*) &outDataSize, &outData);
    *ioNumberDataPackets = outDataSize / srcFormat.mBytesPerPacket;
    ioData->mBuffers[0].mDataByteSize = outDataSize;
    ioData->mBuffers[0].mData = outData; 

    return noErr;
}
#endif

void bx_sound_osx_c::nextbuffer (int *outDataSize, void **outData)
{
    BX_DEBUG(("nextbuffer(), tail=%u", tail));
    if (head - tail <= 0) {
        *outData = NULL;
        *outDataSize = 0;

        // We are getting behind, so stop the output for now
        core_audio_pause();
    }
    else {
        int n = tail % BX_SOUND_OSX_NBUF;
        *outData = (void *) (WaveData[n]);
        *outDataSize = WaveLength[n];
        tail++;
    }
}
#endif

int bx_sound_osx_c::openwaveinput(const char *wavedev, sound_record_handler_t rh)
{
  UNUSED(wavedev);
  record_handler = rh;
  if (rh != NULL) {
    record_timer_index = bx_pc_system.register_timer(this, record_timer_handler, 1, 1, 0, "soundosx");
    // record timer: inactive, continuous, frequency variable
  }
  // TODO
  return BX_SOUNDLOW_OK;
}

int bx_sound_osx_c::startwaverecord(int frequency, int bits, bx_bool stereo, int format)
{
  Bit64u timer_val;
  Bit8u shift = 0;

  UNUSED(format);
  if (record_timer_index != BX_NULL_TIMER_HANDLE) {
    if (bits == 16) shift++;
    if (stereo) shift++;
    record_packet_size = (frequency / 10) << shift; // 0.1 sec
    if (record_packet_size > BX_SOUNDLOW_WAVEPACKETSIZE) {
      record_packet_size = BX_SOUNDLOW_WAVEPACKETSIZE;
    }
    timer_val = (Bit64u)record_packet_size * 1000000 / (frequency << shift);
    bx_pc_system.activate_timer(record_timer_index, (Bit32u)timer_val, 1);
  }
  // TODO
  return BX_SOUNDLOW_OK;
}

int bx_sound_osx_c::getwavepacket(int length, Bit8u data[])
{
  // TODO
  memset(data, 0, length);
  return BX_SOUNDLOW_OK;
}

int bx_sound_osx_c::stopwaverecord()
{
  if (record_timer_index != BX_NULL_TIMER_HANDLE) {
    bx_pc_system.deactivate_timer(record_timer_index);
  }
  // TODO
  return BX_SOUNDLOW_OK;
}

int bx_sound_osx_c::closewaveinput()
{
  stopwaverecord();
  // TODO
  return BX_SOUNDLOW_OK;
}

void bx_sound_osx_c::record_timer_handler(void *this_ptr)
{
  bx_sound_osx_c *class_ptr = (bx_sound_osx_c *) this_ptr;

  class_ptr->record_timer();
}

void bx_sound_osx_c::record_timer(void)
{
  record_handler(this, record_packet_size);
}

#endif  // defined(macintosh)
