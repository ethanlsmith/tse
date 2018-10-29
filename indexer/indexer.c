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
#include "webpage.h"
#include "hash.h"
#include "queue.h"
#include "pageio.h"

int NormalizeWord(char **word);

int main(void) {

// 	printf("HELLO!\n");
	char *word="hello";
	int pos=1;

	webpage_t *page=pageload(1,"../pages/");
	
	printf("url is %s\n",webpage_getURL(page));
// 	printf("depth is %i\n",webpage_getDepth(page));
// 	printf("htmllen is %i\n",webpage_getHTMLlen(page));
// 	printf("html is %s\n",webpage_getHTML(page));

	while((pos=webpage_getNextWord(page, pos, &word))>0) {
		if(NormalizeWord(&word)>0) {
		printf("%s\n",word);
		free(word);
		}
	}

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