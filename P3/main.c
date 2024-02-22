#include "crc.h"
#include "fileManager.h"
#include "myutils.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for sleep function : waits for seconds

FileManager fm;

void* worker_function(void * arg){
    while (1){
        dataEntry  d;
        char * buff[256];
        short int crc;
        int res = getAndReserveFile(&fm, &d); // Reserves a file. The release is missing. Where should you put it?
        read(d.fdcrc, &crc, sizeof(short int));
        int nBytesReadData = read(d.fddata, buff, 256);

        if (crc != crcSlow(buff, nBytesReadData)) {
            printf("CRC error in file %d\n", d.filename);
        }
    }
}

int main(int argc, char ** argv) {
    initialiseFdProvider(&fm, argc, argv);
    pthread_t threadID[N];
    for (int i = 0; i < N; ++i) {
        pthread_t thread;
        pthread_create(&threadID[i], NULL, worker_function, NULL);
    }

    for (int i = 0; i < N; ++i) {
        pthread_join(threadID[i], NULL);
    }
    destroyFdProvider(&fm);
}