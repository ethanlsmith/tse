 /*
* Structure Read and Print: Ethan Smith
* purpose: read file of strings and numbers into structure and print to screen*/
#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include "lqueue.h"

typedef struct lqueue {
	pthread_mutex_t m;
	queue_t *qp;
} lqueue_t;

/* lqopen creates a locked queue
*	the mutex m must be destroyed in lqclose()
*	returns a locked queue which is malloc'd space
*/
lqueue_t* lqopen(void) {
	lqueue_t *lqp = malloc(sizeof(lqueue_t));
	pthread_mutex_t *mptr;
	*mptr=lqp->m;
	pthread_mutex_init(mptr,NULL);
	lqp->qp=qopen();
	return(lqp);
}        

/* deallocate a locked queue, frees everything in it */
void lqclose(lqueue_t *lqp) {
	qclose(lqp->qp);
	pthread_mutex_destroy(&lqp->m);
	free(lqp);
}

int32_t lqput(lqueue_t *lqp, void *elementp) {
	pthread_mutex_lock(&lqp->m);
	qput(lqp->qp,elementp);
	pthread_mutex_unlock(&m);
	return(0);
}

void* lqget(lqueue_t *lqp) {
	pthread_mutex_lock(&lqp->m);
	void*ep=qget(lqp->qp);
	pthread_mutex_unlock(&m);
	return(ep);
}

void lqapply(lqueue_t *lqp, void (*fn)(void* elementp)) {
	pthread_mutex_lock(&lqp->m);
	qapply(lqp->qp,fn);
	pthread_mutex_unlock(&m);
}

void* lqsearch(lqueue_t *lqp, bool (*searchfn)(void* elementp,const void* keyp), const void* skeyp) {
	pthread_mutex_lock(&lqp->m);
	void*ep=qsearch(lqp->qp,searchfn,skeyp);
	pthread_mutex_unlock(&m);
	return(ep);
}

