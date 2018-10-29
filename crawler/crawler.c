/*Ethan Smith Nick Blasey
 * crawler.c - parses the initial seed webpage, extracts 
 * any embedded URLs, and saves the webpage's HTML into a 
 * given directory, then recursively retrieves, scans, and 
 * saves found webpages until the max depth is reached
 * 
 * usage: ./crawler seedURL pageDirectory maxDepth
 * 		  must have exactly three arguments, a char* 
 *		  seed URL to start the crawl from, a char* path
 *		  to a directory to save the page files in, and 
 *		  an int maxDepth to recursively crawl pages to
 * set args https://thayer.github.io/engs50/ ../pages/ 1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "webpage.h"
#include "hash.h"
#include "queue.h"
#include "pageio.h"

/* saves page into given directory under filename
 * given by filenum in the format url, depth, HTMLlen and
 * HTML each on their own lines
 */
//int32_t pagesave(webpage_t* page, int filenum, char *pageDir);

/* makes webpage structs from the given file, assuming
 * it is in the format "URL\n depth\n HTML\n". Assigns 
 * this struct to the given pointer. Page must later
 * be freed by caller
 */



/**************** global variables ****************/
char *checked = "checked"; // dummy item for hashtable of URLs

/**************** local function prototypes ****************/
int checkarguments(char *seedURL, char *pageDir, int maxDepth);
void crawler(queue_t *pagesToCrawl, int maxDepth, char *pageDir, hashtable_t *pagesSeen);
void pagescanner(webpage_t* page, hashtable_t *pagesSeen, int depth, queue_t *pagesToCrawl);
bool searchfn(void* elementp,const void* keyp);
	
/**************** main ***************
 * calls functions to check number and validity
 * of arguments, initialize data structures, start 
 * program's functionality, and clean up by freeing memory
 */
int main(const int argc, char *argv[])
{
	// check for exactly three parameters
	if (argc != 4) {
		fprintf(stderr, "usage: exactly 3 arguments (seedURL, pageDir, maxDepth)\n");
		return 1;
	}

	// create variables of arguments
	char *seedURL = argv[1];
	char *pageDir = argv[2];
	int maxDepth = atoi(argv[3]);

	// run function to check validity of arguments
	if (checkarguments(seedURL, pageDir, maxDepth) != 0) {
		return 2;
	}

	// initialize data structures
	queue_t *pagesToCrawl = qopen(); // a Queue that holds pages to crawl
	hashtable_t *pagesSeen = hopen(10); // holds seen URLs
	
	
	// create webpage for seed URL and insert it into data structs
	webpage_t *seed = webpage_new(seedURL, 0, NULL);
	
	qput(pagesToCrawl, seed);

	char*np=(char*) malloc(sizeof(seedURL)*strlen(seedURL));                                                                           
  	strcpy(np,seedURL);
	hput(pagesSeen,np,np,strlen(np));

	// crawl from seed URL
	fprintf(stdout, "STARTING CRAWL...\n");
	fprintf(stdout, "----------------\n");
	crawler(pagesToCrawl, maxDepth, pageDir, pagesSeen);
	fprintf(stdout, "...CRAWL ENDED\n");

	// clean up
	qclose(pagesToCrawl);
	hclose(pagesSeen);
	return 0;
}


/**************** checkarguments ***************
 * checks that URL is internal and can be normalized, 
 * pageDir is writable, and maxDepth is non-negative
 */
int checkarguments(char *seedURL, char *pageDir, int maxDepth) 
{
	// check validity of depth parameter
	if (maxDepth < 0) {
		fprintf(stderr, "maxDepth must be non-negative\n");
		return 2;
	}

	// check validity of seedURL parameter
	if (! IsInternalURL(seedURL)) {
		fprintf(stderr, "seedURL must be internal\n");
		return 2;
	} else if (! NormalizeURL(seedURL)) {
		fprintf(stderr, "%s cannot be normalized\n", seedURL);
		return 2;
	}

	// check validity of pageDir parameter by inserting a test file
	char *dirTemp = (char *) malloc(sizeof(pageDir)*5);
	strcpy(dirTemp, pageDir);
	strcat(dirTemp, ".crawler");
	FILE *fp;
	if ((fp = fopen(dirTemp, "w")) == NULL) {
		fprintf(stderr, "pageDir must be a writable directory\n");
		free(dirTemp); // clean up
		return 2;
	} else {
		fclose(fp);
	}

	free(dirTemp); // clean up
	return 0;
}

