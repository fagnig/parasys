/* communication.c 

   02346, F19, DTU

   For instructions, see Databar Exercise 3.
*/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#define  MASTER		0

int main (int argc, char *argv[])
{
  int  numtasks, taskid, len, partner, message;
  char hostname[MPI_MAX_PROCESSOR_NAME];
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

  /* need an even number of tasks  */
  if (numtasks % 2 != 0) {
   if (taskid == MASTER) 
    printf("Quitting. Need an even number of tasks: numtasks=%d\n", numtasks);
  } else {
    if (taskid == MASTER) 
      printf("MASTER: Number of MPI tasks is: %d\n",numtasks);
	
	//Wait for all nodes to reach this state
	MPI_Barrier(MPI_COMM_WORLD);
  
    MPI_Get_processor_name(hostname, &len);
    printf ("Hello from task %d on %s!\n", taskid, hostname);

	//Wait for all nodes to reach this state
	MPI_Barrier(MPI_COMM_WORLD);
	
    /* determine partner and then send/receive with partner */
    if (taskid < numtasks/2) {
      partner = numtasks/2 + taskid;
      MPI_Send(&taskid, 1, MPI_INT, partner, 1, MPI_COMM_WORLD);
      MPI_Recv(&message, 1, MPI_INT, partner, 1, MPI_COMM_WORLD, &status);
    } else if (taskid >= numtasks/2) {
      partner = taskid - numtasks/2;
      MPI_Recv(&message, 1, MPI_INT, partner, 1, MPI_COMM_WORLD, &status);
      MPI_Send(&taskid, 1, MPI_INT, partner, 1, MPI_COMM_WORLD);
    }

    //Order print starting with master
	if (taskid == MASTER)
	{
		go = 'r';
		/* print partner info and exit*/
		printf("Task %d is partner with %d\n",taskid,message);
		MPI_Send(&go, 1, MPI_CHAR, taskid + 1, 1, MPI_COMM_WORLD);
	}
	else
	{
		MPI_Recv(&go, 1, MPI_CHAR, taskid - 1, 1, MPI_COMM_WORLD, &status);
		/* print partner info and exit*/
		printf("Task %d is partner with %d\n",taskid,message);
		if (taskid < numtasks)
			MPI_Send(&go, 1, MPI_CHAR, taskid + 1, 1, MPI_COMM_WORLD);
	}
  }

  MPI_Finalize();
}
