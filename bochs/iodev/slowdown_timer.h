/////////////////////////////////////////////////////////////////////////
// $Id: slowdown_timer.h,v 1.6 2002-08-27 19:54:46 bdenney Exp $
/////////////////////////////////////////////////////////////////////////
//

#if BX_USE_SLOWDOWN_TIMER

class bx_slowdown_timer_c {

private:
  struct {
    Bit64u start_time;
    Bit64u start_emulated_time;
    Bit64u lasttime;

    int timer_handle;

    float MAXmultiplier;
    Bit64u Q; // (Q (in seconds))
  } s;

public:
  bx_slowdown_timer_c();

  void init(bx_devices_c *d);
  void reset(unsigned type);

  static void timer_handler(void * this_ptr);

  void handle_timer();

};

extern bx_slowdown_timer_c bx_slowdown_timer;

#endif

