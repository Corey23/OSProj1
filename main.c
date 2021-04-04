#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "customer.h"
#include "queue.h"


/* this data is shared by the thread(s) */
int *bids, *wins, *occupied, *shouldbid;
pthread_t *tid;
int ready = 0, currItem = 0;
pthread_mutex_t mutex1, mutex2;
unsigned short xsub1[3];
struct Customer* customs;
struct Queue* queue;

void *runner(void *param); /* the thread */

int main(int argc, char *argv[]) {
    queue = createQueue(100);
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex1, NULL);
    time_t t;
    srand((unsigned) time(&t));
    xsub1[0] = (short) (rand() % 256);
    xsub1[1] = (short) (rand() % 256);
    xsub1[2] = (short) (rand() % 256);


    FILE *in_file = fopen("input.txt","r");


    if (argc != 2) {
        fprintf(stderr, "incorrect number of input arguments\n");
        /*exit(1);*/
        return -1;
    }

    int numThreads, itemTypes, numC, numRounds, order, extra;
    fscanf(in_file, "%d", &numThreads);
    fscanf(in_file, "%d", &itemTypes);
    fscanf(in_file, "%d", &numC);
    fscanf(in_file, "%d", &numRounds);
    fscanf(in_file, "%d", &order);
    fscanf(in_file, "%d", &extra);


    int id, money, numItems, temp;
    while(!feof(in_file)){
        fscanf(in_file, "%d", &id);
        fscanf(in_file, "%d", &money);
        fscanf(in_file, "%d", &numItems);

        struct Customer c;

        c.items = (int *) malloc(numItems * sizeof(int));
        c.numItems = numItems;
        c.id = id;
        c.money = money;
        c.wins = 0;
        for(int i = 0; i < numItems; i++){
            fscanf(in_file, "%d", &temp);
            c.items[i] = temp;
        }
        enqueue(queue, c);


    }


    tid = malloc(sizeof(pthread_t) * numThreads); /* the thread identifier */
    pthread_attr_t attr; /* set of attributes for the thread */

/* get the default attributes */
    pthread_attr_init(&attr);


    /*Allocates Variables*/
    customs = (struct Customer*) malloc(numThreads * sizeof(struct Customer));
    bids = (int *) malloc(numThreads * sizeof(int));
    shouldbid = (int *) malloc(numThreads * sizeof(int));
    occupied = (int *) malloc(numThreads * sizeof(int));
    wins = (int *) malloc(numThreads * sizeof(int));
    int *scores = (int *) malloc(numThreads * sizeof(int));
    int moneyEarned = 0, itemsAuctioned = 0;

    /*Sets the scores to 0*/
    for (int i = 0; i < numThreads; i++) {
        scores[i] = 0;
    }

    /*Sets everyone to loser to start*/
    for (int j = 0; j < numThreads; j++) {
        wins[j] = 0;
    }

    /*Sets Occupied to 0 to show no table is occupied*/
    for (int i = 0; i < numThreads; i++) {
        occupied[i] = 0;
    }

