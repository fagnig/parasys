/* Exercise: Pi                                                                 
 *                                                                              
 * In this exercise you will determine the value                                
 * of PI using the integral  of                                                 
 *    4/(1+x*x) between 0 and 1.                                                
 *                                                                              
 * The integral is approximated by a sum of n intervals.                        
 *                                                                              
 * The approximation to the integral in each interval is:                       
 *    (1/n)*4/(1+x*x).                                                          
 */

#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#ifdef _OPENMP
#include <omp.h>
#endif

#define PI25DT 3.141592653589793238462643

#define INTERVALS 10000

double gettime() {
  /* omp_get_wtime() might also be used */
  struct timespec timeSpec;
  clock_gettime(CLOCK_MONOTONIC_RAW,&timeSpec);
  return timeSpec.tv_sec + timeSpec.tv_nsec/1.0e9;
}

int main(int argc, char **argv)
{
  long int i, intervals = INTERVALS;
  double x, dx, f, sum, pi;
  double true_pi = PI25DT;
  double time2;
  double time1 = gettime();
  long int start, end;

  printf("Number of intervals: %ld\n", intervals);


  int nthreads, tid;

  /* Fork a team of threads giving them their own copies of variables */
  #pragma omp parallel private(nthreads, tid, sum, start, end, dx, x, f) reduction(+: pi)
  { 
    tid = omp_get_thread_num();
    nthreads = omp_get_num_threads();

    sum = 0.0;
    start = (intervals/nthreads) * tid;
    end = (intervals/nthreads) *tid+1;
    dx = 1.0 / (double) intervals;

    for (i = start; i < end; i++) {
      x = dx * ((double) (i + 0.5));
      f = 4.0 / (1.0 + x*x);
      sum = sum + f;
    }

    pi += dx*sum;

  }


  time2 = gettime();

  printf("Computed Pi %27.24f\n", pi);
  printf("The true Pi %27.24f\n", true_pi);
  printf("Error       %27.24f\n\n", true_pi-pi);
  printf("Elapsed time (s) = %f\n", time2-time1);

  return 0;
}                
