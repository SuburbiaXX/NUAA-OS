#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define Limit 1000000
#define ThreadNum 10
#define PerThreadLimit (Limit / ThreadNum)

struct ThreadArgs {// 线程参数
    int start, end;
};

struct Result {// 线程结果
    double res;
};

void *Assist(void *args) {// 辅助线程计算
    struct Result *result = (struct Result *)malloc(sizeof(struct Result));
    struct ThreadArgs *arg = (struct ThreadArgs *)args;
    double per_res = 0.0;

    for (int i = arg->start; i <= arg->end; i++)
        per_res += (i % 2 == 0 ? -1.0 : 1.0) / (2 * i - 1);
    result->res = per_res;
    return result;
}

int main() {
    pthread_t tid[ThreadNum];
    struct ThreadArgs args[ThreadNum];
    
    for (int i = 0; i < ThreadNum; i++) {// 创建线程
        args[i].start = i * PerThreadLimit + 1;
        args[i].end = (i + 1) * PerThreadLimit;
        pthread_create(&tid[i], NULL, Assist, &args[i]);
    }

    double res = 0.0;
    for (int i = 0; i < ThreadNum; i++) {// 等待所有线程结束, 累加结果
        struct Result *result;
        pthread_join(tid[i], (void **)&result);
        res += result->res;
        free(result);
    }

    printf("PI / 4 = %lf\n", res);
    printf("PI = %lf\n", res * 4);
    return 0;
}