// $Id: ctrlc.c,v 1.1 2002-11-19 15:38:13 bdenney Exp $
// 
// ctrlc.c
//
// This tests whether we can trap control-C or not.
//
//////////
// Results with redhat 6.2 box:
//   compiler: gcc version egcs-2.91.66 19990314/Linux (egcs-1.1.2 release)
//   glibc version 2.1.3
//   libpthread version 0.8:
// Compile with: gcc -Wall -g ctrlc.c -o ctrlc -lreadline
// and it works fine
// Compile with: gcc -Wall -g ctrlc.c -o ctrlc -lreadline -pthread
// and control-C's are not handled during readline().
//////////
//

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
  printf ("* $Id: ctrlc.c,v 1.1 2002-11-19 15:38:13 bdenney Exp $\n");
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
