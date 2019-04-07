/*
 * test.c
 *
 *  Created on: 7. apr. 2019
 *      Author: mikael
 */


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int MAX_THREADS = 64;
int thread_count;

long arrived, rd;
pthread_mutex_t barex, mutex;
pthread_cond_t go;
int sum = 0;

void bar_init() {
	pthread_mutex_init(&barex, NULL);
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&go, NULL);
	arrived = 0; rd = 0;
}

void bar_pass() {
	pthread_mutex_lock(&barex);
	long my_rd = rd;
	arrived++;
	if (arrived == thread_count) {
		rd++;
		arrived = 0;
		pthread_cond_broadcast(&go);
	}
	while (rd == my_rd) {
		pthread_cond_wait(&go, &barex);
	}
	pthread_mutex_unlock(&barex);
}

void all_reduce(int * k) {
	pthread_mutex_lock(&mutex);
	sum += *k;
	pthread_mutex_unlock(&mutex);
	bar_pass();
	*k = sum;
	bar_pass();
	sum = 0;  // Bad way to do this, but i works...
	bar_pass();
}

void * test(void* rank) {
	long my_rank = (long) rank;
	unsigned int myseed = my_rank;
	srand(myseed + time(0));

	int num;

	//Test x time(s)
	int x = 1;
	for (int i = 0; i < x; i++) {
		num = rand()%100 - 50; // "Random" number i range -50 to 50
		printf("Thread %ld has k = %d\n", my_rank, num);
		all_reduce(&num);
		printf("Thread %ld has now k = %d\n", my_rank, num);
	}
	return NULL;
}

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: Number of threads as first arg\n.");
		exit(1);
	}
	long thread;

	pthread_t* thread_handles [MAX_THREADS];
	thread_count = atoi(argv[1]);
	bar_init();

	for (thread = 0; thread < thread_count; thread++)
		pthread_create(&thread_handles[thread], NULL, test, (void*) thread);

	for (thread = 0; thread < thread_count; thread++)
		pthread_join(thread_handles[thread], NULL);

    return 0;
}
