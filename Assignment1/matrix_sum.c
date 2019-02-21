/* matrix_sum.c 

   02346, F19, DTU

   For instructions, see Databar Exercise 2.
*/

#include <stdio.h>
#include <time.h>

#define ROWS  (8*1024)
#define COLS  (8*1024)

long matrix[ROWS][COLS];

double get_seconds() { 
  struct timespec time;
  clock_gettime(CLOCK_MONOTONIC_RAW, &time);
  return (double) ( (double) time.tv_sec + (double) time.tv_nsec/1000000000);
}


void fill(long m[ROWS][COLS]) {
  int i,j;

  for (i=0; i < ROWS; i++) {
      for (j=0; j < COLS; j++) {
        m[i][j] = i+j; 
      }
  }
}

long sum1(long m[ROWS][COLS]) {
  long register sum = 0;
  int i,j;

  for (i=0; i < ROWS; i++) {
    for (j=0; j < COLS; j++) {
      sum += m[i][j];
    }
  }
  return sum;
}

long sum2(long m[ROWS][COLS]) {
  long register sum = 0;
  int i,j;

  for (j=0; j < COLS; j++) {
    for (i=0; i < ROWS; i++) {
      sum += m[i][j];
    }
  }
  
  return sum;
}


int main() {

  int sum;
  double start, end;

  fill(matrix);

  printf("Matrix size: %dk x %dk = %dM\n", 
	 ROWS/1024, COLS/1024, ROWS*COLS/(1024*1024));

  start = get_seconds();
  sum = sum1(matrix);
  end = get_seconds();

  printf("sum1(matrix) = %ld using %1.6fs\n", sum, end - start);

  start = get_seconds();
  sum = sum2(matrix);
  end = get_seconds();

  printf("sum2(matrix) = %ld using %1.6fs\n", sum, end - start);

}
