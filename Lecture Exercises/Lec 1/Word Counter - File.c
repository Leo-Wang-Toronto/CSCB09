#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    int counter = 0;

    char input[20];

    FILE *file = fopen(argv[1], "r");

    while (fscanf(file, "%s", input) != EOF) {
        counter++;
    }        
    printf("\nWords: %d\n", counter);
}
