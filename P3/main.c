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
    while (1){
        dataEntry  d;
        char * buff[256];
        short int crc;
        int res = getAndReserveFile(&fm, &d); // Reserves a file. The release is missing. Where should you put it?: WE SHOULD PUT IT IN THE END AFTER THE IF
        read(d.fdcrc, &crc, sizeof(short int));
        int nBytesReadData = read(d.fddata, buff, 256);

        if (crc != crcSlow(buff, nBytesReadData)) {
            printf("CRC error in file %d\n", d.filename);
        }

        unreserveFile(&fm, &d); //here we could have done:  markFileAsFinished(&fm, &d)
        // optional sleep for a while before processing the next file
        sleep(1);
    }
}

int main(int argc, char ** argv) {
    initialiseFdProvider(&fm, argc, argv);
    pthread_t threadID[N];      //as we can see this N refers to number of threads, which has to be defined before
    for (int i = 0; i < N; ++i) {
        pthread_t thread;
        pthread_create(&threadID[i], NULL, worker_function, NULL);
    }

    for (int i = 0; i < N; ++i) {
        pthread_join(threadID[i], NULL);
    }
    destroyFdProvider(&fm);
}