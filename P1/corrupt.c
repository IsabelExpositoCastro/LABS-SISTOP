#include <stdio.h>     // printf, sprintf, scanf, sscanf 
#include <stdlib.h>    // exit (process ending and clean up)
#include <unistd.h>    // Unix-like Syscalls:  _exit (low level), read, write,
#include <fcntl.h>     // Unix-like Syscalls: open, close
#include <string.h>    // strlen (length of a string array), strcpy
#include <time.h>
#include <sys/stat.h>

int getFileSize(char * filename){
    struct stat st;
    stat(filename, &st);
    return st.st_size;
}

int elemInVector(int i, int * vector, int size){
    int j;
    for (j = 0; j < size; j++){
        if (vector[j] == i){
            return 1;
        }
    }
    return 0;
}

// Usage corrupt file.in -o file.out -numCorruptions N
int main(int argc, char* argv[]) {
    int nRands = 5;
    char * filenameOut = NULL;
    int i;
    for (i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-numCorruptions") == 0 ){
            i += 1;
            nRands = atoi(argv[i]);
            printf("%d\n", nRands);
        }
        else if(strcmp(argv[i], "-o") == 0){
            i += 1;
            filenameOut = argv[i];
        }
        else{
            exit(22);
        }
    }

    // If the file has not been found
    if (filenameOut == NULL){
        exit(22);
    }

    srand(time(NULL));   // Initialization, should only be called once.
    int fd = open(argv[1], O_RDONLY);
    int fdOut = open(filenameOut, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd == -1 || fdOut == -1){
        exit(5);
    }
    int fileSize = getFileSize(argv[1]);

    int * rands = malloc(sizeof(int) * nRands);
    for (i = 0; i < nRands; i++){
        rands[i] = rand() % fileSize;
    }

    char buffer[1];
    int byteCount = 0;
    while ( read(fd, buffer, 1) > 0){
        if (elemInVector(byteCount,  rands, nRands)){
            buffer[0] = rand()%256; // Change the value to random
        }
        write(fdOut, buffer, 1);
        byteCount += 1;
    }

    close(fd);
    close(fdOut);
    free(rands);
    printf("Corruption done\n");
    return 0;
}
