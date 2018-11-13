#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include "lqueue.h"
#include "queue.h"
#define MAXNM 20

typedef struct person {
	struct person *next;
	char name[MAXNM];
	int age;
	double sal;
} person_t;

typedef struct car {
	struct car *next;
	char name[MAXNM];
	double price;
	int year;
	} car_t;

void fn(void *ptr) {
	car_t *cp=(car_t*)ptr;
	printf("name: %s\n",cp->name);
}

bool searchfn(void* elementp,const void* keyp) {
	car_t *cp=(car_t*)elementp;
	if (strcmp(cp->name,(char*)keyp)==0)
		return(true);
	else return(false);
}

bool searchfnp(void* elementp,const void* keyp) {  
	printf("searching...\n");                                                                               
	person_t *cp=(person_t*)elementp;                                                                                                   
	if (strcmp(cp->name,(char*)keyp)==0)                                                                                                
    return(true);                                                                                                                     
	else return(false);                                                                                                                 
}

car_t *make_car(char *namep,double price,int year)  { 
	car_t *cp;
	
	if(!(cp = (car_t*)malloc(sizeof(car_t)))) {
		printf("[Error: malloc failed allocating person]\n");return NULL; 
	} 
	
	cp->next = NULL;
	strcpy(cp->name,namep); 
	cp->price=price; 
	cp->year=year; 
	return cp; 
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

	pthread_t tid1,tid2;
	lqueue_t *lq1;
	car_t got;
	car_t *gptr=&got;
	car_t *sptr=make_car("subie",2500,2000);
	car_t *mptr=make_car("mustang",2400,2000);
	car_t *vptr=make_car("volvo",2400,2000);
	person_t *p1=make_person("Fred",25,100002);
	person_t *p2=make_person("George",26,150002);
	person_t *p3=make_person("Sarah",28,300000);
	person_t *p4=make_person("Anna",28,300000);
	
	bool res=true;
	switch(choice) {
	case 1:
		lq1=lqopen();
		if(pthread_create(&tid1,NULL,lqget,lq1)!=0)
		res=false;
		if(pthread_join(tid1,NULL)!=0)
		res=false;
		lqclose(lq1);
		break;

	case 2:
		lq1=lqopen();
		if(pthread_create(&tid1,NULL,lqget,lq1)!=0)
		res=false;
		if(pthread_create(&tid2,NULL,lqget,lq1)!=0)
		res=false;
		
		if(pthread_join(tid1,NULL)!=0)
		res=false;
		if(pthread_join(tid2,NULL)!=0)
		res=false;
		lqclose(lq1);
		break;
		
	case 3:
		lq1=lqopen();
		lqapply(lq1,fn);
		lqclose(lq1);
		res=true;
		break;
		
	case 4:
		lq1=lqopen();
		if (lqput(lq1,(void*)p1)!=0)
			res=false;
		lqclose(lq1);
		break;
		
	case 5:
		lq1=lqopen();
		lqput(lq1,(void*)p1);
		if (lqput(lq1,(void*)p2)!=0)
			res=false;
		lqclose(lq1);
		break;
		
	case 6:
		lq1=lqopen();
		lqput(lq1,(void*)sptr);
		gptr=(car_t*)lqget(lq1);
		if (gptr->price!=sptr->price)
			res=false;
		lqclose(lq1);
		break;
		
	case 7:
		lq1=lqopen();
		lqput(lq1,(void*)sptr);
		lqput(lq1,(void*)mptr);
		lqput(lq1,(void*)vptr);
		lqapply(lq1,fn);
		res=true;
		lqclose(lq1);
		break;
		
	case 8:
		printf("case 8\n");
		lq1=lqopen();
		lqput(lq1,(void*)sptr);
		lqput(lq1,(void*)mptr);
		lqput(lq1,(void*)vptr);
		gptr=lqsearch(lq1,searchfn,(void*)"subie");
		if (gptr==NULL) {
			printf("doesnt exist\n");
			res=false;
			break;
		}
		if (strcmp(gptr->name,"subie")!=0) {
			printf("didnt find it\n");
			res=false;
			break;
		}
		printf("search worked\n");
		lqclose(lq1);
		break;
		
	case 9:
		lq1=lqopen();
		lqput(lq1,(void*)sptr);
		lqput(lq1,(void*)mptr);
		lqput(lq1,(void*)vptr);
		gptr=lqsearch(lq1,searchfn,(void*)"mustang");
		if (strcmp(gptr->name,"mustang")!=0)
			res=false;
		lqclose(lq1);
		break;
		
	case 10:
		lq1=lqopen();
		lqput(lq1,(void*)sptr);
		lqput(lq1,(void*)mptr);
		lqput(lq1,(void*)vptr);
		gptr=lqsearch(lq1,searchfn,(void*)"volvo");
		if (strcmp(gptr->name,"volvo")!=0)
			res=false;
		lqclose(lq1);
		break;
	
	case 11:
		lq1=lqopen();
		if (lqsearch(lq1,searchfn,(void*)"ford")!=NULL)
			res=false;
		lqclose(lq1);
		break;
								
	default:
		printf("invalid test\n");
		res=false;
	}

	free(sptr);
	free(mptr);
	free(vptr);
	free(p1);
	free(p2);
	free(p3);
	free(p4);
	
	if(!res)
		exit(EXIT_FAILURE);
	exit(EXIT_SUCCESS);
}
