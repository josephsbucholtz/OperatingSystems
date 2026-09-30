/**
 * Description: This module implements matrix operations on matrices and vectors. It returns a vector.
 * Author names: Joseph Bucholtz
 * Author emails: joseph.bucholtz@sjsu.edu
 * Last modified date: September 8, 2025
 * Creation date: September 3, 2025
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * This function performs a multiplication (dot product) of two matrices
 * Assumption: matrices have sizes of R[1][5], A[1][3], W[3][5], B[1][5]
 * Input parameters: Result matrix, A matrix, W matrix, B matrix
 * Returns: a vector R;
**/
void calculateMatrixMult(int R[1][5], int A[1][3], int W[3][5], int B[1][5]) {
    // Loop over Columns
    for (int j = 0; j < 5; j++) {                  
        //Loop over Rows
        for (int i = 0; i < 3; i++) {                 
            // Multiply A and W
            R[0][j] += A[0][i] * W[i][j];   
        }

        //Add R and B.
        R[0][j] += B[0][j];      
    }
}

/**
 * This function Opens a file, reads it into Matrix, and closes file
 * Input parameters: rows, cols, filename, matrix
 * Returns: void
**/
void readFile(int rows, int cols, char* file, int matrix[rows][cols]) {
    FILE* mFile = fopen(file, "r");
    if (mFile == NULL) {
        fprintf(stderr, "error: cannot open file %s", file);
        exit(1);
    }

    char buf[256];  
    int i = 0;

    //Loop over rows getting line and reading in buffer
    while (i < rows && fgets(buf, sizeof(buf), mFile) != NULL) {
        // Tokenize the line
        int j = 0;
        char* token = strtok(buf, " \t\r\n");

        // Loop through tokens. Convert to string to int and store in matrix 
        while (token && j < cols) {
            matrix[i][j] = atoi(token);
            token = strtok(NULL, " \t\r\n");
            j++;
        }
        i++;
    }

    fclose(mFile);
}

/**
 * This function prints a Matrix to stdout
 * Assumption: matrices of 2D array
 * Input parameters: rows, cols, matrix
 * Returns: void
**/
void printMatrix(int rows, int cols, int matrix[][cols]) {
    // Loop over rows
    for (int i = 0; i < rows; i++) {
        //Loop over Columns
        for (int j = 0; j < cols; j++) {
            printf("%d", matrix[i][j]);
            if (!(i == rows - 1 && j == cols - 1)) {
                printf(" ");
            }
        }
    }
}

int main(int argc, char *argv[]) {
    //User must input 3 files
    if (argc != 4) {
        fprintf(stderr, "error: expecting exactly 3 files as input\n");
        exit(1);
    }

    int A[1][3] = {0};
    int W[3][5] = {0};
    int B[1][5] = {0};
    int R[1][5] = {0};

    // Read all files arguments and store in 2D arrays
    readFile(1, 3, argv[1], A); 
    readFile(3, 5, argv[2], W); 
    readFile(1, 5, argv[3], B); 

    //Calculate the Matrix Multiplication
    calculateMatrixMult(R, A, W, B);

    //Print Functions
    printf("Result of %s * %s + %s = [", argv[1], argv[2], argv[3]);
    printMatrix(1, 5, R);
    printf("]\n");

    return 0;
}

