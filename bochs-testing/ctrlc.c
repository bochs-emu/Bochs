// $Id: ctrlc.c,v 1.2 2002-11-19 15:56:26 bdenney Exp $
// 
// ctrlc.c
//
// This tests whether we can trap control-C or not.
//
//////////
// Results with redhat 6.2 box:
//   compiler: gcc version egcs-2.91.66 19990314/Linux (egcs-1.1.2 release)
//   glibc version 2.1.3
//   libpthread version 0.8
//   readline 4.3
// Compile with: gcc -Wall -g ctrlc.c -o ctrlc -lreadline
// and it works fine
// Compile with: gcc -Wall -g ctrlc.c -o ctrlc -lreadline -pthread
// FAILS: control-C's are not handled during readline().
//////////
// Results on Debian 3.0 box:
//   compiler: gcc version 2.95.4 20011002 (Debian prerelease)
//   glibc version 2.2.5
//   libpthread version 0.9
// Compile with: gcc -Wall -g ctrlc.c -o ctrlc -lreadline
// and it works fine.
// Compile with: gcc -Wall -g ctrlc.c -o ctrlc -lreadline -pthread
// and it works fine.
//////////
// Results on Redhat 7.3 box:
//   compiler: gcc version 2.96 20000731 (Red Hat Linux 7.3 2.96-113)
//   glibc version 2.2.5
//   libpthread version 0.9
//   readline version 4.2
// Compile with: gcc -Wall -g ctrlc.c -o ctrlc -lreadline -ltermcap
// and it works fine.
// Compile with: gcc -Wall -g ctrlc.c -o ctrlc -lreadline -ltermcap -pthread
// and it works fine.
//////////
//
// Conclusion:
// My libc/libpthread is just too old.  Even with the newest version of
// readline 4.3, control-C handling does not work right.  Every newer system
// I've tried does not have this problem.

#include <stdio.h>
#include <signal.h>
#include <readline/readline.h>

void handler (int signal)
{
  printf ("Signal handler was called.  You must have pressed control-C.\n");
}

int main()
{
  char buf[1024];
  printf ("**************\n");
  printf ("* $Id: ctrlc.c,v 1.2 2002-11-19 15:56:26 bdenney Exp $\n");
  printf ("**************\n");
  printf ("\n");
  printf ("If you press control-c now, it will terminate the process.\n");
  printf ("Press ENTER, and I will install the signal handler.\n");
  fgets (buf, sizeof(buf), stdin);
  printf ("Installing the signal handler.\n");
  signal (SIGINT, handler);
  printf ("If you press control-C now, it should call the signal handler.\n");
  printf ("Using fgets to read one line of input...\n");
  fgets (buf, sizeof(buf), stdin);
  printf ("If you press control-C now, it should call the signal handler.\n");
  readline ("Using readline to read one line of input...");
  printf ("\nDone\n");
  return 0;
}
