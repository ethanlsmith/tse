#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <inttypes.h>
#include <pthread.h>
#include "lhash.h"
#include "hash.h"
#include "queue.h"

typedef struct lhashtable {
	pthread_mutex_t m;
	hashtable_t *htp;
} lhash_s;

/* hopen -- opens a hash table with initial size hsize */
lhashtable_t *lhopen(uint32_t hsize) {
	pthread_mutex_t *mp;
	hashtable_t *hp;
	lhash_s *lhtp;
	
	if((lhtp = malloc(sizeof(lhash_s)))==NULL) // create locked ht
		return NULL;
	mp = &(lhtp->m);					// init associated mutex
	pthread_mutex_init(mp,NULL);
	if((hp = hopen(hsize))==NULL)		// open the hash table
		return NULL;
	lhtp->htp = hp;						// put in locked ht
	return lhtp;						// return locked ht
}        

/* hclose -- closes a hash table */
void lhclose(lhashtable_t *lhtp) {
	lhash_s *lhtc=(lhash_s *)lhtp;
	hclose(lhtc->htp);
	pthread_mutex_destroy(&lhtc->m);
	free(lhtc);
}

/* hput -- puts an entry into a hash table under designated key 
 * returns 0 for success; non-zero otherwise
 */
int32_t lhput(lhashtable_t *lhtp, void *ep, const char *key, int keylen) {
	lhash_s *tp;
	pthread_mutex_t *mp;
	
	tp = (lhash_s*)lhtp;
	mp = &(tp->m);
	pthread_mutex_lock(mp);
	if((hput(tp->htp,ep,key,keylen))!=0) {
		pthread_mutex_unlock(mp);
		return(1);
	}
	pthread_mutex_unlock(mp);
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
	void*ep=hsearch(lhtc->htp,searchfn,key,keylen);
	pthread_mutex_unlock(&lhtc->m);
	return(ep);
}