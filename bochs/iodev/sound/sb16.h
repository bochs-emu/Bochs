/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001-2020  The Bochs Project
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

// The original version of the SB16 support written and donated by Josef Drexler

#ifndef BX_IODEV_SB16_H
#define BX_IODEV_SB16_H

#if BX_USE_SB16_SMF
#  define BX_SB16_SMF   static
#  define BX_SB16_THIS  theSB16Device->
#  define BX_SB16_THISP (theSB16Device)
#else
#  define BX_SB16_SMF
#  define BX_SB16_THIS  this->
#  define BX_SB16_THISP (this)
#endif

// If the buffer commands are to be inlined:
#define BX_SB16_BUFINL BX_CPP_INLINE
// BX_CPP_INLINE is defined to the inline keyword for the C++ compiler.

// maximum number of MIDI remaps
#define BX_SB16_MAX_REMAPS 256

// the resources. Of these, IRQ and DMA's can be changed via a DSP command
#define BX_SB16_IO      0x220       // IO base address of DSP, mixer & FM part
#define BX_SB16_IOLEN   16          // number of addresses covered
#define BX_SB16_IOMPU   0x330       // IO base address of MPU402 part
#define BX_SB16_IOMPULEN 4          // number of addresses covered
#define BX_SB16_IOADLIB 0x388       // equivalent to 0x220..0x223 and 0x228..0x229
#define BX_SB16_IOADLIBLEN 4        // number of addresses covered
#define BX_SB16_IRQ     theSB16Device->currentirq
#define BX_SB16_IRQMPU  BX_SB16_IRQ // IRQ for the MPU401 part - same value
#define BX_SB16_DMAL    theSB16Device->currentdma8
#define BX_SB16_DMAH    theSB16Device->currentdma16

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
   emulated at addresses 0x388..0x38b, or two separate OPL2's.
*/

#define BX_SB16_MIX_REG  0x100        // total number of mixer registers

// The array containing an instrument/bank remapping
struct bx_sb16_ins_map {
  Bit8u oldbankmsb, oldbanklsb, oldprogch;
  Bit8u newbankmsb, newbanklsb, newprogch;
};

// This is the class for the input and
// output FIFO buffers of the SB16

class bx_sb16_buffer {
public:

  BX_SB16_BUFINL  bx_sb16_buffer(void);
  BX_SB16_BUFINL ~bx_sb16_buffer();
  BX_SB16_BUFINL void init(int bufferlen);
  BX_SB16_BUFINL void reset();

      /* These functions return 1 on success and 0 on error */
  BX_SB16_BUFINL bx_bool put(Bit8u data);    // write one byte in the buffer
  BX_SB16_BUFINL bx_bool puts(const char *data, ...);  // write a formatted string to the buffer
  BX_SB16_BUFINL bx_bool get(Bit8u *data);   // read the next available byte
  BX_SB16_BUFINL bx_bool getw(Bit16u *data); // get word, in order lo/hi
  BX_SB16_BUFINL bx_bool getw1(Bit16u *data);// get word, in order hi/lo
  BX_SB16_BUFINL bx_bool full(void);         // is the buffer full?
  BX_SB16_BUFINL bx_bool empty(void);        // is it empty?

  BX_SB16_BUFINL void flush(void);           // empty the buffer
  BX_SB16_BUFINL int bytes(void);            // return number of bytes in the buffer
  BX_SB16_BUFINL Bit8u peek(int ahead);      // peek ahead number of bytes

      /* These are for caching the command number */
  BX_SB16_BUFINL void newcommand(Bit8u newcmd, int bytes);   // start a new command with length bytes
  BX_SB16_BUFINL Bit8u currentcommand(void); // return the current command
  BX_SB16_BUFINL void clearcommand(void);    // clear the command
  BX_SB16_BUFINL bx_bool commanddone(void);  // return if all bytes have arrived
  BX_SB16_BUFINL bx_bool hascommand(void);   // return if there is a pending command
  BX_SB16_BUFINL int commandbytes(void);     // return the length of the command


private:
  Bit8u *buffer;
  int head,tail,length;
  Bit8u command;
  bx_bool havecommand;
  int bytesneeded;
};


