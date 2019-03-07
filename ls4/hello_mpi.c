/* hello_mpi.c 

   02346, F19, DTU

   For instructions, see Databar Exercise 3.
*/
#include "mpi.h"
#include <stdio.h>
#define MASTER 0

int main(int argc, char *argv[]) {
 int  numtasks, rank, len, rc; 
 char hostname[MPI_MAX_PROCESSOR_NAME];

  rc = MPI_Init(&argc,&argv);
  if (rc != MPI_SUCCESS) {
    printf ("Error starting MPI program. Terminating.\n");
    MPI_Abort(MPI_COMM_WORLD, rc);
  }

  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Get_processor_name(hostname, &len);
  printf ("Number of tasks= %d My rank= %d Running on %s\n", numtasks,rank,hostname);

  if (rank == MASTER) {
    printf ("The master is running on %s\n", hostname);
  }

  MPI_Finalize();
}
