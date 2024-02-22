#include "crc.h"
#include "fileManager.h"
#include "myutils.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for sleep function : waits for seconds

#define N 4 // number of threads that we are going to create


FileManager fm;

void* worker_function(void * arg){
    // initialize the semaphore within the worker function
    my_semaphore *semaphore = (my_semaphore *)arg;


    while (1){
        dataEntry  d;
        char *buff[256];
        short int crc;
        getAndReserveFile(&fm, &d); // Reserves a file. The release is missing. Where should you put it?: WE SHOULD PUT IT IN THE END AFTER THE IF
        read(d.fdcrc, &crc, sizeof(short int));
        int nBytesReadData = read(d.fddata, buff, 256);

        if (crc != crcSlow(buff, nBytesReadData)) {
            printf("CRC error in file %d\n", d.filename);
        }

        unreserveFile(&fm, &d); //here we could have done:  markFileAsFinished(&fm, &d)
        // optional sleep for a while before processing the next file
        sleep(1);

        my_sem_wait(semaphore);
    }
}

int main(int argc, char ** argv) {
    initialiseFdProvider(&fm, argc, argv);

    my_semaphore semaphore;
    my_sem_init(&semaphore, 0);
    //With this adjustment, each worker thread will initialize its own semaphore,
    //wait on it before processing the next file, and then continue its execution.
    //This ensures that each worker thread waits independently on the semaphore without interfering with other threads.



    pthread_t threadID[N];      //as we can see this N refers to number of threads, which has to be defined before
    for (int i = 0; i < N; ++i) {

        pthread_create(&threadID[i], NULL, worker_function, &semaphore);
    }

    for (int i = 0; i < N; ++i) {
        pthread_join(threadID[i], NULL);
    }
    destroyFdProvider(&fm);
}