/*
Copyright David N. Welton <davidw@dedasys.com> 2003

This code may be distributed under the same terms as bochs.
*/

/* This file defines a class to deal with the speaker.
 * This class also tries to play beeps on the console
 * (linux only) and also forwards beeps to the gui
 * so we have the possiblity to signal the beep
 */


class bx_speaker_c : public bx_speaker_stub_c {
public:
    bx_speaker_c();
    ~bx_speaker_c();

    virtual void init(void);
    virtual void reset(unsigned int);

    void beep_on(float frequency);
    void beep_off();
private:
    float beep_frequency;  // 0 : beep is off
#ifdef __linux__
    /* Do we have access?  If not, just skip everything else. */
    signed int consolefd;
    const static unsigned int clock_tick_rate = 1193180;
#elif defined(WIN32)
    Bit64u usec_start;
#endif
};
