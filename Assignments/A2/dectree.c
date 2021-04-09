#include "dectree.h"

/**
 * This function takes in the name of the binary file containing the data and
 * loads it into memory. The binary file format consists of the following:
 *
 *     -   4 bytes : `N`: Number of images / labels in the file
 *     -   1 byte  : Image 1 label
 *     - 784 bytes : Image 1 data (28x28)
 *          ...
 *     -   1 byte  : Image N label
 *     - 784 bytes : Image N data (28x28)
 *
 * You can simply set the `sx` and `sy` values for all the images to 28, we
 * will not test this with different image sizes.
 */
Dataset *load_dataset(const char *filename) {
    // TODO: Allocate data, read image data / labels, return

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
        dataset->images[i].data = malloc(sizeof(unsigned char) * 784);

        for (int j = 0; j < 784; j++) {
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

/**
 * This function computes and returns the Gini impurity of the M images
 * (with their indices stored in the array) if they were to be split
 * based on the given pixel.
 * 
 * DO NOT CHANGE THIS FUNCTION; It is already implemented for you.
 */
double split_gini_impurity(Dataset *data, int M, int indices[M], int pixel) {
  int a_freq[10] = {0}, a_count = 0;
  int b_freq[10] = {0}, b_count = 0;
  for (int i = 0; i < M; i++) {
    int img_idx = indices[i];
    // The pixels are always either 0 or 255, but using < 128 for generality.
    if (data->images[img_idx].data[pixel] < 128)
      a_freq[data->labels[img_idx]]++, a_count++;
    else
      b_freq[data->labels[img_idx]]++, b_count++;
  }

  double a_gini = 0, b_gini = 0;
  for (int i = 0; i < 10; i++) {
    double a_i = ((double)a_freq[i]) / ((double)a_count);
    double b_i = ((double)b_freq[i]) / ((double)b_count);
    if (a_count) a_gini += a_i * (1 - a_i);       // This was changed
    if (b_count) b_gini += b_i * (1 - b_i);       // This was changed
  }
  // Weighted average of gini impurity of children
  return (a_gini * a_count + b_gini * b_count) / M;
}


// Helper functions

int max_index(int arr[], int size) {
    int max_index = 0;
    int max_val = 0;

    for (int i = 0; i < size; i++) {
        if (arr[i] > max_val) {
            max_index = i;
            max_val = arr[i];
        }
    }

    return max_index;
}

int min_index(int arr[], int size) {
    int min_index = 0;
    int min_val = arr[0];

    for (int i = 1; i < size; i++) {
        if (arr[i] < min_val) {
            min_index = i;
            min_val = arr[i];
        }
    }

    return min_index;
}

int min_index_double(double arr[], int size) {
    int min_index = 0;
    double min_val = arr[0];

    for (int i = 1; i < size; i++) {
        if (arr[i] < min_val) {
            min_index = i;
            min_val = arr[i];
        }
    }

    return min_index;
}


int max_val(int arr[], int size) {
    int max_val = 0;

    for (int i = 0; i < size; i++) {
        if (arr[i] > max_val) {
            max_val = arr[i];
        }
    }

    return max_val;
}

/**
 * Given a subset of M images and their corresponding indices, find and return
 * the most frequent label and it's frequency. The last 2 parameters are
 * only for outputs.
 *
 * - The most frequent label should be stored in `*label`
 * - The frequency of this label should be stored in `*freq`
 * 
 * If multiple labels have the same maximal frequency, return the smallest one.
 */
void get_most_frequent(Dataset *data, int M, int indices[M], 
                       int *label, int *freq) {
    // TODO: Set the correct values and return
    int *labels = calloc(sizeof(int), 10);
    if (labels == NULL) {
        perror("labels calloc");
        exit(1);
    }

    for (int i = 0; i < M; i++)
    {
        labels[data->labels[indices[i]]]++;
    }

    *label = max_index(labels, 10);
    *freq = max_val(labels, 10);

    free(labels);
}

/**
 * Given a subset of M images and their corresponding indices, find and return
 * the best pixel to split the data based on. The best pixel is the one which
 * has the minimum Gini impurity as computed by `split_gini_impurity()`.
 * 
 * The return value should be a number between 0-783 (inclusive), representing
 *  the pixel the M images should be split based on.
 * 
 * If multiple pixels have the same minimal Gini impurity, return the smallest.
 */
int find_best_split(Dataset *data, int M, int indices[M]) {
    // TODO: Return the correct pixel 

    double gini_arr[784];

    for (int i = 0; i < 784; i++)
    {
        gini_arr[i] = split_gini_impurity(data, M, indices, i);
    }

    return min_index_double(gini_arr, 784);
}

/**
 * This is the recursive function that creates the Decision tree. In each
 * recursive call, we only want to consider some subset of the entire dataset
 * corresponding to the node. To represent this, we pass in an array of indices
 * of these images in the dataset, along with it's length M. Be careful to
 * allocate this on the array for any recursive calls made, and free it when
 * you're done with creating the tree. Here, you need to:
 *
 *    - Compute ratio of most frequent image in current call, terminate if >95%
 *    - Find the best pixel to split on using `split_gini_impurity()`
 *    - Split the data, allocate corresponding arrays, and recurse
 *    - In either case, allocate a new node, set the correct values and return
 *       - If it's a child, set `classification`, and both children = NULL.
 *       - If it's not a child, set `pixel` and `left`/`right` (recursively). 
 */
DTNode *make_helper(Dataset *data, int M, int indices[M]) {
    DTNode *node = malloc(sizeof(DTNode));
    if (node == NULL) {
        perror("malloc node");
        exit(1);
    }

    // TODO: Construct and return tree
    int *label = malloc(sizeof(int));
    int *freq = malloc(sizeof(int));
    get_most_frequent(data, M, indices, label, freq);

    if ((double)(*freq) / (double)M >= TERMINATE_RATIO) {
        node->left = NULL;
        node->right = NULL;
        node->classification = *label;
    }
    else {
        node->pixel = find_best_split(data, M, indices);

        int M2 = 0;
        for (int i = 0; i < M; i++)
        {
            if (data->images[indices[i]].data[node->pixel] == 0) {
                M2++;
            }
        }

        int *indicesLeft = malloc(sizeof(int) * M2);
        int *indicesRight = malloc(sizeof(int) * (M - M2));
        int indexLeft = 0;
        int indexRight = 0;

        for (int i = 0; i < M; i++)
        {
            if (data->images[indices[i]].data[node->pixel] == 0) {
                indicesLeft[indexLeft] = indices[i];
                indexLeft++;
            }
            else {
                indicesRight[indexRight] = indices[i];
                indexRight++;
            }
        }

        node->left = make_helper(data, M2, indicesLeft);
        node->right = make_helper(data, M - M2, indicesRight);

        free(indicesLeft);
        free(indicesRight); 
    }

    free(label);
    free(freq);

    return node; 
}

/**
 * This is the function exposed to the user. All you should do here is set
 * up the `indices` array correctly for the entire dataset and call 
 * `make_helper()` with the correct parameters.
 */
DTNode *make_dec_tree(Dataset *data) {
    // TODO: Set up `indices` array, call `make_helper` and return tree.
    //     Make sure you free any data that is not needed anymore (hint)

    int *indices = malloc(sizeof(int) * data->num_items);
    for (int i = 0; i < data->num_items; i++)
    {
        indices[i] = i;
    }

    DTNode *node = make_helper(data, data->num_items, indices);

    free(indices);

    return node;
}

/**
 * Given a decision tree and an image to classify, return the predicted label.
 */
int dec_tree_classify(DTNode *root, Image *img) {
    // TODO: Return the correct label

    while (!(root->left == NULL && root->right == NULL)) {
        if (img->data[root->pixel] == 0) {
            root = root->left;
        }
        else {
            root = root->right;
        }
    }

    return root->classification;
}

/**
 * This function frees the Decision tree.
 */
void free_dec_tree(DTNode *node) {
    // TODO: Free the decision tree
    if (node->left != NULL) {
        free_dec_tree(node->left);
    }
    if (node->right != NULL) {
        free_dec_tree(node->right);
    }
    free(node);
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
    free(data);
}