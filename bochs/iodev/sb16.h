/////////////////////////////////////////////////////////////////////////
// $Id: sb16.h,v 1.7 2002-08-27 19:54:46 bdenney Exp $
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

// This file (SB16.H) written and donated by Josef Drexler


#if BX_USE_SB16_SMF
#  define BX_SB16_SMF   static
#  define BX_SB16_THIS  bx_sb16.
#  define BX_SB16_THISP (&bx_sb16)
#else
#  define BX_SB16_SMF
#  define BX_SB16_THIS  this->
#  define BX_SB16_THISP (this)
#endif

#if BX_USE_SOUND_VIRTUAL
#  define BX_SOUND_VIRTUAL  virtual
#  define BX_SOUND_OUTPUT_C_DEF bx_sound_output_c
#else
#  define BX_SOUND_VIRTUAL
#  define BX_SOUND_OUTPUT_C_DEF BX_SOUND_OUTPUT_C
#endif

// If the buffer commands are to be inlined:
#define BX_SB16_BUFINL BX_CPP_INLINE
// BX_CPP_INLINE is defined to the inline keyword for the C++ compiler.

// maximum number of patch translations
#define BX_SB16_PATCHTABLESIZE 1024

// the resources. Of these, IRQ and DMA's can be changed via a DSP command
#define BX_SB16_IO      0x220       // IO base address of DSP, mixer & FM part
#define BX_SB16_IOLEN   16          // number of addresses covered
#define BX_SB16_IOMPU   0x330       // IO base address of MPU402 part
#define BX_SB16_IOMPULEN 4          // number of addresses covered
#define BX_SB16_IOADLIB 0x388       // equivalent to 0x220..0x223 and 0x228..0x229
#define BX_SB16_IOADLIBLEN 4        // number of addresses covered
#define BX_SB16_IRQ     bx_sb16.currentirq
#define BX_SB16_IRQMPU  BX_SB16_IRQ // IRQ for the MPU401 part - same value
#define BX_SB16_DMAL    bx_sb16.currentdma8
#define BX_SB16_DMAH    bx_sb16.currentdma16

/*
   A few notes:
   IRQ, DMA8bit and DMA16bit are for the DSP part. These
   are changeable at runtime in mixer registers 0x80 and 0x81.
   The defaults after a mixer initialization are IRQ 5, DMA8 1, no DMA16

   Any of the address lengths can be zero to disable that particular
   subdevice. Turning off the DSP still leaves FM music enabled on the
   BX_SB16_IOADLIB ports, unless those are disabled as well.
   
   BX_SB16_IOMPULEN should be 4 or 2. In the latter case, the emulator
   is completely invisible, and runtime changes are not possible

   BX_SB16_IOADLIBLEN should be 2 or 4. If 0, Ports 0x388.. don't
   get used, but the OPL2 can still be accessed at 0x228..0x229. 
   If 2, the usual Adlib emulation is enabled. If 4, an OPL3 is
   emulated at adresses 0x388..0x38b, or two separate OPL2's.
*/

/* Definitions for the output functions */
#define BX_SOUND_OUTPUT_OK   0
#define BX_SOUND_OUTPUT_ERR  1
#define BX_SOUND_OUTPUT_WAVEPACKETSIZE  4096
             // the is the size of a DMA chunk sent to output
             // it should not be too large to avoid lag, and not too
             // small to avoid unnecessary overhead.

#define BX_SB16_MIX_REG  0x90         // total number of mixer registers

// The array containing an instrument/bank remapping
struct bx_sb16_ins_map {
  Bit8u oldbankmsb, oldbanklsb, oldprogch;
  Bit8u newbankmsb, newbanklsb, newprogch;
};

// One operator of the FM emulation
#define BX_SB16_FM_NOP       36 // OPL3 has 36 operators
#define BX_SB16_FM_OPB       6  // one operator has 6 bytes
typedef Bit8u bx_sb16_fm_operator[BX_SB16_FM_OPB];
   /* Explanation of the values: 
      (note [xx] is one bit for xx; [5 xx] is five bits for xx,
      all bits listed MSB to LSB)

      [0] = [Tremolo][Vibrato][Sustain][KSR][4 Frequency Multiply]
      [1] = [2 Key Scale Level][6 Output Level for modulators, reserved for others]
      [2] = [4 Attack Rate][4 Decay Rate]
      [3] = [4 Sustain Level][4 Release Rate]
      [4] = [2 reserved][Right][Left][3 Feedback Factor][SynthType]
      [5] = [5 reserved][3 Waveform Select]

      Frequency and Output Level are really properties of the channel,
      so they get stored there. However, Output Level of the modulator
      in FM synthesis varies the instrument.

      [4] is only set for the first operator, and zeroed for the others.

      All reserved bits are zeroed.
   */

