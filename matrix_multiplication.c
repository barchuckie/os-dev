#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

int get_idx(int i, int j, int size) {
    return (i + j*size);
}


int* transpose(int* matrix, int size) {
    int* transposed = malloc(size*size*sizeof(int));

    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j) {
            transposed[get_idx(i, j, size)] = matrix[get_idx(j, i, size)];
        }
    }

    return transposed;
}

int* multiplicate(int* matrixA, int* matrixB, int size) {
    int r;
    int* result_matrix = malloc(size*size*sizeof(int));
    //producing one row at once #cache_optimization

    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j) {
            result_matrix[get_idx(i, j, size)] = 0;
        }
    }

    for (int i = 0; i < size; i++) {
        for(int k = 0; k < size; k++) {
            r = matrixA[get_idx(i, k, size)];
            for(int j = 0; j < size; j++) {
                result_matrix[get_idx(i, j, size)] += r*matrixB[get_idx(k, j, size)];
            }
        }
    }

    return result_matrix;
}

int* x_multiplicate(int* matrixA, int* matrixB, int size) {
    int r;
    int* result_matrix = malloc(size*size*sizeof(int));
    //producing one row at once #cache_optimization
    int* transposedA = transpose(matrixA, size);
    int* transposedB = transpose(matrixB, size);

    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j) {
            result_matrix[get_idx(i, j, size)] = 0;
        }
    }

    for (int i = 0; i < size; i++) {
        for(int k = 0; k < size; k++) {
            r = transposedA[get_idx(k, i, size)];
            for(int j = 0; j < size; j++) {
                result_matrix[get_idx(j, i, size)] += r*transposedB[get_idx(j, k, size)];
            }
        }
    }

    return transpose(result_matrix, size);
}


int* transposed_multiplicate(int* matrixA, int* matrixB, int size) {
    int r;
    int* result_matrix = malloc(size*size*sizeof(int));
    //producing one row at once #cache_optimization

    int* transposedB = transpose(matrixB, size);
    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j) {
            result_matrix[get_idx(i, j, size)] = 0;
        }
    }

    for (int i = 0; i < size; i++) {
        for(int k = 0; k < size; k++) {
            r = matrixA[get_idx(i, k, size)];
            for(int j = 0; j < size; j++) {
                //swapping j with k in matrixB index
                result_matrix[get_idx(i, j, size)] += r*transposedB[get_idx(j, k, size)];
            }
        }
    }

    return result_matrix;
}


int main(int argc, char const *argv[]) {
    if(argc < 2) {
        printf("Argument needed");
    }
    int size;
    size = atoi(argv[1]);
    struct timeval init_val;
    struct timeval end_val;

    int* matrixA = malloc(size*size*sizeof(int));
    int* matrixB = malloc(size*size*sizeof(int));

    srand(time(0));

    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j) {
            matrixA[get_idx(i, j, size)] = rand();
            matrixB[get_idx(i, j, size)] = rand();
        }
    }

    gettimeofday(&init_val, NULL);

    int* res_x = x_multiplicate(matrixA, matrixB, size);

    gettimeofday(&end_val, NULL);
    printf("X Multiplication took: %ld seconds.\n", end_val.tv_sec - init_val.tv_sec);

    gettimeofday(&init_val, NULL);

    int* res1 = transposed_multiplicate(matrixA, matrixB, size);

    gettimeofday(&end_val, NULL);
    printf("Multiplication with transposition took: %ld seconds.\n", end_val.tv_sec - init_val.tv_sec);

    gettimeofday(&init_val, NULL);

    int* res2 = multiplicate(matrixA, matrixB, size);

    gettimeofday(&end_val, NULL);
    printf("Multiplication took: %ld seconds.\n", end_val.tv_sec - init_val.tv_sec);

    for(int i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j) {
            if(res_x[get_idx(i, j, size)] != res2[get_idx(i, j, size)]) {
                printf("[%d][%d]\n", i, j);
                exit(1);
            }
        }
    }

    return 0;
}
