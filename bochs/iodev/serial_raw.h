/////////////////////////////////////////////////////////////////////////
// $Id: serial_raw.h,v 1.12 2006-03-07 21:11:19 sshwarts Exp $
/////////////////////////////////////////////////////////////////////////
//

#if USE_RAW_SERIAL

#ifdef __linux__
#include <linux/serial.h>
#endif

#ifdef WIN32
// experimental raw serial receive support on win32
//#define WIN32_RECEIVE_RAW
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

#define THREAD_RX_BUFSIZE 8192
#define RX_BUFSIZE 256

class serial_raw : public logfunctions {
public:
    serial_raw (char *devname);
    virtual ~serial_raw ();
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
#ifdef WIN32_RECEIVE_RAW
    void serial_thread ();
#endif

  private:
    void setup_port ();
#ifdef WIN32_RECEIVE_RAW
    void enq_event (Bit16s event);
#endif
    bx_bool present;
    unsigned rxdata_count;
#ifdef WIN32
    HANDLE hCOM;
    DCB dcb;
    BOOL DCBchanged;
    DWORD MSR_value;
    Bit16s rxdata_buffer[RX_BUFSIZE];
#ifdef WIN32_RECEIVE_RAW
    HANDLE hRawSerialThread;
    BOOL thread_active;
    BOOL thread_quit;
    OVERLAPPED rx_ovl;
    OVERLAPPED thread_ovl;
    unsigned thread_rxdata_count;
    Bit16s thread_rxdata_buffer[THREAD_RX_BUFSIZE];
#endif
#endif
};
#endif
