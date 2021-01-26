#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    int sum = 0;
    int lower = strtol(argv[2], NULL, 10);
    int upper = strtol(argv[3], NULL, 10);
    
    if (strtol(argv[1], NULL, 10) == 0) {
        for (int i = lower; i <= upper; i++) {
            if (i % 2 == 0) {
             	sum += i;   
            }
        }
    }
    
    if (strtol(argv[1], NULL, 10) == 1) {
        for (int i = lower; i <= upper; i++) {
            if (i % 2 == 1) {
             	sum += i;   
            }
        }
    }

    printf("%d", sum);
    return 0;
}
