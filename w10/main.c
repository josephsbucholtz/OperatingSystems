#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool checkComma(char* buf) {
    for (int i = 0; i < strlen(buf); i++) {
        if (buf[i] == ',') {
            return true;
        }
    }

    return false;
}

void printHelper(char* string) {
    for (int i = 0; i < strlen(string); i++) {
        if (string[i] == ' ') {
            continue;
        }
        printf("%c", string[i]);
    }
}

int main(void) {
    char buf[100];
    while (1) {
        printf("Enter input string:\n");
        fgets(buf, sizeof(buf), stdin);
        buf[strcspn(buf, "\n")] = '\0';

        if (strcmp(buf, "q") == 0) {
            break;
        }


        if(checkComma(buf) == false) {
            printf("Error: No comma in string.\n");
            printf("\n");
            continue;
        }

        char* token1 = strtok(buf, ",");
        char* token2 = strtok(NULL, "");
        
        printf("First word: ");
        printHelper(token1);
        printf("\n");
        printf("Second word: ");
        printHelper(token2);
        printf("\n");

    }

    return 0;
}
