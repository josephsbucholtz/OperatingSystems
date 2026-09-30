#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

bool checkComma(char* buf) {
    for (int i = 0; i < strlen(buf); i++) {
        if (buf[i] == ',') {
            return true;
        }
    }

    return false;
}

void extractWords(char* buf) { 
    char* sptr = strtok(buf, ",");
    if (sptr != NULL) {
        printf("First word: %s\n", sptr);
        sptr = strtok(NULL, ",");
    }
    if (sptr != NULL) {
        printf("Second word: %s\n", sptr);
    }

}

void errorHandling(char* buf, size_t size){
    while(!checkComma(buf)) {
        printf("Enter input string:\n");
        fgets(buf, sizeof(buf), stdin);
    }
}

int main(void) {
    char buf[100];

    while (1) {
        printf("Enter input string (or 'q' to quit):\n");
        fgets(buf, sizeof(buf), stdin);
        buf[strcspn(buf, "\n")] = '\0';

        if (strcmp(buf, "q") == 0) {
            break;
        }

        errorHandling(buf, sizeof(buf));
        extractWords(buf);
    }
    return 0;
}

