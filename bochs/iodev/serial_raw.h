/////////////////////////////////////////////////////////////////////////
// $Id: serial_raw.h,v 1.10 2004-03-28 12:41:12 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//

#if USE_RAW_SERIAL

#ifdef __linux__
#include <linux/serial.h>
#endif

#define P_NONE  0
#define P_ODD   1
#define P_EVEN  2
#define P_HIGH  3
#define P_LOW   4

#define RAW_EVENT_BREAK    -1
#define RAW_EVENT_CTS_ON   -2
#define RAW_EVENT_CTS_OFF  -3
#define RAW_EVENT_DSR_ON   -4
#define RAW_EVENT_DSR_OFF  -5
#define RAW_EVENT_RING_ON  -6
#define RAW_EVENT_RING_OFF -7
#define RAW_EVENT_RLSD_ON  -8
#define RAW_EVENT_RLSD_OFF -9
#define RAW_EVENT_FRAME   -10
#define RAW_EVENT_OVERRUN -11
#define RAW_EVENT_PARITY  -12

class serial_raw : public logfunctions {
  public:
    serial_raw (char *devname);
    ~serial_raw (void);
    void set_baudrate (int rate);
    void set_data_bits (int );
    void set_stop_bits (int);
    void set_parity_mode (int mode);
    void set_break (int mode);
    void set_modem_control (int ctrl);
    int get_modem_status ();
    void transmit (Bit8u byte);
    bx_bool ready_transmit ();
    bx_bool ready_receive ();
    int receive ();

  private:
    void setup_port ();
    bx_bool present;
    Bit8u rxdata_count;
#ifdef WIN32
    HANDLE hCOM;
    DCB dcb;
    BOOL DCBchanged;
#endif
};
#endif
