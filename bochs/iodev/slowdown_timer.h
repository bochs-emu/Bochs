/////////////////////////////////////////////////////////////////////////
// $Id: slowdown_timer.h,v 1.7 2002-10-24 21:07:52 bdenney Exp $
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

  void init(void);
  void reset(unsigned type);

  static void timer_handler(void * this_ptr);

  void handle_timer();

};

extern bx_slowdown_timer_c bx_slowdown_timer;

#endif

