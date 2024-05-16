#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

#define NUM_MULTIPLICATIONS 1000

typedef struct {
    double *A;
    double *B;
    double *C;
    int n;
    int i;
} matrix_args;

void *worker(void *arg) {
    matrix_args *args = (matrix_args *)arg;
    for (int j = 0; j < args->n; ++j) {
        args->C[args->i * args->n + j] = 0;
        for (int k = 0; k < args->n; ++k) {
            args->C[args->i * args->n + j] += args->A[args->i * args->n + k] * args->B[k * args->n + j];
        }
    }
    pthread_exit(NULL);
}

void matrix_multiplication(int n, int num_processes, int num_multiplications) {
    // Allocate memory for matrices
    double *A = (double *)malloc(n * n * sizeof(double));
    double *B = (double *)malloc(n * n * sizeof(double));
    double *C = (double *)malloc(n * n * sizeof(double));

    for (int i = 0; i < n * n; ++i) {
        A[i] = (double)rand() / RAND_MAX;
        B[i] = (double)rand() / RAND_MAX;
    }

    clock_t start_time = clock();

    pthread_t *threads = (pthread_t *)malloc(num_processes * sizeof(pthread_t));
    matrix_args *args = (matrix_args *)malloc(num_processes * sizeof(matrix_args));

    for (int i = 0; i < floor(num_multiplications / num_processes); ++i) {
        for (int j = 0; j < num_processes; ++j) {
            args[j].A = A;
            args[j].B = B;
            args[j].C = C;
            args[j].n = n;
            args[j].i = j;
            pthread_create(&threads[j], NULL, worker, (void *)&args[j]);
        }

        for (int j = 0; j < num_processes; ++j) {
            pthread_join(threads[j], NULL);
        }
    }

    clock_t end_time = clock();

    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Time taken for %dx%d matrix multiplication with %d processes: %f seconds\n", n, n, num_processes, time_taken);
}

int main() {
    // Test for different matrix sizes
    int matrix_sizes[] = {500, 1000, 2500};
    int process_counts[] = {1, 6, 14, 20};
    int num_sizes = sizeof(matrix_sizes) / sizeof(matrix_sizes[0]);
    int num_processes = sizeof(process_counts) / sizeof(process_counts[0]);

    for (int i = 0; i < num_sizes; ++i) {
        for (int j = 0; j < num_processes; ++j) {
            matrix_multiplication(matrix_sizes[i], process_counts[j], NUM_MULTIPLICATIONS);
        }
    }

    return 0;
}
