#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>


/* todo
 *   [] finish initThread
 *   [] fix ptr based functions
 *   [] add functionality to test program
 *   [] finish threadLoop
 * */

pthread_mutex_t jq_mutex; //job queue mutex (for pushing and pulling)

typedef struct job_node {
    void (*function) (void* arg); // function
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
    struct job_queue queue;
} pool;

typedef struct thread {
    uint8_t id;
    pthread_t pt;
    struct pool* tpool; /* refer back to the threadpool */
    bool isalive;
} thread;

job_queue initQueue(){
	job_queue* jq = (struct job_queue*) malloc(sizeof(struct job_queue));
	jq->len=0;		
	jq->head=NULL;
	jq->tail=NULL;

	pthread_mutex_init(&jq_mutex,NULL); 
	return *jq;	
}


pool* initPool(uint8_t len){
	pool* p = (struct pool*) malloc(sizeof(struct pool));
	p->threads = NULL;
	p->queue = initQueue();
	p->size = len;
	return p;
}

/* start a thread in pool p
 * have it wait around using the threadLoop function
 * on success, return 1
 */

int initThread(pool* p){
	return 0;
}

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
	
	if (jq->head){
		job_node* j = jq->head;
		//check for null here
		jq->head = j->next;	
		return j;
	}else{
		return NULL;	
	}

	pthread_mutex_unlock(&jq_mutex);
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
	pthread_mutex_unlock(&jq_mutex);

	return 0;
}

bool pollJobs(job_queue jq){
	// do we need a semaphore for this?
	return !(jq.head==NULL);	

}

/* each thread will wait around in thread purgatory until there's 
 * something to do (i.e. we put jobs in the queue) *
 */
static void* threadLoop(thread* thrd) {
	while(thrd->isalive){
		if(pollJobs(thrd->tpool->queue)){
			//run the job	
		}	
	}	

	//kill the thread here
	
}
