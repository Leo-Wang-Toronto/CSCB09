#include "knn.h"

/****************************************************************************/
/* For all the remaining functions you may assume all the images are of the */
/*     same size, you do not need to perform checks to ensure this.         */
/****************************************************************************/

/**************************** A1 code ****************************************/

double max_val_double(double arr[], int size, int *index) {
    int max_index = 0;
    double max_val = 0;

    for (int i = 0; i < size; i++) {
        if (arr[i] > max_val) {
            max_index = i;
            max_val = arr[i];
        }
    }

    *index = max_index;

    return max_val;
}

int mode(int arr[], int n) {
   int maxValue = 0, maxCount = 0;

   for (int i = 0; i < n; i++) {
        int count = 0;
        
        for (int j = 0; j < n; j++) {
            if (arr[j] == arr[i])
            count++;
        }
        
        if (count > maxCount) {
            maxCount = count;
            maxValue = arr[i];
        }
   }

   return maxValue;
}

/* Same as A1, you can reuse your code if you want! */
double distance(Image *a, Image *b) {
    double dist = 0;
    for (int i = 0; i < a->sy * a->sx; i++)
    {
        double d = b->data[i] - a->data[i];
        dist += pow(d, 2);
    }
    
    return sqrt(dist); 
}

/* Same as A1, you can reuse your code if you want! */
int knn_predict(Dataset *data, Image *input, int K) {
    int similar[K];
    double similar_dist[K];

    for (int i = 0; i < data->num_items; i++)
    {
        if (i < K) {
            similar[i] = data->labels[i];
            similar_dist[i] = distance(input, &data->images[i]);
        }
        else {
            double dist = distance(input, &data->images[i]);
            int index;

            if (dist < max_val_double(similar_dist, K, &index)) {
                similar[index] = data->labels[i];
                similar_dist[index] = dist;
            }
        }
    }

    return mode(similar, K);
}

/**************************** A2 code ****************************************/

/* Same as A2, you can reuse your code if you want! */
Dataset *load_dataset(const char *filename) {
    Dataset* dataset = malloc(sizeof(Dataset));
    if (dataset == NULL) {
        perror("malloc dataset");
        exit(1);
    }
    
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        perror(filename);
        exit(1);
    }

    int num_images;
    
    if (fread(&num_images, 4, 1, file) == 0) {
        fprintf(stderr, "Failed to scan file\n");
        exit(1);
    }

    dataset->num_items = num_images;
    dataset->images = malloc(sizeof(Image) * num_images);
    dataset->labels = malloc(sizeof(Image) * num_images);

    if (dataset->images == NULL || dataset->labels == NULL) {
        perror("malloc images or labels");
        exit(1);
    }

    unsigned char input;

    for (int i = 0; i < num_images; i++)
    {
        if (fread(&input, 1, 1, file) == 0) {
            fprintf(stderr, "Failed to scan file\n");
            exit(1);
        }
        dataset->labels[i] = input;
        
        dataset->images[i].sx = 28;
        dataset->images[i].sy = 28;
        dataset->images[i].data = malloc(sizeof(unsigned char) * 28 * 28);

        for (int j = 0; j < 28 * 28; j++) {
            if (fread(&input, 1, 1, file) == 0) {
                fprintf(stderr, "Failed to scan file\n");
                exit(1);
            }
            
            dataset->images[i].data[j] = input;
        }
    }

    if (fclose(file) == -1) {
        fprintf(stderr, "Failed to close file\n");
        exit(1);
    }
    
    return dataset;
}

/* Same as A2, you can reuse your code if you want! */
void free_dataset(Dataset *data) {
    free(data->labels);
    for (int i = 0; i < data->num_items; i++)
    {
        free(data->images[i].data);
    }
    free(data->images);
    free(data);
}


/************************** A3 Code below *************************************/

/**
 * NOTE ON AUTOTESTING:
 *    For the purposes of testing your A3 code, the actual KNN stuff doesn't
 *    really matter. We will simply be checking if (i) the number of children
 *    are being spawned correctly, and (ii) if each child is recieving the 
 *    expected parameters / input through the pipe / sending back the correct
 *    result. If your A1 code didn't work, then this is not a problem as long
 *    as your program doesn't crash because of it
 */

/**
 * This function should be called by each child process, and is where the 
 * kNN predictions happen. Along with the training and testing datasets, the
 * function also takes in 
 *    (1) File descriptor for a pipe with input coming from the parent: p_in
 *    (2) File descriptor for a pipe with output going to the parent:  p_out
 * 
 * Once this function is called, the child should do the following:
 *    - Read an integer `start_idx` from the parent (through p_in)
 *    - Read an integer `N` from the parent (through p_in)
 *    - Call `knn_predict()` on testing images `start_idx` to `start_idx+N-1`
 *    - Write an integer representing the number of correct predictions to
 *        the parent (through p_out)
 */
void child_handler(Dataset *training, Dataset *testing, int K, 
                   int p_in, int p_out) {
    // TODO: Compute number of correct predictions from the range of data 
    //      provided by the parent, and write it to the parent through `p_out`.

    int total_correct = 0;

    int start_idx;
    int items;
    
    if (read(p_in, &start_idx, sizeof(int)) == -1) {
        perror("read from parent");
        exit(1);
    }
    if (read(p_in, &items, sizeof(int)) == -1) {
        perror("read from parent");
        exit(1);
    }

    for (int i = start_idx; i < start_idx + items; i++)
    {
        if (knn_predict(training, &testing->images[i], K) == (int)testing->labels[i]) {
            total_correct++;
        }
    }
    
    if (write(p_out, &total_correct, sizeof(int)) == -1) {
        perror("write to parent");
        exit(1);
    }
}