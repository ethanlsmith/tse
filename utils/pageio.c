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
// 	fprintf(stdout,"buffer is %s\n\n",buffer);

	// open file and scan URL, depth, and HTML
	FILE *fp = fopen(buffer, "r");
	fscanf(fp,"%s %i %i",url,&depth,&html_size);
	printf("html size is %i\n",html_size);
// 	char new_html[1000*];
	size_t n = 0, nchar = NCHAR;
    char *new_html = malloc (sizeof *new_html * nchar);
// 	char *new_html=malloc(sizeof(char)*sizeof(html_size)*6);
	
// 	fprintf(stdout,"depth is %i, html size is %i\n",depth,html_size);
	
	char c;
	
	while ((c = fgetc (fp)) != EOF) {  /* for each char in file */
        new_html[n++] = c;       /* assign char to array */

        if (n == nchar) {   /* realloc preserving space for nul */
            void *tmp = realloc (new_html, nchar + NCHAR);
            if (!tmp) {           /* validate realloc succeeded */
//                 fprintf (stderr, "realloc() error: memory exhausted.\n");
                break; /* break read loop, using existing 'arr' */
            }
            new_html = tmp;     /* assign reallocated pointer to arr */
            nchar += NCHAR;        /* update the value of nchar */
        }
    }

// 	fprintf(stdout,"the html is %s\n",new_html);
	
	//make the webpage
	webpage_t *new=webpage_new(url,depth,new_html);
		
	// clean up 
// 	fprintf(stdout, "WEBPAGE LOADED FROM URL: %s\n", url);
	free(buffer);
	if(fclose(fp))
		return(NULL);
	return(new);

}