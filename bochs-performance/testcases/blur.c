/*
 *
 * $Id: blur.c,v 1.2 2002-04-16 14:30:47 bdenney Exp $
 *
 */

#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>

#define MAX 128
int array[MAX][MAX];
int array2[MAX][MAX];
#define BLUR_SIZE 3
#define BLUR_WINDOW_HALF 1

#define DEFAULT_TIMES 1000

void blur()
{
  int sum;
  int x,y,x2,y2;
  for (x=1; x<MAX-1; x++)
    for (y=1; y<MAX-1; y++)
    {
      sum = 0;
      for (x2=x-BLUR_WINDOW_HALF; x2<=x+BLUR_WINDOW_HALF; x2++)
	for (y2=y-BLUR_WINDOW_HALF; y2<=y+BLUR_WINDOW_HALF; y2++)
	  sum += array[x2][y2];
      array2[x][y] = sum;
    }
}

void fill_array()
{
  int x,y;
  for (x=0; x<MAX; x++)
    for (y=0; y<MAX; y++)
      array[x][y] = (x*17+y*31)%29;
}

void dump_array (FILE *fp)
{
  int x,y;
  for (x=0; x<MAX; x++) {
    for (y=0; y<MAX; y++) {
      fprintf (fp, "%3d ", array[x][y]);
    }
    fprintf (fp, "\n");
  }
}

struct timeval start, stop;
#define start_timer() gettimeofday (&start, NULL);
#define stop_timer() gettimeofday (&stop, NULL);

void report_time (FILE *fp, int iters)
{
  int usec_duration = 
    (stop.tv_sec*1000000 + stop.tv_usec)
    - (start.tv_sec*1000000 + start.tv_usec);
  double sec = (double)usec_duration / 1.0e3;
  double sec_per_iter = sec / (double)iters;
  fprintf (fp, "Total time elapsed = %f msec\n", sec);
  fprintf (fp, "Iterations = %d\n", iters);
  fprintf (fp, "Time per iteration = %f msec\n", sec_per_iter);
}

int main (int argc, char *argv[])
{
  int i;
  int times = 0;
  if (argc>1) {
    assert (sscanf (argv[1], "%d", &times) == 1);
  } else {
    times = DEFAULT_TIMES;
  }
  fill_array();
  //dump_array (stderr);
  start_timer();
  for (i=0; i<times; i++) 
    blur();
  stop_timer();
  report_time (stdout, times);
  //fprintf (stderr, "-----------------------------------\n");
  //dump_array (stderr);
  return 0;
}
