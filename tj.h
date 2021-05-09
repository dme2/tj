#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

/* todo
 *   [x] finish initThread
 *   [x] add functionality to test program
 *   [x] finish threadLoop
 *   [x] add destructors (job queue, pool, thread)
 *   []  add thread condition for waiting
 *   []  add debuging info
 *   []  fix threadwait
 * */

pthread_mutex_t jq_mutex; //job queue mutex (for pushing and pulling)
typedef struct job_node {
    void (*function) (void* arg); // function to be executed
    void *args; //arg list
    struct job_node* prev;
    struct job_node* next;
} job_node;

typedef struct job_queue {
    job_node* head;
    job_node* tail;
    uint8_t len;
} job_queue;

typedef struct pool {
    struct thread** threads; /* our list of threads */
    uint8_t size;   /* number of threads */
    struct job_queue* queue;
} pool;

typedef struct thread {
    uint8_t id;
    pthread_t pt;
    struct pool* tpool; /* refer back to the threadpool */
    bool isalive;
} thread;

static void* threadLoop(thread* thrd);

job_queue* initQueue(){
	job_queue* jq = (struct job_queue*) malloc(sizeof(struct job_queue));
	jq->len=0;
	jq->head=NULL;
	jq->tail=NULL;

	pthread_mutex_init(&jq_mutex,NULL);
	return jq;
}

thread* initThread(pool* p, uint8_t id){
  thread* th = (struct thread*) malloc(sizeof(struct thread));

  th->tpool = p;
  th->isalive = true;
  th->id = id;

  //send to thread purgatory
  pthread_create(&th->pt, NULL, (void * (*)(void *)) threadLoop, th);
  pthread_detach(th->pt);

  return th;
}

pool* initPool(uint8_t len){
	pool* p = (struct pool*) malloc(sizeof(struct pool));
	p->threads = (struct thread**) malloc( len * sizeof(struct thread)); //malloc for len threads
	p->queue = initQueue();
	p->size = len;

	//init (len) threads
	for (int i = 0; i < len; i++){
	  p->threads[i] = initThread(p,i);
	}
	return p;
}

/* start a thread in pool p
 * have it wait around using the threadLoop function
 * on success, return 1
 */

job_node* buildJobNode(void (*function_ptr)(void*), void* arg_ptr){
	job_node* job = (struct job_node*) malloc(sizeof(struct job_node));

	job->function = function_ptr;
       	job->args = arg_ptr;
	job->next=NULL;
       	job->prev=NULL;
	return job;
}

job_node* pullJob(job_queue* jq){
	pthread_mutex_lock(&jq_mutex);
	job_node* job = NULL;
	if (jq->head){
		job_node* j = jq->head;

		if (j->next != NULL)
		  jq->head = j->next;
		else{
		  jq->head = NULL;
		}
		job = j;
		jq->len -= 1;
	}

	pthread_mutex_unlock(&jq_mutex);
	return job;
}

int enqueueJob(job_queue* jq, job_node* job){
	/* 2 scenarios here
	*  head is null: list is empty, set jq.head == jq.tail = job
	*   and set job.prev = NULL
	*  otherwise:
	*  	job.prev = jq.tail
	*  	jq.tail = jobf
	*
	*/
  pthread_mutex_lock(&jq_mutex);
  if (jq->head == NULL){
	jq->head = job;
	jq->tail = job;
  }
  else{
	job->prev = jq->tail;
	jq->tail->next=job;
	jq->tail = job;
  }
  jq->len+=1;
  pthread_mutex_unlock(&jq_mutex);

  return 0;
}

bool pollJobs(job_queue jq){
  // do we need a semaphore for this?
  pthread_mutex_lock(&jq_mutex);
  bool retVal = false;
  if (jq.head != NULL)
	retVal = true;
  pthread_mutex_unlock(&jq_mutex);
  return retVal;

}

/* each thread will wait around in thread purgatory until there's
 * something to do (i.e. we put jobs in the queue) *
 */
static void* threadLoop(thread* thrd) {
  while(thrd->isalive){
	if(pollJobs(*thrd->tpool->queue)){
	  //run the job
	  //mutex here??
	  void (*func) (void*);
	  void* args;

	  job_node* job = pullJob(thrd->tpool->queue);
	  if(job){
		func = job->function;
		args = job->args;
		func(args);
		free(job);
	  }
	}
  }

  return NULL;
}

void poolWait(pool* p){

	while(p->queue->len != 0) {
	//temporary and bad
	sleep(0.5);
  }
}

int drainPool(pool* p){
  //delete threads
  for (int i = 0; i < p->size; i++){
	free(p->threads[i]);
  }

  //delete jobs
  job_node* cur = NULL;
  if (p->queue->head){
	cur = p->queue->head;
  }

  uint8_t s = p->queue->len;
  for(int i = 0; i < s; i++){
	if (cur == NULL){
	  break;
	}
	job_node* nxt = NULL;

	if(cur->next){
	  nxt = cur->next;
	}

	free(cur);
	cur = nxt;
  }

  //delete pool
  free(p);

  return 0;
}
