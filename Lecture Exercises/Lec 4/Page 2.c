#include <stdio.h>
#include <string.h>

int main() {
    char first[] = "Monday";    // memory in stack
    char *second = "Tuesday";  // str literal (memory in read-only)
    char *third = malloc(sizeof(char) * 10);    // memory in heap
    strncpy(third, "Wednesday", 10);

    // TODO: shorten the day names
    first[3] = "\0";
    second = "Tue";     // str literals are immutable
    third[3] = "\0";

    // TODO: make an array to pointers to the days above
    char *arr[3];

    arr[0] = first;
    arr[1] = second;
    arr[2] = third;

    return 0;
}