/* indexer.c --- 
 * 
 * 
 * Author: Ethan L. Smith
 * Created: Sun Oct 28 19:57:05 2018 (-0400)
 * Version: 
 * 
 * Description: indexes words in a given webpage_t
 * set args ../pages/ lindexout 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>
#include <unistd.h>
#include <pthread.h>
#include "webpage.h"
#include "lhash.h"
#include "queue.h"
#include "pageio.h"

int count=0;
int pnum;
FILE*fp=NULL;
lhashtable_t*Index;

typedef struct word {
  char *word;                               // word
  queue_t *Docs;                            // que of documents
} word_t;

typedef struct doc {
  int id;									//document id
  int count;								//occurrences of words in that doc
} doc_t;

typedef struct args {
	lhashtable_t *Index;
	char *pageDir;
	int tid;
} arg_t;

int NormalizeWord(char **word);
bool searchfn(void* elementp,const void* keyp);
void fn(void* ep);
word_t *entry_new(char *word, const int id, const int count);
void entry_delete(void *data);
static void *checkp(void *p, char *message);
void sumwords(void*ep);
bool docsearch(void* elementp,const void* keyp);
doc_t *new_doc(const int id, const int count);
void sum(void*ep);
void doc_delete(void *data);
void printindexword(void*ep);
void printindexdoc(void*ep);
char* itoa(int val, int base);
void indexsave(lhashtable_t*Index,char*indexnm);
void *indexload(void*argz);

int main(const int argc, char *argv[])
{
	// check for exactly three parameters
	if (argc != 4) {
		fprintf(stderr, "usage: exactly 3 arguments (id)\n");
		return 1;
	}

	// create variables of arguments
	char *pageDir = argv[1];
	char *indexnm = argv[2];
	pnum = atoi(argv[3]);
	int numthreads=0;
	Index=lhopen(1000);
	arg_t *args[pnum];
	pthread_t thread[pnum];
	
	for(int m=1;m<pnum+1;m++) {
		args[m]=malloc(sizeof(arg_t*));
		args[m]->Index=Index;
		args[m]->pageDir=malloc(sizeof(pageDir)*strlen(pageDir));
		strcpy(args[m]->pageDir,pageDir);
		args[m]->tid=m;
		printf("creating thread %d\n",args[m]->tid);
		pthread_create(&thread[m],NULL,indexload,(void*)args[m]);
		numthreads++;
	}
	
	for(int p=1;p<pnum+1;p++) {
		pthread_join(thread[p],NULL);	
		numthreads--;
	}
	
	indexsave(Index,indexnm);
	
	lhapply(Index,entry_delete);
	lhclose(Index);
	for(int i=1;i<pnum+1;i++) {
		free(args[i]->pageDir);
		free(args[i]);
	}
	return(0);
	
}

void *indexload(void*argz)
{
	arg_t *args=(arg_t*)argz;
	int pos=0;
	int docid;
	int maxpos=0;
	char *dirtemp = malloc(sizeof(args->pageDir)*strlen(args->pageDir));
	strcpy(dirtemp,args->pageDir);
	int tid=args->tid;
	printf("thread id is %d\n",tid);
		
	char *word="hellotherehowareyou";
	FILE*fp1=NULL;
	
	for(int c=tid;(fp1=fopen(strcat(dirtemp,itoa(c,10)),"r"));c+=pnum) {
		
		strcpy(dirtemp,args->pageDir);
		
		webpage_t *page=pageload(c,args->pageDir);
		docid=c;
		printf("the page number is %d\n",c);
		pos=1;
		maxpos=0;
		
		printf("url is %s\n",webpage_getURL(page));
		maxpos=webpage_getHTMLlen(page);
		
		while((pos=webpage_getNextWord(page, pos, &word))>0 && (pos<maxpos) )  {
			if(NormalizeWord(&word)>0) {
				//check for the word in the Index
				word_t *temp=(word_t*)lhsearch(args->Index,searchfn,word,strlen(word));
				if (temp!=NULL) {
					//check for the doc in the word's queue
					doc_t*moddoc=qsearch(temp->Docs,docsearch,(void*)&docid);
					if (moddoc!=NULL) {
						//increment the count if the doc is already in the word's queue
						moddoc->count+=1;
					}
					else {
						//create a doc type associated with that document
						doc_t *newdoc=new_doc(docid,1);
						qput(temp->Docs,(void*)newdoc);
					}
				}
				else {
					//create a new entry in the Index
					word_t *wordstruct=entry_new(word,docid,1);		//infinite loop
					lhput(args->Index,(void*)wordstruct,word,strlen(word));
				}
				free(word);
			}
		}
		//deallocate and find the number of words in the Index
		webpage_delete(page);
		fclose(fp1);
	}
	free(dirtemp);
	return(0);
}

void indexsave(lhashtable_t*Index,char*indexnm)
{
	fp=fopen(indexnm,"w");
	lhapply(Index,printindexword);
	fclose(fp);
}

void printindexword(void*ep)
{
	word_t*temp=(word_t*)ep;
	fprintf(fp,"%s ",temp->word);
	qapply(temp->Docs,printindexdoc);
	fprintf(fp,"\n");
}

void printindexdoc(void*ep)
{
	doc_t*temp=(doc_t*)ep;
	fprintf(fp,"%i %i ",temp->id,temp->count);
}

int NormalizeWord(char **word)
{
		
	if(*word==NULL)	{
		free(*word);
		return(-1);
	}
	
	if ((strlen(*word))<3) {
		free(*word);
		return(-1);
	}
	
	unsigned char *p = (unsigned char *)*word;

  while (*p) {
     *p = tolower((unsigned char)*p);
      p++;
  }
	return(1);
}

bool searchfn(void* elementp,const void* keyp)
{
	word_t *compare=(word_t *)elementp;
	char *comp=compare->word;
	if (strcmp(comp,(char*)keyp)==0)
		return(true);
	else return(false);
}

bool docsearch(void* elementp,const void* keyp)
{
	doc_t *compare=(doc_t *)elementp;
	int comp=compare->id;
	if (comp==*((int*)keyp))
		return(true);
	else return(false);
}

void fn(void* ep)
{
	 char*temp=(char*)ep;
	fprintf(stdout,"word is %s\n",temp);
}

word_t *entry_new(char *word, const int id, const int count)
{
  if (word == NULL || count < 0) {
    return NULL;
  }
  word_t *page = checkp(malloc(sizeof(word_t)), "word_t");

	page->word = checkp(malloc(strlen(word)+1), "page->word");
	strcpy(page->word, word);
	page->Docs=qopen();
	doc_t*document=new_doc(id,count);
// 	pthread_mutex_t *m=*((pthread_mutex_t)page->Docs->m);
// 	pthread_mutex_lock(m);
// 	pthread_mutex_unlock(m);
	qput(page->Docs,document);		//seg fault
	return page;
}

void entry_delete(void *data)
{
  word_t *page = data;
  if (page != NULL) {
    if (page->word) free(page->word);
    qclose(page->Docs);
  }
}

doc_t *new_doc(const int id, const int count)
{
	doc_t *new = checkp(malloc(sizeof(doc_t)), "doc_t");
	new->id=id;
	new->count=count;
	return(new);
}

void doc_delete(void *data)
{
  doc_t *doc = data;
  if (doc != NULL) {
    free(doc);
  }
}

/**************** checkp ****************/
/* if pointer p is NULL, print error message and die,
 * otherwise, return p unchanged.
 */
static void *
checkp(void *p, char *message)
{
  if (p == NULL) {
    fprintf(stderr, "CHECKP: %s\n", message);
    exit (99);
  }
  return p;
}

void sumwords(void*ep)
{
	word_t* temp=(word_t*)ep;
	qapply(temp->Docs,sum);
}

void sum(void*ep)
{
	doc_t* temp=(doc_t*)ep;
	count+=temp->count;
}

char* itoa(int val, int base)
{
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
}