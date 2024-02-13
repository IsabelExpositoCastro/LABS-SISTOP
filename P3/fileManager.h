#pragma once
#include <pthread.h>
#include <fcntl.h>
#include <stdlib.h>

typedef struct {
    int fdcrc;
    int fddata;
    int index;
    char * filename;
} dataEntry;


pthread_mutex_t lock;

typedef struct {
    int * fdData;
    int * fdCRC;
    int * fileFinished;
    int * fileAvailable;

    int nFilesRemaining;
    int nFilesTotal;
} FileManager;

void initialiseFdProvider(FileManager * fm, int argc, char **argv);
void destroyFdProvider(FileManager * fm);
int getAndReserveFile(FileManager *fm, dataEntry * d);
void unreserveFile(FileManager *fm,dataEntry * d);
void markFileAsFinished(FileManager * fm, dataEntry * d);
