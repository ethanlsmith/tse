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

int main(const int argc, char *argv[])
{
	// check for exactly three parameters
	if (argc != 2) {
		fprintf(stderr, "usage: exactly 1 arguments (id)\n");
		return 1;
	}

	// create variables of arguments
	int MaxDocId = atoi(argv[1]);
// 	char *pageDir = argv[2];
	
	char *word="hellotherehowareyou";
	hashtable_t *Index=hopen(10);

	for(int c=0;c<MaxDocId;c++) {
	
		webpage_t *page=pageload(c+1,"../pages/");
		int docid=c+1;
		int pos=1;
		
// 		printf("value of c is %i\n",c);
		printf("url is %s\n",webpage_getURL(page));

		while((pos=webpage_getNextWord(page, pos, &word))>0) {
			if(NormalizeWord(&word)>0) {
// 				printf("position is %d\n",pos);
				fprintf(stdout,"the word is '%s'\n",word);
	//			happly(Index,fn);
				word_t *temp=(word_t*)hsearch(Index,searchfn,word,strlen(word));
				if (temp!=NULL) {
					fprintf(stdout,"we already found '%s'\n",temp->word);
					doc_t*moddoc=qsearch(temp->Docs,docsearch,(void*)&docid);
					if (moddoc!=NULL) {
						moddoc->count+=1;
						fprintf(stdout,"count is now %i\n",moddoc->count);
					}
					else {
						doc_t *newdoc=new_doc(docid,1);
						qput(temp->Docs,(void*)newdoc);
					}
				}
				else {
					word_t *wordstruct=entry_new(word,docid,1);
					hput(Index,(void*)wordstruct,word,strlen(word));
	// 	  			fprintf(stdout,"we put in '%s'\n",word);
				}
				free(word);
			}
		}
		webpage_delete(page);
		happly(Index,sumwords);
		printf("the number of words is %i\n",count);
	}
// 	happly(Index,sumwords);
// 	printf("the number of words is %i\n",count);
// 	happly(Index,entry_delete);
	hclose(Index);
	return(0);
	
}

int NormalizeWord(char **word)
{
	
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
// 	fprintf(stdout,"comparing %s to search key: %s\n",comp,(char*)keyp);
	if (strcmp(comp,(char*)keyp)==0)
		return(true);
	else return(false);
}

bool docsearch(void* elementp,const void* keyp)
{
	//fprintf(stdout, "we in search function\n");
	doc_t *compare=(doc_t *)elementp;
	int comp=compare->id;
// 	fprintf(stdout,"comparing %s to search key: %s\n",comp,(char*)keyp);
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
    free(page);
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
// 	count++;
}

void sum(void*ep)
{
	doc_t* temp=(doc_t*)ep;
	count+=temp->count;
}