#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include <inttypes.h>
#include <webpage.h>
#include <queue.h>
#include <hash.h>


int main(void) {

	bool res=false;
	int pos;
	char *url;
	webpage_t* page = webpage_new("https://thayer.github.io/engs50/", 0, NULL);
	//webpage_t* sp;
	webpage_t* temp;
	queue_t *q1;
	q1=qopen();
	
	if(webpage_fetch(page)) {
		
		while((pos = webpage_getNextURL(page, pos, &url)) > 0) {
			if(IsInternalURL(url)) {
				temp = webpage_new(url, 0, NULL);
				qput(q1,(void*)temp);
				//free(url);
				free(webpage_getHTML(temp));
				//webpage_delete((void*)temp);
			}
			free(url);
		}
// 		for (int i=0;i=2;i++) {
		while((temp=(webpage_t*)qget(q1))!=NULL) {
			temp=(webpage_t*)qget(q1);
			printf("%s\n",webpage_getURL(temp));
			webpage_delete((void*)temp);
		}
		qclose(q1);
		//free(temp);
		//free(url);
		//webpage_delete((void*)temp);
		res=true;
		free(webpage_getHTML(page));
	}
	else res=false;
	webpage_delete((void*)page);
	if(res==true) exit(EXIT_SUCCESS);
	else exit(EXIT_FAILURE);
}
