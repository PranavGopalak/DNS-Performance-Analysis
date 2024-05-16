#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>

#include "util.h"

#define SIZE 2000

struct thread_params {
    int matrix[SIZE][SIZE]; 
    int n;
};


void* multiply_matrices(void* arg){
    int A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
    struct thread_params* params = (struct thread_params*) arg;
    for (int m = 0; m < params->n; m++) {
        // Perform matrix multiplication
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                C[i][j] = 0;
                for (int k = 0; k < SIZE; k++) {
                    C[i][j] += A[i][k] * B[k][j];
                }
            }
        }
    }
    return NULL;
}

int main() {
    int A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];

    int i, j;
    clock_t start, end;
    double cpu_time_used;

    int n1 = 500;
    int n2 = 1000;
    int n3 = 2000;

    // Call a processor and assign it a multiplication
    int processors = sysconf(_SC_NPROCESSORS_ONLN);

    pthread_t threads[processors];
    struct thread_params args[processors];
    // Initialize matrices A and B with random values
    for(int i = 0; i < processors; i++){
        args[i].n = n3;
        for (int j = 0; j < SIZE; j++) {
            for (int k = 0; k < SIZE; k++) {
                args[i].matrix[j][k] = rand() % 10;
            }
        }
        args[i].n = 1000;
    }

    for (i = 0; i < processors; i++) {
        void* arg = (void*)&args[i];
        pthread_create(&threads[i], NULL, multiply_matrices, arg);
    }

    for (i = 0; i < processors; i++) {
        pthread_join(threads[i], NULL);
    }
}