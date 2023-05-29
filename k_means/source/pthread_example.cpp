// C++ program creates threads

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_THREADS 16

void* func(void* arg) {
		printf("This is thread %ld working\n", (long)arg);
	pthread_exit(NULL);
}


int main() {
	pthread_t thread1, thread2;
	long id1 = 1, id2=2;
	int iret1 = pthread_create( &thread1, NULL, func, (void*)id1 );
	int ired2 = pthread_create( &thread2, NULL, func, (void*)id2 );

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	return 0;
}

