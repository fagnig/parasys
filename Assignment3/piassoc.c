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
  clock_gettime(CLOCK_MONOTONIC,&timeSpec);
  return timeSpec.tv_sec + timeSpec.tv_nsec/1.0e9;
}

int main(int argc, char **argv)
{
  long int i, intervals = INTERVALS;
  double x, dx, f, sum, pi, revpi;
  double true_pi = PI25DT;
  double time2;
  double time1 = gettime();
  long int start, end;

  printf("Number of intervals: %ld\n", intervals);

  sum = 0.0;
  start = 0;
  end = intervals;
  dx = 1.0 / (double) intervals;
  for (i = start; i < end; i++) {
    x = dx * ((double) (i + 0.5));
    f = 4.0 / (1.0 + x*x);
    sum = sum + f;
  }

  pi = dx*sum;

  sum = 0.0;
  start = intervals;
  end = 0;
  dx = 1.0 / (double) intervals;
  for (i = start; i > end; i--) {
    x = dx * ((double) (i + 0.5));
    f = 4.0 / (1.0 + x*x);
    sum = sum + f;
  }

  revpi = dx*sum;

  time2 = gettime();

  printf("Computed Pi %27.24f\n", pi);
  printf("Reversed Pi %27.24f\n", revpi);
  printf("The true Pi %27.24f\n", true_pi);
  printf("Pi vs Reverse Pi %27.24f\n\n", revpi-pi);

  return 0;
}                
