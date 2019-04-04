#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <omp.h>

int main(int argc, char* argv[]) 
{	
	if (argc < 2) {
		printf("Usage: ./scheduling <nb. of iterations>\n.");
		exit(1);
	}
	
	int iterations = atoi(argv[1]);
	#pragma omp parallel
	{
		int i;
		int first = 1;
		int tid = omp_get_thread_num();
		int start, end;
		
		#pragma omp for
		for (i=0 ; i<iterations ; i++) {
			if(first) {
				first = 0;
				start = i;
				end = i;
			} else {
				end++;
			}
		}
		
		printf("Thread %d: Iterations %d - %d\n", tid, start, end);
	}
}

