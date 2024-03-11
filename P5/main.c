#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>   // for usleep waits for microsec
#include <unistd.h> // for sleep: waits for seconds
#include "crc.h"
#include "fileLock.h"
#define N 4

typedef struct {
    int nBlock;
    int isGet;
} Request;

typedef struct {
    int nBlock;
    short int crc;
} Result;


int main(int argc, char * argv[]) {

    int pipeA[2], pipeB[2];

    // Create two pipes. Also remember to close the channels when needed, otherwise it will not work!
    if (pipe(pipeA) == -1 || pipe(pipeB) == -1) {
        printf("Error creating pipes");
        exit(5);
    }

    for (int i = 0; i < N; ++i) {
        int n = fork();
        if (n == 0) {
            close(pipeA[1]);  //close unused write end of A
            close(pipeB[0]);  //close unused read end of B

            int fd = open(argv[1], O_RDONLY);
            if (fd == -1) {
                printf("Error opening data file");
                exit(5);
            }
            int fdCRC =open(argv[2], O_RDWR);
            if (fdCRC == -1) {
                printf("Error opening CRC file");
                exit(5);
            }
            Request r;
            while(read(pipeA[0], &r, sizeof(r)) > 0) {      // read from pipe A which is still open

                if (!r.isGet) {
                    // we acquire a lock and then update the CRC file
                    unsigned char buff[256];
                    file_lock_read(fd, r.nBlock * sizeof(buff), sizeof(buff));
                    lseek(fd, r.nBlock * sizeof(buff), SEEK_SET);
                    read(fd, buff, sizeof(buff));
                    file_unlock(fd, r.nBlock * sizeof(buff), sizeof(buff));

                    // Calculate CRC using crcSlow function
                    unsigned char crc = crcSlow(buff, sizeof(buff));

                    // Update CRC file with the new CRC value
                    file_lock_write(fdCRC, r.nBlock * sizeof(crc), sizeof(crc));
                    lseek(fdCRC, r.nBlock * sizeof(crc), SEEK_SET);
                    write(fdCRC, &crc, sizeof(crc));
                    file_unlock(fdCRC, r.nBlock * sizeof(crc), sizeof(crc));
                    /* Recompute the CRC, use lseek to get the correct datablock,
                    and store it in the correct position of the CRC file. Remember to use approppriate locks! */

                    usleep(rand()%1000 *1000); // Make the computation a bit slower


                }
                else{
                    usleep(rand()%1000 *1000);
                    Result res;
                    res.nBlock = r.nBlock;
                    // Read the CRC from the CRC file, using lseek + read. Remember to use the correct locks!
                    file_lock_read(fdCRC, r.nBlock * sizeof(res.crc), sizeof(res.crc));
                    lseek(fdCRC, r.nBlock * sizeof(res.crc), SEEK_SET);
                    read(fdCRC, &res.crc, sizeof(res.crc));
                    file_unlock(fdCRC, r.nBlock * sizeof(res.crc), sizeof(res.crc));

                    // Write the result in pipeB!
                    write(pipeB[1], &res, sizeof(res));

                }
            }
            close(pipeA[0]); //close the remaining parts of the pipe that were opened
            close(pipeB[1]);

            exit(0);
        }
    }
    char s[100];
    int nBytesRead;
    /* Read until the standard output*/
    while((nBytesRead = read(0, s, 100) ) > 0) {
        char op[200];
        s[nBytesRead] = '\0';
        int nBlock;

        sscanf(s, "%s %d", op, &nBlock);
        Request r;
        r.nBlock = nBlock;
        r.isGet = strcmp(op, "get") == 0;
        // Write r in the pipe!
        write(pipeA[1], &r, sizeof(r));
    }
    close(pipeA[1]);
    printf("FINISHED\n");

    while(wait(NULL) == -1);

    // Now that is finished, write all the results
    Result res;
    while((nBytesRead = read(pipeB[0], &res, sizeof(res)) ) > 0) {
        printf("The CRC of block #%d is %d \n", res.nBlock, res.crc);
    }
    close(pipeB[0]);
    return 0;
}