// One channel (1 to 4 operators)
#define BX_SB16_FM_NCH         18      // OPL3 has at most 18 channels

typedef struct {
  int nop;          // number of operators used: 0=disabled, 1=percussion, 2 or 4=melodic
  int ncarr;        // how many carriers this channel has (1..3)
  int opnum[4];     // operator numbers
  Bit16u freq;      // frequency (in a special code)
  Bit32u afreq;     // actual frequency in milli-Hertz (10^-3 Hz)
  Boolean freqch;   // byte of the frequency that has changed recently
  Bit8u midichan;   // assigned midi channel
  Boolean needprogch;  // has the instrument changed
  Bit8u midinote;   // currently playing midi note
  Boolean midion;     // is the note on
  Bit16u midibend;  // current value of the pitch bender
  Bit8u outputlevel[4];// 6-bit output level attenuations
  Bit8u midivol;    // current midi volume (velocity)
} bx_sb16_fm_channel;


// This is the class for the input and
// output FIFO buffers of the SB16

class bx_sb16_buffer {
public:

  BX_SB16_BUFINL bx_sb16_buffer(void);
  BX_SB16_BUFINL ~bx_sb16_buffer(void);
  BX_SB16_BUFINL void init(int bufferlen);
  BX_SB16_BUFINL void reset();

      /* These functions return 1 on success and 0 on error */
  BX_SB16_BUFINL Boolean put(Bit8u data);    // write one byte in the buffer
  BX_SB16_BUFINL Boolean puts(char *data, ...);  // write a formatted string to the buffer
  BX_SB16_BUFINL Boolean get(Bit8u *data);   // read the next available byte
  BX_SB16_BUFINL Boolean getw(Bit16u *data); // get word, in order lo/hi
  BX_SB16_BUFINL Boolean getw1(Bit16u *data);// get word, in order hi/lo
  BX_SB16_BUFINL Boolean full(void);         // is the buffer full?
  BX_SB16_BUFINL Boolean empty(void);        // is it empty?

  BX_SB16_BUFINL void flush(void);           // empty the buffer
  BX_SB16_BUFINL int bytes(void);            // return number of bytes in the buffer
  BX_SB16_BUFINL Bit8u peek(int ahead);      // peek ahead number of bytes

      /* These are for caching the command number */
  BX_SB16_BUFINL void newcommand(Bit8u newcmd, int bytes);   // start a new command with length bytes
  BX_SB16_BUFINL Bit8u currentcommand(void); // return the current command
  BX_SB16_BUFINL void clearcommand(void);    // clear the command
  BX_SB16_BUFINL Boolean commanddone(void);  // return if all bytes have arrived
  BX_SB16_BUFINL Boolean hascommand(void);   // return if there is a pending command
  BX_SB16_BUFINL int commandbytes(void);     // return the length of the command


private:
  Bit8u *buffer;
  int head,tail,length;
  Bit8u command;
  Boolean havecommand;
  int bytesneeded;
};


// forward definition
class BX_SOUND_OUTPUT_C_DEF;

// The actual emulator class, emulating the sound blaster ports
class bx_sb16_c : public logfunctions {
public:

  bx_sb16_c(void);
  ~bx_sb16_c(void);
  BX_SB16_SMF void init(bx_devices_c *);
  BX_SB16_SMF void reset(unsigned type);

      /* DMA input and output, 8 and 16 bit, have to be public */
  BX_SB16_SMF void   dma_write8(Bit8u *data_byte);
  BX_SB16_SMF void   dma_read8(Bit8u *data_byte);
  BX_SB16_SMF void   dma_write16(Bit16u *data_word);
  BX_SB16_SMF void   dma_read16(Bit16u *data_word);

      /* the OPL timer event handler has to be called by the timer */
  BX_SB16_SMF void   opl_timerevent();

      /* Make writelog available to output functions */
  BX_SB16_SMF void   writelog(int loglevel, const char *str, ...);

      // DMA8 and DMA16 also public, they're read in devices.cc
  int currentdma8;
  int currentdma16;

private:

  FILE *logfile;
  FILE *midifile,*wavefile;     // the output files or devices
  BX_SOUND_OUTPUT_C_DEF *output;// the output class
  int currentirq;

  // the MPU 401 relevant variables
  struct bx_sb16_mpu_struct {
    bx_sb16_buffer datain, dataout, cmd, midicmd;
    Boolean uartmode, irqpending, forceuartmode, singlecommand;

    int banklsb[BX_SB16_PATCHTABLESIZE];
    int bankmsb[BX_SB16_PATCHTABLESIZE];   // current patch lists
    int program[BX_SB16_PATCHTABLESIZE];

