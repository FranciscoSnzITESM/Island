#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h> 
#include <time.h>

const int island[8][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 2, 1, 2, 1, 2, 1, 0},
    {0, 2, 2, 2, 3, 3, 3, 1, 2, 0},
    {0, 2, 1, 2, 3, 4, 3, 2, 2, 0},
    {0, 3, 3, 3, 3, 3, 3, 2, 1, 0},
    {0, 3, 4, 3, 1, 2, 2, 2, 1, 0},
    {0, 3, 3, 3, 1, 2, 1, 2, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

int positions[8][10] = {
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

pthread_mutex_t lockTurn;

struct ballData {
    pthread_t tid;
    long id;
    int x;
    int y;
};

struct ballData *balls;

void printArray(int array[][10]){
    int i, j;
    for(i = 0; i < sizeof(island)/sizeof(island[0]);i++){
        for (j =0; j < sizeof(island[i])/sizeof(island[i][0]); j++){
            printf("%d, ", array[i][j]);
        }
        printf("\n");
    }
}

void setColor(int level){
    switch(level) {
        case 0:
            printf("\033[0;34m"); // Set to blue
            break;
        case 1:
            printf("\033[0;36m"); // Set to cyan
            break;
        case 2:
            printf("\033[0;32m"); // Set to green 
            break;
        case 3:
            printf("\033[0;33m"); // Set to yellow
            break;
        case 4:
            printf("\033[0;31m"); // Set to red
            break;
        case 5:
             printf("\033[0;35m"); // Set to magenta
            break;
        default:
            printf("\033[0m");
    }
}

void printStatus(){
    int i, j;
    for(i = 0; i < sizeof(island)/sizeof(island[0]);i++){
        for (j =0; j < sizeof(island[i])/sizeof(island[i][0]); j++){
            setColor(island[i][j]);
            printf("%d", island[i][j]);
            printf("\033[0m");
            printf(", ");
        }
        printf("\t\t");
        for (j =0; j < sizeof(island[i])/sizeof(island[i][0]); j++){
            setColor(island[i][j]);
            if(positions[i][j] == -1){
                printf("_");
            }else{
                printf("%d", positions[i][j]);
            }
            printf("\033[0m");
            printf(", ");
        }
        printf("\n");
    }
}

int getDirection(int x, int y){
    //dir :: N=0 S=1 E=2 W=3
    int dir = 0; // North is the first one
    int currH = island[x-1][y]; 
    if(island[x+1][y] < currH) { // Check if South is better
        dir = 1;
        currH = island[x+1][y];
    }
    if(island[x][y+1] < currH) { // Check if East is better
        dir = 2;
        currH = island[x][y+1];
    }
    if(island[x][y-1] < currH) { // Check if West is better
        dir = 3;
    }
    return dir;
}

void collision(struct ballData *ball1, int oldX, int oldY, struct ballData *ball2);

void getXY(int dir, int oldX, int oldY, int *newX, int *newY){
    switch (dir){
        case 0: // N
            *newX = oldX-1;
            *newY = oldY;
            break;
        case 1: // S
            *newX = oldX+1;
            *newY = oldY;
            break;
        case 2: // E
            *newX = oldX;
            *newY = oldY+1;
            break;
        case 3: // W
            *newX = oldX;
            *newY = oldY-1;
            break;
    }
}

void tryMoving(struct ballData *ball, int oldX, int oldY, int newX, int newY){
    if (positions[newX][newY] == -1){// No collision
        positions[newX][newY] = ball->id;
        if (positions[ball->x][ball->y] == ball->id){
            positions[ball->x][ball->y] = -1;
        }
        ball->x = newX;
        ball->y = newY;
    } else{ // Crashing
        setColor(4);
        printf("Collision!\n");
        setColor(-1);
        struct ballData *ball2 = &balls[positions[newX][newY]];
        collision(ball, newX, newY, ball2);
    }
}

void collision(struct ballData *ball1, int oldX, int oldY, struct ballData *ball2){
    int dir1 = rand() % 4;
    int dir2;
    while((dir2 = rand() % 4) == dir1);
    int newX, newY;
    getXY(dir1, oldX, oldY, &newX, &newY);
    tryMoving(ball1, oldX, oldY, newX, newY);
    getXY(dir2, oldX, oldY, &newX, &newY);
    tryMoving(ball2, oldX, oldY, newX, newY);
}

void *ballBehaviour(void *threadId) { 
    long tid = (long)threadId;
    int finished = 1;
    int alreadyInWater = 0;
    struct ballData *ball = &balls[tid];
    // First position
    while(finished){
        int x = rand() % sizeof(island)/sizeof(island[0]);
        int y = rand() % sizeof(island[0])/sizeof(island[0][0]);
        pthread_mutex_lock(&lockTurn);
        if(positions[x][y] == -1){
            printf("Init pos ball %ld: [%d][%d]\n", tid, x, y);
            positions[x][y] = tid;
            ball->x = x;
            ball->y = y;
            finished = 0;
                // Check if landed in water from the start
            if(island[ball->x][ball->y] == 0){
                alreadyInWater = 1;
                positions[ball->x][ball->y] = -1;
                printf("Landed directly in water\n");
            }
        }
        pthread_mutex_unlock(&lockTurn);
    }
    finished = 1;
    if(alreadyInWater){
        finished = 0;
    }
    // Movement
    int speed = 1000; // 1 second
    while(finished){
        usleep(speed * 1000); // sleep in microseconds
        pthread_mutex_lock(&lockTurn);
        printf("Ball %ld moving from [%d][%d] : level %d\n", tid, ball->x, ball->y, island[ball->x][ball->y]);
        int currHeight = island[ball->x][ball->y];
        int direction = getDirection(ball->x, ball->y);
        int newX, newY;
        getXY(direction, ball->x, ball->y, &newX, &newY);
        if(island[newX][newY] <= island[ball->x][ball->y]){ // Check if height is possible
            tryMoving(ball, ball->x, ball->y, newX, newY);
            printf("To [%d][%d] : level %d\n", balls[tid].x, balls[tid].y, island[ball->x][ball->y]);
            if(island[ball->x][ball->y] == 0){
                finished = 0;
                positions[ball->x][ball->y] = -1;
                printf("Landed in water\n");
            }
            printStatus();
        } else {
            printf("Ball %ld stucked in island at [%d][%d] : level %d\n", tid, ball->x, ball->y, island[ball->x][ball->y]);
            finished = 0;
            positions[ball->x][ball->y] = -1;
        }
        pthread_mutex_unlock(&lockTurn);
    }
    pthread_exit(NULL);
} 

void *printFirstMap(void *threadId){
    usleep(1000 * 500);
    printStatus();
    pthread_exit(NULL);
}

int main(int argc, char** argv){
    if(argc < 2) {
        printf("Error, missing args\n");
        printf("Usage: ./island [numberOfBalls]\n");
        return -1;
    }
    srand(time(0));
    int n = atoi(argv[1]);
    if(n <= 0){
        printf("Error, number of balls must a number or be higher than 0\n");
        return -1;
    }

    // pthread_t threads[n];
    balls = malloc(sizeof(struct ballData)*n);
    int rc;
    long t;
    for(t=0; t<n; t++){
        // printf("In main: creating thread %ld\n", t);
        rc = pthread_create(&(balls[t].tid), NULL, ballBehaviour, (void *)t);
        balls[t].id = t;
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
    pthread_t firstMapPrint;
    pthread_create(&firstMapPrint, NULL, printFirstMap, NULL);
    pthread_join(firstMapPrint, NULL);
    for(t=0;t<n;t++){
        pthread_join(balls[t].tid, NULL);
    }
    return 0;
}