// forward definition
class bx_sound_lowlevel_c;

// The actual emulator class, emulating the sound blaster ports
class bx_sb16_c : public bx_devmodel_c {
public:
  bx_sb16_c();
  virtual ~bx_sb16_c();
  virtual void init(void);
  virtual void reset(unsigned type);
  virtual void register_state(void);
  virtual void after_restore_state(void);

  /* Make writelog available to output functions */
  BX_SB16_SMF void writelog(int loglev, const char *str, ...);
  // runtime options
  static Bit64s sb16_param_handler(bx_param_c *param, int set, Bit64s val);
  static const char* sb16_param_string_handler(bx_param_string_c *param, int set,
                                               const char *oldval, const char *val,
                                               int maxlen);
  static void runtime_config_handler(void *);
  void runtime_config(void);

  BX_SB16_SMF Bit32u fmopl_generator(Bit16u rate, Bit8u *buffer, Bit32u len);

private:

  int midimode, wavemode, loglevel;
  Bit8u midi_changed, wave_changed;
  Bit32u dmatimer;
  FILE *logfile;
  bx_soundlow_waveout_c *waveout[2]; // waveout support
  bx_soundlow_wavein_c  *wavein;  // wavein support
  bx_soundlow_midiout_c *midiout[2]; // midiout support
  int currentirq;
  int currentdma8;
  int currentdma16;
  int fmopl_callback_id;
  int rt_conf_id;
  Bit16u fm_volume;

  // the MPU 401 relevant variables
  struct bx_sb16_mpu_struct {
    struct {
      bx_sb16_buffer datain, dataout, cmd, midicmd;
    } b;
    struct {
      bx_bool uartmode, irqpending, forceuartmode, singlecommand;

      int banklsb[16];
      int bankmsb[16];   // current patch lists
      int program[16];

      int timer_handle, current_timer;           // no. of delta times passed
      Bit32u last_delta_time;                    // timer value at last command
      Bit8u outputinit;
    } d;
  } mpu401;