    int outputinit, timer_handle;
    int current_timer;                         // no. of delta times passed
    Bit32u last_delta_time;                    // timer value at last command
  } mpu401;

  // the DSP variables
  struct bx_sb16_dsp_struct {
    bx_sb16_buffer datain, dataout;
    Bit8u resetport;                           // last value written to the reset port
    Bit8u speaker,prostereo;                   // properties of the sound input/output
    Boolean irqpending;                        // Is an IRQ pending (not ack'd)
    Boolean midiuartmode;                      // Is the DSP in MIDI UART mode
    struct bx_sb16_dsp_dma_struct {
      // Properties of the current DMA transfer:
      // mode= 0: no transfer, 1: single-cycle transfer, 2: auto-init DMA
      // bits= 8 or 16
      // fifo= ??  Bit used in DMA command, no idea what it means...
      // output= 0: input, 1: output
      // bps= bytes per sample; =(dmabits/8)*(dmastereo+1)
      // stereo= 0: mono, 1: stereo
      // issigned= 0: unsigned data, 1: signed data
      // highspeed= 0: normal mode, 1: highspeed mode (only SBPro)
      // timer= so many us between data bytes
      int mode, bits, fifo, output, bps;
      int stereo, issigned, highspeed, format, timer;
      Bit16u count;     // bytes remaining in this transfer
      Bit8u *chunk;	// buffers up to BX_SB16_WAVEPACKETSIZE bytes
      int chunkindex;	// index into the buffer
      int chunkcount;   // for input: size of the recorded input
      Bit16u timeconstant;
      Bit16u blocklength, samplerate;
    } dma;
    int timer_handle;   // handle for the DMA timer
    int outputinit;	// have the output functions been initialized
  } dsp;

  enum bx_sb16_fm_mode {single, adlib, dual, opl3};

  // the variables common to all FM emulations
  struct bx_sb16_opl_struct {
    bx_sb16_fm_mode mode;
    // modes: single: one OPL2 (OPL3 disabled), 
    //        adlib:  one OPL2 (no OPL3),
    //        dual:   two seperate OPL2
    //        opl3:   one OPL3 (enabled)

    int timer_handle;
    int timer_running;
    Bit16u midichannels;          // bitmask: unused midichannels
    int drumchannel;              // midi channel for percussion (10)
    int index[2];                 // index register for the two chips
    int wsenable[2];              // wave form select enable
    int timer[4];                 // two timers on each chip
    int timerinit[4];             // initial timer counts
    int tmask[2];                 // the timer masking byte for both chips
    int tflag[2];                 // shows if the timer overflow has occured
    int percmode[2];              // percussion mode enabled
    int cyhhnote[2];              // cymbal and high hat midi notes
    int cyhhon[2];                // cymbal and high hat notes on
    bx_sb16_fm_operator oper[BX_SB16_FM_NOP];
    bx_sb16_fm_channel chan[BX_SB16_FM_NCH];
  } opl;

  struct bx_sb16_mixer_struct {
    Bit8u regindex;
    Bit8u reg[BX_SB16_MIX_REG];
  } mixer;

  struct bx_sb16_emul_struct {
    bx_sb16_buffer datain, dataout;
    bx_sb16_ins_map remaplist[256];
    int remaps;
  } emuldata;

  bx_devices_c *devices;

      /* the MPU 401 part of the emulator */
  BX_SB16_SMF Bit32u mpu_status();                   // read status port   3x1
  BX_SB16_SMF void   mpu_command(Bit32u value);      // write command port 3x1
  BX_SB16_SMF Bit32u mpu_dataread();                 // read data port     3x0
  BX_SB16_SMF void   mpu_datawrite(Bit32u value);    // write data port    3x0
  BX_SB16_SMF void   mpu_mididata(Bit32u value);     // get a midi byte
  static void   mpu_timer (void *);

      /* The DSP part */
  BX_SB16_SMF void   dsp_reset(Bit32u value);        // write to reset port 2x6
  BX_SB16_SMF Bit32u dsp_dataread();                 // read from data port 2xa
  BX_SB16_SMF void   dsp_datawrite(Bit32u value);    // write to data port  2xa
  BX_SB16_SMF Bit32u dsp_bufferstatus();             // read buffer status  2xc
  BX_SB16_SMF Bit32u dsp_status();                   // read dsp status     2xe
  BX_SB16_SMF void   dsp_getsamplebyte(Bit8u value);
  BX_SB16_SMF Bit8u  dsp_putsamplebyte();
  BX_SB16_SMF void   dsp_sendwavepacket();
  BX_SB16_SMF void   dsp_getwavepacket();
  BX_SB16_SMF Bit32u dsp_irq16ack();                 // ack 16 bit IRQ      2xf
  BX_SB16_SMF void   dsp_dma(Bit8u command, Bit8u mode, Bit16u length, Bit8u comp);
						     // initiate a DMA transfer
  BX_SB16_SMF void   dsp_dmadone();		     // stop a DMA transfer
  BX_SB16_SMF void   dsp_enabledma();		     // enable the transfer
  BX_SB16_SMF void   dsp_disabledma();		     // temporarily disable DMA
  static void   dsp_dmatimer (void *);

