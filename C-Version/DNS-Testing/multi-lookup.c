/*
 * File: multi-lookup.c
 * Author: Pranav Gopal
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#include "util.h"
#include "multi-lookup.h"

#define MINARGS 3
#define USAGE "<inputFilePath> <outputFilePath>"
#define SBUFSIZE 1025
#define INPUTFS "%1024s"

struct thread_params{
    pthread_t thread_number;
    FILE* file;
    char* filename;
    queue* q;
};

queue host_queue;
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;
bool producer_done = false;

int getRandomNumber(int min, int max) {
    return rand() % (max - min + 1) + min;
}

int enqueue(queue* q, char* hostname){
    pthread_mutex_lock(&queue_mutex);
    if(queue_is_full(q)){
        pthread_mutex_unlock(&queue_mutex);
        return -1;
    }
    queue_push(q,hostname);
    pthread_mutex_unlock(&queue_mutex);
    return 0;
}

void* producer_thread(void* arg){
    struct thread_params* params = (struct thread_params*) arg;
    char hostname[SBUFSIZE];
    FILE* file = fopen(params->filename, "r");
    if(file == NULL){
        fprintf(stderr, "Error opening file: %s\n", params->filename);
        return NULL;
    }
    while(fscanf(file, INPUTFS, hostname) > 0){
        char *payload = malloc(SBUFSIZE);
        if(!payload){
            printf("Error: Payload NULL");
            return NULL;
        }
        strncpy(payload, hostname, SBUFSIZE);
        while(enqueue(params->q, payload) == -1){
            usleep(getRandomNumber(0,100) * 1000);
        }
    }
    fclose(file);
    return NULL;
}

char* dequeue(queue* q){
    pthread_mutex_lock(&queue_mutex);
    if(queue_is_empty(q)){
        pthread_mutex_unlock(&queue_mutex);
        return NULL;
    }
    char *hostname = queue_pop(q);
    pthread_mutex_unlock(&queue_mutex);
    return hostname;
}

void* consumer_thread(void* arg){
    struct thread_params* params = (struct thread_params*) arg;
    char* hostname;
    while(!producer_done) {
        //hostname = dequeue(params->q);
        while ( (hostname = dequeue(params->q)) != NULL) {
            char firstipstr[INET6_ADDRSTRLEN];
            if (dnslookup(hostname, firstipstr, sizeof(firstipstr))
                == UTIL_FAILURE) {
                fprintf(stderr, "dnslookup error: %s\n", hostname);
                strncpy(firstipstr, "", sizeof(firstipstr));
            }
            pthread_mutex_lock(&file_mutex);
            fprintf(params->file, "%s,%s\n", hostname, firstipstr);
            fflush(params->file);
            pthread_mutex_unlock(&file_mutex);
            free(hostname);
        }
    }
    return NULL;
}

int main(int argc, char* argv[]){
    /* Local Vars */
    FILE* outputfp = NULL;
//    char errorstr[SBUFSIZE];
//    char firstipstr[INET6_ADDRSTRLEN];
    int i;
    pthread_mutex_init(&queue_mutex, NULL);
    pthread_mutex_init(&file_mutex, NULL);
    /* Initialize Queue */
    if(queue_init(&host_queue, 500) == QUEUE_FAILURE){
        fprintf(stderr, "queue init failed\n");
    }

    /* Check Arguments */
    if(argc < MINARGS){
        fprintf(stderr, "Not enough arguments: %d\n", (argc - 1));
        fprintf(stderr, "Usage:\n %s %s\n", argv[0], USAGE);
        return EXIT_FAILURE;
    }

    /* Open Output File */
    outputfp = fopen(argv[(argc-1)], "w");
    if(!outputfp){
        perror("Error Opening Output File");
        return EXIT_FAILURE;
    }
    

    int threadcounts[4] = {1, 6, 14, 20};
    for(int i = 0; i < 4; i++){
        int num_request_threads = argc - 2;
        struct thread_params request[num_request_threads];
        /* Loop Through Input Files */
        for(i=0; i<(num_request_threads); i++){
            request[i].filename = argv[i+1];
            request[i].q = &host_queue;
            pthread_create( &request[i].thread_number, NULL, producer_thread, &request[i]);
        }
        int num_resolver_threads = threadcounts[i];
        struct thread_params resolver[num_resolver_threads];
        /* Creater resolver threads */
        for(i=0; i<(num_resolver_threads); i++){
            resolver[i].file = outputfp;
            resolver[i].q = &host_queue;
            pthread_create( &resolver[i].thread_number, NULL, consumer_thread, &resolver[i]);

        }
        for(i=0; i<num_request_threads; i++){
            pthread_join(request[i].thread_number, NULL);
        }
        producer_done = true;
        for(i=0; i<num_resolver_threads; i++){
            pthread_join(resolver[i].thread_number, NULL);
        }
        fclose(outputfp);
        pthread_mutex_destroy(&queue_mutex);
        pthread_mutex_destroy(&file_mutex);
    }

    return EXIT_SUCCESS;
}
