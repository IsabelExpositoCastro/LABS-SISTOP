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
    crc* crc = malloc(sizeof(crc));

    while (fm.nFilesRemaining>0){
        unsigned char buff[256];
        dataEntry  d;
        d.fddata = -1;
        d.fdcrc = -1;
        int res = getAndReserveFile(&fm, &d); // Reserves a file (it will later be released)

        if (d.fddata != -1 && d.fdcrc != -1) {  // firstly we should check if file is successfully reserved
            read(d.fdcrc, crc, sizeof(*crc));
            int nBytesReadData = read(d.fddata, buff, 256);


            if (*crc != crcSlow(buff, nBytesReadData)) {
                printf("CRC error in file %d\n", d.index);
            }

            unreserveFile(&fm, &d);     // it is important to unreserve the file and it's done here!
            markFileAsFinished(&fm, &d);

            // we have decided to add a sloop before processing the next file
            sleep(1);
        }
        if(res==1 && fm.nFilesRemaining==0){
            free(crc);
            pthread_exit(NULL);
        }
    }
    free(crc);
    return NULL;
}

int main(int argc, char ** argv) {
    // start a timer
    startTimer(0);
    //long timeTotal = 0;

    // initialize FileManager
    initialiseFdProvider(&fm, argc, argv);
    // initialize the semaphore
    my_semaphore semaphore;
    my_sem_init(&semaphore, 0);     // each worker thread will initialize and release its own semaphore.

    // create an array to store the threads IDs
    pthread_t threadID[N];      //this N refers to number of threads, which has to be defined before
    for (int i = 0; i < N; ++i) {
        pthread_create(&threadID[i], NULL, worker_function, (void*)&semaphore);
    }
    // wait for worker threads to finish
    for (int i = 0; i < N; ++i) {
        pthread_join(threadID[i], NULL);
    }

    // finally destroy the fileManager resources
    destroyFdProvider(&fm);

    long timeTotal = endTimer(0);
    printf("total time taken: %ld \n", timeTotal);
    exit(0);
}