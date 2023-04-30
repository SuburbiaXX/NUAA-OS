#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define CAPACITY 4
int buffer1[CAPACITY], buffer2[CAPACITY];
int in1, in2;
int out1, out2;

int buffer1_is_empty() {
    return in1 == out1;
}

int buffer2_is_empty() {
    return in2 == out2;
}

int buffer1_is_full() {
    return (in1 + 1) % CAPACITY == out1;
}

int buffer2_is_full() {
    return (in2 + 1) % CAPACITY == out2;
}

int get_item1() {
    int item;
    item = buffer1[out1];
    out1 = (out1 + 1) % CAPACITY;
    return item;
}

int get_item2() {
    int item;
    item = buffer2[out2];
    out2 = (out2 + 1) % CAPACITY;
    return item;
}

void put_item1(int item) {
    buffer1[in1] = item;
    in1 = (in1 + 1) % CAPACITY;
}

void put_item2(int item) {
    buffer2[in2] = item;
    in2 = (in2 + 1) % CAPACITY;
}

pthread_mutex_t mutex1, mutex2;
pthread_cond_t wait_empty_buffer1, wait_empty_buffer2;
pthread_cond_t wait_full_buffer1, wait_full_buffer2;

#define ITEM_COUNT (CAPACITY * 2)

void *consume(void *arg) {
    int item;
    for (int i = 0; i < ITEM_COUNT; i++) {
        pthread_mutex_lock(&mutex2);
        while (buffer2_is_empty())
            pthread_cond_wait(&wait_full_buffer2, &mutex2);
        item = get_item2();
        printf("\t\tconsume item: %c\n", item);
        fflush(stdout);
        pthread_cond_signal(&wait_empty_buffer2);
        pthread_mutex_unlock(&mutex2);
    }
    return NULL;
}

void *compute(void *arg) {
    int item;
    for (int i = 0; i < ITEM_COUNT; i++) {
        pthread_mutex_lock(&mutex1);
        while (buffer1_is_empty())
            pthread_cond_wait(&wait_full_buffer1, &mutex1);
        item = get_item1();
        pthread_cond_signal(&wait_empty_buffer1);
        pthread_mutex_unlock(&mutex1);
        item -= 32;
        pthread_mutex_lock(&mutex2);
        while (buffer2_is_full())
            pthread_cond_wait(&wait_empty_buffer2, &mutex2);
        put_item2(item);
        printf("\tcompute item: %c\n", item);
        fflush(stdout);
        pthread_cond_signal(&wait_full_buffer2);
        pthread_mutex_unlock(&mutex2);
    }
    return NULL;
}

void *produce(void *arg) {
    int item;
    for (int i = 0; i < ITEM_COUNT; i++) {
        pthread_mutex_lock(&mutex1);
        while (buffer1_is_full())
            pthread_cond_wait(&wait_empty_buffer1, &mutex1);
        item = i + 'a';
        put_item1(item);
        printf("produce item: %c\n", item);
        fflush(stdout);
        pthread_cond_signal(&wait_full_buffer1);
        pthread_mutex_unlock(&mutex1);
    }
    return NULL;
}

int main() {
    pthread_t consumer_tid, computer_tid; // 主线程为生产者, 创建消费者线程和计算者线程
    
    pthread_mutex_init(&mutex1, NULL);
    pthread_mutex_init(&mutex2, NULL);
    pthread_cond_init(&wait_empty_buffer1, NULL);
    pthread_cond_init(&wait_empty_buffer2, NULL);
    pthread_cond_init(&wait_full_buffer1, NULL);
    pthread_cond_init(&wait_full_buffer2, NULL);

    pthread_create(&consumer_tid, NULL, consume, NULL);
    pthread_create(&computer_tid, NULL, compute, NULL);
    produce(NULL);

    pthread_join(consumer_tid, NULL);
    pthread_join(computer_tid, NULL);
    return 0;
}