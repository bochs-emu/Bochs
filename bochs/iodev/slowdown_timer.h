/////////////////////////////////////////////////////////////////////////
// $Id: slowdown_timer.h,v 1.8 2003-08-19 00:10:38 cbothamy Exp $
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

