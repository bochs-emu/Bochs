/////////////////////////////////////////////////////////////////////////
// $Id: slowdown_timer.h,v 1.5.4.1 2002-03-17 08:50:39 bdenney Exp $
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

  int init();

  static void timer_handler(void * this_ptr);

  void handle_timer();

};

extern bx_slowdown_timer_c bx_slowdown_timer;

#endif

