#include "fileManager.h"
#include "myutils.h"
#include <pthread.h>
// initialize the mutex and semaphore
static pthread_mutex_t lock_initialized = PTHREAD_MUTEX_INITIALIZER;
static my_semaphore semaphore;

// !!  If this mutex is meant to protect access to shared resources across multiple functions or threads,
// then it makes sense. If it's specific to file management, you might want to encapsulate it in a local scope
// or use a different mutex specific to file operations. ?????????? check after


void  initialiseFdProvider(FileManager * fm, int argc, char **argv) {
    // Complete the initialisation
    my_sem_init(&semaphore, 0);

    // lock the mutex
    pthread_mutex_lock(&lock_initialized);

    // unlock the mutex after initialization is done
    pthread_mutex_unlock(&lock_initialized);

    // Your rest of the initialisation comes here(statement)
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
        fm->fdData[i] = open(argv[i+1], O_RDONLY); //here we change the index initialization to 0 and so argv[i+1] instead of just argv[i]
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
    //lock first
    pthread_mutex_lock(&lock_initialized);

    int i;
    for (i = 0; i < fm->nFilesTotal; ++i) {
        if (fm->fileAvailable[i] && !fm->fileFinished[i]) {
            d->fdcrc = fm->fdCRC[i];
            d->fddata = fm->fdData[i];
            d->index = i;

            //mark the non-available file
            fm->fileAvailable[i]=0;

            //unlock before returning
            pthread_mutex_unlock(&lock_initialized);

            return 0;
        }
    }

    //in case of not enter inside the if, also we have to unlock
    pthread_mutex_unlock(&lock_initialized);
    return 1;
}


void unreserveFile(FileManager *fm,dataEntry * d) {
    fm->fileAvailable[d->index] = 1;
}


// we can use semaphores for synchronization between threads
void markFileAsFinished(FileManager * fm, dataEntry * d) {
    fm->fileFinished[d->index] = 1;
    fm->nFilesRemaining--; //mark that a file has finished

    if (fm->nFilesRemaining == 0) {
        printf("All files have been processed\n");
        //TO COMPLETE: unblock all waiting threads, if needed
        my_sem_signal(&semaphore);
    }
}
