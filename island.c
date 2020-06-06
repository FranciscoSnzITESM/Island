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

pthread_mutex_t lockTurn;

struct ballData {
    pthread_t id;
    int x;
    int y;
};

struct ballData* balls;

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
    while(1){
        int x = rand() % 5;
        int y = rand() % 7;
        printf("init pos: [%d][%d]\n", x, y);

        pthread_mutex_lock(&lockTurn);
        if(positions[x][y] == 0){
            positions[x][y] = 1;
        }
        pthread_mutex_unlock(&lockTurn);
    }
    

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

    int n = atoi(argv[1]);
    if(n <= 0){
        printf("Error, number of balls must a number or be higher than 0\n");
        return -1;
    }

    // pthread_t threads[n];
    balls = malloc(sizeof(struct ballData)*n);
    int rc;
    long t;
    pthread_t id;
    for(t=0; t<n; t++){
        // printf("In main: creating thread %ld\n", t);
        id = balls[t]->id;
        rc = pthread_create(&id, NULL, ballBehaviour, (void *)t);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    for(t=0;t<n;t++){
        pthread_join(balls[t]->id, NULL);
    }
    
    printf("Main for Island.c\n");

    return 0;
}

