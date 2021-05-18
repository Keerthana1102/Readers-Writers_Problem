#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>

sem_t mutex, wrt, queue;
int readcount = 0;
int resource = 1;

void *reader(void *id){
	// Entry section
	sem_wait(&queue); // Wait in the queue
	sem_wait(&mutex); // Lock the entry section for this reader thus not allowing other readers.
	readcount++;// Increment the number of readers.
	
    // If it is the first reader, it blocks the critical section for writers
	if(readcount==1){
		sem_wait(&wrt);
	}
	sem_post(&queue); // release queue allowing next to get added
	sem_post(&mutex); // Release the entry section for the readers
	
	// Critical Section i.e., perform reading
	printf("Reader%d accessed the resource %d\n", *((int *) id), resource);
	
	// Exit critical section
	sem_wait(&mutex);// Lock the exit section for this reader thus not allowing other readers.
	readcount--;
	// If there is no reader, it releases the critical section for writers.
	if(readcount==0){
		sem_post(&wrt);
	}
	sem_post(&mutex); // Release the exit section for the readers
	return id;
}

void *writer(void *id){
	// Request for critical section
	sem_wait(&queue);
	sem_wait(&wrt);
	sem_post(&queue);
	
	// Critical Section i.e., perform writing
	resource++;
	
	// Exit critical section
	printf("Writer%d modified resource to %d\n", *((int *) id), resource);
	sem_post(&wrt);
	return id;
}

int main(){
	pthread_t readerArray[5], writerArray[5];
	sem_init(&mutex, 0, 1);
	sem_init(&wrt, 0, 1);
	sem_init(&queue, 0, 1);
	
	for(int i=0;i<5;i++){
		pthread_create(&writerArray[i], NULL, writer, (void *)&i);
		pthread_create(&readerArray[i], NULL, reader, (void *)&i);
	}
	
	for(int i=0;i<5;i++){
		pthread_join(writerArray[i], NULL);
		pthread_join(readerArray[i], NULL);
	}
}
