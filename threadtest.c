#include <stdio.h>
#include <pthread.h>
#include "tj.h"

void* PrintHello(void* d){
	printf("running print function: Hello\n");
	return NULL;
}

int main(){
  pool* p = initPool(5); //initialize pool with 5 threads

  //enqueue 20 jobs
  for(int i = 0; i < 20; i++){
	job_node* jn = buildJobNode((void*)PrintHello, NULL);
	int err = enqueueJob(p->queue,jn);
	if (err){
		printf("queue error");
		return -1;
	}
  }

  poolWait(p);
  printf("killing threads\n");
  drainPool(p);
  return 0;
}
