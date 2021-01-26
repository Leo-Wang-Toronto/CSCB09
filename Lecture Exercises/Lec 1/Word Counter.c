#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    int counter = 0;

    char input[20];
    char input_char;

    if (strcmp(argv[1], "-w") == 0) {
        while (scanf("%s", input) != EOF) {
            counter++;
        }        
        printf("\nWords: %d\n", counter);
        return 0;
    }

    if (strcmp(argv[1], "-c") == 0) {
        while (scanf("%c", &input_char) != EOF) {
            counter++;
        }        
        printf("\nChars: %d\n", counter);
        return 0;
    }

    printf("Invalid argument");
}