/**************** crawler ***************
 * while there is a page left in the bag, pull out a page
 * fetch the HTML, and if the page is below the max depth,
 * crawl the page for embedded URLs
 */
void crawler(queue_t *pagesToCrawl, int maxDepth, char *pageDir, hashtable_t *pagesSeen)
{
	int i = 1; // filename for saving page information
	webpage_t *page;

	// for all pages in the queue...
	while ((page = qget(pagesToCrawl)) != NULL) {
		fprintf(stdout, "pulling %s from queue\n", webpage_getURL(page));

		// fetch and save page 
		if (webpage_fetch(page)) {  
			pagesave(page, i, pageDir); 
			i++;
		} else {
			fprintf(stderr, "failed to fetch page, url: %s\n", webpage_getURL(page));
		}

		// if below max depth, scan for deeper URLs
		if ((webpage_getDepth(page))<maxDepth) {
			fprintf(stdout, "scanning %s...\n", webpage_getURL(page));
			pagescanner(page, pagesSeen, webpage_getDepth(page), pagesToCrawl); 
			//webpage_delete(page);
		} else {
		 fprintf(stdout, "%s at max depth\n", webpage_getURL(page));
		}

		// clean up
		webpage_delete(page);
	}

	fprintf(stdout, "----------------\n");
	fprintf(stdout, "%d PAGES WERE SAVED\n", i-1);
}

/**************** pagescanner ***************
 * use webpage.h functions to scan HTML
 * of pages for embedded URLs, check if they are 
 * internal and can be normalized, and insert page 
 * into queue and hashtable if it hasn't already been 
 * seen
 */
void pagescanner(webpage_t* page, hashtable_t *pagesSeen, int depth, queue_t *pagesToCrawl)
{
 	int pos = 0; // store position of HTML scan
 	char *result;
 	
 	// while loop modified from engs50's webpage.h
 	while ((pos = webpage_getNextURL(page, pos, &result)) > 0) {
 		printf("Found url: %s\n", result);

 		// normalize url
 		if (! NormalizeURL(result)) {
			 fprintf(stderr, "%s cannot be normalized\n", result);
		} 

		// check if internal, add to hashtable and create an object
		if (! IsInternalURL(result)) {
			 fprintf(stderr, "%s is not internal\n", result);
		} else {
			// check if URL has been seen
			//fprintf(stdout,"result is %s\n",result);
			if (hsearch(pagesSeen,searchfn,result,strlen(result))==NULL) {
				//			if (hput(pagesSeen,checked,result,strlen(result))==0) {
				webpage_t *new = webpage_new(result, depth+1, NULL);
				qput(pagesToCrawl, new);
				char*np=(char*) malloc(sizeof(result)*strlen(result));
				strcpy(np,webpage_getURL(new));
				//char*np=webpage_getURL(new);
				hput(pagesSeen,(void*)np,np,strlen(webpage_getURL(new)));
				 fprintf(stdout, "%s inserted successfully\n\n", result);
			} else {
				 fprintf(stderr, "%s already seen\n\n", result);
			}
		}

		// clean up
 		free(result);
 	}
}

// int32_t pagesave(webpage_t* page, int filenum, char *pageDir)
// {
// 	// create file path
// 	char *buffer = (char *) malloc(sizeof(pageDir)*10);
// 	char num[5];
// 	strcpy(buffer, pageDir);
// 	sprintf(num, "%d", filenum);
// 	strcat(buffer, num);
// 
// 	// open file and print URL, depth, and HTML
// 	FILE *fp = fopen(buffer, "w"); 
// 	fprintf(fp, "%s\n%i\n%i\n%s", webpage_getURL(page), webpage_getDepth(page), webpage_getHTMLlen(page), webpage_getHTML(page));
// 	
// 	// clean up 
// 	fprintf(stdout, "WEBPAGE SAVED IN: %s\n", buffer);
// 	free(buffer);
// 	if(fclose(fp))
// 		return(1);
// 	return(0);
// }

bool searchfn(void* elementp,const void* keyp)
{
	//fprintf(stdout, "we in search function\n");
	char *comp=(char*)elementp;
	//	char *comp="url";
	//fprintf(stdout,"comp is %s\n",comp);
	//fprintf(stdout,"keyp is %s\n",(char*)keyp);
	//fprintf(stdout,"comparing %s to %s\n",comp,(char*)keyp);
	if (strcmp(comp,(char*)keyp)==0)
		return(true);
	else return(false);
}