  // the DSP variables
  struct bx_sb16_dsp_struct {
    struct {
      bx_sb16_buffer datain, dataout;
    } b;
    struct {
      Bit8u resetport;                    // last value written to the reset port
      Bit8u speaker,prostereo;            // properties of the sound input/output
      bx_bool irqpending;                 // Is an IRQ pending (not ack'd)
      bx_bool midiuartmode;               // Is the DSP in MIDI UART mode
      bx_bool nondma_mode;                // Set if DSP command 0x10 active
      Bit32u nondma_count;                // Number of samples sent in non-DMA mode
      Bit8u samplebyte;                   // Current data byte in non-DMA mode
      Bit8u testreg;
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
        int mode, bps, timer;
        bx_bool fifo, output, highspeed;
        bx_pcm_param_t param;
        Bit16u count;     // bytes remaining in this transfer
        Bit8u *chunk;     // buffers up to BX_SOUNDLOW_WAVEPACKETSIZE bytes
        int chunkindex;   // index into the buffer
        int chunkcount;   // for input: size of the recorded input
        Bit16u timeconstant;
        Bit16u blocklength;
      } dma;
      int timer_handle;   // handle for the DMA timer
      Bit8u outputinit; // have the lowlevel output been initialized
      bx_bool inputinit;  // have the lowlevel input been initialized
    } d;
  } dsp;

  // the ASP/CSP registers
  Bit8u csp_reg[256];

  // the variables common to all FM emulations
  struct bx_sb16_opl_struct;
  friend struct bx_sb16_opl_struct;
  struct bx_sb16_opl_struct {
    int timer_handle;
    int timer_running;
    int index[2];                 // index register for the two chips
    Bit16u timer[4];              // two timers on each chip
    Bit16u timerinit[4];          // initial timer counts
    int tmask[2];                 // the timer masking byte for both chips
    int tflag[2];                 // shows if the timer overflow has occurred
  } opl;

  struct bx_sb16_mixer_struct {
    Bit8u regindex;
    Bit8u reg[BX_SB16_MIX_REG];
  } mixer;

  struct bx_sb16_emul_struct {
    bx_sb16_buffer datain, dataout;
    bx_sb16_ins_map remaplist[BX_SB16_MAX_REMAPS];
    Bit16u remaps;
  } emuldata;

      /* DMA input and output, 8 and 16 bit */
  BX_SB16_SMF Bit16u dma_write8(Bit8u *buffer, Bit16u maxlen);
  BX_SB16_SMF Bit16u dma_read8(Bit8u *buffer, Bit16u maxlen);
  BX_SB16_SMF Bit16u dma_write16(Bit16u *buffer, Bit16u maxlen);
  BX_SB16_SMF Bit16u dma_read16(Bit16u *buffer, Bit16u maxlen);

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
  BX_SB16_SMF Bit32u dsp_irq16ack();                 // ack 16 bit IRQ      2xf
  BX_SB16_SMF void   dsp_dma(Bit8u command, Bit8u mode, Bit16u length, Bit8u comp);
						     // initiate a DMA transfer
  BX_SB16_SMF void   dsp_dmadone();		     // stop a DMA transfer
  BX_SB16_SMF void   dsp_enabledma();		     // enable the transfer
  BX_SB16_SMF void   dsp_disabledma();		     // temporarily disable DMA
  BX_SB16_SMF void   dsp_disable_nondma();	     // disable DSP direct mode
  static void dsp_dmatimer_handler(void *);
  void dsp_dmatimer(void);
  static Bit32u sb16_adc_handler(void *, Bit32u len);
  Bit32u dsp_adc_handler(Bit32u len);

      /* The mixer part */
  BX_SB16_SMF Bit32u mixer_readdata(void);
  BX_SB16_SMF void   mixer_writedata(Bit32u value);
  BX_SB16_SMF void   mixer_writeregister(Bit32u value);
  BX_SB16_SMF Bit16u calc_output_volume(Bit8u reg1, Bit8u reg2, bx_bool shift);
  BX_SB16_SMF void   set_irq_dma();

      /* The emulator ports to change emulator properties */
  BX_SB16_SMF Bit32u emul_read (void);               // read emulator port
  BX_SB16_SMF void   emul_write(Bit32u value);       // write emulator port

      /* The FM emulation part */
  BX_SB16_SMF Bit32u opl_status(int chipid);
  BX_SB16_SMF void   opl_data(Bit32u value, int chipid);
  BX_SB16_SMF void   opl_settimermask(int value, int chipid);
  static void   opl_timer(void *);
  BX_SB16_SMF void   opl_timerevent(void);

      /* several high level sound handlers */
  BX_SB16_SMF int    currentdeltatime();
  BX_SB16_SMF void   processmidicommand(bx_bool force);
  BX_SB16_SMF void   midiremapprogram(int channel);  // remap program change
  BX_SB16_SMF void   writemidicommand(int command, int length, Bit8u data[]);

  BX_SB16_SMF void   closemidioutput();         // close midi file / device
  BX_SB16_SMF void   closewaveoutput();         // close wave file
  BX_SB16_SMF void   create_logfile();

      /* The port IO multiplexer functions */

  static Bit32u read_handler(void *this_ptr, Bit32u address, unsigned io_len);
  static void   write_handler(void *this_ptr, Bit32u address, Bit32u value, unsigned io_len);

#if !BX_USE_SB16_SMF
  Bit32u read(Bit32u address, unsigned io_len);
  void   write(Bit32u address, Bit32u value, unsigned io_len);
#endif
};

#define BOTHLOG(x)      (x)
#define WRITELOG        (BX_SB16_THIS writelog)
#define MIDILOG(x)      ((BX_SB16_THIS midimode>0?x:0x7f))
#define WAVELOG(x)      ((BX_SB16_THIS wavemode>0?x:0x7f))

#endif
