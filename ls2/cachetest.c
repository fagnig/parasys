/*  COPYING:
 *  Dave Patterson at UCB modified this code from a program by Andrea 
 *  Dusseau of U.C. Berkeley,which was based on a description in 
 *  Saavedra-Barrera [1992]:
 *    Saavedra-Barrera, R. H. [1992]. CPU Performance Evaluation and 
 *    Execution Time Prediction Using Narrow Spectrum Benchmarking, 
 *    Ph.D. Dissertation, University of Calif., Berkeley (May).
 *  Patterson has given permission to use and modify this code as long
 *  as these people and the U. of Cal., Berkeley get proper credit.
 */

#include <stdio.h>
#include <sys/times.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#define CACHE_MIN  (1024)         /* smallest cache */
#define CACHE_MAX  (32*1024*1024) /* largest cache */
#define STRIDE_MAX (256)          /* largest stride (array indices) */

#define SAMPLE     (1000000)      /* to get reasonably large time sample */
#define TIME_MAX   (0.5)          /* measurement time (seconds) */

#define CLK_TCK    (100)          /* number of system clock ticks per second */


int x[CACHE_MAX]; /* array going to stride through */
 
double get_seconds() { /* routine to read time */
  struct tms rusage;
  times(&rusage);    /* UNIX utility: time in clock ticks */
  return (double) (rusage.tms_utime)/CLK_TCK;
}

double get_secondsXX() { 
  /* Get current time using clock_gettime using CLOCK_MONOTONIC */

  struct timespec time;
  clock_gettime(CLOCK_MONOTONIC_RAW, &time);

  return (double) ( (double) time.tv_sec + (double) time.tv_nsec/1000000000);
}

int main() {
  int register i, stride;
  int register * ptr, * limit;
  int register dummy;
  int register steps, tsteps;
  unsigned char register b1, b2;     /* comparison flags */
  int csize;
  double register sec0, sec1, sec2;  /* timing variables */

  /* Check for proper clock tick constant definition */
  int sys_clock_tick = sysconf(_SC_CLK_TCK);
  if (CLK_TCK != sys_clock_tick) {
    printf("CLK_TCK = %d differs from system clock tick = %d\n", 
           CLK_TCK, sys_clock_tick);
    printf("Define CLK_TCK to (%d) and run again\n", sys_clock_tick);  
    exit(1);
  }

  for (csize=CACHE_MIN; csize <= CACHE_MAX; csize=csize*2) 
    for (stride=1; stride <= STRIDE_MAX; stride=stride*2) {

      limit = &x[csize-1]; /* last element of current cache size */
     
      steps = tsteps = 0;
      ptr = x;
      sec0 = get_seconds();            // get start time 
      do {
        for (i=0; i < SAMPLE; i++) {
          if (ptr > limit) ptr = x;    // reset pointer if end reached 
          *ptr = *ptr + 1;             // Access to memory/cache 
          ptr = ptr + stride;
        }
        steps = steps + 1;             // count while loop iterations
        sec1 = get_seconds() - sec0;   // measure time
        b1 = sec1 < TIME_MAX;
        b2 = tsteps < steps;
      } while (b1);
     
      /* Repeat empty loop to measure overhead */
      ptr = x;
      sec0 = get_seconds();            // get start time 
      do {
          for (i=0; i < SAMPLE; i++) {
          if (ptr > limit) ptr = x;    // reset pointer if end reached
          dummy = dummy + 1;           // Dummy access to register
          ptr = ptr + stride;
        }
        tsteps = tsteps + 1;           // count while loop iterations
        sec2 = get_seconds() - sec0;   // measure time
        b1 = sec2 < TIME_MAX;
        b2 = tsteps < steps;
      } while (b2);
     
      printf("Size (bytes):%9d, Stride (bytes):%4d, "
             "read+write:%6.2f ns  (steps: %3d)\n",
             csize*sizeof(int), stride*sizeof(int),
             (double) (sec1-sec2)*1e9/(steps*SAMPLE), steps);

    }; /* end of both outer for loops */
}
