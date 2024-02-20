#include "crc.h"
#include "fileManager.h"
#include "myutils.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for sleep function : waits for seconds

#define N 4     //n of worker threads

FileManager fm;

void *worker_function(void *arg) {
    while (1) {
        dataEntry d;
        char buff[256];
        short int crc;
        int res = getAndReserveFile(&fm, &d);

        if (res == 0) {
            // read CRC from the file
            read(d.fdcrc, &crc, sizeof(short int));

            // read data from the file
            int nBytesReadData = read(d.fddata, buff, 256);

            // perform CRC verification
            if (crc != crcSlow((const unsigned char*)buff, nBytesReadData)) {
                printf("CRC error in file %d\n", d.index);
            }

            // unreserve the file
            unreserveFile(&fm, &d);
        } else {
            // if no file available, break the loop
            break;
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    initialiseFdProvider(&fm, argc, argv);
    pthread_t threadID[N];
    // create worker threads
    for (int i = 0; i < N; ++i) {
        pthread_create(&threadID[i], NULL, worker_function, NULL);
    }
    // wait for all worker threads to finish
    for (int i = 0; i < N; ++i) {
        pthread_join(threadID[i], NULL);
    }
    // destroy FileManager resources
    destroyFdProvider(&fm);
    return 0;
}