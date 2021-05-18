#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>

sem_t readmutex, writemutex, wrt, r;
int readcount = 0;
int writecount = 0;
int resource = 1;

void *reader(void *id){
	// Entry section
	sem_wait(&r);
	sem_wait(&readmutex);
	readcount++;
	
	if(readcount==1){
		sem_wait(&wrt);
	}
	
	sem_post(&readmutex);
	sem_post(&r);
	
	// Critical Section i.e., perform reading
	printf("Reader%d accessed the resource %d\n", *((int *) id), resource);
	
	// Exit critical section
	sem_wait(&readmutex);
	readcount--;
	if(readcount==0){
		sem_post(&wrt);
	}
	sem_post(&readmutex);
	return id;
}

void *writer(void *id){
	// Entry section
	sem_wait(&writemutex);
	writecount++;
	
	if(readcount == 1){
        sem_wait(&r);
    }
    sem_post(&writemutex); 
    sem_wait(&wrt);

	// Critical Section i.e., perform writers
    resource++;
    printf("Writer%d modified resource to %d\n", *((int *) id), resource);
    sem_post(&wrt);

	// Exit critical section
    sem_wait(&writemutex);
    writecount--;
    
    if(readcount == 0){
        sem_post(&r);
    }
    sem_post(&writemutex);
	
	return id;
}

int main(){
	pthread_t readerArray[5], writerArray[5];
	sem_init(&readmutex, 0, 1);
	sem_init(&writemutex, 0, 1);
	sem_init(&wrt, 0, 1);
	sem_init(&r, 0, 1);
	
	for(int i=0;i<5;i++){
		pthread_create(&writerArray[i], NULL, writer, (void *)&i);
		pthread_create(&readerArray[i], NULL, reader, (void *)&i);
	}
	
	for(int i=0;i<5;i++){
		pthread_join(writerArray[i], NULL);
		pthread_join(readerArray[i], NULL);
	}
}