/* create the thread */
    for (int i = 0; i < numThreads; i++) {
        *bids = i;

        pthread_create(&(tid[i]), &attr, runner, bids);
        sleep(0.5); // Used to give time for threads to initialize
        //printf("Thread %d created.\n", i+1);

    }

    for (int i = 0; i < numThreads; i++) {
        if(!isEmpty(queue)){
            customs[i] = dequeue(queue);
            occupied[i] = 1;
        }
    }


    int MAX = 0;

    if(order == 1) {
        /*Loop Through Rounds*/
        currItem = 1;

        for (int i = 0; i < numRounds; i++) {
            int numbids = 0;
            printf("the following customers are eligible for this round of auction: ");
            for (int j = 0; j < numThreads; j++) {
                bids[j] = -1;
                if(occupied[j] == 1){
                    shouldbid[j] = 0;
                    numbids++;
                    printf(" %d ", customs[j].id);
                }
            }


            printf("\nItem of type %d up for auction\n", currItem);
            ready = 1;
            for (int j = 0; j < numThreads; j++) {

                while (bids[j] < 0) {
                    /*
                     Could Not Ensure that the thread wouldn't recalculate another random variable
                     The if statement attempts to check a two seperate locations to see if a new random number has been generated

                    if(x[numThreads-1] > 0 && x[(numThreads-1)/2] > 0 ) {
                      break;
                    //}
                     *************************************/
                    sleep(0.5); // Gives time for each to generate a number
                }

            }
            ready = 0;
            for(int j = 0; j < numThreads; j++){
                printf("bid %d: %d\n", customs[j].id, bids[j]);
            }

            sleep(0.5); // Sleeps so output can be organized
            if(numbids > 1){
                itemsAuctioned++;
                printf("Second Round of bidding needed\n");
                ready = 2;
                for (int j = 0; j < numThreads; j++) {

                    while (bids[j] < 0) {
                        /*
                         Could Not Ensure that the thread wouldn't recalculate another random variable
                         The if statement attempts to check a two seperate locations to see if a new random number has been generated

                        if(x[numThreads-1] > 0 && x[(numThreads-1)/2] > 0 ) {
                          break;
                        //}
                         *************************************/
                        sleep(0.5); // Gives time for each to generate a number
                    }

                }
                ready = 0;
                for(int j = 0; j < numThreads; j++){
                    printf("bid %d: %d\n", customs[j].id, bids[j]);
                }
            }
            for (int n = 0; n < numThreads; n++) {
                if (bids[n] > bids[MAX]) {
                    MAX = n;
                }
            }
            moneyEarned += bids[MAX];

            printf("Customer %d won the auction, with amount %d.\n", customs[MAX].id, bids[MAX]);

            wins[MAX] = 1;

            printf("\n\n");
            MAX = 0;

            currItem++;
            if(currItem > itemTypes){
                currItem = 1;
            }
        }
    }
    else {
        /*Loop Through Rounds*/

        for (int i = 0; i < numRounds; i++) {
            currItem = (jrand48(xsub1) % itemTypes) + 1;
            int numbids = 0;
            printf("the following customers are eligible for this round of auction: ");
            for (int j = 0; j < numThreads; j++) {
                bids[j] = -1;
                if(occupied[j] == 1){
                    shouldbid[j] = 1;
                    numbids++;
                    printf(" %d ", customs[j].id);
                }
            }


            printf("\nItem of type %d up for auction\n", currItem);
            ready = 1;
            for (int j = 0; j < numThreads; j++) {

                while (bids[j] < 0) {
                    /*
                     Could Not Ensure that the thread wouldn't recalculate another random variable
                     The if statement attempts to check a two seperate locations to see if a new random number has been generated

                    if(x[numThreads-1] > 0 && x[(numThreads-1)/2] > 0 ) {
                      break;
                    //}
                     *************************************/
                    sleep(0.5); // Gives time for each to generate a number
                }

            }
            ready = 0;

            sleep(0.5); // Sleeps so output can be organized
            if(numbids > 1){
                printf("Second Round of bidding needed\n");
                ready = 2;
                for (int j = 0; j < numThreads; j++) {

                    while (bids[j] < 0) {
                        /*
                         Could Not Ensure that the thread wouldn't recalculate another random variable
                         The if statement attempts to check a two seperate locations to see if a new random number has been generated

                        if(x[numThreads-1] > 0 && x[(numThreads-1)/2] > 0 ) {
                          break;
                        //}
                         *************************************/
                        sleep(0.5); // Gives time for each to generate a number
                    }

                }
                ready = 0;

            }
            for (int n = 0; n < numThreads; n++) {
                if (bids[n] > bids[MAX]) {
                    MAX = n;
                }
            }

            printf("Customer %d won the auction, with amount %d.\n", customs[MAX], bids[MAX]);
            wins[MAX] = 1;

            printf("\n\n");
            MAX = 0;

        }
    }

    printf("Total number of items auctioned: %d\n", numRounds);
    printf("Total number of items succesfully auctioned: %d\n", itemsAuctioned);
    printf("Total Amount: %d\n", moneyEarned);

    // Indicates Game Is Over
    printf("\nAll Auctions Over\n\n");
    ready = 3;


/* now wait for the thread to exit */
    for (int i = 0; i < numThreads; i++)
    {
        pthread_join(tid[i],NULL);
        //printf("Parent: Joined thread %d\n", i);
    }
    printf("\nProgram Ended\n");


    return 0;
}



/**
 * The thread will begin control in this function
 */
void *runner(void *param)
{
    int q = *((int *) param);
    int loc = 0;
    int moneySpent = 0;

    //While game not over
    while( ready < 3) {
        pthread_mutex_lock(&mutex1);
        if(occupied[q] == 0){
            customs[q] = dequeue(queue);
            occupied[q] = 1;
            customs[q].wins = 0;
            moneySpent = 0;
        }
        pthread_mutex_unlock(&mutex1);
        // 1 indicates Readiness - 2 indicates second bid round

        while(occupied[q] == 1 && ready < 3) {
            int num = 0; // stores bidding amouunt
            for(int i = 0; i < customs[q].numItems; i++){
                if(currItem == customs[q].items[i]){
                    shouldbid[q] = 1;
                    loc = i;
                }
            }
            pthread_mutex_lock(&mutex2);
            if(ready == 1) {
                if(shouldbid[q] == 1){
                    num = (jrand48(xsub1) % customs[q].money) + 1;
                }
                else{
                    num = 0;
                }
                bids[q] = num;

            }
            else if(ready == 2){
                if( (jrand48(xsub1) % 2) == 0 && shouldbid[q] == 1){
                    num += (jrand48(xsub1) % (customs[q].money - bids[q]));
                    bids[q] = num;
                }
            }
            pthread_mutex_lock(&mutex2);
            if(wins[q] == 1){
                customs[q].money = customs[q].money - num;
                moneySpent += num;
                customs[q].items[loc] = 0;
                customs[q].wins++;
                wins[q] = 0;
            }

            pthread_mutex_lock(mutex1);
            if (customs[q].numItems == 0 || customs[q].money == 0) {
                occupied[q] = 0;
                printf("Customer %d leaves, winning %d auctions, "
                       "amount of money spent = %d, amount of money left = %d\n",
                       customs[q].id, customs[q].wins, moneySpent, customs[q].money);
            }
            pthread_mutex_unlock(mutex1);
        }


    }



    pthread_exit(0);
}

