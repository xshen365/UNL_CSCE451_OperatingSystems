//
//  main.cpp
//  hw3_part1
//
//  Created by xingjian shen on 2/27/20.
//  Copyright © 2020 xingjian shen. All rights reserved.
//

#include <iostream>
#include <string>
#include <pthread.h>
#include <thread>
#include <stdio.h>
#include <mutex>
#include <stdlib.h>
#include <semaphore.h>

int p_num, c_num, charCount;

typedef struct {
    char *chars;
    int total;
    int curr;
    sem_t mtx, empty, full;
} buffer;

buffer buff;

void *consumer(void *param) {
//    std::cout << "making consumer thread id\n";
    int id = *((int*) param);
    bool done = false;
    
//    std::cout << "enter consumer while\n";
    
    while(1) {
        sem_wait(&buff.full);
        sem_wait(&buff.mtx);
        // remove
        if(buff.curr == 0 && buff.total == charCount) {
            done = true;
        }
        else if(buff.curr > 0) {
            buff.curr--;
            printf("c:<%u>, item: %c, at %d\n", id, buff.chars[buff.curr], buff.curr);
            buff.chars[buff.curr] = 0;
        }
        sem_post(&buff.mtx);
        sem_post(&buff.empty);
        
        if(done) {
            pthread_exit(0);
        }
    }
}

void *producer(void *param) {
//    std::cout << "making producer thread id\n";
    int id = *((int*) param);
    bool done = false;
    
//    std::cout << "enter producer while\n";
//    std::cout << done << "\n";
    
    while(1) {
        sem_wait(&buff.empty);
        sem_wait(&buff.mtx);
        
        // insert
        if(buff.total == charCount) {
            done = true;
        }
        else if(buff.total < charCount) {
            buff.curr++;
            buff.total++;
            buff.chars[buff.curr-1] = 'X';
            printf("p:<%u>, item: %c, at %d\n", id, buff.chars[buff.curr-1], buff.curr-1);
        }
        
        sem_post(&buff.mtx);
        sem_post(&buff.full);
        
        if(done) {
            pthread_exit(0);
        }
    }
}

int *getThreadIDs(int total) {
    int *threadNums = (int *) malloc(total * sizeof(int));
    for(int i = 0; i < total; i++){
        threadNums[i] = i + 1;
    }
    return threadNums;
}

int main(int argc, const char * argv[]) {
    buff.chars = (char*) malloc(sizeof(char) * atoi(argv[2]));
    p_num = atoi(argv[4]);
    c_num = atoi(argv[6]);
    charCount = atoi(argv[8]);
    sem_init(&buff.empty,0,atoi(argv[2]));
    sem_init(&buff.full,0,0);
    sem_init(&buff.mtx,0,1);
    
    buff.total = 0;
    buff.curr = 0;
    
    pthread_t p_threads[p_num];
    pthread_t c_threads[c_num];
    
//    std::cout << "about to make threads\n";
    int *producerThreadNums = getThreadIDs(p_num);
    int *consumerThreadNums = getThreadIDs(c_num);
    
//    for(int j = 0; j < p_num; j++){
//        std::cout << "producer[j] == " << producerThreadNums[j] << "\n";
//    }
    for (int i = 0; i < p_num; i++) {
        pthread_create(&p_threads[i], NULL, producer, (void*) &producerThreadNums[i]);
//        std::cout << "created producer thread " << i+1 << "\n";
    }
    
    for (int i = 0; i < c_num; i++) {
        pthread_create(&c_threads[i], NULL, consumer, (void*) &consumerThreadNums[i]);
//        std::cout << "created consumer thread " << i+1 << "\n";
    }
    
    for (int i = 0; i < p_num; i++) {
        pthread_join(p_threads[i], NULL);
//        std::cout << "joined producer thread " << i+1 << "\n";
    }
    
    for (int i = 0; i < c_num; i++) {
        pthread_join(c_threads[i], NULL);
//        std::cout << "joined consumer thread " << i+1 << "\n";
    }
    
//    std::cout << "threads have been joined\n";
    return 0;
}
