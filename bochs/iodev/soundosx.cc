/////////////////////////////////////////////////////////////////////////
// $Id: soundosx.cc,v 1.1 2004-02-09 22:23:48 danielg4 Exp $
/////////////////////////////////////////////////////////////////////////

// This file (SOUNDOSX.CC) written and donated by Brian Huffman

#ifdef PARANOID
#include <MacTypes.h>
#endif
#include "bochs.h"

#if defined(macintosh) && BX_SUPPORT_SB16
#define LOG_THIS bx_sb16.

#if BX_WITH_MACOS
#include <QuickTimeMusic.h>
#else
#include <CoreAudio/CoreAudio.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/DefaultAudioOutput.h>
#include <AudioToolbox/AudioConverter.h>
#include <AudioToolbox/AUGraph.h>
#include <QuickTime/QuickTimeMusic.h>
#endif
#include <string.h>

#ifdef BX_SOUND_OSX_use_converter
OSStatus MyRenderer (void *inRefCon, AudioUnitRenderActionFlags inActionFlags,
    const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, AudioBuffer *ioData);
OSStatus MyACInputProc (AudioConverterRef inAudioConverter, UInt32* outDataSize,
    void** outData, void* inUserData);
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

bx_sound_osx_c::bx_sound_osx_c(bx_sb16_c *sb16)
    :bx_sound_output_c(sb16)
{
    this->sb16 = sb16;

    MidiOpen = 0;
    WaveOpen = 0;
    head = 0;
    tail = 0;
    for (int i=0; i<BX_SOUND_OSX_NBUF; i++)
        WaveLength[i] = 0;
}

bx_sound_osx_c::~bx_sound_osx_c()
{
    // nothing for now
}


int bx_sound_osx_c::midiready()
{
    return BX_SOUND_OUTPUT_OK;
}

int bx_sound_osx_c::openmidioutput(char *device)
{
#ifdef BX_SOUND_OSX_use_converter
    ComponentDescription description;
    AUNode synthNode, outputNode;

    // Create the graph
    NewAUGraph (&MidiGraph);

    // Open the DLS Synth
    description.componentType           = kAudioUnitComponentType;
    description.componentSubType        = kAudioUnitSubType_MusicDevice;
    description.componentManufacturer   = kAudioUnitID_DLSSynth;
    description.componentFlags          = 0;
    description.componentFlagsMask      = 0;
    AUGraphNewNode (MidiGraph, &description, 0, NULL, &synthNode);

    // Open the output device
    description.componentType           = kAudioUnitComponentType;
    description.componentSubType        = kAudioUnitSubType_Output;
    description.componentManufacturer   = kAudioUnitID_DefaultOutput;
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
    WRITELOG( WAVELOG(4), "openmidioutput(%s)", device);
    MidiOpen = 1;
    return BX_SOUND_OUTPUT_OK;
}

int bx_sound_osx_c::sendmidicommand(int delta, int command, int length, Bit8u data[])
{
    WRITELOG( WAVELOG(5), "sendmidicommand(%i,%02x,%i)", delta, command, length);
    if (!MidiOpen) return BX_SOUND_OUTPUT_ERR;

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
    return BX_SOUND_OUTPUT_OK;
}

