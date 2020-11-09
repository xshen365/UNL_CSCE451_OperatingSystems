//
//  main.cpp
//  hw3_part2
//
//  Created by xingjian shen on 3/9/20.
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

void mon_insert(char alpha) {
    std::cout << "entering  while loop in mon_insert" << "\n";
    while(buff.total == charCount) {
        sem_wait(&buff.full);
    }
    buff.chars[buff.curr] = alpha;
    buff.curr++;
    buff.total++;
    sem_post(&buff.empty);
}

char mon_remove() {
    char result;
    std::cout << "entering  while loop in mon_remove" << "\n";
    while(buff.total == 0) {
        sem_wait(&buff.empty);
    }
    buff.total--;
    result = *buff.chars;
    buff.chars = 0;
    buff.curr--;
    sem_post(&buff.full);
    return result;
}

char generate_random_alphabet() {
    char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char alphabet = letters[rand() % 52];
    return alphabet;
}

void *consumer(void *param) {
    char result;
    std::cout << "In consumer function" << "\n";
    while(1) {
        result = mon_remove();
    }
    return 0;
}

void *producer(void *param) {
    char alpha;
    std::cout << "In producer function" << "\n";
    while(1) {
        alpha = generate_random_alphabet();
        mon_insert(alpha);
    }
    return 0;
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
    
    int *producerThreadNums = getThreadIDs(p_num);
    int *consumerThreadNums = getThreadIDs(c_num);
    
    for (int i = 0; i < p_num; i++) {
        std::cout << "creating producer thread" << "\n";
        pthread_create(&p_threads[i], NULL, producer, (void*) &producerThreadNums[i]);
    }
    
    for (int i = 0; i < c_num; i++) {
        std::cout << "creating consumer thread" << "\n";
        pthread_create(&c_threads[i], NULL, consumer, (void*) &consumerThreadNums[i]);
    }
    
    for (int i = 0; i < p_num; i++) {
        std::cout << "joined producer thread" << "\n";
        pthread_join(p_threads[i], NULL);
    }
    
    for (int i = 0; i < c_num; i++) {
        std::cout << "joined consumer thread" << "\n";
        pthread_join(c_threads[i], NULL);
    }
    
    return 0;
}
