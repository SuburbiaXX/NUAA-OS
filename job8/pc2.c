#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define CAPACITY 4
int buffer1[CAPACITY], buffer2[CAPACITY];
int in1, in2;
int out1, out2;

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

typedef struct {
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} sema_t;

void sema_init(sema_t *sema, int value) {
    sema->value = value;
    pthread_mutex_init(&sema->mutex, NULL);
    pthread_cond_init(&sema->cond, NULL);
}

void sema_wait(sema_t *sema) {
    pthread_mutex_lock(&sema->mutex);
    while (sema->value <= 0)
        pthread_cond_wait(&sema->cond, &sema->mutex);
    sema->value--;
    pthread_mutex_unlock(&sema->mutex);
}

void sema_signal(sema_t *sema) {
    pthread_mutex_lock(&sema->mutex);
    ++sema->value;
    pthread_cond_signal(&sema->cond);
    pthread_mutex_unlock(&sema->mutex);
}

sema_t mutex1, mutex2;
sema_t wait_empty_buffer1, wait_empty_buffer2;
sema_t wait_full_buffer1, wait_full_buffer2;

#define ITEM_COUNT (CAPACITY * 2)

void *consume(void *arg) {
    int item;
    for (int i = 0; i < ITEM_COUNT; i++) {
        sema_wait(&wait_full_buffer2);
        sema_wait(&mutex2);
        item = get_item2();
        printf("\t\tconsume item: %c\n", item);
        sema_signal(&mutex2);
        sema_signal(&wait_empty_buffer2);
    }
    return NULL;
}

void *compute(void *arg) {
    int item;
    for (int i = 0; i < ITEM_COUNT; i++) {
        sema_wait(&wait_full_buffer1);
        sema_wait(&mutex1);
        item = get_item1();
        item -= 32;
        printf("\tcompute item: %c\n", item);
        sema_signal(&mutex1);
        sema_signal(&wait_empty_buffer1);
        // 先计算好, 但是不会立即放入buffer2, 而是等待buffer2有空位
        sema_wait(&wait_empty_buffer2);
        sema_wait(&mutex2);
        put_item2(item);
        printf("\tcompute put item: %c\n", item);
        sema_signal(&mutex2);
        sema_signal(&wait_full_buffer2);
    }
    return NULL;
}

void *produce(void *arg) {
    int item;
    for (int i = 0; i < ITEM_COUNT; i++) {
        sema_wait(&wait_empty_buffer1);
        sema_wait(&mutex1);
        item = i + 'a';
        put_item1(item);
        printf("produce item: %c\n", item);
        sema_signal(&mutex1);
        sema_signal(&wait_full_buffer1);
    }
    return NULL;
}

int main() {
    pthread_t consumer_tid, computer_tid; // 主线程为生产者, 创建消费者线程和计算者线程
    sema_init(&mutex1, 1);
    sema_init(&mutex2, 1);
    sema_init(&wait_empty_buffer1, CAPACITY);
    sema_init(&wait_empty_buffer2, CAPACITY);
    sema_init(&wait_full_buffer1, 0);
    sema_init(&wait_full_buffer2, 0);

    pthread_create(&consumer_tid, NULL, consume, NULL);
    pthread_create(&computer_tid, NULL, compute, NULL);
    produce(NULL);

    pthread_join(consumer_tid, NULL);
    pthread_join(computer_tid, NULL);
    return 0;
}
