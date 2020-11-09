//  Quinn helped me in this assignment
//  main.cpp
//  hw3_part2
//
//  Created by xingjian shen on 3/9/20.
//  Copyright © 2020 xingjian shen. All rights reserved.
//

#include <iostream>
#include <string>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

int p_num, c_num, charCount;

typedef struct {
    int length;
    char *chars;
    int total;
    int curr;
    sem_t mtx, signal; // signal - block monitor when waiting signal is seen
    int signal_count; // keeping track of threads signaling each other that a full thread produces and an empty thread consumes
} buffer_monitor;


typedef struct {
    sem_t queue; // semaphore to signal for restarting the waiting threads
    int threads; // waiting threads
} thread_queue;

void queue_init(thread_queue* q) {
    q->threads = 0;
    sem_init(&q->queue, 0, 0);
}

buffer_monitor buff;
thread_queue empty_queue, full_queue;

void mon_insert(char alpha, int id) {
//    std::cout << "entering  while loop in mon_insert" << "\n";
    sem_wait(&buff.mtx);

    if(buff.total >= charCount) {
        sem_post(&buff.mtx);
        pthread_exit(0);
    }
//    if buffer is full, wait
    while (buff.curr == charCount) {
        full_queue.threads++;

        if(buff.signal_count > 0) {
            sem_post(&buff.signal);
        }else {
            sem_post(&buff.mtx);
        }
        sem_wait(&full_queue.queue);
        full_queue.threads--;
    }

    if(buff.total <= charCount && buff.curr < buff.length) {
        buff.curr++;
        buff.chars[buff.curr-1] = alpha;
        printf("p:<%u>, item: %c, at %d\n", id, alpha, buff.curr-1);
        buff.total++;
    }

    if(empty_queue.threads > 0) {
        buff.signal_count++;
        sem_post(&empty_queue.queue);
        sem_wait(&buff.signal);
        buff.signal_count--;
    }

    if(buff.signal_count > 0) {
        sem_post(&buff.signal);
    }else {
        sem_post(&buff.mtx);
    }
}

char mon_remove(int id) {
    char result;
//    std::cout << "entering  while loop in mon_remove" << "\n";
    sem_wait(&buff.mtx);

    // check if all threads are done - produced and consumed all items
    if(buff.total >= charCount && buff.curr == 0) {
        exit(0);
    }

    // while the buffer is empty, keep waiting and increase waiting thread
    while(buff.curr == 0) {
        empty_queue.threads++;

        if(buff.signal_count > 0) {
            sem_post(&buff.signal);
        }else {
            sem_post(&buff.mtx);
        }
        sem_wait(&empty_queue.queue);
        empty_queue.threads--;
    }

    // if there is something to consume, consume
    if(buff.total <= charCount && buff.curr > 0) {
        buff.curr--;
        result = buff.chars[buff.curr];
        printf("c:<%u>, item: %c, at %d\n", id, result, buff.curr);
    }

    // checking if the waiting threads is full and there is producer waiting
    if(full_queue.threads > 0) {
        buff.signal_count++;
        sem_post(&full_queue.queue);
        sem_wait(&buff.signal);
        buff.signal_count--;
    }

    if(buff.signal_count > 0) {
        sem_post(&buff.signal);
    }else {
        sem_post(&buff.mtx);
    }

    return result;
}

char generate_random_alphabet() {
    char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char alphabet = letters[rand() % 52];
    return alphabet;
}

void *consumer(void *param) {
    int id = *((int*)param);
//    std::cout << "In consumer function" << "\n";
    while(1) {
        mon_remove(id);
    }
    return EXIT_SUCCESS;
}

void *producer(void *param) {
    char alpha;
    int id = *((int*)param);
//    std::cout << "In producer function" << "\n";
    while(1) {
        alpha = generate_random_alphabet();
        mon_insert(alpha, id);
    }
    return EXIT_SUCCESS;
}

int *getThreadIDs(int total) {
    int *threadNums = (int *) malloc(total * sizeof(int));
    for(int i = 0; i < total; i++){
        threadNums[i] = i + 1;
    }
    return threadNums;
}

int main(int argc, const char * argv[]) {
    buff.length = atoi(argv[2]);
    buff.chars = (char*) malloc(sizeof(char) * atoi(argv[2]));
    p_num = atoi(argv[4]);
    c_num = atoi(argv[6]);
    charCount = atoi(argv[8]);

    sem_init(&buff.signal, 0, 0);
    buff.signal_count = 0;

    sem_init(&buff.mtx,0, 1);

    queue_init(&empty_queue);
    queue_init(&full_queue);

    buff.total = 0;
    buff.curr = 0;

    pthread_t p_threads[p_num];
    pthread_t c_threads[c_num];

    int *producerThreadNums = getThreadIDs(p_num);
    int *consumerThreadNums = getThreadIDs(c_num);

    for (int i = 0; i < p_num; i++) {
//        std::cout << "creating producer thread" << "\n";
        pthread_create(&p_threads[i], NULL, producer, (void*) &producerThreadNums[i]);
    }

    for (int i = 0; i < c_num; i++) {
//        std::cout << "creating consumer thread" << "\n";
        pthread_create(&c_threads[i], NULL, consumer, (void*) &consumerThreadNums[i]);
    }

    for (int i = 0; i < p_num; i++) {
//        std::cout << "joined producer thread" << "\n";
        pthread_join(p_threads[i], NULL);
    }

    for (int i = 0; i < c_num; i++) {
//        std::cout << "joined consumer thread" << "\n";
        pthread_join(c_threads[i], NULL);
    }

    return 0;
}
