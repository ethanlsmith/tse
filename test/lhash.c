#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include "lhash.h"
#include "queue.h"

typedef struct lhashtable {
	pthread_mutex_t m;
	hashtable_t *htp;
} lhash_s;

/* hopen -- opens a hash table with initial size hsize */
lhash_t *lhopen(uint32_t hsize) {
	lhash_s *lhtp = malloc(sizeof(lhash_s));
	pthread_mutex_t *mptr;
	*mptr=lqp->m;
	pthread_mutex_init(mptr,NULL);
	lhtp->htp=hopen(hsize);
	return(lhtp);
}        

/* hclose -- closes a hash table */
void lhclose(lhash_t *lhtp) {
	lhash_s *lhtc=(lhash_s *)lhtp;
	qclose(lhtc->htp);
	pthread_mutex_destroy(&lhtc->m);
	free(lhtp);
}

/* hput -- puts an entry into a hash table under designated key 
 * returns 0 for success; non-zero otherwise
 */
int32_t lhput(lhash_t *lhtp, void *ep, const char *key, int keylen) {
	lhash_s *lhtc=(lhash_s *)lhtp;
	pthread_mutex_lock(&lhtc->m);
	hput(lhtc->htp,ep,key,keylen);
	pthread_mutex_unlock(&lhtc->m);
	return(0);
}

/* happly -- applies a function to every entry in hash table */
void lhapply(lhashtable_t *lhtp, void (*fn)(void* ep)) {
	lhash_s *lhtc=(lhash_s *)lhtp;
	pthread_mutex_lock(&lhtc->m);
	happly(lhtc->htp,fn);
	pthread_mutex_unlock(&lhtc->m);
}

/* hsearch -- searchs for an entry under a designated key using a
 * designated search fn -- returns a pointer to the entry or NULL if
 * not found
 */
void *lhsearch(lhashtable_t *lhtp, bool (*searchfn)(void* elementp, const void* searchkeyp), 
	      const char *key, 
	      int32_t keylen) {
	lhash_s *lhtc=(lhash_s *)lhtp;
	pthread_mutex_lock(&lhtc->m);
	void*ep=hsearch(lhtc->htp,searchfn,keyp,keylen);
	pthread_mutex_unlock(&lhtc->m);
	return(ep);
}