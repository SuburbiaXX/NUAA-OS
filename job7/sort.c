#include <stdio.h>
#include <pthread.h>

int array[] = {67, 87,186, -69, 156, 44, 91, -56, 171, -184, 52, 5, 134, 188, 193, -136, 157, 100, 84, 193};
#define NUM sizeof(array) / sizeof(array[0])
#define ThreadNum 2

struct ThreadArgs {
    int start, end;
};

void *Assist(void *args) {
    struct ThreadArgs *arg = (struct ThreadArgs *)args;
    // 选择排序
    for (int i = arg->start; i <= arg->end; i++) {
        int min_idx = i;
        for (int j = i + 1; j <= arg->end; j++) {
            if (array[j] < array[min_idx])
                min_idx = j;
        }
        int temp = array[i];
        array[i] = array[min_idx];
        array[min_idx] = temp;
    }
    return NULL;
}

void Merge() {// 归并排序, 目前只实现了两个线程的情况
    int temp[NUM];
    int i = 0, j = NUM / 2, k = 0;
    while (i < NUM / 2 && j < NUM) {
        if (array[i] < array[j])
            temp[k++] = array[i++];
        else
            temp[k++] = array[j++];
    }
    while (i < NUM / 2)
        temp[k++] = array[i++];
    while (j < NUM)
        temp[k++] = array[j++];
    for (int i = 0; i < NUM; i++)
        array[i] = temp[i];
}

int main() {
    pthread_t tid[ThreadNum];
    struct ThreadArgs args[ThreadNum];

    for (int i = 0; i < ThreadNum; i++) {// 创建线程
        args[i].start = i * (NUM / ThreadNum);
        args[i].end = (i + 1) * (NUM / ThreadNum) - 1;
        pthread_create(&tid[i], NULL, Assist, &args[i]);
    }

    for (int i = 0; i < ThreadNum; i++) {// 等待所有线程结束
        pthread_join(tid[i], NULL);
    }
    // 归并排序
    Merge();
    printf("排序后的数组为:\n");
    for (int i = 0; i < NUM; i++)
        printf("%d ", array[i]);
    puts("");
    return 0;
}