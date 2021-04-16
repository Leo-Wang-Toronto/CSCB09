#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Read a user id and password from standard input, 
   - create a new process to run the validate program
   - send validate the user id and password on a pipe, 
   - print a message 
        "Password verified" if the user id and password matched, 
        "Invalid password or no such user"
     depending on the return value of the validate program.
*/


int main() {
    char userid[10];
    char password[10];

    /* Read a user id and password from stdin */
    printf("User id:\n");
    scanf("%s", userid);
    printf("Password:\n");
    scanf("%s", password);

    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t f = fork();
    
    if (f > 0) {
        close(fd[0]);   // close reading end

        write(fd[1], userid, sizeof(userid));
        write(fd[1], password, sizeof(password));

        close(fd[1]);

        int status;
        if (wait(&status) == -1) {
            perror("wait");
            exit(1);
        }

        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 0) {
                printf("Password verified\n");
            }
            else if (WEXITSTATUS(status) == 2) {
                printf("Invalid password or no such user\n");
            }
            else if (WEXITSTATUS(status) == 1) {
                printf("Child error\n");
            }
        }
    }
    else if (f == 0) {
        close(fd[1]);   // close writing end
        dup2(fd[0], STDIN_FILENO);

        char *args[] = {"./validate", NULL};

        if (execvp(args[0], args) == -1) {
            perror("execvp");
            exit(1);
        }
    }
    else {
        perror("fork");
    }

    return 0;
}
