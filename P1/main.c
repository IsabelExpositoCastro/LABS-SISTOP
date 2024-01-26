

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "crc.c"
#include "main.h"


//Calculate CRC
void generate(char *inputFilename){

    // Create CRC filename by appending ".crc" to the input filename
    char crcFilename[256];
    strcpy(crcFilename, inputFilename);
    strcat(crcFilename, ".crc");

    // Open input file for reading
    int inputFile = open(inputFilename, O_RDONLY);
    if (inputFile == -1) {
        printf("Couldn't open input file!");
        exit(5);
    }

    // Open or create CRC file for writing with the name created before
    int crcFile = open(crcFilename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (crcFile == -1) {
        printf("Couldn't open CRC file!");
        exit(5);
    }


    // Initialize the CRC to 0
    crc result=0;
    // Buffer to read data in chunks
    char buff[256];
    int bytesRead = 0;

    // Read input file in blocks of 256 bytes and compute CRC
    while ((bytesRead = read(inputFile, buff, sizeof(buff))) > 0) {
        result = crcSlow((unsigned char*)buff, bytesRead);

        // Write CRC to CRC file
        write(crcFile, &result, sizeof(result));
    }

    // Close files
    close(inputFile);
    close(crcFile);

}


void verify(char *inputFilename, int maxNumErrors) {

    // create the name for the output file once again
    char crcFilename[256];
    strcpy(crcFilename, inputFilename);
    strcat(crcFilename, ".crc");

    // opening the input file passed as argument for reading
    int inputFile = open(inputFilename, O_RDONLY);
    if (inputFile == -1) {
        printf("Couldn't open input file!");
        exit(5);
    }

    // we assume that the crcFile is already created from the generate function
    int crcFile = open(crcFilename, O_RDONLY);
    if (crcFile == -1) {
        printf("Couldn't open CRC file!");
        close(inputFile);
        exit(5);
    }

    // Initialize the CRC to 0
    crc result = 0;
    // Buffer to read data in chunks
    char buff[256];
    int bytesRead = 0;
    int numErrors = 0;

    // Read input file in blocks of 256 bytes and compare CRC
    while ((bytesRead = read(inputFile, buff, sizeof(buff))) > 0) {
        // Compute CRC for the current block
        result = crcSlow((unsigned char*)buff, bytesRead);

        // Read CRC from CRC file
        crc crcFromFile;
        if (read(crcFile, &crcFromFile, sizeof(crc)) != sizeof(crc)) {
            printf("Error reading CRC from file!");
            close(inputFile);
            close(crcFile);
            exit(5);
        }

        // Compare CRC values
        if (result != crcFromFile) {
            numErrors++;
            // Check if there are errors and print the result
            if (numErrors > maxNumErrors) {
                printf("file has errors\n");
                close(inputFile);
                close(crcFile);
                exit(0);
            } else {
                printf("file OK\n");
            }
        }
    }

    // Close files
    close(inputFile);
    close(crcFile);
}



int main(int argc, char* argv[]) {
   // ... (unchanged)


   // New command-line options
   int generate = 0;
   int verify = 0;
   int maxNumErrors = 0;


   for (int i = 2; i < argc; i++) {
       if (strcmp(argv[i], "-generate") == 0) {
           generate = 1;
       } else if (strcmp(argv[i], "-verify") == 0) {
           verify = 1;
       } else if (strcmp(argv[i], "-maxNumErrors") == 0) {
           i += 1;
           maxNumErrors = atoi(argv[i]);
       }
       // ... (Previous options remain unchanged)
   }


   // Call the appropriate function based on command-line arguments
   if (generate) {
       generateCRCFile(argv[1]);
   }


   if (verify) {
       verifyCRCFile(argv[1], maxNumErrors);
   }


   // ... (Rest of the code remains unchanged)


   return 0;
}

