// SPDX-License-Identifier: GPL-2.0
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/mman.h>
#include <time.h>

#include <fcntl.h>
#include "mythreads.h"

int *matrix1, *matrix2; // Read-only by the threads
int dimension; // Read-only by the threads
int *productMatrix; // Read and write by threads

typedef struct {
	int startPoint; // Index at which thread starts computing
	int numRowsAndCols; // Number of row and column combinations to compute
} threadInfo;

// PURPOSE: Exits the program if the pointer value is NULL
void checkMalloc(void *pointer)
{
	if (pointer == NULL) {
		printf("Malloc failed! Exiting program..\n");
		exit(1);
	}
}

/* PURPOSE: Getting the flags from the user
 * RETURN: FileName in the char *
 */
char *getArguments(char *fileName, int *numThreads, int argc, char *argv[])
{
	int c;
	FILE *fptr;

	while ((c = getopt(argc, argv, ":t:o:")) != -1) {
		switch (c) {
		case 't':
			*numThreads = atoi(optarg);
			if (*numThreads <= 0) {
				printf("Error: INVALID NUMBER OF THREADS!\n");
				exit(1);
			}
			break;
		case 'o':
			fileName = malloc(strlen(optarg)+1);
			checkMalloc(fileName);
			strncpy(fileName, optarg, strlen(optarg));
			fileName[strlen(optarg)] = '\0';
			//Overwriting the file if it already exists
			fptr = fopen(fileName, "w");
			fclose(fptr);
			break;
		case':'://missing argument
			printf("\nError: %s: option '-%c'", argv[0], optopt);
			printf(" requires an argument\n\n");
			exit(1);

		case'?'://invalid argument
			printf("\nError: %s: option '-%c'", argv[0], optopt);
			printf(" is invalid: ignored\n\n");
			exit(1);
		}
	}
	return fileName;
}

/* PURPOSE: Importing a 2-D matrix from the given file to a 1-D array
 * RETURN: FileName in the char *
 */
void getMatrixFromFile(char *fileName, int **matrix)
{
	FILE *fp;

	fp = fopen(fileName, "rb");
	if (fp == NULL) {
		printf("ERROR: File does not exist.\n");
		exit(1);
	}
	*matrix = (int *) mmap(0, (dimension*dimension) * sizeof(int),
			PROT_READ, MAP_FILE | MAP_PRIVATE, fileno(fp), 0);
	fclose(fp);
}

/* PURPOSE:	- This function passed to each newly created thread.
 *			- Sets the loop starting values based on the values in
 *			- the struct.
 *			- Terminates the loops when the given number of
 *			- rows/columns are calculated.
 */
void *matrixMultiplication(void *threadPtr)
{
	threadInfo *threadStruct = (threadInfo *) threadPtr; // typecasting
	int i, j, k, count, indexForProduct, sum, loopCount = 0;

	i = threadStruct->startPoint - (threadStruct->startPoint % dimension);
	j = threadStruct->startPoint % dimension;
	indexForProduct = threadStruct->startPoint;

	for (; i < (dimension*dimension); i = i + dimension) {
		for (; j < dimension; j++) {
			// When Thread has finished calculation
			// for given number of Rows and cols
			if (++loopCount > threadStruct->numRowsAndCols)
				return NULL;
			count = 0;
			sum = 0;
			for (k = 0; k < (dimension*dimension);
						k = k + dimension) {
				sum = sum + (matrix1[i+count] * matrix2[j+k]);
				count++;
			}
			productMatrix[indexForProduct] = sum;
			indexForProduct++;
		}
		j = 0; // To start over
	}
	return NULL;
}

/*
 * PURPOSE: - Calculates the size dimension of the matrix in a given file.
 *		  - Reports error if the matrix is not square
 * RETURN: The dimension of the matrix
 */
int getDimension(char *fileName)
{
	FILE *fp; int sz;

	fp = fopen(fileName, "r");
	if (fp == NULL) {
		printf("ERROR: File does not exist.\n");
		exit(1);
	}
	// Determining the length of the file
	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);
	fclose(fp); // Closing the file

	// File contains 32 bit integers
	double size = sqrt((double) sz / (double) 4);

	if (size - (int) size != 0) {
		printf("ERROR: Matrix in file %s is not square.\n", fileName);
		exit(1);
	}
	return (int) size; // typecasting double to int
}

/*
 * PURPOSE: Writes the solution matrix to a file if 'fileName' contains a
 *			file name.
 * RETURN: N/A
 */
