
#define BX_USE_VIRTUAL_TIMERS 0

#define BX_MAX_VIRTUAL_TIMERS (15+BX_SMP_PROCESSORS)
#define BX_NULL_VIRTUAL_TIMER_HANDLE 10000

#define BX_MAX_VIRTUAL_TIME (100000)

class bx_virt_timer_c {
 private:

  struct {
    bx_bool inUse;      // Timer slot is in-use (currently registered).
    Bit64u  period;     // Timer periodocity in virtual useconds.
    Bit64u  timeToFire; // Time to fire next (in virtual useconds).
    bx_bool active;     // 0=inactive, 1=active.
    bx_bool continuous; // 0=one-shot timer, 1=continuous periodicity.
    bx_timer_handler_t funct;  // A callback function for when the
                               //   timer fires.
                               //   This function MUST return.
    void *this_ptr;            // The this-> pointer for C++ callbacks
                               //   has to be stored as well.
    char id[BxMaxTimerIDLen]; // String ID of timer.
  } timer[BX_MAX_VIRTUAL_TIMERS];

  unsigned   numTimers;  // Number of currently allocated timers.

  //Variables for the timer subsystem:
  Bit64u current_virtual_time;
  Bit64u next_event_time;

  //Variables for the time sync subsystem:
  Bit64u last_ips_time;
  Bit64u last_virtual_time;
  Bit64u last_host_time;

  Bit64u initial_ips_time;
  Bit64u initial_virtual_time;
  Bit64u initial_host_time;

  Bit64u next_virtual_time;
  Bit64u next_ips_time;

  Bit64u virtual_time_calls;

  bx_bool use_virtual_timers;

  // A special null timer is always inserted in the timer[0] slot.  This
  // make sure that at least one timer is always active, and that the
  // duration is always less than a maximum 32-bit integer, so a 32-bit
  // counter can be used for the current countdown.
  static const Bit64u NullTimerInterval;
  static void nullTimer(void* this_ptr);


  Bit64u get_next_event_time(void);

  void periodic(Bit64u time_passed);

 public:

  //Get the current virtual time.
  //  This may return the same value on subsequent calls.
  Bit64u time_usec(void);

  //Get the current virtual time.
  //  This will return a monotonically increasing value.
  // MUST NOT be called from within a timer interrupt.
  Bit64u time_usec_sequential(void);

  //Register a timer handler to go off after a given interval.
  //Register a timer handler to go off with a periodic interval.
  int    register_timer( void *this_ptr, bx_timer_handler_t handler,
                         Bit32u useconds,
                         bx_bool continuous, bx_bool active, const char *id);

  //unregister a previously registered timer.
  unsigned unregisterTimer(int timerID);

  void   start_timers(void);

  //activate a deactivated but registered timer.
  void   activate_timer( unsigned timer_index, Bit32u useconds,
                         bx_bool continuous );

  //deactivate (but don't unregister) a currently registered timer.
  void   deactivate_timer( unsigned timer_index );

}


extern bx_virt_timer_c bx_virt_timer;
