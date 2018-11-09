/*Ethan Smith Nick Blasey
 * pageio.c - ability to save a webpage to a file, and
 * load a webpage from a file to a webpage_t
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <unistd.h>
#include <webpage.h>
#include <pageio.h>
#define NCHAR 1024

/**************** pagesave ****************/
/* saves page into given directory under filename
 * given by filenum in the format url, depth, HTMLlen and
 * HTML each on their own lines
 */
 int32_t pagesave(webpage_t* page, int filenum, char *pageDir)
{
	// create file path
	char *buffer = (char *) malloc(sizeof(pageDir)*10);
	char num[5];
	strcpy(buffer, pageDir);
	sprintf(num, "%d", filenum);
	strcat(buffer, num);

	// open file and print URL, depth, HTMLlen and HTML
	FILE *fp = fopen(buffer, "w"); 
	fprintf(fp, "%s\n%i\n%i\n%s", webpage_getURL(page), webpage_getDepth(page), webpage_getHTMLlen(page), webpage_getHTML(page));
	
	// clean up 
	fprintf(stdout, "WEBPAGE SAVED IN: %s\n", buffer);
	free(buffer);
	if(fclose(fp))
		return(1);
	return(0);
}

 
/**************** pageload ****************/
/* converts a file of a saved page to a webpage_t
 * puts the url, depth, htmllen, and html into the
 * struct
 */
webpage_t *pageload(int filenum, char *pageDir)
{
	char url[100];
	int depth;
	int html_size;
	
	// create file path
	char *buffer = (char *) malloc(sizeof(pageDir)*10);
	char num[5];
	strcpy(buffer, pageDir);
	sprintf(num, "%d", filenum);
	strcat(buffer, num);

	// open file and scan URL, depth, and HTML
	FILE *fp = fopen(buffer, "r");
	fscanf(fp,"%s%d%d\n",url,&depth,&html_size);
// 	printf("html size is %d\n",html_size);
    char *new_html;
     if((new_html= (char*) malloc (html_size*2))==NULL) {
     	printf("error on malloc\n");
     	exit(EXIT_FAILURE);
     }
    char *p=new_html;
	int i;
	
	for(i=0;i<html_size;i++,p++) {
		*p=fgetc(fp);
	}
		*p='\0';
	
	//make the webpage
	webpage_t *new=webpage_new(url,depth,new_html);
	// clean up 
	free(buffer);
	if(fclose(fp))
		return(NULL);
	return(new);

}