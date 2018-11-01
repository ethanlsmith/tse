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
  int count;                               // count of words in document 1
} word_t;

int NormalizeWord(char **word);
bool searchfn(void* elementp,const void* keyp);
void fn(void* ep);
word_t *entry_new(char *word, const int count);
void entry_delete(void *data);
static void *checkp(void *p, char *message);
void sumwords(void*ep);

int main(void) {

// 	printf("HELLO!\n");
	char *word="hellotherehowareyou";
	int pos=1;
	
	webpage_t *page=pageload(1,"../pages/");
	hashtable_t *Index=hopen(10);
	
// 	FILE *fp2=fopen("output","w");

	
// 	printf("url is %s\n",webpage_getURL(page));

	while((pos=webpage_getNextWord(page, pos, &word))>0) {
		if(NormalizeWord(&word)>0) {
// 			fprintf(stdout,"the word is '%s'\n",word);
//			happly(Index,fn);
 			word_t *temp=(word_t*)hsearch(Index,searchfn,word,strlen(word));
//			char *temp=(char*)hsearch(Index,searchfn,word,strlen(word));
			if (temp!=NULL) {
				fprintf(stdout,"we already found %s\n",temp->word);
// 				fprintf(stdout,"we already found %s\n",temp);
 				int i=temp->count;
				temp->count=i+1;
				fprintf(stdout,"count is now %i\n",temp->count);
			}
			else {
// 				word_t *wordstruct=malloc(sizeof(*wordstruct));
// // 				strcpy(wordstruct->wrd,word);
// 				wordstruct->count=1;
// // 				hput(Index,(void*)wordstruct,word,strlen(word));
// //  			fprintf(stdout,"we put in '%s'\n",wordstruct->wrd);
// 				char*np=(char*) malloc(sizeof(word)*strlen(word));
// 				strcpy(np,word);
// 				strcpy(wordstruct->wrd,np);
// 				temp->count=1;
				word_t *wordstruct=entry_new(word,1);
				hput(Index,(void*)wordstruct,word,strlen(word));
// 				hput(Index,(void*)np,np,strlen(np));
	  			fprintf(stdout,"we put in '%s'\n",word);
// 				fprintf(stdout,"the count is %i\n",wordstruct->count);
			}
// 		fprintf(fp2,"%s\n",word);
		free(word);
		}
	}

// 	fclose(fp2);
	happly(Index,sumwords);
	printf("the number of words is %i\n",count);
	happly(Index,entry_delete);
	hclose(Index);
	webpage_delete(page);
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
	//fprintf(stdout, "we in search function\n");
	word_t *compare=(word_t *)elementp;
	char *comp=compare->word;
// 	char *comp=(char *)elementp;
	//char *comp="course";
	//fprintf(stdout,"comp is %s\n",comp);
	//fprintf(stdout,"keyp is %s\n",(char*)keyp);
// 	fprintf(stdout,"comparing %s to search key: %s\n",comp,(char*)keyp);
	if (strcmp(comp,(char*)keyp)==0)
		return(true);
	else return(false);
}

void fn(void* ep)
{
// 	word_t*temp=(word_t*)ep;
// 	fprintf(stdout,"word is %s, count is %i\n",temp->wrd,temp->count);
	 char*temp=(char*)ep;
	fprintf(stdout,"word is %s\n",temp);
}

word_t *entry_new(char *word, const int count)
{
  if (word == NULL || count < 0) {
    return NULL;
  }
  word_t *page = checkp(malloc(sizeof(word_t)), "word_t");

  page->word = checkp(malloc(strlen(word)+1), "page->word");
  strcpy(page->word, word);
  page->count = count;
  return page;
}

void entry_delete(void *data)
{
  word_t *page = data;
  if (page != NULL) {
    if (page->word) free(page->word);
    free(page);
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
	count+=temp->count;
// 	count++;
}