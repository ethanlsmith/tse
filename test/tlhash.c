#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "queue.h"
#include "hash.h"
#include "lhash.h"
#define MAXNM 20

typedef struct person {
	struct person *next;
	char name[MAXNM];
	int age;
	double sal;
} person_t;

typedef struct args {
	lhashtable_t *Index;
	char *pageDir;
} arg_t;

void fn(void *ptr) {
	person_t *pp=(person_t*)ptr;
	printf("name: %s\n",pp->name);
}

void *threadfn(void*arg)
{
	sleep(1);
	printf("printing for thread\n");
	return(NULL);
}

bool searchfn(void* elementp,const void* keyp) {
	person_t *pp=(person_t*)elementp;
	if (strcmp(pp->name,(char*)keyp)==0)
		return(true);
	else return(false);
}

person_t *make_person(char *namep,int age,double sal)  { 
	person_t *pp;
	
	if(!(pp = (person_t*)malloc(sizeof(person_t)))) {
		printf("[Error: malloc failed allocating person]\n");return NULL; 
	} 
	
	pp->next = NULL;
	strcpy(pp->name,namep); 
	pp->age=age; 
	pp->sal=sal; 
	return pp; 
}

int main(int argc, char *argv[]) {
	int choice=atoi(argv[1]);
	
	lhashtable_t *h1, *h2;
	pthread_t tid1,tid2;
	person_t got;
	person_t *gptr=&got;
	person_t *p1=make_person("Fred",25,100002);
	person_t *p2=make_person("George",26,150002);
	person_t *p3=make_person("Sarah",28,300000);
	person_t *p4=make_person("Anna",28,178000);
	
	bool res=true;
	switch(choice) {
	
	case 1:
		h1=lhopen(10);
		if(pthread_create(&tid1,NULL,threadfn,NULL)!=0)
		res=false;
		if(pthread_join(tid1,NULL)!=0)
		res=false;
		break;

	case 2:
		h1=lhopen(10);
		if(pthread_create(&tid1,NULL,threadfn,NULL)!=0)
		res=false;
		sleep(5);
		if(pthread_create(&tid2,NULL,threadfn,NULL)!=0)
		res=false;
		if(pthread_join(tid1,NULL)!=0)
		res=false;
		if(pthread_join(tid2,NULL)!=0)
		res=false;
		break;
		
	case 3:
		h1=lhopen(10);
		lhapply(h1,fn);
		lhclose(h1);
		res=true;
		break;
		
	case 4:
		h1=lhopen(10);
		if (lhput(h1,(void*)p1,"Fred",4)!=0)
			res=false;
		lhclose(h1);
		break;
		
	case 5:
		h1=lhopen(10);
		lhput(h1,(void*)p1,"Fred",4);
		if (lhput(h1,(void*)p2,"George",6)!=0)
			res=false;
		lhclose(h1);
		break;
		
	case 6:
		h1=lhopen(10);
		lhput(h1,(void*)p1,"Fred",4);
		gptr=(person_t*)lhsearch(h1,searchfn,(void*)"Fred",4);
		if (gptr->age!=p1->age)
			res=false;
		lhclose(h1);
		break;
		
	case 7:
		h1=lhopen(10);
		lhput(h1,(void*)p1,"Fred",4);
		lhput(h1,(void*)p2,"George",6);
		lhput(h1,(void*)p3,"Sarah",5);
		lhapply(h1,fn);
		res=true;
		lhclose(h1);
		break;
		
	case 8:
		h1=lhopen(10);
		lhput(h1,(void*)p1,"Fred",4);
		lhput(h1,(void*)p2,"George",6);
		lhput(h1,(void*)p3,"Sarah",5);
		gptr=lhsearch(h1,searchfn,(void*)"Fred",4);
		if (gptr==NULL)
			res=false;
		break;
		if (strcmp(gptr->name,"Fred")!=0)
			res=false;
		lhclose(h1);
		break;
		
	case 9:
		h1=lhopen(10);
		lhput(h1,(void*)p1,"Fred",4);
		lhput(h1,(void*)p2,"George",6);
		lhput(h1,(void*)p3,"Sarah",5);
		gptr=lhsearch(h1,searchfn,(void*)"George",6);
		if (strcmp(gptr->name,"George")!=0)
			res=false;
		lhclose(h1);
		break;
		
	case 10:
		h1=lhopen(10);
		lhput(h1,(void*)p1,"Fred",4);
		lhput(h1,(void*)p2,"George",6);
		lhput(h1,(void*)p3,"Sarah",5);
		gptr=lhsearch(h1,searchfn,(void*)"Sarah",5);
		if (strcmp(gptr->name,"Sarah")!=0)
			res=false;
		lhclose(h1);
		break;
	
	case 11:
		h1=lhopen(10);
		if (lhsearch(h1,searchfn,(void*)"ford",4)!=NULL)
			res=false;
		lhclose(h1);
		break;

	case 12:
		h1=lhopen(10);
		h2=lhopen(10);
		lhput(h1,(void*)p1,"Fred",4);
		lhput(h2,(void*)p2,"George",6);	//add other people to 2nd queue
		lhput(h2,(void*)p3,"Sarah",5);
		gptr=lhsearch(h2,searchfn,(void*)"Fred",4);
		if (gptr!=NULL)
			res=false;
		break;
		
	case 13:
		h1=lhopen(10);
		lhput(h1,(void*)p1,"Fred",4);
		lhput(h1,(void*)p2,"George",6);
		lhput(h1,(void*)p3,"Sarah",5);
		lhput(h1,(void*)p4,"Anna",5);
		gptr=lhsearch(h1,searchfn,(void*)"Sarah",5);
		if (strcmp(gptr->name,"Sarah")!=0)
			res=false;
		lhclose(h1);
		break;

	case 14:
		h1=lhopen(10);
		lhput(h1,(void*)p1,"Fred",4);
		lhput(h1,(void*)p2,"George",6);
		lhput(h1,(void*)p3,"Sarah",5);
		lhput(h1,(void*)p4,"Anna",5);
		gptr=lhsearch(h1,searchfn,(void*)"Anna",5);
		if (strcmp(gptr->name,"Anna")!=0)
			res=false;
		lhclose(h1);
		break;
		
	default:
		printf("invalid test\n");
		res=false;
	}

	free(p1);
	free(p2);
	free(p3);
	free(p4);
	
	if(!res)
		exit(EXIT_FAILURE);
	exit(EXIT_SUCCESS);
}
