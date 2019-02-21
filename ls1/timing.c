/*  timing.c

02346, F19, DTU

See Databar Exercise 1 for instructions.

*/

#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#define LIMIT_I 1000000
#define LIMIT_J 1000000

 
double get_current_time_seconds1() { 
    /* Get current time using gettimeofday */
	struct timeval time;
	gettimeofday(&time, NULL);
    return (double) ((double) time.tv_sec + (double) time.tv_usec/1000000);
}

double get_current_time_seconds2() { 
    /* Get current time using clock_gettime using CLOCK_REALTIME */
	struct timespec time;
	clock_gettime(CLOCK_REALTIME, &time);
    return (double) ( (double) time.tv_sec + (double) time.tv_nsec/1000000000);
}

double get_current_time_seconds3() { 
    /* Get current time using clock_gettime using CLOCK_MONOTONIC */
	struct timespec time;
	clock_gettime(CLOCK_MONOTONIC, &time);
    return (double) ( (double) time.tv_sec + (double) time.tv_nsec/1000000000);
}

double get_res2() { 
    /* Get current time using clock_gettime using CLOCK_REALTIME */
	struct timespec time;
	clock_getres(CLOCK_REALTIME, &time);
    return (double) ( (double) time.tv_sec + (double) time.tv_nsec/1000000000);
}

double get_res3() { 
    /* Get current time using clock_gettime using CLOCK_MONOTONIC */
	struct timespec time;
	clock_getres(CLOCK_MONOTONIC, &time);
    return (double) ( (double) time.tv_sec + (double) time.tv_nsec/1000000000);
}

int main() {
    int i, j;
    int x = 0;
    double start1, end1; /* timing variables [seconds] */
    double start2, end2; /* timing variables [seconds] */
    double start3, end3; /* timing variables [seconds] */

	printf("time: %1.50f\n",get_current_time_seconds3());
	printf("time: %d\n",get_res3());
	/*
    start1 = get_current_time_seconds1();
    start2 = get_current_time_seconds2();
    start3 = get_current_time_seconds3();
	*/
    /* Some hard work */
	/*
    for (i = 0; i < LIMIT_I; i++) {
        for (j = 0; j < LIMIT_J; j++) {
            x = x + i*j;
        }
    }

    end1 = get_current_time_seconds1();
    end2 = get_current_time_seconds2();
    end3 = get_current_time_seconds3();

    printf("Elapsed time: %1.50f sec\n"
           "              %1.50f sec\n"
           "              %1.50f sec\n", 
           end1-start1, end2-start2, end3-start3);*/
	
}