int bx_sound_osx_c::closemidioutput()
{
    WRITELOG( WAVELOG(4), "closemidioutput()");
    MidiOpen = 0;
#ifdef BX_SOUND_OSX_use_converter
    AUGraphStop (MidiGraph);
    AUGraphClose (MidiGraph);
#endif
    return BX_SOUND_OUTPUT_OK;
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

int bx_sound_osx_c::openwaveoutput(char *device)
{
    OSStatus err;

    WRITELOG( WAVELOG(4), "openwaveoutput(%s)", device);

    // open the default output unit
#ifdef BX_SOUND_OSX_use_quicktime
    err = SndNewChannel (&WaveChannel, sampledSynth, 0, NewSndCallBackUPP(WaveCallbackProc));
    if (err != noErr) return BX_SOUND_OUTPUT_ERR;
#endif
#ifdef BX_SOUND_OSX_use_converter
    err = OpenDefaultAudioOutput (&WaveOutputUnit);
    if (err != noErr) return BX_SOUND_OUTPUT_ERR;
    AudioUnitInitialize (WaveOutputUnit);

    // Set up a callback function to generate output to the output unit
    AudioUnitInputCallback input = { inputProc: MyRenderer, inputProcRefCon: (void *) this };
    AudioUnitSetProperty (WaveOutputUnit, kAudioUnitProperty_SetInputCallback, 
        kAudioUnitScope_Global, 0, &input, sizeof(input));
#endif

    WaveOpen = 1;
    return BX_SOUND_OUTPUT_OK;
}

int bx_sound_osx_c::startwaveplayback(int frequency, int bits, int stereo, int format)
{
#ifdef BX_SOUND_OSX_use_converter
    static int oldfreq, oldbits, oldstereo, oldformat;
    AudioStreamBasicDescription srcFormat, dstFormat;
    UInt32 formatSize = sizeof(AudioStreamBasicDescription);
#endif

    WRITELOG( WAVELOG(4), "startwaveplayback(%d, %d, %d, %x)", frequency, bits, stereo, format);

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
    if ( (frequency == oldfreq) &&
        (bits == oldbits) && 
        (stereo == oldstereo) &&
        (format == oldformat) )
        return BX_SOUND_OUTPUT_OK;    // nothing to do

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

    if (WavePlaying) AudioOutputUnitStop (WaveOutputUnit);
    if (WaveConverter) AudioConverterDispose (WaveConverter);

    AudioUnitGetProperty (WaveOutputUnit, kAudioUnitProperty_StreamFormat,
        kAudioUnitScope_Output, 0, &dstFormat, &formatSize);

    AudioConverterNew (&srcFormat, &dstFormat, &WaveConverter);

    if (srcFormat.mChannelsPerFrame == 1 && dstFormat.mChannelsPerFrame == 2) {
        // map single-channel input to both output channels
        SInt32 map[2] = {0,0};
        AudioConverterSetProperty (WaveConverter, kAudioConverterChannelMap, 
                            sizeof(map), (void*) map);
    }

    if (WavePlaying) AudioOutputUnitStart (WaveOutputUnit);
#endif

    return BX_SOUND_OUTPUT_OK;
}

int bx_sound_osx_c::waveready()
{
    // HACK: the -4 is to keep from overwriting buffers that
    // have been sent, but possibly not yet played. There
    // should be a better way of doing this.
    if (WaveOpen && (head - tail < BX_SOUND_OSX_NBUF-4)) {
        return BX_SOUND_OUTPUT_OK;
    }
    else {
#ifdef BX_SOUND_OSX_use_converter
        // If buffer is full, make sure sound is playing
        if (WaveOutputUnit && !WavePlaying) {
            AudioOutputUnitStart (WaveOutputUnit);
            WavePlaying = 1;
        }
#endif
        return BX_SOUND_OUTPUT_ERR;
    }
}

int bx_sound_osx_c::sendwavepacket(int length, Bit8u data[])
{
#ifdef BX_SOUND_OSX_use_quicktime
    SndCommand mySndCommand;
#endif

    WRITELOG( WAVELOG(4), "sendwavepacket(%d, %p), head=%u", length, data, head);

    // sanity check
    if ((!WaveOpen) || (head - tail >= BX_SOUND_OSX_NBUF))
        return BX_SOUND_OUTPUT_ERR;

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
        AudioOutputUnitStart (WaveOutputUnit);
        WavePlaying = 1;
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

    return BX_SOUND_OUTPUT_OK;
}

int bx_sound_osx_c::stopwaveplayback()
{
    return BX_SOUND_OUTPUT_OK;
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
    return BX_SOUND_OUTPUT_OK;
}

#ifdef BX_SOUND_OSX_use_converter
OSStatus MyRenderer (void *inRefCon, AudioUnitRenderActionFlags inActionFlags,
    const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, AudioBuffer *ioData)
{
    UInt32 size = ioData->mDataByteSize;
    AudioConverterFillBuffer (WaveConverter, MyACInputProc, inRefCon, &size, ioData->mData);
    return noErr;
}

OSStatus MyACInputProc (AudioConverterRef inAudioConverter,
    UInt32* outDataSize, void** outData, void* inUserData)
{
    bx_sound_osx_c *self = (bx_sound_osx_c*) inUserData;
    self->nextbuffer ((int*) outDataSize, outData);
    return noErr;
}

void bx_sound_osx_c::nextbuffer (int *outDataSize, void **outData)
{
    WRITELOG( WAVELOG(4), "nextbuffer(), tail=%u", tail);
    if (head - tail <= 0) {
        *outData = NULL;
        *outDataSize = 0;

        // We are getting behind, so stop the output for now
        AudioOutputUnitStop (WaveOutputUnit);
        WavePlaying = 0;
    }
    else {
        int n = tail % BX_SOUND_OSX_NBUF;
        *outData = (void *) (WaveData[n]);
        *outDataSize = WaveLength[n];
        tail++;
    }
}
#endif

#endif  // defined(macintosh)
