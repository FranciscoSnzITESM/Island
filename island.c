#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> 

const int island[5][7] = {{0, 0, 0, 0, 0, 0, 0},
                          {0, 1, 1, 1, 1, 1, 0},
                          {0, 1, 2, 3, 2, 1, 0},
                          {0, 1, 1, 1, 1, 1, 0},
                          {0, 0, 0, 0, 0, 0, 0}};

int positions[5][7] = {0};

void printArray(int array[][7]){
    int i, j;
    for(i = 0; i < 5;i++){
        for (j =0; j < 7; j++){
            printf("%d, ", array[i][j]);
        }
        printf("\n");
    }
}

void *ballBehaviour(void *threadId) 
{ 
    long tid = (long)threadId;
    int speed = 1000; // 1 second
    int finished = 1;
    int x = rand() % 5;
    int y = rand() % 7;
    printf("init pos: [%d][%d]\n", x, y);
    while(finished){
        usleep(speed * 1000); // sleep in microseconds
        printf("Printing GeeksQuiz from Thread %ld \n", tid);
        
        // printf("random number: %d\n", rand()%13);
    }
    pthread_exit(NULL);
} 

int main(int argc, char** argv){

    if(argc < 2) {
        printf("Error, missing args\n");
        printf("Usage: ./island [numberOfBalls]\n");
        return -1;
    }

    int balls = atoi(argv[1]);
    if(balls <= 0){
        printf("Error, number of balls must a number or be higher than 0\n");
        return -1;
    }

    pthread_t threads[balls];
    int rc;
    long t;
    for(t=0; t<balls; t++){
        // printf("In main: creating thread %ld\n", t);
        rc = pthread_create(&threads[t], NULL, ballBehaviour, (void *)t);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    for(t=0;t<balls;t++){
        pthread_join(threads[t], NULL);
    }
    
    printf("Main for Island.c\n");

    return 0;
}

