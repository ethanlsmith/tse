 /*
* Structure Read and Print: Ethan Smith
* purpose: read file of strings and numbers into structure and print to screen*/
#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include "lqueue.h"
#include "queue.h"

typedef struct lqueue {
	pthread_mutex_t m;
	queue_t *qp;
} lqueue_s;

/* lqopen creates a locked queue
*	the mutex m must be destroyed in lqclose()
*	returns a locked queue which is malloc'd space
*/
lqueue_t* lqopen(void) {
	lqueue_s *lqp = malloc(sizeof(lqueue_s));
	pthread_mutex_t *mptr;
	*mptr=lqp->m;
	pthread_mutex_init(mptr,NULL);
	lqp->qp=qopen();
	return(lqp);
}        

/* deallocate a locked queue, frees everything in it */
void lqclose(lqueue_t *lqp) {
	lqueue_s *lqc=(lqueue_s *)lqp;
	qclose(lqc->qp);
	pthread_mutex_destroy(&lqc->m);
	free(lqp);
}

int32_t lqput(lqueue_t *lqp, void *elementp) {
	lqueue_s *lqc=(lqueue_s *)lqp;
	pthread_mutex_lock(&lqc->m);
	qput(lqc->qp,elementp);
	pthread_mutex_unlock(&lqc->m);
	return(0);
}

void* lqget(lqueue_t *lqp) {
	lqueue_s *lqc=(lqueue_s *)lqp;
	pthread_mutex_lock(&lqc->m);
	void*ep=qget(lqc->qp);
	pthread_mutex_unlock(&lqc->m);
	return(ep);
}

void lqapply(lqueue_t *lqp, void (*fn)(void* elementp)) {
	lqueue_s *lqc=(lqueue_s *)lqp;
	pthread_mutex_lock(&lqc->m);
	qapply(lqc->qp,fn);
	pthread_mutex_unlock(&lqc->m);
}

void* lqsearch(lqueue_t *lqp, bool (*searchfn)(void* elementp,const void* keyp), const void* skeyp) {
	lqueue_s *lqc=(lqueue_s *)lqp;
	pthread_mutex_lock(&lqc->m);
	void*ep=qsearch(lqc->qp,searchfn,skeyp);
	pthread_mutex_unlock(&lqc->m);
	return(ep);
}

