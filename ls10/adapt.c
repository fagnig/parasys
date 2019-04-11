/*
   adapt.c  Adaptive Integration

   For instructions see Databar Exercise 7

   Compilation: gcc -fopenmp -std=c99 -o adapt adapt.c -lm
   
   Usage: ./adapt <decimals> [<threads>]

   Function and interval are set in main.
 */

#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

const double pi = 3.141592653589793238462643;

long int calls = 0;

void iterateCalls(){
  #pragma omp atomic
  calls++;
}

/* Some functions to integrate */

double f1(double x) {
  return x;   // Integral over [0,2] = 1
}

double f2(double x) {
  return x * x / 4.0;  // Integral over [0,2] = 5.333...
}

double f3(double x) {
  return 4.0 / (1 + x * x);   // Integral over [0,1] = pi
}

double sin2(double x) { /* More costly way of computing sin(x) */
  return sqrt(1 - cos(x) * cos(x));  // Integral over [0,pi] = 2 
}

double sin3(double x) { /* Even more costly way of computing sin(x) */
  double cos =  sqrt(1 - sin2(x) * sin2(x));
  return sqrt(1 - cos * cos);  // Integral over [0,pi] = 2 
}

/*
  Integrate the function f(x) from a to b (a <= b) with tolerance e.
  The tolerance e is relative, e.g. 1e-5.
*/ 
double integrate_seq(double a, double b, double (*f) (double), double e) {
  calls ++;
  double m  = (a + b) / 2;
  double one_trap_area = (b - a) * (f(a) + f(b)) / 2;
  double two_trap_area = (b - a) * (f(a) + f(b) + 2 * f(m))/ 4;

  if (fabs(one_trap_area - two_trap_area) <= e) {
    return two_trap_area;
  } else {
    double left_area, right_area;
    left_area  = integrate_seq(a, m, f, e/2);
    right_area = integrate_seq(m, b, f, e/2);
    return left_area + right_area;
  }
}
    
/*
  Parallelize using OpenMP tasks.
*/
double integrate_par(double a, double b, double (*f) (double), double e) {
  iterateCalls();
  double m  = (a + b) / 2;
  double one_trap_area = (b - a) * (f(a) + f(b)) / 2;
  double two_trap_area = (b - a) * (f(a) + f(b) + 2 * f(m))/ 4;

  if (fabs(one_trap_area - two_trap_area) <= e) {
    return two_trap_area;
  } else {
    double left_area, right_area;

    #pragma omp task default(shared) final(b - a < 0.1)
    left_area  = integrate_par(a, m, f, e/2);

    #pragma omp task default(shared) final(b - a < 0.1)
    right_area = integrate_par(m, b, f, e/2);

    #pragma omp taskwait
    return left_area + right_area;
  }
}

int main(int argc, char*argv[]) {

  if(argc != 2 && argc != 3) {
    printf("Usage: adapt <decimals> [<threads>]\n");
    return 1;
  }
  
  int exp = atoi(argv[1]);
  if (exp < 1 ) {
    printf("Exponent must be a positive number\n");
    return 1;
  }
    
  if (argc == 3) {
    int threads = atoi(argv[2]);
    if (threads < 1 ) {
      printf("Number of threads must be positive\n");
      return 1;
    } else {
      // Set default no. of threads
      omp_set_num_threads(threads);
    }
  }
  
  double start_time;
  double time_seq, time_par;

  double area_seq; 
  double area_par;

  long int calls_seq = 0;
  long int calls_par = 0;
  
  double a, b;
  double e = pow(10,-exp);
  double (*fun) (double);

  /* Formulate integration problem */
  a = 0.0;
  b = pi;
  fun = sin3;
  
  printf("\nPrecision: %d, threads: %d\n\n", exp, omp_get_max_threads());
  
  /* Warm-up */
  area_seq = integrate_seq(a, b, fun, e);

  calls = 0;
  start_time = omp_get_wtime();
  area_seq = integrate_seq(a, b, fun, e);
  time_seq = omp_get_wtime() - start_time;
  calls_seq = calls;

  calls = 0;
  start_time = omp_get_wtime();
  #pragma omp parallel
    #pragma omp single
      area_par = integrate_par(a, b, fun, e);
  time_par = omp_get_wtime() - start_time;
  calls_par = calls;

  printf("Seq: Area %16.14f using %6ld calls in %2.3f s\n", area_seq, calls_seq, time_seq);
  printf("Par: Area %16.14f using %6ld calls in %2.3f s\n", area_par, calls_par, time_par);

  if (fabs(area_seq - area_par) > 2*e  ||  calls_seq != calls_par)
    printf("\n*** ERROR ***\n");
  printf("\nSpeedup: %2.2f\n\n", time_seq/time_par);

  return 0;
}
