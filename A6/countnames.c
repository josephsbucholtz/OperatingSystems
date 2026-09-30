/**
 * Description: For this assignment we will be implementing an operation that is missing from the the default Linux libraries, 
*  which counts for all the names in files how many times each name appears.
 * Author names: Joseph Bucholtz
 * Author emails: joseph.bucholtz@sjsu.edu 
 * Last modified date: September 20, 2025
 * Creation date: September 13, 2025
 **/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

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

void removeEndingSpaces(char* buffer) {
    int len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == ' ' || buffer[len - 1] == '\t')) {
        buffer[len - 1] = '\0';
        len--;
    }
}


int main(int argc, char* argv[])
{
    //Map, buffer, name/line count intialization
    map names[MAX_NAMES];
    char buffer[STRING_LENGTH];
    int name_count = 0;
    int line_count = 1;

    // Switch to handle argc values
    switch (argc) {
        // argc = 1, no file functionality 
        case 1: 
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

            break;

        // argc = 2, file functionality 
        case 2: {
            // Open File
            FILE* mFile = fopen(argv[1], "r");
            //Check File
            if (mFile == NULL) {
                fprintf(stderr, "error: cannot open file %s", argv[1]);
                exit(1);
            }

            while (fgets(buffer, sizeof(buffer), mFile)){

                // Null terminator
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

            fclose(mFile);
        }

            break;

        // argc != 1 || argc != 2 so throw error
        default:
            fprintf(stderr, "Error: correct usage ./countnames || ./countnames <filename>\n");
            exit(1);
    }

    printMap(names, name_count);

    return 0;
}

