#include <stdio.h>
#include <pthread.h>

#define Limit 10000000

double res = 0.0, master_res = 0.0, assist_res = 0.0;

void *Assist() {// 辅助线程, 计算后半部分
    for (int i = Limit / 2 + 1; i < Limit; i ++)
        assist_res += (i % 2 ? -1 : 1) * 1.0 / (2 * i + 1); 
}

void Master() {// 主线程, 计算前半部分
    for (int i = 0; i < Limit / 2; i ++)
        master_res += (i % 2 ? -1 : 1) * 1.0 / (2 * i + 1); 
}

int main() {
    pthread_t assist;
    pthread_create(&assist, NULL, Assist, NULL);
    Master();
    pthread_join(assist, NULL);
    res = master_res + assist_res;
    printf("PI / 4 = %lf\n", res);
    printf("PI = %lf\n", res * 4);
    return 0;
}