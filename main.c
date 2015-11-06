/**
 * web crawler in C
 * Now I run out of time to dibug it.
 * A class that does a web pages crawler.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<time.h>
#include <pthread.h>
#include "util.h"

/**
 * a collection strut.
 */
typedef struct {
	char* subUrl;
	int count;
	int size;
	int pages_retrived;
	int state;
	int * average;
	char * url;
	char* collPerPage;
	int isTrue;
	int CountPerPage;
	pthread_mutex_t mutex;
	int  id;
} collection;

void MIMIType(collection * c);
void Collectcontent(collection * c);
void startMime(const collection* arg);
void startContent(const collection* arg);
int content_isEmpty(collection * c);
int count(collection * c);
void collection_destroy(collection * c);
void pause(int delay);

/**
 * returns how many url's per a page.
 */
int count(collection * c) {
	return c->count;
}

/**
 * check if the content is not empty.
 */
int content_isEmpty(collection * c) {
	return c->isTrue;
}

/**
 * verify if the type is HTML documents.
 */
void MIMIType(collection * c) {
	pthread_mutex_lock(&c->mutex);
	char buffer[80];
	int boll =0;
	if (c->url != NULL) {
		printf("I must be here to print this \n %s \n", c->url);
		boll = getMIMEType(c->url, buffer, 80);
		printf("\n have I ever made it here ??? \n %i", boll);
		if (boll == 1) {
			printf("\n am I here yet ... \n");
			c->pages_retrived = c->pages_retrived + 1;
			c->state = 1;
		}
	} else if (c->subUrl != NULL && c->url == NULL) {

		 int lines =0;int i=0;
		 char temp [50];
		 while(c->subUrl[i]!='\0'){
			 if(c->subUrl[i]!='\n') {
				 temp[i] = c->subUrl[i++];
			 }
			 else {
				 lines++;
				 i++;
				 boll = getMIMEType(temp, buffer, 80);
					if (boll == 1) {
						printf("\n am I here yet ... \n");
						c->pages_retrived = c->pages_retrived + 1;
						c->state = 1;
					}
			 }
		 }
	}
	pthread_mutex_unlock(&c->mutex);
}

/**
 * if the MIMI type is HTML get all contents
 * and extract to get the subURL.
 */
void Collectcontent(collection * c) {
	pthread_mutex_lock(&c->mutex);
	clock_t begin, end;
	begin = clock();
	double time_spent;
	if (c->state == 1) {
		char * content = getContent(c->url);
		printf("how about here have I ever made it here ...");
				  char* str = content;
				  char * pch;
				  pch = strtok (str," ,.-");
				  char ** str3 = (char *) malloc(1 + strlen(c)+ strlen(pch) );
				  while (pch != NULL)
				  {
				    printf ("%s\n",pch);
				    if (strstr(pch, "href=") != NULL) {
				    	int j = strlen(pch);
				    	printf("before :: %s \n", pch);
				        pch[j-3] = '\0';
				        printf("%s \n",pch);
				        memmove(pch, pch+6, strlen(pch));
				        strcpy(str3, c);
				        strcat(str3, pch);
				        c->subUrl = strcat(str3, pch);
				        c->CountPerPage = c->CountPerPage  + 1;
				        c->url = NULL;
				        printf("%s", str3);
				        printf("\n now the front: %s \n", pch);
				    }
				    pch = strtok (NULL, " ,.-");
				  }
				  int k;
				  int len = strlen(str3);
				  printf("length = %i", len);
				  if (str3 != NULL) {
					  for (k =0; k < 10; k++ ) {

					  }
					  printf("\n subString: %c \n", str3[0]);
				  } else {
					  printf("it is null :(");
				  }

				  free(str3);
				  	end = clock();
				  	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
				  	printf("Pages retrieved: %i", c->pages_retrived);
				  	printf("Average URLs per page", c->CountPerPage);
				  	printf("Elapsed: %f seconds\n", time_spent);
	}

			  	pthread_mutex_unlock(&c->mutex);
}

/**
 * a main method
 */
collection col;
int main(int argc, char* argv[]) {
	char buffer[80], * content;
	pthread_t mime[2];
	pthread_t cont[2];
	int i = 0;
	int j;
	int rv;
	setvbuf(stdout, NULL, _IONBF, 0);
	collection coll[2];
	for (j =0; j < 2; j++) {
	coll[j].url = "http://css.insttech.washington.edu/~mealden/";
	coll[j].subUrl = NULL;
	coll[j].count = 0;
	coll[j].size = 0;
	coll[j].pages_retrived = 0;
	coll[j].state = 0;
	coll[j].CountPerPage = 0;
	coll[j].isTrue = 0;
	pthread_mutex_init(&coll->mutex, NULL);
	}

	for (i = 0; i < 1; i++) {
		if (pthread_create(&mime[i], NULL, startMime, &coll[i]) != 0) {
			printf("ERROR: Unable to create new thread.\n");
			exit(EXIT_FAILURE);
		}
	}
	for (i = 0; i < 1; i++) {
		rv = pthread_create(&cont[i], NULL, startContent, (void *)(i + 1));
		if (rv != 0) {
			printf("ERROR: unable to create new thread.\n");
			return EXIT_FAILURE;
		}
	}
		for (i = 0; i < 1; i++)
			pthread_join(cont[i], NULL);
			printf("how about here ...");
		/* ...then waits for the consumers to empty the queue... */
		while (!content_isEmpty(&col) || count(&col) == 200);
		/* ...then kills all consumer threads. */
		for (i = 0; i < 1; i++)
			pthread_cancel(cont[i]);

	if (count(&col) == 200) {
		pthread_cancel(cont[i]);
	}
	collection_destroy(&col);
	return EXIT_SUCCESS;
}

/**
 * the MIME thread will start here.
 */
void startMime(const collection* arg) {
	MIMIType(arg);
	printf("is there another way calling this ...");
}

/**
 * The content thread starts here.
 */
void startContent(const collection* arg) {
	pause(2000000);
	Collectcontent(arg);
}

void collection_destroy(collection * c) {
	pthread_mutex_destroy(&c->mutex);
}
void pause(int delay) {
	int i;

	for (i = 0; i < delay; i++);
}
