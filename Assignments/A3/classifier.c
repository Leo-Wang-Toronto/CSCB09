#include "knn.h"

// Makefile included in starter:
//    To compile:               make
//    To decompress dataset:    make datasets
//
// Example of running validation (K = 3, 8 processes):
//    ./classifier 3 datasets/training_data.bin datasets/testing_data.bin 8

/*****************************************************************************/
/* This file should only contain code for the parent process. Any code for   */
/*      the child process should go in `knn.c`. You've been warned!          */
/*****************************************************************************/

/**
 * main() takes in 4 command line arguments:
 *   - K:  The K value for kNN
 *   - training_data: A binary file containing training image / label data
 *   - testing_data: A binary file containing testing image / label data
 *   - num_procs: The number of processes to be used in validation
 * 
 * You need to do the following:
 *   - Parse the command line arguments, call `load_dataset()` appropriately.
 *   - Create the pipes to communicate to and from children
 *   - Fork and create children, close ends of pipes as needed
 *   - All child processes should call `child_handler()`, and exit after.
 *   - Parent distributes the testing set among childred by writing:
 *        (1) start_idx: The index of the image the child should start at
 *        (2)    N:      Number of images to process (starting at start_idx)
 *     Each child should gets N = ceil(test_set_size / num_procs) images
 *      (The last child might get fewer if the numbers don't divide perfectly)
 *   - Parent waits for children to exit, reads results through pipes and keeps
 *      the total sum.
 *   - Print out (only) one integer to stdout representing the number of test 
 *      images that were correctly classified by all children.
 *   - Free all the data allocated and exit.
 */
int main(int argc, char *argv[]) {
    // TODO: Handle command line arguments
    if (argc != 5) {
        fprintf(stderr, "Usage: ./classifier K training_file testing_file num_procs\n");
        exit(1);
    }

    const char *train = argv[2];
    const char *test = argv[3];
    int K = strtol(argv[1], NULL, 10);
    int num_procs = strtol(argv[4], NULL, 10);

    if (num_procs < 1) {
        fprintf(stderr, "Invalid num_proc value: %d\n", num_procs);
        exit(1);
    }
    if (K < 1) {
        fprintf(stderr, "Invalid K value: %d\n", K);
        exit(1);
    }

    Dataset *training = load_dataset(train);
    Dataset *testing = load_dataset(test);

    // TODO: Spawn `num_procs` children

    int pipes_in[num_procs][2];
    int pipes_out[num_procs][2];
    pid_t f = 0;
    int pipe_number = 0;

    for (int i = 0; i < num_procs; i++)
    {
        if (pipe(pipes_in[i]) == -1) {
            perror("pipe");
            exit(1);
        }
        if (pipe(pipes_out[i]) == -1) {
            perror("pipe");
            exit(1);
        }

        f = fork();

        if (f == 0) {
            if (close(pipes_in[i][1]) == -1) {
                perror("close");
                exit(1);
            }
            if (close(pipes_out[i][0]) == -1) {
                perror("close");
                exit(1);
            }
            pipe_number = i;

            // close all pipes that are not needed
            for (int j = 0; j < i; j++)
            {
                if (close(pipes_in[j][1]) == -1) {
                    perror("close");
                    exit(1);
                }
                if (close(pipes_out[j][0]) == -1) {
                    perror("close");
                    exit(1);
                }
            }
            
            break;
        }
        else if (f == -1) {
            perror("fork");
            exit(1);
        }
        else {
            if (close(pipes_in[i][0]) == -1) {
                perror("close");
                exit(1);
            }
            if (close(pipes_out[i][1]) == -1) {
                perror("close");
                exit(1);
            }
        }
    }

    // TODO: Send information to children
    if (f > 0) {
        int child_size = ceil((double)testing->num_items / (double)num_procs);

        int total = 0;
        for (int i = 0; i < num_procs; i++)
        {
            int j = 0;
            while (j < child_size && total < testing->num_items) {
                j++;
                total++;
            }
            int index = total - j;
            // printf("Writing index: %d size: %d\n", index, j);
            if (write(pipes_in[i][1], &index, sizeof(int)) == -1) {
                perror("write to child");
                exit(1);
            }

            if (write(pipes_in[i][1], &j, sizeof(int)) == -1) {
                perror("write to child");
                exit(1);
            }

            if (close(pipes_in[i][1]) == -1) {
                perror("close");
                exit(1);
            }
        }

        int status;

        for (int i = 0; i < num_procs; i++)
        {
            if (wait(&status) != -1) {
                if (!WIFEXITED(status)) {
                    printf("[%d] Child exited abnormally\n", getpid());
                    exit(1);
                }
            }
        }

        // TODO: Compute the total number of correct predictions from returned values
        int total_correct = 0;
        int output;

        for (int i = 0; i < num_procs; i++)
        {
            if (read(pipes_out[i][0], &output, sizeof(int)) == -1) {
                perror("read from child");
                exit(1);
            }

            if (close(pipes_out[i][0]) == -1) {
                perror("close");
                exit(1);
            }
            
            // printf("Parent received %d\n", output);
            total_correct += output;
        }
        
        // Print out answer
        printf("%d\n", total_correct);
    }
    else {
        child_handler(training, testing, K, pipes_in[pipe_number][0], pipes_out[pipe_number][1]);
        if (close(pipes_in[pipe_number][0]) == -1) {
            perror("close p_in");
            exit(1);
        }
        if (close(pipes_out[pipe_number][1]) == -1) {
            perror("close p_out");
            exit(1);
        }
    }  

    free_dataset(training);
    free_dataset(testing);
    return 0;
}
