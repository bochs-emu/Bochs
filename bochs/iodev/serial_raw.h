/////////////////////////////////////////////////////////////////////////
// $Id: serial_raw.h,v 1.8 2004-03-17 17:08:57 vruppert Exp $
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
#define C_BREAK 201

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
    void transmit (int byte);
    int ready_transmit ();
    int ready_receive ();
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
