#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

char name[32];

void sing(int sig) 
{
    printf("Happy Birthday to you\n");
    sleep(3);
    printf("Happy Birthday to you\n");
    sleep(3);
    printf("Happy Birthday dear %s\n", name);
    sleep(3);
    printf("Happy Birthday to you!\n");
    return;
}

int main(int argc, char **argv) 
{
    strcpy(name, argv[1]);

    struct sigaction action;
    action.sa_handler = sing;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGINT);
    sigaction(SIGUSR1, &action, NULL);

    while(1) {
        // do nothing
    }

    return 0;
}