void addMatrixToFile(char *fileName)
{
	int *file_memory;

	if (fileName != NULL) {
		int fd = open(fileName, O_RDWR | O_CREAT, (mode_t) 0644);
	    // Stretch the file size to the size of the (mmapped) array of char
		if (lseek(fd, (dimension*dimension*4)-1, SEEK_SET) == -1) {
			close(fd);
			perror("Error calling lseek() to 'stretch' the file");
			exit(EXIT_FAILURE);
		}
		// Writing a character to the end of the file
		if (write(fd, "", 1) == -1) {
			close(fd);
			perror("Error writing last byte of the file");
			exit(EXIT_FAILURE);
		}

		file_memory = mmap(0, dimension*dimension*4,
			PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (file_memory == MAP_FAILED) {
			printf("ERROR!\n");
			exit(EXIT_FAILURE);
		}
		// Writing to the file
		memcpy(file_memory, productMatrix, dimension*dimension*4);

		munmap(file_memory, dimension*dimension*4);
		close(fd);
	}
}

/*
 * PURPOSE: Creates a given number of threads, passes the struct having
 *		  information required, and then waits to join.
 */
void createAndJoinThreads(int numThreads, threadInfo *args,
	pthread_t *threadArr)
{
	int i;

	for (i = 0; i < numThreads; ++i) {
		Pthread_create(&threadArr[i], NULL,
			(void *) &matrixMultiplication, (void *) &args[i]);
	}
	printf("Waiting for the threads to join...\n");

	for (i = 0; i < numThreads; ++i)
		Pthread_join(threadArr[i], NULL);
}

int main(int argc, char *argv[])
{
	char *outputFileName = NULL;
	int matrix1dim, matrix2dim, numThreads = 0, extraRows = 0, i,
			countIndices, minRowsPerThread;

	outputFileName = getArguments(outputFileName, &numThreads, argc, argv);

	// optind stores the index of the first non-option argument
	if (argc-optind != 2) { // if the num of unprocessed arguments is not 2
		printf("Invalid arguments. Usage: <file1> <file2> ");
		printf("[-o <fileName>] -t <number of threads>\n");
		exit(1);
	}

	// Extracting the dimension from the file name
	matrix1dim = getDimension(argv[optind]);
	matrix2dim = getDimension(argv[optind+1]);

	if (matrix1dim != matrix2dim) {
		printf("ERROR: the arrays are not the same size\n");
		exit(1);
	}
	dimension = matrix1dim; // Global variable
	printf("Multiplying arrays of dimension %d\n\n", dimension);

	getMatrixFromFile(argv[optind], &matrix1);
	getMatrixFromFile(argv[optind+1], &matrix2);

	if (numThreads == 0) // When -t is not given as an argument
		numThreads = 1; // Default value set to 1

	// Number of threads are greater than number of values to be computed
	if (numThreads > (dimension*dimension)) {
		printf("Number of threads specified are more than required.\n");
		numThreads = (dimension*dimension);
	} else
		extraRows = (dimension*dimension) % numThreads;

	// THREAD-RELATED STUFF
	pthread_t *threadArr;
	threadInfo *args;

	args = (threadInfo *) malloc(sizeof(threadInfo) * numThreads);
	threadArr = (pthread_t *) malloc(sizeof(pthread_t) * numThreads);
	checkMalloc(args); checkMalloc(threadArr);

	// Each thread will execute atleast 'minRowsPerThread' number of
	// row/column operations
	minRowsPerThread = (dimension*dimension) / numThreads;
	countIndices = 0;
	// Calculating the additional rows/column operations for threads
	for (i = 0; i < numThreads; i++) {
		(args+i)->startPoint = countIndices;
		if (extraRows-- > 0)
			(args+i)->numRowsAndCols = minRowsPerThread + 1;
		else
			(args+i)->numRowsAndCols = minRowsPerThread;
		countIndices = countIndices + (args+i)->numRowsAndCols;
	}

	productMatrix = (int *) malloc(sizeof(int)*(dimension*dimension));
	checkMalloc(productMatrix);

	createAndJoinThreads(numThreads, args, threadArr);

	printf("\nMultiplication complete!\n\n");

	addMatrixToFile(outputFileName);

	munmap(matrix1, (dimension*dimension) * sizeof(int));
	munmap(matrix2, (dimension*dimension) * sizeof(int));

	free(args); free(threadArr); free(productMatrix); free(outputFileName);
	return 0;
}
