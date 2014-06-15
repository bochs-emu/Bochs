/* $Id$
 *
 * spoolpipe.c
 * by Carl Sopchak
 *
 * Read a pipe that stays open, send the data to a temp file.  Print
 * the temp file if no new input data is seen within a period of time. This
 * is useful, e.g., to create separate spool files without exiting a program
 * that doesn't close it's printer output file.
 *
 * ---------------------------------------------------------------------------
 *
 * Copyright (c) 2002 Cegis Enterprises, Inc.  Syracuse, NY 13215
 *
 * ---------------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA.
 *
 * --------------------------------------------------------------------------
 * Modification log:
 *
 * 2002/05/20 - Initial programming
 *
 * --------------------------------------------------------------------------
 */

#define BUF_SIZE 4*1024
#define DEBUG 0

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int infd, outfd;

unsigned int delay = 60;  // default delay, in seconds
unsigned int count_down = 0;

char buffer[BUF_SIZE];
ssize_t readcnt, writecnt;

int didwait = 0;   // have we already waited?
int havedata = 0;  // have we read anything in?

int q;

pid_t pid, wait_pid;
int wait_status;

int main (int argc, char *argv[]) {

#if DEBUG
       printf("Command line arguments (%d):\n", argc);
       for (q = 0; q < argc ; q++) {
	       printf("  %d = %s\n", q, argv[q]);
       }
#endif

       if ((argc < 3) || (argc > 4)) {
	       printf("usage: %s <inpipe> <tempfilename> [<maxdelay>]\n", argv[0]);
	       exit(1);
       }

       if (argc == 4) { // get delay
	       delay = strtol(argv[3], (char **) NULL, 10);
	       if (delay < 0) {
		       printf("Unable to convert maximum delay value: %s\n", argv[3]);
		       exit(2);
	       }
       } // get delay

#if DEBUG
  printf("Delay is set to %d seconds.\n", delay);
#endif

      infd = open(argv[1], O_RDONLY | O_NONBLOCK);
      if (infd == -1) {
	      printf("Error opening input pipe %s: %d - %s\n", argv[1], errno, strerror(errno));
	      exit(3);
      }

      outfd = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
      if (outfd == -1) {
	      printf("Error opening output file %s: %d - %s\n", argv[2], errno, strerror(errno));
	      exit(4);
      }

      count_down = delay;
      while (1) {  // must kill with a signal....
	      readcnt = read(infd, buffer, (size_t) BUF_SIZE);
#if DEBUG
  printf("read() returned with readcnt = %d, errno = %d\n", readcnt, errno);
#endif
	      if ((readcnt == -1) && (errno != EAGAIN)) {  // EAGAIN - no data waiting, can ignore
		      printf("Error reading input pipe: %d - %s\n", errno, strerror(errno));
		      exit(5);
	      } else {  // no errors reading input pipe
		      if (readcnt > 0) {
			      writecnt = write(outfd, buffer, readcnt);
			      if (writecnt == -1) {
				      printf("Error writing output file: %d - %s\n", errno, strerror(errno));
				      exit(6);
			      }
			      didwait = 0; // reset wait flag (wait again)
			      count_down = delay;
			      havedata = 1; // set flag that we got some data
		      } else { //readcnt must = 0
			      if (!didwait) { //have not waited yet...
				      if (count_down > 0) { // sleep a bit
					      sleep(1);
					      count_down -= 1;
				      } else {
					      didwait = 1;  // set wait flag (don't wait again)
				      }
			      } else { // already waited
				     if (havedata) { // have data to print, close & reopen output file
					      if (close(outfd) != 0) {
						      printf("Error closing output file: %d - %s\n", errno, strerror(errno));
						      exit(7);
					      }

#if DEBUG
  printf("Spooling temp file...\n");
#endif
	
					      pid = fork();
					      if (pid == -1) {
						      printf("Error forking new process: %d - %s\n", errno, strerror(errno));
						      exit(9);
					      }	
					      if (pid == 0) { // we're now running in the child process...
						      execlp("lpr", "lpr", argv[2], NULL);
						      exit(99); // should never get here...
					      } // we're now running in the child process...
					      if (pid > 0) { // we're running in the parent process...
						      wait_pid = waitpid(pid, (int *)&wait_status, 0);
						      if (wait_pid != pid) { // some sort of error
							      printf("Wait for 'lpr' command returned abnormally!\n");
							      if (WIFEXITED(wait_status)) {
								      printf("  'lpr' exited normally.\n");
							      } else {
								      printf("  'lpr' exited abnormally, return code = %d.\n", WEXITSTATUS(wait_status));
							      }
							      if (WIFSIGNALED(wait_status)) {
								      printf("   'lpr' received uncaught signal %d\n", WTERMSIG(wait_status));
							      }
						      } // some sort of error
					      } // we're running in the parent process...
					      outfd = open(argv[2], O_RDWR | O_TRUNC);
					      if (outfd == -1) {
						      printf("Error re-opening output file: %d - %s\n", errno, strerror(errno));
						      exit(8);
					      }
				     } // have data to print, close & repoen output file.
				     havedata = 0;  // no more data waiting
				     count_down = delay;
				     didwait = 0;  // reset wait flag (wait again)
			      }  // already waited
		      }  // readcnt must = 0
	      }  // no errors reading input pipe
      }  // must kill with a signal...

      exit(0);
}  // eof(spoolpipe.c)
