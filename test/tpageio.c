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

FILE*fp=NULL;

int main(int argc,char *argv[]) {

	int n=atoi(argv[1]);
	webpage_t *page=pageload(n,"../pages/");
	pagesave(page,1000,"../pages/");
	return(0);
}
	