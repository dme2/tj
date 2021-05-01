#include <stdio.h>
#include <pthread.h>
#include "tj.h"
void* PrintHello(void* data){

	printf("received data");
}

int main(){
	int rc;
	pthread_t id;
	int t = 12;

	rc = pthread_create(&id, NULL, PrintHello, (void*) t);
/*
	if(rc){
		printf("error");
		exit(1);
	}
	*/

	printf("created thread\n");
	pthread_exit(NULL);
}

