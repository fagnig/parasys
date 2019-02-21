/*  timing.c

02346, F19, DTU

See Databar Exercise 1 for instructions.

*/

#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#define LIMIT_I 1000
#define LIMIT_J 1000

 
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

int main() {
    int i, j;
    int x = 0;
    double start1, end1; /* timing variables [seconds] */
    double start2, end2; /* timing variables [seconds] */
    double start3, end3; /* timing variables [seconds] */

    start1 = get_current_time_seconds1();

    /* Some hard work */
    for (i = 0; i < LIMIT_I; i++) {
        for (j = 0; j < LIMIT_J; j++) {
            x = x + i*j;
        }
    }

    end1 = get_current_time_seconds1();

    start2 = get_current_time_seconds2();

    /* Some hard work */
    for (i = 0; i < LIMIT_I; i++) {
        for (j = 0; j < LIMIT_J; j++) {
            x = x + i*j;
        }
    }

    end2 = get_current_time_seconds2();

    start3 = get_current_time_seconds3();

    /* Some hard work */
    for (i = 0; i < LIMIT_I; i++) {
        for (j = 0; j < LIMIT_J; j++) {
            x = x + i*j;
        }
    }

    end3 = get_current_time_seconds3();
    
    printf("%s\n", "1000*1000 operations.");
    printf("Elapsed time: %1.50f sec\n"
           "              %1.50f sec\n"
           "              %1.50f sec\n\n", 
           end1-start1, end2-start2, end3-start3);

    //HIGHER LIMIT
    start1 = get_current_time_seconds1();

    /* Some hard work */
    for (i = 0; i < LIMIT_I*10; i++) {
        for (j = 0; j < LIMIT_J*10; j++) {
            x = x + i*j;
        }
    }

    end1 = get_current_time_seconds1();

    start2 = get_current_time_seconds2();

    /* Some hard work */
    for (i = 0; i < LIMIT_I*10; i++) {
        for (j = 0; j < LIMIT_J*10; j++) {
            x = x + i*j;
        }
    }

    end2 = get_current_time_seconds2();

    start3 = get_current_time_seconds3();

    /* Some hard work */
    for (i = 0; i < LIMIT_I*10; i++) {
        for (j = 0; j < LIMIT_J*10; j++) {
            x = x + i*j;
        }
    }

    end3 = get_current_time_seconds3();

    printf("%s\n", "10000*10000 operations.");
    printf("Elapsed time: %1.50f sec\n"
           "              %1.50f sec\n"
           "              %1.50f sec\n\n", 
           end1-start1, end2-start2, end3-start3);

    struct timespec tis;
    struct timespec tis2;

    clock_getres(CLOCK_REALTIME, &tis2);
    printf("%ld,%ld\n", tis2.tv_sec,tis2.tv_nsec);

    clock_getres(CLOCK_MONOTONIC, &tis2);
    printf("%ld,%ld\n", tis2.tv_sec,tis2.tv_nsec);

}