      /* The mixer part */
  BX_SB16_SMF Bit32u mixer_readdata(void);
  BX_SB16_SMF void   mixer_writedata(Bit32u value);
  BX_SB16_SMF void   mixer_writeregister(Bit32u value);
  BX_SB16_SMF void   set_irq_dma();

      /* The emulator ports to change emulator properties */
  BX_SB16_SMF Bit32u emul_read (void);               // read emulator port
  BX_SB16_SMF void   emul_write(Bit32u value);       // write emulator port

      /* The FM emulation part */
  BX_SB16_SMF void   opl_entermode(bx_sb16_fm_mode newmode);
  BX_SB16_SMF Bit32u opl_status(int chipid);
  BX_SB16_SMF void   opl_index(Bit32u value, int chipid);
  BX_SB16_SMF void   opl_data(Bit32u value, int chipid);
  static void   opl_timer(void *);
  BX_SB16_SMF void   opl_changeop(int channum, int opernum, int byte, int value);
  BX_SB16_SMF void   opl_settimermask(int value, int chipid);
  BX_SB16_SMF void   opl_set4opmode(int new4opmode);
  BX_SB16_SMF void   opl_setmodulation(int channel);
  BX_SB16_SMF void   opl_setpercussion(Bit8u value, int chipid);
  BX_SB16_SMF void   opl_setvolume(int channel, int opnum, int outlevel);
  BX_SB16_SMF void   opl_setfreq(int channel);
  BX_SB16_SMF void   opl_keyonoff(int channel, Boolean onoff);
  BX_SB16_SMF void   opl_midichannelinit(int channel);

      /* several high level sound handlers */
  BX_SB16_SMF int    currentdeltatime();
  BX_SB16_SMF void   processmidicommand(Boolean force);
  BX_SB16_SMF void   midiremapprogram(int channel);  // remap program change
  BX_SB16_SMF int    converttodeltatime(Bit32u deltatime, Bit8u value[4]);
  BX_SB16_SMF void   writemidicommand(int command, int length, Bit8u data[]);
  BX_SB16_SMF void   writedeltatime(Bit32u deltatime);
						// write in delta time coding

  BX_SB16_SMF void   initmidifile();            // Write midi file header
  BX_SB16_SMF void   finishmidifile();          // write track length etc.
  BX_SB16_SMF void   initvocfile();             // Write voc file header
  BX_SB16_SMF void   writevocblock(int block, Bit32u headerlen, Bit8u header[],
				   Bit32u datalen, Bit8u data[]);
  BX_SB16_SMF void   finishvocfile();           // close voc file

      /* The port IO multiplexer functions */

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);

#if !BX_USE_SB16_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
};

// The class with the output functions
class bx_sound_output_c : public logfunctions {
public:

      /* These functions are the sound output functions, sending
	 the music/sound to the OS specific driver.
	 They are in a different file (sound.cc) because they are
	 non-portable, while everything in sb16.cc is portable */

  bx_sound_output_c(bx_sb16_c *sb16);
  BX_SOUND_VIRTUAL ~bx_sound_output_c();

  BX_SOUND_VIRTUAL int    waveready();
  BX_SOUND_VIRTUAL int    midiready();

  BX_SOUND_VIRTUAL int    openmidioutput(char *device);
  BX_SOUND_VIRTUAL int    sendmidicommand(int delta, int command, int length, Bit8u data[]);
  BX_SOUND_VIRTUAL int    closemidioutput();

  BX_SOUND_VIRTUAL int    openwaveoutput(char *device);
  BX_SOUND_VIRTUAL int    startwaveplayback(int frequency, int bits, int stereo, int format);
  BX_SOUND_VIRTUAL int    sendwavepacket(int length, Bit8u data[]);
  BX_SOUND_VIRTUAL int    stopwaveplayback();
  BX_SOUND_VIRTUAL int    closewaveoutput();
};

extern bx_sb16_c bx_sb16;

#define WRITELOG        sb16->writelog
#define BOTHLOG(x)      (x)
#define MIDILOG(x)      ((bx_options.sb16.Omidimode->get ()>0?x:0x7f))
#define WAVELOG(x)      ((bx_options.sb16.Owavemode->get ()>0?x:0x7f))

#include "soundlnx.h"
#include "soundwin.h"
