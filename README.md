# **Readers-Writers Problem**
This problem arises when many concurrent threads are trying to access the same resource at one time. Consider a resource which is shared with many processes. One process may try to read the resource i.e., reader and one process may try to write/ change the resource i.e., writer. 

At a time, only one process may modify the resource during which no other process is allowed to modify or read. It is because when processes try to access the resource while it is being modified, the readers will not be able to view changes and the writers may corrupt the resource. However when one process is reading the resource, other processes may read the resource but not modify it.

In simpler words, we want to prevent more than one process/ thread to modify the shared resource simultaneously and allow more than one reader to access and read the shared resource. 

This readers-writers problem can be categorised into 3 types:
* First readers-writers problem
* Second readers-writers problem
* Third readers-writers problem
___

To run the **C** codes use the following command
```
g++ filename.c -lpthread -o outputfilename && ./outputfilename && outputfilename
```
___
## Contents
1. [First Readers-Writers Problem](#first)
2. [Second Readers-Writers Problem](#second)
3. [Third Readers-Writers Problem](#third)
___

## <a name="first"></a> **First readers-writers problem (Readers-Preference, Writers Starve)**
1. As simultaneous reading is safe, we can allow multiple readers to read at once however following mutual exclusion among writers. Hence the first readers-writers problem follows “no reader shall be kept waiting when the resource is being read by other processes” and thus **readers' preference**.
1. As the preference is given to the readers overs writers, the latter will have to wait until there is no reader to modify the resource. Hence it leads to **writer starvation**.

1. Shared Data : File, Database etc
1. Semaphore :
    * mutex: Mutual exclusion semaphore for updating readcount variable i.e., when any reader enters or exits from the critical section.
    * wrt: mutual exclusion semaphore for writers used when first or last reader enters or exits and is common to both readers and writers.
1. Readcount variable of int type which stores the number of readers performing the read operation in the critical section.
1. Initially mutex = 1, wrt = 1, readcount = 0.

    ### Variables Initialization
    ```c
        int readcount = 0;
        // semaphores intitialized to 1
        semaphore mutex;
        semaphore wrt;
    ```
1. Writers Process
    * Initially the writer requests for entry to the critical section i.e., checking if there is any reader present in the critical section.
    * If there is no reader in the critical section, wait() returns a true value and the writer is granted access; else the writer continues to wait.
    * Writer exits the critical section.

    ```c
        writer( ){
	    // Request for critical section
	        wait(wrt); 
        // Lock the shared resource for this writer thus not allowing other writers to modify the resource and readers from accessing.

        // Critical Section i.e., perform writing

        // Exit critical section
            signal(wrt); 
        // Release the shared resource for other processes (both readers and writers).
        }
    ```

1. Readers Process
    * Initially the reader requests for entry to the critical section i.e., checking if there is any writer present in the critical section.
    * If granted access, as there can only one process be in the entry section it locks the entry section for itself until it is done with entry section (it is because, for example if there are 2 readers in the entry section, both increase the readcount simultaneously and lock the resource causing the readers to block). It then increments the number of readers in the critical section. If it is the first reader then it blocks the resource for the writers. After they are done, the entry section is released allowing other readers to perform reading. 
    * It then performs reading. And then exits the critical section.
    * While exiting the critical section, it checks if there are any readers present in the critical section. If there are no readers present then it releases the resource allowing writers to enter the critical section and modify the resource.
    * If access is not granted, it keeps on waiting.

    ```c
    reader( ){
        // Request for critical section
        wait(mutex); // Lock the entry section for this reader thus not allowing other readers. 
        readcount++; // Increment the number of readers.
        // If it is the first reader, it blocks the critical section for writers
        if(readcount == 1){
            wait(wrt);
        }
        signal(mutex); // Release the entry section for the readers

        // Critical Section i.e., perform reading

        // Exit critical section
        wait(mutex); // Lock the exit section for this reader, not allowing other readers. 
        readcount--; // Decrement the number of readers.
            // If there is no reader, it releases the critical section for writers.
        if(readcount == 0){
            signal(wrt);
        }
        signal(mutex); // Release the exit section for the readers
    }
    ```
1. We observe that the resource gets locked by the first reader and only the last reader can unlock/release the resource for writers, this can lead to writer starvation.

---

## <a name="second"></a> **Second readers-writers problem (Writers-Preference, Readers Starve)**
1. To avoid the writers starving as in the above case, the second readers-writers problem follows “no writer, once added to the queue, shall be kept waiting longer than absolutely necessary” and thus **writers preference**.
1. Here the task is accomplished by making all the readers to lock and release the semaphore “r” individually. But the writer's part is similar to the readers part as in the above case, the first writer locks the semaphore “r” and the next writers perform their writing and the last writer releases the semaphore “r” thus allowing readers to access the resource.
1. Shared Data : File, Database etc
1. Semaphore : 
    * readmutex, writemutex: Mutual exclusion semaphore for updating readcount and writercount respectively ans controlling critical section.
    * r: mutual exclusion semaphores for readers.
    * wrt: mutual exclusion semaphore accessible by both readers and writers to control resource access.
1. readcount, writecount variable of int type which stores the number of readers performing the read operation and the number of writers in the critical section respectively.
1. Initially readmutex = 1, r = 1, writemutex = 1, wrt = 1, readcount = 0, writecount = 0.
    ### Variables Initialization
    ```c
        int readcount = 0;
        int writecount = 0;
        // semaphores intitialized to 1
        semaphore writemutex;
        semaphore readmutex;
        semaphore r;
        semaphore wrt;
    ```
1. Readers Process
    * Initially the reader tries to enter the entry section.
    * However the reader can’t enter the entry section if the semaphore “r” is set by the writer. Hence it waits until the last writer releases it.
    * If the reader sets the semaphore “r” it means that there is a reader in the critical section thus not allowing the writer to enter.
    ```c
    reader ( ){
        // Entry section
        wait(r); // Reader trying to enter indication
        wait(readmutex); // Lock the entry section for this reader thus not allowing other readers. 
        readcount++; // Increment the number of readers.
        // If it is the first reader, it blocks the critical section for writers
        if(readcount == 1){
            wait(wrt);
        }
        signal(readmutex); // Release the entry section for the readers
        signal(r); // Indication that the reader is done trying to access

        // Critical Section i.e., perform reading

        // Exit critical section
        wait(readmutex); // Lock the exit section for this reader thus not allowing other readers. 
        readcount--; // Decrement the number of readers.
        // If there is no reader, it releases the critical section for writers.
        if(readcount == 0){
            signal(wrt);
        }
        signal(readmutex); // Release the exit section for the readers
    }
    ```
1. Writers Process
    * Initially the writer requests for entry to the critical section i.e., checking if there is any reader present in the critical section.
    * The writer waits for the reader to release the semaphore “r” and then it locks the resource for itself, not allowing other readers and writers. Only after the resource is released by the reader, the writer will be able to modify it, which the reader does after it completes its reading operation.
    * Writer exits the critical section.

    ```c
    writer( ){
        // Entry section
        wait(writemutex); // Lock the entry section for this writer thus not allowing other writers. 
        writecount++; // Increment the number of writers.
        // If it is the first reader, it blocks the critical section for readers
        if(readcount == 1){
            wait(r);
        }
        signal(writemutex); // Release the entry section for the writers
        wait(wrt); // blocks the critical section for writers

        // Critical Section i.e., perform writing
        signal(wrt);

        // Exit critical section
        wait(writemutex); // Lock the exit section for this writer thus not allowing other readers. 
        writecount--; // Decrement the number of writers.
        // If there is no writer, it releases the critical section for readers.
        if(readcount == 0){
            signal(r);
        }
        signal(writemutex); // Release the exit section for the writers.
    }
    ```
1. Here, if there is no writer, it is indicated with the status of the semaphore “r” and then the readers will not lock the resource thus allowing the writers to access it as soon as they enter the queue and the reader completes its operation. Orelse the writer will have to wait until there are no readers left which causes writers starvation.
1. So as soon as the writer enters the queue, it sets the status of the semaphore “r” and patiently waits for the reader to complete its operation and then takes control of the resource. All the next readers queue up waiting for all the writers to complete their modification. This may cause **readers to starve**.

## <a name="third"></a> **Third readers-writers problem (Starve free)**
1. Both the above 2 methods lead to starvation of either readers or writers and so this implementation is designed to follow “neither reader nor writer will be allowed to starve i.e., no thread shall be allowed to starve”. 
1. Shared Data : File, Database etc
1. Semaphore : 
    * mutex: mutual exclusion semaphore for updating readcount variable i.e., when any reader enters or exits from the critical section.
    * wrt: mutual exclusion semaphore to control resource access, common to both readers and writers.
    * queue: to store the order of the read/ write requests.
    * readcount is a variable of int type which stores the number of readers performing the read operation in the critical section.
1. Initially mutex = 1, readcount = 0, wrt= 1, queue = 1.
1. Readers Process

    ```c
    reader( ){
        // Entry section
        wait(queue); // Wait in the queue
        wait(mutex); // Lock the entry section for this reader thus not allowing other readers. 
        readcount++; // Increment the number of readers.
        // If it is the first reader, it blocks the critical section for writers
        if(readcount == 1){
            wait(wrt);
        }
        signal(queue); // release queue allowing next to get added
        signal(mutex); // Release the entry section for the readers

        // Critical Section i.e., perform reading

        // Exit critical section
        wait(mutex); // Lock the exit section for this reader thus not allowing other readers. 
        readcount--; // Decrement the number of readers.
        // If there is no reader, it releases the critical section for writers.
        if(readcount == 0){
            signal(wrt);
        }
        signal(mutex); // Release the exit section for the readers
    }
    ```
1. Writers Process
    ```c
    writer( ){
    // Request for critical section
        wait(queue); // Wait in the queue
        wait(wrt); // Lock the shared resource for this writer thus not allowing other writers to modify the resource and readers from accessing.
        signal(queue); // release queue allowing next to get added

    // Critical Section i.e., perform writing

    // Exit critical section
        signal(wrt); // Release the shared resource for other processes (both readers and writers).
    }
    ```

1. This satisfies **starve-free** as the queue follows FIFO order during blocking and releasing of threads, as no process coming later is given preference over the already present threads in the queue.

