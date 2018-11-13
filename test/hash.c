/* 
 * hash.c -- implements a generic hash table as an indexed set of queues.
 *
 */
#include <stdint.h>
#include <hash.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <queue.h>
#define MAXNM 20
/* 
 * SuperFastHash() -- produces a number between 0 and the tablesize-1.
 * 
 * The following (rather complicated) code, has been taken from Paul
 * Hsieh's website under the terms of the BSD license. It's a hash
 * function used all over the place nowadays, including Google Sparse
 * Hash.
 */
#define get16bits(d) (*((const uint16_t *) (d)))

/**************** global types ****************/
typedef struct hashtable {
	int size;				// size of array
  struct queue **array;		// array of pointers to queues
} hashtable_s;

typedef struct queue {
	struct qnode *front;/*point to front of list*/
  struct qnode *back;
	} queue_s; 		

typedef struct qnode {
	void *item;        /*Pointer to data of item*/
	struct qnode *next;/*link to next node*/
} qnode_t;



static uint32_t SuperFastHash (const char *data,int len,uint32_t tablesize) {
  uint32_t hash = len, tmp;
  int rem;
  
  if (len <= 0 || data == NULL)
		return 0;
  rem = len & 3;
  len >>= 2;
  /* Main loop */
  for (;len > 0; len--) {
    hash  += get16bits (data);
    tmp    = (get16bits (data+2) << 11) ^ hash;
    hash   = (hash << 16) ^ tmp;
    data  += 2*sizeof (uint16_t);
    hash  += hash >> 11;
  }
  /* Handle end cases */
  switch (rem) {
  case 3: hash += get16bits (data);
    hash ^= hash << 16;
    hash ^= data[sizeof (uint16_t)] << 18;
    hash += hash >> 11;
    break;
  case 2: hash += get16bits (data);
    hash ^= hash << 11;
    hash += hash >> 17;
    break;
  case 1: hash += *data;
    hash ^= hash << 10;
    hash += hash >> 1;
  }
  /* Force "avalanching" of final 127 bits */
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;
  return hash % tablesize;
}

/* hopen -- opens a hash table with initial size hsize */
hashtable_t *hopen(uint32_t hsize){
	
		// initialize array of pointers to sets at given size
	struct queue **array = calloc(hsize, sizeof(struct queue_s*));
	hashtable_s *ht = malloc(sizeof(hashtable_s));

  	if (array == NULL) {
    	return NULL; // error allocating set
  	} else {
    	// initialize hashtable data
    	ht->array = array;
    	ht->size = hsize;
    	return ht;
  	}
	
}


/* hput -- puts an entry into a hash table under designated key 
 * returns 0 for success; non-zero otherwise
 */
int32_t hput(hashtable_t *htp, void *ep, const char *key, int keylen){
	
	hashtable_s *htps = (hashtable_s *)htp;
	if (htps == NULL || key == NULL || ep == NULL /*|| hsearch(htp, key)*/) {
		return false;
	} else {

		// determine address with hashing func provided by engs50
		unsigned long add = SuperFastHash(key, keylen,htps->size);
		char *key_copy = (char *) malloc(sizeof(key));  // copy key for use
		strcpy(key_copy, key);

		if (htps->array[add] == NULL) { //if address not used, make new queue
			queue_s *que = qopen();
			qput(que, ep);
			htps->array[add] = que;
		} else { //if address is used, add item
			qput(htps->array[add], ep);
		}
		
		free(key_copy);
	}
	return(0); // item inserted successfully 
}

/* hclose -- closes a hash table */
void hclose(hashtable_t *htp){
	    hashtable_s *htps = (hashtable_s *)htp;
		if (htps != NULL) {
		// delete each item in each queue
		for(int i=0; i<htps->size; i++) {
			qclose(htps->array[i]);
		}
		free(htps->array); // free hashtable's array
		free(htps);		 // free hashtable
	}
	
}

/* happly -- applies a function to every entry in hash table */
void happly(hashtable_t *htp, void (*fn)(void* ep)){
	    hashtable_s *htps = (hashtable_s *)htp;
		if(htps != NULL && fn != NULL) {
		for (int i=0; i<htps->size; i++) {
			// call itemfunc on each node in each set in hashtable
			qapply(htps->array[i], fn);
		}
	}	
}

/* hsearch -- searchs for an entry under a designated key using a
 * designated search fn -- returns a pointer to the entry or NULL if
 * not found
 */
void *hsearch(hashtable_t *htp, bool (*searchfn)(void* elementp, const void* searchkeyp),const char *key, int32_t keylen){
	hashtable_s *htps = (hashtable_s *)htp;
	if (htps==NULL || key==NULL){
		return NULL;
	} else {
		for(int i=0; i<htps->size; i++) {
			if (qsearch(htps->array[i],searchfn,(void*)key) != NULL) {
				// if key match is found, return the item
				return qsearch(htps->array[i],searchfn,(void*)key); 
			} 
		}
		return NULL; // func reaches here if no match is found
	}
		
}

/* hremove -- removes and returns an entry under a designated key
 * using a designated search fn -- returns a pointer to the entry or
 * NULL if not found
 */
void *hremove(hashtable_t *htp, bool (*searchfn)(void* elementp, const void* searchkeyp), const char *key, int32_t keylen){

	// hashtable_s *htps = (hashtable_s *)htp;
	/* void * ptr;

		if (htp != NULL) {
			ptr = hsearch(htp,searchfn,key,keylen);
      person_t * p1 = (person_t*)ptr;
			char * pname = p1->name;
			printf("found %s in hsearch. We were searching for %s\n",pname,key);
			return(qremove(ptr,searchfn,(void*)key));
		}
		
		return(NULL); */
	  hashtable_s *htps = (hashtable_s *)htp;                                                                                             
  if (htps==NULL || key==NULL){                                                                                                          
    return NULL;                                                                                                                        
  } else {                                                                                                                               
    for(int i=0; i<htps->size; i++) {                                                                                                   
      if (qsearch(htps->array[i],searchfn,(void*)key) != NULL) {                                                                        
        // if key match is found, return the item                                                                                       
        return qremove(htps->array[i],searchfn,(void*)key);                                                                             
      }                                                                                                                                 
    }                                                                                                                                   
    return NULL; // func reaches here if no match is found                                                                              
  } 

	}
