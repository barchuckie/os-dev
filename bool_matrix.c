#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

int* matrixA;
int* matrixB;
int size;
int row;
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

int get_idx(int i, int j, int columns) {
    return (i + j*columns);
}

int* bool_multiplication(void *arg) {
    int* res = (int *) arg;
    int i;

    while(row < size) {
        pthread_mutex_lock(&mymutex);
        i = row;
        row++;
        pthread_mutex_unlock(&mymutex);

        for(int j = 0; j < size; ++j) {
            for(int k = 0; k < size; ++k) {
                if(matrixA[get_idx(i, k, size)]*matrixB[get_idx(k, j, size)] == 1) {
                    res[get_idx(i, j, size)] = 1;
                    break;
                }
                res[get_idx(i, j, size)] = 0;
            }
        }
    }

    return res;
}

int main(int argc, char const *argv[]) {
    if(argc < 3) {
        printf("Insert two arguments:\n1. Size\n2. Number of threads");
        exit(1);
    }
    int threads_count, i;
    void *retval;
    pthread_t* threads;
    int* res;

    size = atoi(argv[1]);
    threads_count = atoi(argv[2]);
    row = 0;

    res = malloc(size*size*sizeof(int));
    matrixA = malloc(size*size*sizeof(int));
    matrixB = malloc(size*size*sizeof(int));
    threads = (pthread_t*) malloc(threads_count*sizeof(pthread_t));

    srandom(time(0));

    for(i = 0; i < size; ++i) {
        for(int j = 0; j < size; ++j) {
            matrixA[get_idx(i, j, size)] = random()&01;
            matrixB[get_idx(i, j, size)] = random()&01;
        }
    }

    for(i = 0; i < threads_count; ++i) {
        pthread_create(threads+i, NULL, (void *) bool_multiplication, (void *) res);
    }

    for(i = 0; i < threads_count; ++i) {
        pthread_join(threads[i], &retval);
    }

    matrixA = NULL;
    matrixB = NULL;
    size = 0;
    row = 0;

    return 0;
}
