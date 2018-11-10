 /*
* Structure Read and Print: Nick Blasey
* purpose: read file of strings and numbers into structure and print to screen*/
#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <inttypes.h>
#include "queue.h"
#define MAXNM 20


typedef struct qnode {
	void *item;        /*Pointer to data of item*/
	struct qnode *next;/*link to next node*/
} qnode_t;

typedef struct queue {
	struct qnode *front;/*point to front of list*/
  struct qnode *back;
	} queue_s; 		

/*create empty queue*/
queue_t* qopen(void){
	
	/*allocate memory to struct*/
	queue_s *new_queue = malloc(sizeof(queue_s));
	
	new_queue->front = NULL; 
	new_queue->back  = NULL; 
	return(new_queue);
}

/* put element at the end of the queue
 * returns 0 is successful; nonzero otherwise 
 */
int32_t qput(queue_t *qp, void *elementp){
	
	queue_s *qc;
	qc = (queue_s *)qp;
	if ((qc != NULL) && (elementp != NULL)) {
			
			/*initliase a node to hold info about this new element*/
			qnode_t *pnode = malloc(sizeof(qnode_t));
			/*Pnode point to element p and has a next that point to NULL*/
			pnode->item = elementp;
			pnode->next = NULL;
			/*If list is empty then front should point to pnode*/
			if (qc->front == NULL){
				qc->front = pnode;
			}
			/*currently back points to some pnode at end of queue
			we want to have this pnode's "next" to point to element p*/
			/*if the back points to some pnode do this*/
			if (qc->back != NULL) {
				qc->back->next = pnode;
			}
			
			/*Have end of queue point to pnode*/
			qc->back = pnode;
	    return(0);
	}
	/*If unsuccesful return 1*/
	return(1);
		
}

/* get the first first element from queue, removing it from the queue */
void* qget(queue_t *qp){
		/*First coerce qp into something we can make sense of*/
		queue_s *qc;
		qnode_t *qtemp;
		qc = (queue_s *)qp;
		
		void* elmptr;
		
		/*if list isnt empty get the item that the first qnode in the list
		points to and move the front pointer to the next element in the list*/
		if (qc->front != NULL) {
			elmptr = qc->front->item;
			qtemp = qc->front;
			qc->front = qc->front->next;
			free(qtemp);
			return(elmptr);
		}
		/*If list empty return NULL*/
		return(NULL);
}

/* deallocate a queue, frees everything in it */
void qclose(queue_t *qp){
		
	if (qp != NULL){
		queue_s *qc;
		qc = (queue_s *)qp;
		/*if not empty*/
		if (qc->front != NULL) {
			/*Set pointer to first qnode in list*/
			qnode_t *prev = qc->front;
			qnode_t *curr = qc->front->next;
			/*Cycle through list and free ptrs*/
			while (curr != NULL) {
				free(prev->item);
				free(prev);
				prev = curr;
				curr = curr->next;
			}
			free(prev->item);
			free(prev);
		}
	  /*free queue pointer as well*/
		free(qc);
		
	}
}

/* apply a function to every element of the queue */
void qapply(queue_t *qp, void (*fn)(void* elementp)){
	
	if (qp != NULL){
	queue_s *qc;
	qc = (queue_s *)qp;
	/*if not empty*/
			if (qc->front != NULL) {
				/*Set pointer to first qnode in list*/
				qnode_t *curr = qc->front;
				
				/*Cycle through list and free ptrs*/
				while (curr != NULL) {
					fn(curr->item);
					curr = curr->next;
					}
			 }
	}
}

/* concatenatenates elements of q2 into q1
 * q2 is dealocated, closed, and unusable upon completion 
 */

void qconcat(queue_t *q1p, queue_t *q2p){
		
	if (q2p != NULL){
		queue_s *qc2;
		queue_s *qc1;
		qc1 = (queue_s *)q1p;
		qc2 = (queue_s *)q2p;
		/*if the list to be merged and removed is not empty and q1 isn't empty*/
		if ((qc2->front != NULL) && (qc1->front != NULL)) {
			printf("non empty non empty\n");
			/*point last item at back of q1 to front of q2*/
			
			qc1->back->next = qc2->front;
			
			/*point back of q1 to back of q2*/
			qc1->back = qc2->back;
			free(qc2);
					/*qclose(q2p);*/
			
		}
		
		/*if q1 is empty and q2 is non empty*/
		else if  ((qc2->front != NULL) && (qc1->front == NULL))  {
			printf("q2 non empty q1 empty\n");   
			qc1->front = qc2->front;
			qc1->back = qc2->back;
			free(qc2);
		} else {

			free(qc2);

		}
				
		
	}	
	
}	



/* search a queue using a supplied boolean function
 * skeyp -- a key to search for
 * searchfn -- a function applied to every element of the queue
 *          -- elementp - a pointer to an element
 *          -- keyp - the key being searched for (i.e. will be 
 *             set to skey at each step of the search
 *          -- returns TRUE or FALSE as defined in bool.h
 * returns a pointer to an element, or NULL if not found
 */
void* qsearch(queue_t *qp, bool (*searchfn)(void* elementp, const void* keyp),const void* skeyp){
			
	if (qp != NULL){
	queue_s *qc;
	qc = (queue_s *)qp;
	/*if not empty*/
			if (qc->front != NULL) {
				/*Set pointer to first qnode in list*/
				qnode_t *curr = qc->front;
				/*Cycle through list and free ptrs*/
				while (curr/*->next*/ != NULL) {
					
					if (searchfn(curr->item,skeyp)){
						return(curr->item);
					}
					curr = curr->next;
					}
			 }
	}					
	return(NULL);
	}
	
	/* search a queue using a supplied boolean function (as in qsearch),
 * removes the element from the queue and returns a pointer to it or
 * NULL if not found
 */
void* qremove(queue_t *qp, bool (*searchfn)(void* elementp,const void* keyp),const void* skeyp){
	
	if (qp != NULL){
	queue_s *qc;
	qnode_t *qtemp;
	qc = (queue_s *)qp;
	/*if not empty*/
	void * elmptr;
			if (qc->front != NULL) {
				/*Set pointer to first qnode in list*/
				qnode_t *curr = qc->front;
				qnode_t *prev = curr;
				/*Cycle through list and free ptrs*/
				while (curr/*->next*/ != NULL) {
					                                                                                                      
					if (searchfn(curr->item,skeyp)){
						/*If first item in list is to be removed nump front up*/
					  if (prev == curr){
              elmptr = curr->item;
							qtemp = curr;
							qc->front = curr->next;      
							qp = (queue_t *)qc;
							free(qtemp);
							/*qp->front = qc->front;*/
							return(elmptr); 
						}
						/*if found*/
					qtemp = prev->next;
					elmptr = curr->item;
					prev->next = curr->next;
					qp = qc;
					free(qtemp);
				  /*remove and return pointer*/
					return(elmptr);
					}
					prev = curr;
					curr = curr->next;
					}
			 }
	}	
	printf("Didnt find it,bummer\n");
return(NULL);	

}



