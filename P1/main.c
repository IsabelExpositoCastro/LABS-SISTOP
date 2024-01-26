//
// Created by expos on 24/01/2024.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "crc.c"
#include "main.h"

//Calculate CRC
//use unsigned short for 2 bytes instead of 1
unsigned short calculateCRC(char *filename){ //pass filename as argument
    int fd= open(filename, O_RDONLY);   //we open the file allowing only to read it
    if(fd==-1){
        perror("Error when the file is opened");
        exit(5);
    }


    unsigned short crc= INITIAL_REMINDER;
    char buff;

    while(read(fd, &buff,2)>0){
        crc=crcSlow((unsigned char*)&buff,2);
    }

    close(fd);
    return crc;

}


// Function to generate CRC file
void generateCRCFile(char *inputFilename) {

    //open input file



    // Create CRC filename by appending ".crc" to the input filename
    char crcFilename[256];
    strcpy(crcFilename, inputFilename);
    strcat(crcFilename, ".crc");

    // Generate CRC value for the input file
    unsigned short crc = calculateCRC(inputFilename);

    // Open or create the CRC file
    int crcFile = open(crcFilename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (crcFile == -1) {
        perror("Error creating CRC file");
        exit(5);
    }

    // Write the calculated CRC to the CRC file
    write(crcFile, &crc, sizeof(crc));

    // Close the CRC file
    close(crcFile);

    printf("CRC file generated: %s\n", crcFilename);
}






// Function to verify CRC file
void verifyCRCFile(char *filename, int maxNumErrors) {
    char crcFilename[256];
    strcpy(crcFilename, filename);
    strcat(crcFilename, ".crc");

    // Check if the CRC file exists
    int crcFileExists = access(crcFilename, F_OK) != -1;

    if (!crcFileExists) {
        fprintf(stderr, "Error: CRC file not found.\n");
        exit(22);
    }

    int crcFile = open(crcFilename, O_RDONLY);
    if (crcFile == -1) {
        perror("Error opening CRC file");
        exit(5);
    }

    // Read the stored CRC from the CRC file
    unsigned short storedCRC;
    read(crcFile, &storedCRC, sizeof(storedCRC));

    // Calculate the CRC of the original file
    unsigned short calculatedCRC = calculateCRC(filename);

    // Compare the stored CRC with the calculated CRC
    if (storedCRC == calculatedCRC) {
        printf("File OK\n");
    } else {
        printf("File has errors\n");

        // Check if the number of errors is below the threshold
        if (maxNumErrors > 0 && (calculatedCRC != storedCRC)) {
            printf("Number of errors below threshold: %d\n", maxNumErrors);
        }
    }

    // Close the CRC file
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


