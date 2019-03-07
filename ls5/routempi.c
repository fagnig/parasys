/* routempi.c 
  parralel implementation of route.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

int main(int argc, char* args[]){

  int  numtasks, taskid;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

  if (argc < 2) {
    printf("Your must supply a route file\n");
    exit(1);
  } 
  read_route(args[1]); 

  define_section();


}