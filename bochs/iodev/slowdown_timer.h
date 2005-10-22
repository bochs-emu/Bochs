/////////////////////////////////////////////////////////////////////////
// $Id: slowdown_timer.h,v 1.9 2005-10-22 17:31:02 vruppert Exp $
/////////////////////////////////////////////////////////////////////////
//

class bx_slowdown_timer_c : public logfunctions {

private:
  struct {
    Bit64u start_time;
    Bit64u start_emulated_time;
    Bit64u lasttime;

    int timer_handle;

    float MAXmultiplier;
    Bit64u Q; // sleep rate in usec
  } s;

public:
  bx_slowdown_timer_c();

  void init(void);

  static void timer_handler(void * this_ptr);

  void handle_timer();

};

extern bx_slowdown_timer_c bx_slowdown_timer;

