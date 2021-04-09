#include "knn.h"
#include <string.h>

Image *load_image(char *filename) {
    int input;
    char input_char[32];
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }

    Image *image = malloc(sizeof(Image));
    if (image == NULL) {
        perror("malloc");
        exit(1);
    }

    if (fscanf(file, "%s", input_char) == -1) {     // P2
        fprintf(stderr, "Failed to scan file");
        exit(1);
    }   

    if (fscanf(file, "%d", &image->sy) == -1) {
        fprintf(stderr, "Failed to scan file");
        exit(1);
    }
    if (fscanf(file, "%d", &image->sx) == -1) {
        fprintf(stderr, "Failed to scan file");
        exit(1);
    }

    image->data = malloc(sizeof(unsigned char) * image->sx * image->sy);
    if (fscanf(file, "%d", &input) == -1) {
        fprintf(stderr, "Failed to scan file");
        exit(1);
    }

    for (int i = 0; i < image->sx * image->sy; i++)
    {
        if (fscanf(file, "%d", &input) == -1) {
            fprintf(stderr, "Failed to scan file");
            exit(1);
        }
        *(image->data + i * sizeof(unsigned char)) = input;
    }

    if (fclose(file) == -1) {
        fprintf(stderr, "Failed to close file");
        exit(1);
    }
    
    return image;
}

int file_lines(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }

    char input[32];

    int len = 0;

    while (fscanf(file, "%s", input) != EOF) {
        len++;
    }
    if (fclose(file) == -1) {
        fprintf(stderr, "Failed to close file");
        exit(1);
    }
    return len;
}

Dataset *init_dataset(int size) {
    Dataset *dataset = malloc(sizeof(Dataset));
    if (dataset == NULL) {
        perror("malloc");
        exit(1);
    }

    dataset->num_items = size;

    dataset->labels = malloc(sizeof(unsigned char) * size);
    if (dataset->labels == NULL) {
        perror("malloc");
        exit(1);
    }
    
    dataset->images = malloc(sizeof(Image) * size);
    if (dataset->images == NULL) {
        perror("malloc");
        exit(1);
    }

    return dataset;
}

/**
 * This function takes in the name of the text file containing the image names
 * This function should NOT assume anything about the image resolutions,
 * and may be tested with images of different sizes. You will need to allocate
 * your own data for the struct, and also for the images / labels.
 */
Dataset *load_dataset(const char *filename) {
    // TODO: Allocate data, read image data / labels, return
    char input[32]; 
    Dataset *dataset = init_dataset(file_lines(filename));
    // TODO: allocate memory for labels and Images
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "failed to open file");
        exit(1);
    }

    for (int i = 0; i < dataset->num_items; i++)
    {
        if (fscanf(file, "%s", input) == -1) {
            fprintf(stderr, "Failed to scan file");
            exit(1);
        }
        dataset->labels[i] = input[strlen(input) - 5];
        dataset->images[i] = *(load_image(input));
    }

    if (fclose(file) == -1) {
        fprintf(stderr, "Failed to close file");
        exit(1);
    }

    return dataset;
}

/****************************************************************************/
/* For all the remaining functions you may assume all the images are of the */
/*     same size, you do not need to perform checks to ensure this.         */
/****************************************************************************/

/** 
 * Return the euclidean distance between the image pixels (as vectors).
 */
double distance(Image *a, Image *b) {
    // TODO: Return correct distance
    double dist = 0;
    for (int i = 0; i < a->sy * a->sy; i++)
    {
        double d = a->data[i] - b->data[i];
        dist += d * d;
    }
    
    return sqrt(dist); 
}

/**
 * Given the input training dataset, an image to classify and K,
 *   (1) Find the K most similar images to `input` in the dataset
 *   (2) Return the most frequent label of these K images
 * 
 * Note: If there's multiple images with the same smallest values, pick the
 *      ones that come first. For automarking we will make sure the K smallest
 *      ones are unique, so it doesn't really matter.
 */ 

int max_index_double(double arr[], int size) {
    int max_index = 0;
    double max_val = 0;

    for (int i = 0; i < size; i++) {
        if (arr[i] > max_val) {
            max_index = i;
            max_val = arr[i];
        }
    }

    return max_index;
}

double max_val_double(double arr[], int size) {
    double max_val = 0;

    for (int i = 0; i < size; i++) {
        if (arr[i] > max_val) {
            max_val = arr[i];
        }
    }

    return max_val;
}

// algo from https://www.geeksforgeeks.org/frequent-element-array/
int mode(unsigned char arr[], int n) {
    // find the max frequency using linear traversal 
    int max_count = 1, curr_count = 1; 
    unsigned char res = arr[0];
    for (int i = 1; i < n; i++) { 
        if (arr[i] == arr[i - 1]) 
            curr_count++; 
        else { 
            if (curr_count > max_count) { 
                max_count = curr_count; 
                res = arr[i - 1]; 
            } 
            curr_count = 1; 
        } 
    } 
  
    // If last element is most frequent 
    if (curr_count > max_count) 
    { 
        max_count = curr_count; 
        res = arr[n - 1]; 
    } 
  
    return res; 
}

int knn_predict(Dataset *data, Image *input, int K) {
    unsigned char similar[K];
    double similar_dist[K];

    for (int i = 0; i < data->num_items; i++)
    {
        if (i < K) {
            similar[i] = data->labels[i];
            similar_dist[i] = distance(input, &data->images[i]);
        }
        else {
            double dist = distance(input, &data->images[i]);

            if (dist < max_val_double(similar_dist, K)) {
                int index = max_index_double(similar_dist, K);
                similar[index] = data->labels[i];
                similar_dist[index] = dist;
            }
        }
    }
    
    return mode(similar, K);
}

/** 
 * Free all the allocated memory for the dataset
 */
void free_dataset(Dataset *data) {
    free(data->labels);
    for (int i = 0; i < data->num_items; i++)
    {
        free(data->images[i].data);
    }
    free(data->images);
}
