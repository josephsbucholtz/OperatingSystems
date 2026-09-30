/**
 * Description: Assignment 6 is very similar in objective to countnames_parallel assignment, except
 * instead of counting names in parallel processes, we will be using multi-threading. The objective
 * of this assignment is to implement multi-threading programming and locking shared variables.
 * Author names: Joseph Bucholtz
 * Author emails: joseph.bucholtz@sjsu.edu 
 * Last modified date: November 26, 2025
 * Creation date: November 3, 2025
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

/*****************************************
//CS149 FA25
//Template for assignment 6
//San Jose State University
//originally prepared by Bill Andreopoulos
*****************************************/

//Globals
#define MAX_NAMES 100
#define STRING_LENGTH 30

//thread mutex lock for access to the log index
//TODO you need to use this mutexlock for mutual exclusion
//when you print log messages from each thread
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void* thread_runner(void*);
pthread_t t1, t2;


//variable for indexing of messages by the logging function.
int logindex=0;
int *logip = &logindex;


//The name counts.
// You can use any data structure you like, here are 2 proposals: a linked list OR an array (up to 100 names).
//The linked list will be faster since you only need to lock one node, while for the array you need to lock the whole array.
//You can use a linked list template from A5. You should also consider using a hash table, like in A5 (even faster).
struct NAME_STRUCT
{
    char key[STRING_LENGTH];
    int value;
};
typedef struct NAME_STRUCT THREAD_NAME;

//array of 100 names
THREAD_NAME names_array[MAX_NAMES] = {0};
pthread_mutex_t names_mutex = PTHREAD_MUTEX_INITIALIZER;

//-----------------------My FUNCTIONS---------------------
/**
 * This function checks if a name is already in map, then increments value
 * or if it isn't in map, it will add it and set value to 1. Locks the array while it is
 * checking name / value pair.
 * Input parameters: map of names, buffer of line from file, and count of names in map
 * Returns: void
**/
void checkName(THREAD_NAME names[MAX_NAMES], const char* buffer) {
    pthread_mutex_lock(&names_mutex);

    // First: search existing entries
    for (int i = 0; i < MAX_NAMES; i++) {
        if (names[i].key[0] == '\0')  // reached end of used entries
            break;

        if (strcmp(names[i].key, buffer) == 0) {
            names[i].value++;
            pthread_mutex_unlock(&names_mutex);
            return;
        }
    }

    // Second: find first free slot
    for (int i = 0; i < MAX_NAMES; i++) {
        if (names[i].key[0] == '\0') {  // empty slot found
            strcpy(names[i].key, buffer);
            names[i].value = 1;
            pthread_mutex_unlock(&names_mutex);
            return;
        }
    }

    // Optional: array full
    fprintf(stderr, "Error: name array full\n");
    pthread_mutex_unlock(&names_mutex);
}

