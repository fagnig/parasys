/*
   nqueen.c  N-queen solution count

   For instructions see Databar Exercise 7

   Compilation: gcc -fopenmp -std=c99 -o nqueen nquuen.c 
   
   Usage: ./nqueen <board size> [<threads>]
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

int size = 0;
int sols = 0;

int *privSol;

/* Not used -- may be used for debuggin */
void print(int queens[]) {
  printf("Solution for size: %d\n\n", size);
  for(int row = 0; row < size; row++) {
    for(int col = 0; col < size; col++) {
      if (queens[col] == row) {
        printf("Q");
      } else {
        printf("-");
      }
    }
    printf("\n");
  }
  printf("\n");
}

/* Is it legal to place a queen at (row, col) given placements for prior cols*/
int legal(int sol[], int row, int col) {
  for (int c = 0; c < col; c++) {
    if (sol[c] == row) return 0;
    if ( abs(sol[c] - row) == col - c) return 0;
  }
  return 1;
}

/*
  Given a legal placement of j queens in columns 0..j-1
  add number of possible legal full exentions of this to
  global count,
 */ 
void count_sols_seq(int sol[], int j) {
  for (int i = 0; i < size; i++) {
    if ( legal(sol, i, j) ) {
      if (j == size-1) { 
        // last column, we have a full solution
        sols++;
      } else {
        // place queen in column j and keep counting
        sol[j] = i;
        count_sols_seq(sol, j + 1);
      }
    }
  }
}

/*
  Parallelize using OpenMP tasks.
 */
void count_sols_par(int sol[], int j) {

  for (int i = 0; i < size; i++) {
    if ( legal(sol, i, j) ) {
      if (j == size-1) { 
        // last column, we have a full solution
        #pragma omp critical
        {sols++;}
      } else {
        int * privSol = (int *) malloc(size*sizeof(int));
        memcpy(privSol, sol, size*sizeof(int));
        // place queen in column j and keep counting
        privSol[j] = i;

        #pragma omp task default(none) firstprivate(privSol,j)
        count_sols_par(privSol, j + 1);
      }
    }
  }

  free(sol);
}


int main(int argc, char*argv[]) {

  if(argc != 2 && argc != 3) {
    printf("Usage: nqueen <board_size> [<threads>]\n");
    return 1;
  }
  
  size = atoi(argv[1]);
  if(size < 1 ) {
    printf("Board size must be positive\n");
    return 1;
  }
  
  if (argc == 3) {
    int threads = atoi(argv[2]);
    if(threads < 1 ) {
      printf("Number of threads must be positive\n");
      return 1;
    } else {
      // Set default no. of threads
      omp_set_num_threads(threads);
    }
  }
  
  printf("\nBoard size: %d x %d, threads: %d\n\n", 
         size, size, omp_get_max_threads()); 
  
  int queens[size];

  int * sol = (int *) malloc(size*sizeof(int));
  memcpy(sol, queens, size*sizeof(int));

  double start_time;
  double time_seq, time_par;

  int sols_seq; 
  int sols_par;


  /* Warm-up */
  sols = 0;
  count_sols_seq(queens, 0);

  sols = 0;
  start_time = omp_get_wtime();
  count_sols_seq(queens, 0);
  time_seq = omp_get_wtime() - start_time;
  sols_seq = sols;

  sols = 0;
  start_time = omp_get_wtime();
  #pragma omp parallel 
    #pragma omp single
      count_sols_par(sol, 0);
  time_par = omp_get_wtime() - start_time;
  sols_par = sols;

  // print(queens);

  printf("Seq: Found %d solutions in %2.3f s\n", sols_seq, time_seq); 
  printf("Par: Found %d solutions in %2.3f s\n", sols_par, time_par);

  if (sols_seq != sols_par) printf("\n*** ERROR ***\n\n");
  printf("\nSpeedup: %2.2f\n\n", time_seq/time_par);

  return 0;
}
