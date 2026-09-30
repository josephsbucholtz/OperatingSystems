/**
 * Description: Similar to GNU parallel, the goal is to implement parallel exected processes in which
 * we keep logs in childPID.out and track exit codes and signals in childPID.err
 * Author names: Joseph Bucholtz
 * Author emails: joseph.bucholtz@sjsu.edu
 * Last modified date: October 22, 2025
 * Creation date: October 4, 2025
 **/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

//globals
#define MAX_COMMANDS 100
#define MAX_COMMAND_LENGTH 100
#define MAX_ARGS 100

int main() {

    char line[MAX_COMMAND_LENGTH];
    int nChildren = 0;
    int lineCount = 1;

    //Loop through lines of commands
    while(nChildren < MAX_COMMANDS && fgets(line, sizeof(line), stdin) != NULL) {
        // Skip empty/blank lines | trim newline
        if (strspn(line, " \t\r\n") == strlen(line)) 
        { lineCount++; continue; }
        size_t n = strlen(line);
        if (n && line[n-1] == '\n') line[n-1] = '\0';


        pid_t p = fork();

        //fork error
        if (p < 0) {
            perror("Fork process error\n");
            exit(1);
        }

        // Child Process
        if (p == 0) {
            // Build Arg array
            char *argv[MAX_ARGS];
            int argc = 0;
            char buf[MAX_COMMAND_LENGTH];
            strncpy(buf, line, sizeof(buf));
            buf[sizeof(buf)-1] = '\0';

            char *tok = strtok(buf, " \t");
            while (tok && argc < MAX_ARGS - 1) {
                argv[argc++] = tok;
                tok = strtok(NULL, " \t");
            }
            argv[argc] = NULL;

            if (argc == 0) _exit(0); // nothing to exec

            pid_t childPID = getpid();

            //childPID.out / childPID.err
            char outFile[64], errFile[64];
            snprintf(outFile, sizeof(outFile), "%d.out", childPID);
            snprintf(errFile, sizeof(errFile), "%d.err", childPID);

            int outFD = open(outFile, O_RDWR | O_CREAT | O_APPEND, 0777);
            int errFD = open(errFile, O_RDWR | O_CREAT | O_APPEND, 0777);

            if (outFD < 0) { perror("outFD error"); exit(2);}
            if (errFD < 0) { perror("errFD error"); exit(2);}
            
            if (dup2(outFD, STDOUT_FILENO) < 0) { perror("dup2 stdout"); _exit(2); }
            if (dup2(errFD, STDERR_FILENO) < 0) { perror("dup2 stderr"); _exit(2); }

            close(outFD);
            close(errFD);

            printf("Starting command %d: child %d pid of parent %d\n", lineCount, childPID, getppid());
            fflush(stdout);

            //Run execute
            execvp(argv[0], argv);

            //Execute returned
            perror(argv[0]);
            _exit(2);
        }

        // Parent Process
        else if (p > 0) {
            nChildren++;
            lineCount++;
        }
    }

    //EOF. Wait for children to finish. 
    int status;
    int wpid;
    while ((wpid = wait(&status)) > 0) {

        //childPID.out / childPID.err
        char outFile[64], errFile[64];
        snprintf(outFile, sizeof(outFile), "%d.out", wpid);
        snprintf(errFile, sizeof(errFile), "%d.err", wpid);

        //Child Finished print
        int outFD = open(outFile, O_RDWR | O_CREAT | O_APPEND, 0777);
        if (outFD >= 0) {
            dprintf(outFD, "Finished child %d pid of parent %d\n", wpid, getppid());
            fsync(outFD);
            close(outFD);
        }

        //Child exit/signal print
        int errFD = open(errFile, O_WRONLY | O_CREAT | O_APPEND, 0666);
        if (errFD >= 0) {
            if (WIFEXITED(status)) {
                dprintf(errFD, "Exited with exitcode = %d\n", 
                        WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                dprintf(errFD, "Killed with signal %d\n", 
                        WTERMSIG(status));
            }     

            fsync(errFD);
            close(errFD);
        }

    }

    return 0;
}


