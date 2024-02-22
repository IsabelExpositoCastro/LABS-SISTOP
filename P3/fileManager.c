#include "fileManager.h"

void  initialiseFdProvider(FileManager * fm, int argc, char **argv) {
    // Complete the initialisation

    // initialize the mutex
    static pthread_mutex_t lock_initialized = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&lock_initialized, NULL);


    // lock the mutex
    pthread_mutex_lock(&lock_initialized);

    // check if the mutex has ben initialized, no necessary but better
    if (!lock_initialized) {
        pthread_mutex_init(&lock, NULL);
        lock_initialized = 1;
    }

    // unlock the mutex after initialization is done
    pthread_mutex_unlock(&lock_initialized);


    /* Your rest of the initailisation comes here*/



    fm->nFilesTotal = argc -1;
    fm->nFilesRemaining = fm->nFilesTotal;
    // Initialise enough memory to  store the arrays
    fm->fdData = malloc(sizeof(int) * fm->nFilesTotal);
    fm->fdCRC= malloc(sizeof(int) * fm->nFilesTotal);
    fm->fileFinished = malloc(sizeof(int) * fm->nFilesTotal);
    fm->fileAvailable = malloc(sizeof(int) * fm->nFilesTotal);

    int i;
    for (i = 0; i < fm->nFilesTotal ; ++i) {
        char path[100];
        strcpy(path, argv[i+1]);
        strcat(path, ".crc");
        fm->fdData[i] = open(argv[i+1], O_RDONLY); //here we changes the index initialization to 0 and so argv[i+1] instead of just argv[i]
        fm->fdCRC[i] = open(path, O_RDONLY);

        fm->fileFinished[i] = 0;
        fm->fileAvailable[i] = 1;
    }
}
void  destroyFdProvider(FileManager * fm) {
    int i;
    for (i = 0; i < fm->nFilesTotal; i++) {
        close(fm->fdData[i]);
        close(fm->fdCRC[i]);
    }
    free(fm->fdData);
    free(fm->fdCRC);
    free(fm->fileFinished);
}
int getAndReserveFile(FileManager *fm, dataEntry * d) {
    // This function needs to be implemented by the students
    int i;
    for (i = 0; i < fm->nFilesTotal; ++i) {
        if (fm->fileAvailable[i] && !fm->fileFinished[i]) {
            d->fdcrc = fm->fdCRC[i];
            d->fddata = fm->fdData[i];
            d->index = i;

            // You should mark that the file is not available
            ??

            return 0;
        }
    }
    return 1;
}
void unreserveFile(FileManager *fm,dataEntry * d) {
    fm->fileAvailable[d->index] = 1;
}

void markFileAsFinished(FileManager * fm, dataEntry * d) {
    fm->fileFinished[d->index] = 1;
    fm->nFilesRemaining--; //mark that a file has finished
    if (fm->nFilesRemaining == 0) {
        printf("All files have been processed\n");
        //TO COMPLETE: unblock all waiting threads, if needed
    }
}
