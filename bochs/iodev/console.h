/*
Copyright David N. Welton <davidw@dedasys.com> 2003

This code may be distributed under the same terms as bochs.
*/

/* This file defines a class to deal with the console that started
 * bochs as an output device.  Currently used only to beep the system
 * speaker. */


class bx_console_c : public logfunctions {
public:
    bx_console_c();
    ~bx_console_c();
    void beep_on(float frequency);
    void beep_off();
private:
    /* Do we have access?  If not, just skip everything else. */
    int access;
    unsigned int consolefd;
    const static unsigned int clock_tick_rate = 1193180;
};
