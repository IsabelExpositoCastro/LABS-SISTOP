

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "crc.h"
#include <time.h>



//Calculate CRC
void generate(char *inputFilename){

    //Start timer
    clock_t start_time = clock();


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
    int bytesRead ;

    // Read input file in blocks of 256 bytes and compute CRC
    while ((bytesRead = read(inputFile, buff, sizeof(buff))) > 0) {
        result = crcSlow((unsigned char*)buff, bytesRead);

        // Write CRC to CRC file
        write(crcFile, &result, sizeof(result));
    }

    // Close files
    close(inputFile);
    close(crcFile);

    //End timer
    clock_t end_time = clock();  // Record end time
    double elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Time taken: %f seconds\n", elapsed_time);

}


void verify(char *inputFilename, int maxNumErrors) {

    //Start timer
    clock_t start_time = clock();

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
    crc result ;
    // Buffer to read data in chunks
    char buff[256];
    int bytesRead ;
    int numErrors = 0;

    // Read input file in blocks of 256 bytes and compare CRC
    while ((bytesRead = read(inputFile, buff, sizeof(buff))) > 0) {

        // Introduce errors using corrupt.c before computing CRC
        system("./corrupt input.txt -o corrupted_input.txt -numCorruptions 5");

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
            }
        }
    }

    // Check if there were any errors and print the final result
    if (numErrors > 0) {
        printf("file has errors\n");
    } else {
        printf("file OK\n");
    }

    // Close files
    close(inputFile);
    close(crcFile);

    //End timer
    clock_t end_time = clock();  // Record end time
    double elapsed_time = ((double) (end_time - start_time)) / CLOCKS_PER_SEC;
    printf("Time taken: %f seconds\n", elapsed_time);
}


int main(int argc, char *argv[]) {

    // Check if there are enough arguments
    if (argc < 3) {
        printf("Incorrect number of arguments.\n");
        exit(22);
    }

    // Parse command-line arguments
    char *inputFilename = NULL;
    int generateOption = 0;
    int verifyOption = 0;
    int maxNumErrors = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-generate") == 0) {
            generateOption = 1;
        } else if (strcmp(argv[i], "-verify") == 0) {
            verifyOption = 1;
        } else if (strcmp(argv[i], "-maxNumErrors") == 0) {
            if (i + 1 < argc) {
                maxNumErrors = atoi(argv[i + 1]);
                i++;  // Move to the next argument
            } else {
                printf("Invalid use of -maxNumErrors option.\n");
                exit(22);
            }
        } else {
            // Assume it's the input filename
            if (inputFilename == NULL) {
                inputFilename = argv[i];
            } else {
                // Invalid use of arguments
                printf("Incorrect arguments.\n");
                exit(22);
            }
        }
    }

    //Start timer
    clock_t start_time_total = clock();

    // Check if inputFilename is provided
    if (inputFilename == NULL) {
        printf("Input filename not provided.\n");
        exit(22);
    }


    // Generate CRC if -generate option is active
    if (generateOption) {
        generate(inputFilename);
        printf("CRC file generated.\n");
    }

    // Verify CRC if -verify option is active
    if (verifyOption) {
        verify(inputFilename, maxNumErrors);
    }

    // End timer
    clock_t end_time_total = clock();  // Record end time
    double elapsed_time_total = ((double) (end_time_total - start_time_total)) / CLOCKS_PER_SEC;
    printf("Total time taken: %f seconds\n", elapsed_time_total);


    return 0;
}