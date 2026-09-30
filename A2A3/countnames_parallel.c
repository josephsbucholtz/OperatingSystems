/**
 * Description: This assignment is a modification to Assignment 2. In this program, we are creating a program that can take 
 * many files and count names using parallel processes.File count unknown. 
 * Author names: Joseph Bucholtz
 * Author emails: joseph.bucholtz@sjsu.edu 
 * Last modified date: September 24, 2025
 * Creation date: September 15, 2025
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

//Globals
#define MAX_NAMES 100
#define STRING_LENGTH 31

// Map Structure for our name / value pair data 
typedef struct {
    char key[STRING_LENGTH];
    int value;
} map;

/**
 * This function checks if a name is already in map, then increments value
 * or if it isn't in map, it will add it and set value to 1
 * Input parameters: map of names, buffer of line from file, and count of names in map
 * Returns: void
**/
void checkName(map names[MAX_NAMES], char* buffer, int* name_count) {
    bool found = 0;
    for ( int i = 0; i < *name_count; i++) {
        if (strcmp(names[i].key, buffer) == 0) {
            names[i].value++;
            found = true;     
            break;
        }
    }

    if (!found) {
        strcpy(names[*name_count].key, buffer);
        names[*name_count].value = 1;
        (*name_count)++;
    }
}

/**
 * Prints out the contents of the map. Name / Value
 * Input parameters: Map with name / value pair, name_count = count of names in map
 * Returns: void
**/
void printMap(map names[MAX_NAMES], int name_count) {
    for (int i = 0; i < name_count; i++) {
        printf("%s: %d\n", names[i].key, names[i].value);
    }
}

/**
 * Removes Trailing Spaces that user or in file is inputted.
 * Input parameters: buffer line of file.
 * Returns: void
**/
void removeEndingSpaces(char* buffer) {
    int len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == ' ' || buffer[len - 1] == '\t')) {
        buffer[len - 1] = '\0';
        len--;
    }
}

/**
 * Process a file by opening, reading into buffer, put into map like A2, but this time with parallel processes
 * write count, set key / value pairs , and exit 
 * Input parameters: filename, write_fd
 * Returns: void
**/
void processFile(char* filename, int write_fd) {
    // Open File
    FILE* mFile = fopen(filename, "r");
    //Check File
    if (mFile == NULL) {
        fprintf(stderr, "error: cannot open file %s\n", filename);
        close(write_fd);
        exit(1);
    }

    map names[MAX_NAMES];
    char buffer[STRING_LENGTH];
    int name_count = 0;
    int line_count = 1;

    while (fgets(buffer, sizeof(buffer), mFile)){

        // Null terminator
        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';
        }

        //Remove any spaces at the end as they are not apart of name
        removeEndingSpaces(buffer);

        // Skip empty lines
        if (strlen(buffer) < 1) {
            fprintf(stderr, "Warning - file %s line %d is empty\n", filename, line_count);
            continue;
        }

        checkName(names, buffer, &name_count);
        line_count++;
    }

    fclose(mFile);

    write(write_fd, &name_count, sizeof(int));

    // set key / value pair with write pipe
    for (int i = 0 ; i < name_count; i ++) {
        write(write_fd, &names[i], sizeof(map));
    }

    close(write_fd);
    exit(0);
}


/**
 * loop through child map, inner loop through parent. If child and parent share name
 * increment value, fould == true, and break. If key is not found, add to parent_names
 * the key and value and increment count of parent.
 * Input parameters: map of parent_names, count of parent, map of child_names, count of child
 * Returns: void
**/
void mergeResults(map parent_names[MAX_NAMES], int* parent_count, map child_names[MAX_NAMES], int child_count) {
    // loop through child trying to find if parent and child share a key
    for (int i = 0 ; i < child_count; i ++){
        bool found = false;

        // loop through parent breaking if parent and child share key
        for (int  j = 0; j < *parent_count; j++) {
            if (strcmp(parent_names[j].key, child_names[i].key) == 0) {
                parent_names[j].value += child_names[i].value;
                found = true;
                break;
            }
        }
        
        // if key not found, add key / value pair to parent from child
        if (!found) {
            strcpy(parent_names[*parent_count].key, child_names[i].key);
            parent_names[*parent_count].value = child_names[i].value;
            (*parent_count)++;
        }
    }
}

int main(int argc, char* argv[])
{
    map names[MAX_NAMES];
    char buffer[STRING_LENGTH];
    int name_count = 0;
    int line_count = 1;

    // Zero Arugments
    if (argc == 1) {
        while (fgets(buffer, sizeof(buffer), stdin)) {
            // Null terminator Handling
            if (buffer[strlen(buffer) - 1] == '\n') {
                buffer[strlen(buffer) - 1] = '\0';
            }

            //Remove any spaces at the end as they are not apart of name
            removeEndingSpaces(buffer);

            // Skip empty lines
            if (strlen(buffer) < 1) {
                fprintf(stderr, "Warning - Line %d is empty.\n", line_count);
                continue;
            }

            checkName(names, buffer, &name_count);
            line_count++;
        }

        printMap(names, name_count);
        return 0;
    }


    // Parallel Process with Multiple File Arguments

    int fileCount = argc - 1;
    int pipes[fileCount][2];
    pid_t pids[fileCount];

    // Loop through files
    for (int i = 0; i < fileCount; i++) {
        // Create pipe, if < 0 fail
        if (pipe(pipes[i]) < 0) {
            fprintf(stderr, "pipe failed\n"); 
            exit(1);
        }

        pids[i] = fork();

        // Fork fail
        if (pids[i] < 0) {
            fprintf(stderr, "fork failed\n"); 
            exit(1);
        }

        // Child... Close read and process file
        if (pids[i] == 0) {
            close(pipes[i][0]);
            processFile(argv[i + 1], pipes[i][1]);
        }
        else {
            // Parent... close write
            close(pipes[i][1]);
        }
    }

    // Loop through files
    for (int i = 0; i < fileCount; i++) {
        // Wait for status of processes
        int status;
        pid_t finished_pid = wait(&status);

        // find pipe of finished process 
        int pipe_index = -1;
        for (int j = 0; j < fileCount; j++) {
            if (pids[j] == finished_pid) {
                pipe_index = j;
                break;
            }
        }

        if (pipe_index != -1) {
            // Read from pipe
            int child_name_count;
            if (read(pipes[pipe_index][0], &child_name_count, sizeof(int)) > 0) {
                map child_names[MAX_NAMES];

                // Read key / value pair from pipe into child_names map
                for (int k = 0; k < child_name_count; k++) {
                    read(pipes[pipe_index][0], &child_names[k], sizeof(map));
                }

                mergeResults(names, &name_count, child_names, child_name_count);
            }

            // Close read
            close(pipes[pipe_index][0]);
        }
    }

    printMap(names, name_count);
    return 0;
}

