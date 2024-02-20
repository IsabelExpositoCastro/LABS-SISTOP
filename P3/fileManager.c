
#include "fileManager.h"



void  initialiseFdProvider(FileManager * fm, int argc, char **argv) {
    // Complete the initialisation
    /* Your rest of the initailisation comes here*/
    fm->nFilesTotal = argc -1;
    fm->nFilesRemaining = fm->nFilesTotal;
    // Initialise enough memory to  store the arrays
    fm->fdData = malloc(sizeof(int) * fm->nFilesTotal);
    fm->fdCRC= malloc(sizeof(int) * fm->nFilesTotal);
    fm->fileFinished = malloc(sizeof(int) * fm->nFilesTotal);
    fm->fileAvailable = malloc(sizeof(int) * fm->nFilesTotal);

    int i;
    for (i = 1; i < fm->nFilesTotal +1; ++i) {
        char path[100];
        strcpy(path, argv[i]);
        strcat(path, ".crc");
        fm->fdData[i] = open(argv[i], O_RDONLY);
        fm->fdCRC[i] = open(path, O_RDONLY);

        fm->fileFinished[i] = 0;
        fm->fileAvailable[i] = 1;
    }
    // initialize the mutex lock
    pthread_mutex_init(&lock, NULL);
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
    //destroy the mutex lock
    pthread_mutex_destroy(&lock);
}
int getAndReserveFile(FileManager *fm, dataEntry *d) {
    pthread_mutex_lock(&lock);
    int i;
    for (i = 0; i < fm->nFilesTotal; ++i) {
        if (fm->fileAvailable[i] && !fm->fileFinished[i]) {
            d->fdcrc = fm->fdCRC[i];
            d->fddata = fm->fdData[i];
            d->index = i;

            //mark that the file is not available
            fm->fileAvailable[i] = 0;

            pthread_mutex_unlock(&lock);
            return 0;
        }
    }
    pthread_mutex_unlock(&lock);
    return 1;
}
void unreserveFile(FileManager *fm, dataEntry *d) {
    pthread_mutex_lock(&lock);
    fm->fileAvailable[d->index] = 1;
    pthread_mutex_unlock(&lock);
}

void markFileAsFinished(FileManager *fm, dataEntry *d) {
    pthread_mutex_lock(&lock);

    fm->fileFinished[d->index] = 1;
    fm->nFilesRemaining--;

    if (fm->nFilesRemaining == 0) {
        printf("All files have been processed\n");
        // unblock all waiting threads?
    }

    pthread_mutex_unlock(&lock);
}