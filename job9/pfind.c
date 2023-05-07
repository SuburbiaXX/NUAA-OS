#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/stat.h>

#define WORKER_NUMBER 4
#define TASK_QUEUE_SIZE 10

struct task {
    int is_end;
    char path[128];
    char string[128];
} task_queue[TASK_QUEUE_SIZE];

int in = 0, out = 0;
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

sema_t mutex;
sema_t empty_task_queue;
sema_t full_task_queue;

void put_task(struct task job) {// 放任务
    sema_wait(&empty_task_queue);
    sema_wait(&mutex);

    task_queue[in] = job;
    in = (in + 1) % TASK_QUEUE_SIZE;

    sema_signal(&mutex);
    sema_signal(&full_task_queue);
}

struct task get_task() {// 获取任务
    sema_wait(&full_task_queue);
    sema_wait(&mutex);

    struct task job = task_queue[out];
    out = (out + 1) % TASK_QUEUE_SIZE;

    sema_signal(&mutex);
    sema_signal(&empty_task_queue);

    return job;
}

void find_file(char *path, char *target) {// 具体文件中匹配字符串
    FILE *file = fopen(path, "r");

    char line[256];
    while (fgets(line, sizeof(line), file)) { 
        if (strstr(line, target))
            printf("%s: %s", path, line);
    }

    fclose(file);
}

void find_dir(char *path, char *target) {
    DIR *dir = opendir(path);
    struct dirent *entry;
    while (entry = readdir(dir)) {
        if (strcmp(entry->d_name, ".") == 0) 
            continue;

        if (strcmp(entry->d_name, "..") == 0) 
            continue;

        if (entry->d_type == DT_DIR) {// 如果是目录, 递归调用 find_dir
            char *new_path = malloc(strlen(path) + strlen(entry->d_name) + 2);
            sprintf(new_path, "%s/%s", path, entry->d_name);
            find_dir(new_path, target);
            free(new_path);
        }

        if (entry->d_type == DT_REG) {// 如果是文件, 加入到任务队列中
            char *new_path = malloc(strlen(path) + strlen(entry->d_name) + 2);
            sprintf(new_path, "%s/%s", path, entry->d_name);
            // 把 new_path 加入到任务队列中
            struct task job;
            job.is_end = 0;
            strcpy(job.path, new_path);
            strcpy(job.string, target);
            put_task(job);
            free(new_path);
        }
    }
    closedir(dir);
}

// 子线程的入口
// 在一个循环中
// 从任务队列中，获取一个任务，去执行
// 当读取到一个特殊的任务(is_end 为真)，循环结束

void *worker_entry(void *arg) {
    while (1) {
        //从任务队列中获取一个任务 task;
        struct task job = get_task();
        if (job.is_end)
            break;
        //执行该任务;
        find_file(job.path, job.string);
    }
}

int main(int argc, char **argv[]) {
    if (argc != 3) {// 参数提示
        puts("Usage: pfind [file] [string]");
        return 0;
    }

    char *path = argv[1];
    char *string = argv[2];

    struct stat info;
    stat(path, &info);
    // 如果是文件, 直接调用 find_file
    if (S_ISREG(info.st_mode)) {
        find_file(path, string);
        return 0;
    }

    // 初始化任务队列
    for (int i = 0; i < TASK_QUEUE_SIZE; i++) {
        task_queue[i].is_end = 0;
        task_queue[i].path[0] = '\0';
        task_queue[i].string[0] = '\0';
    }

    // 创建 WORKER_NUMBER 个子线程, 并且初始化信号量
    pthread_t workers[WORKER_NUMBER];
    sema_init(&mutex, 1);
    sema_init(&empty_task_queue, TASK_QUEUE_SIZE - 1);
    sema_init(&full_task_queue, 0);

    for (int i = 0; i < WORKER_NUMBER; i++) {// 创建子线程
        pthread_create(&workers[i], NULL, worker_entry, NULL);
    }

    // 对目录 path 进行递归遍历, 遇见叶子节点时, 把叶子节点的路径加入到任务队列中
    find_dir(path, string);

    // 创建 WORER_NUMBER 个特殊任务, 特殊任务的 is_end 为真, 子线程读取到特殊任务时, 表示主线程已经完成递归遍历，不会再向任务队列中放置任务, 此时，子线程可以退出, 把这些特殊任务加入到任务队列中
    struct task job;
    job.is_end = 1;
    for (int i = 0; i < WORKER_NUMBER; i++) {
        put_task(job);
    }

    // 等待所有的子线程结束;
    for (int i = 0; i < WORKER_NUMBER; i++) {
        pthread_join(workers[i], NULL);
    }
    return 0;
}
