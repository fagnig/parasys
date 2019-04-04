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

#define GENERIC_MAX(x, y) ((x) > (y) ? (x) : (y))

#define PI25DT 3.141592653589793238462643

#define INTERVALS 1000000

double gettime() {
  /* omp_get_wtime() might also be used */
  struct timespec timeSpec;
  clock_gettime(CLOCK_MONOTONIC_RAW,&timeSpec);
  return timeSpec.tv_sec + timeSpec.tv_nsec/1.0e9;
}

int main(int argc, char **argv)
{
  long int intervals = INTERVALS;
  double true_pi = PI25DT;
  double time2;
  double pi, dx;
  double time1 = gettime();

  printf("Number of intervals: %ld\n", intervals);

  int nthreads, tid;
  dx = 1.0 / (double) intervals;

  /* Fork a team of threads giving them their own copies of variables */
  #pragma omp parallel private(nthreads, tid) \
  reduction(+: pi)
  { 
    tid = omp_get_thread_num();
    nthreads = omp_get_num_threads();
    long int start, end;
    long int i;
    double x, f, sum;

    sum = 0.0;
    start = (double) (intervals/nthreads) * tid;
    end = (double) (intervals/nthreads) * (tid+1);

    for (i = start; i < end; i++) {
      x = dx * ((double) (i + 0.5));
      f = 4.0 / (1.0 + x*x);
      sum = sum + f;
    }

    pi += dx*sum;
  }

  time2 = gettime();

  //Double is precise until 15th decimal
  printf("Computed Pi %27.14f\n", pi);
  printf("The true Pi %27.14f\n", true_pi);
  printf("Error       %27.14f\n\n", true_pi-pi);
  printf("Elapsed time (s) = %f\n", time2-time1);

  return 0;
}                
