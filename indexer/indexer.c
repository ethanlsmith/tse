/* indexer.c --- 
 * 
 * 
 * Author: Ethan L. Smith
 * Created: Sun Oct 28 19:57:05 2018 (-0400)
 * Version: 
 * 
 * Description: indexes words in a given webpage_t
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>
#include <unistd.h>
#include "webpage.h"
#include "hash.h"
#include "queue.h"
#include "pageio.h"

int count=0;
FILE*fp=NULL;

typedef struct word {
  char *word;                               // word
  queue_t *Docs;                            // que of documents
} word_t;

typedef struct doc {
  int id;									//document id
  int count;								//occurrences of words in that doc
} doc_t;

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
void indexsave(hashtable_t*Index,char*indexnm);
void indexload(hashtable_t*Index,char*pageDir);

int main(const int argc, char *argv[])
{
	// check for exactly three parameters
	if (argc != 3) {
		fprintf(stderr, "usage: exactly 1 arguments (id)\n");
		return 1;
	}

	// create variables of arguments
	char *indexnm = argv[2];
	char *pageDir = argv[1];
	hashtable_t *Index=hopen(10000);
	
	indexload(Index,pageDir);	
	indexsave(Index,indexnm);
	
	happly(Index,entry_delete);
	hclose(Index);
	return(0);
	
}

void indexload(hashtable_t*Index,char*pageDir)
{
	int pos=0;
	int docid;
	int maxpos=0;
	char *dirtemp = malloc(sizeof(pageDir)*strlen(pageDir));
	strcpy(dirtemp,pageDir);
	
	char *word="hellotherehowareyou";
	FILE*fp1=NULL;
	
	for(int c=0;(fp1=fopen(strcat(dirtemp,itoa(c+1,10)),"r"));c++) {
		
		strcpy(dirtemp,pageDir);
		
		webpage_t *page=pageload(c+1,pageDir);
		docid=c+1;
		printf("the page number is %d\n",c+1);
		pos=1;
		maxpos=0;
		
		printf("url is %s\n",webpage_getURL(page));
		maxpos=webpage_getHTMLlen(page);
		
		while((pos=webpage_getNextWord(page, pos, &word))>0 && (pos<maxpos) )  {
			if(NormalizeWord(&word)>0) {
				//check for the word in the Index
				word_t *temp=(word_t*)hsearch(Index,searchfn,word,strlen(word));
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
					word_t *wordstruct=entry_new(word,docid,1);
					hput(Index,(void*)wordstruct,word,strlen(word));
				}
				free(word);
			}
		}
		//deallocate and find the number of words in the Index
		webpage_delete(page);
		fclose(fp1);
	}
	free(dirtemp);
}

void indexsave(hashtable_t*Index,char*indexnm)
{
	fp=fopen(indexnm,"w");
	happly(Index,printindexword);
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
  qput(page->Docs,document);
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