/**
 * Prints out the contents of the map. Name / Value
 * Input parameters: Map with name / value pair, name_count = count of names in map
 * Returns: void
**/
void printMap(THREAD_NAME names[MAX_NAMES]) {
    printf("================ NAME COUNTS ================\n");
    for (int i = 0; i < MAX_NAMES; i++) {
        if (names[i].value == 0)  
            break;

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

/**
 * Locks log index and prints out log message. Then unlocks log index 
 * Input parameters: char* to file, int the thread index 
 * Returns: void
**/
void logprint(const char *filename, int thread_index) {
    pthread_mutex_lock(&log_mutex);

    // increment shared log index
    (*logip)++;
    int curr_index = *logip;

    pid_t pid = getpid();

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestr[64];

    // 17/04/2023 09:23:25 pm style
    strftime(timestr, sizeof(timestr), "%d/%m/%Y %I:%M:%S %p", t);

    printf("Logindex %d, thread %d, PID %d, %s:  opened file %s\n",
           curr_index, thread_index, (int)pid, timestr, filename);
    fflush(stdout);

    pthread_mutex_unlock(&log_mutex);
}

/**
 * Get the thread index 
 * Input parameters: void 
 * Returns: int
**/
int get_thread_index() {
    pthread_t me = pthread_self();

    if (pthread_equal(me, t1)) return 1;
    if (pthread_equal(me, t2)) return 2;

    // main thread or any non-t1/t2 thread
    return 0;
}

/*********************************************************
// function main 
*********************************************************/
int main(int argc, char* argv[])
{
    //TODO similar interface as A2: give as command-line arguments up to three filenames of names (the names in the files are newline-separated).
    //Map, buffer, name/line count intialization
    char buffer[STRING_LENGTH];
    int line_count = 1;
    
    // SWITCH BLOCK 
    switch (argc) {
        // argc = 1, 0 file functionality (USER CLI INPUT)
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

                checkName(names_array, buffer);
                line_count++;
            }

            break;

        // argc = 2 | 1 file functionality (1 file argument)
        case 2: {
            // Open File
            FILE* mFile = fopen(argv[1], "r");
            //Check File
            if (mFile == NULL) {
                fprintf(stderr, "error: cannot open file %s\n", argv[1]);
                exit(1);
            }

            //Print Log File1
            logprint(argv[1], 0);

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

                checkName(names_array, buffer);
                line_count++;
            }

            fclose(mFile);
        }
            break;


        // MULTIPLE FILES: MUTLITHREADING 
        // argc = 3 | 2 file functionality (2 file arguments)
        case 3: {
            printf("create first thread\n");
            pthread_create(&t1,NULL,thread_runner, (void *)argv[1]);

            // Open File
            FILE* mFile = fopen(argv[2], "r");

            //Check File
            if (mFile == NULL) {
                fprintf(stderr, "error: cannot open file %s\n", argv[2]);
                exit(1);
            }

            //Print Log File1
            logprint(argv[1], 0);

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

                checkName(names_array, buffer);
                line_count++;
            }

            fclose(mFile);

            printf("wait for first thread to exit\n");
            pthread_join(t1,NULL);
            printf("first thread exited\n");

        }
            break;

        // MULTIPLE FILES: MUTLITHREADING
        // argc = 4 | 3 file functionality (3 file arguments)
        case 4: {
            printf("create first thread\n");
            pthread_create(&t1,NULL,thread_runner,argv[1]);

            printf("create second thread\n");
            pthread_create(&t2,NULL,thread_runner,argv[2]);

            // Open File
            FILE* mFile = fopen(argv[3], "r");
            //Check File
            if (mFile == NULL) {
                fprintf(stderr, "error: cannot open file %s\n", argv[3]);
                exit(1);
            }

            //Print Log File1
            logprint(argv[3], 0);

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

                checkName(names_array, buffer);
                line_count++;
            }

            fclose(mFile);


            printf("wait for first thread to exit\n");
            pthread_join(t1,NULL);
            printf("first thread exited\n");

            printf("wait for second thread to exit\n");
            pthread_join(t2,NULL);
            printf("second thread exited\n");
        }
            break;

        // argc > 4  so throw error
        default:
            fprintf(stderr, "Error: correct usage ./countnames_threaded\n./countnames_threaded <file>\n./countnames_threaded <file> <file>\n");
            exit(1);
    }

    //TODO print out the sum variable with the sum of all the numbers
    printMap(names_array);

    exit(0);

}//end main


/**********************************************************************
// function thread_runner runs inside each thread 
**********************************************************************/
void* thread_runner(void* x)
{
    /**
   * //TODO implement any thread name counting functionality you need. 
   * Assign one file per thread. Hint: you can either pass each argv filename as a thread_runner argument from main.
   * Or use the logindex to index argv, since every thread will increment the logindex anyway 
   * when it opens a file to print a log message (e.g. logindex could also index argv)....
   * //Make sure to use any mutex locks appropriately
   */

    char* filename = (char*)x;
    int line_count = 0;
    char buffer[STRING_LENGTH];

    // Open File
    FILE* mFile = fopen(filename, "r");
    //Check File
    if (mFile == NULL) {
        fprintf(stderr, "error: cannot open file %s\n", filename);
        pthread_exit(NULL);
    }

    // Print Log Threads 
    int thread_index = get_thread_index();
    logprint(filename, thread_index);

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

        checkName(names_array, buffer);
        line_count++;
    }

    fclose(mFile);

    pthread_exit(NULL);
    //return NULL;

}//end thread_runner
