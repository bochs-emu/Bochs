Readme for wxWidgets Interface
Updated Tue Dec 24 09:54:00 CET 2013

Contributors to wxWidgets port:
  Don Becker (Psyon)
  Bryce Denney
  Dave Poirier
  Volker Ruppert

wxWidgets Configuration Interface

The wxWidgets (formerly called wxWindows) port began in June 2001 around the
time of Bochs 1.2.1.  Dave Poirier and Bryce Denney started adding a wxWidgets
configuration interface. We made some progress, but stopped after a while.
Then in March/April 2002 Bryce and Psyon revived the wxWidgets branch and turned
it into a usable interface.  Psyon did most of the work to get text and graphics
working, and Bryce worked on event passing between threads, and keyboard
mapping. Starting in August 2002, Bryce added lots of dialog boxes to allow you
to set all the bochsrc parameters.  At the time of release 2.0, there
are still some bugs but it is pretty stable and usable.

Bochs should be build with wxWidgets 2.3.3 or later. The release wxWidgets 2.3.3
includes a patch by Bryce Denney to allow us to get raw keycode data for several
OSes.

On any UNIX platform with wxWidgets installed, configure with
--with-wx to enable the wxWidgets display library.

To build in MS VC++:
- edit .conf.win32-vcpp and add "--with-wx" to the configure line.
  If you want different configure options from what you see, change them
  too.
- in cygwin, do "sh .conf.win32-vcpp" to run configure
- unzip build/win32/wxworkspace.zip into the main directory.
  For cygwin: unzip build/win32/wxworkspace.zip
  or use winzip or whatever else.
- open up bochs.dsw, the workspace file
- edit project settings so that VC++ can find the wxWidgets include
  files and libraries on your system.  Bryce installed them in
  d:/wx/wx233/include and d:/wx/wx233/lib.  Specifically, edit
  - Project>Settings>C/C++>Category=Preprocessor: include directories.
  - Project>Settings>Link>Category=Input: additional library path.
- build

Note that the project is set up for wxWidgets 2.3.3.  To use on other
wxWidgets versions, you will have to change some of the names of the libraries
to include.  Use the samples that came with that version of wxWidgets for
reference.

------------------------------------------------------

Random notes follow

Added some sketches.  I'm thinking that the control
panel will be able to basically show one of these screens at a time.  When
you first start you would see ChooseConfigScreen which chooses between the
configurations that you have loaded recently (which it would remember
by the pathname of their bochsrc).  Whether you choose an existing
configuration to be loaded or a new one, when you click Ok you go to
the first configuration screen, ConfigDiskScreen.

Each of the configuration screens takes up the whole control panel window.
We could use tabs on the top and/or "<-Prev" and "Next->" buttons to make
it quick to navigate the configuration screens.  Each screen should
probably have a Prev, Next, Revert to Saved, and Accept button.
The menu choices like Disk..., VGA..., etc. just switch directly to
that tab.


------------------------------------------------------
Notes:

events from gui to sim:
- [async] key pressed or released
- [async] mouse motion with button state
- [sync] query parameter
- [sync] change parameter
- [async] start, pause, stop, reset simulation.  Can be implemented
  as changing a parameter.
- [async] request notification when some param changes

events from sim to gui:
- [async] log message to be displayed (or not)
- [async] ask user how to proceed (like panic: action=ask)
- [async] param value changed
- make my thread sleep for X microseconds  (call wxThread::sleep then return)

In a synchronous event, the event object will contain space for the entire
response.  The sender allocates memory for the event and builds it.  The
receiver fills in blanks in the event structure (or could overwrite parts)
and returns the same event pointer as a response.  For async events, probably
the sender will allocate and the receiver will have to delete it.

implement the floppyA and floppyB change buttons using new event
structure.  How should it work?

vga gui detects a click on floppyA bitmap
construct a BxEvent type BX_EVT_ASK_PARAM
post the event to the wxWidgets gui thread (somehow) and block for response
when it arrives in the gui thread, show a modal dialog box
get the answer back to the simulator thread


right now, this is working ok within the simulator thread using
wxMutexGuiEnter/Leave.  Still I'm going to change it so that the
siminterface.cc code builds an event structure and the gui code
fills in the blank in the structure, instead of the stupid
notify_get_int_arg stuff.


Starting and Killing Threads

When a detachable (default) thread finishes (returns from its Entry()
function), wxWidgets frees the memory associated with that thread.
Unless the thread is never going to end, it is potentially dangerous to have a
pointer to it at all.  Even if you try to "check if it's alive" first, you may
be dereferencing the pointer after it has already been deleted, leading to it
claiming to be alive when it's not, or a segfault.  To solve this, the approach
used in the wxWidgets threads example is to have code in the thread's OnExit()
method remove the thread's pointer from the list of usable threads.  In
addition, any references or changes to the list of threads is controlled by a
critical section to ensure that it stays correct.  This post finally
explained what I was seeing.
