#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_WORKERS 5
#define QUEUE_SIZE 20
#define TaskNum 5

// 任务结构体
typedef struct Task 
{
    void (*function)(void*);   // 任务函数
    void *arg;                 // 参数
} Task;

// 线程池结构体
typedef struct 
{
    pthread_t threads[NUM_WORKERS];
    Task tas[TaskNum];
    int rear;
    int front;
    //Task *head;                // 任务队列头
    //Task *tail;                // 任务队列尾
    int count;                 // 当前任务数
    int shutdown;              // 关闭标志
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;  // 有任务了
    pthread_cond_t not_full;
} ThreadPool;
ThreadPool pool;
// 工作线程（消费者）
void* worker(void *arg) {
    while (1) 
    {
        pthread_mutex_lock(&pool.mutex);

        // 队列空且未关闭 → 等待
        while (pool.count == 0 && !pool.shutdown) 
        {
            pthread_cond_wait(&pool.not_empty, &pool.mutex);
        }

        // 关闭且队列空 → 退出
        if (pool.shutdown && pool.count == 0) 
        {
            pthread_mutex_unlock(&pool.mutex);
            return NULL;
        }

        Task task = pool.tas[pool.front];
        pool.front = (pool.front + 1) % TaskNum;
        pool.count--;
        pthread_cond_signal(&pool.not_full);
        pthread_mutex_unlock(&pool.mutex);

        // 执行任务（锁外执行，避免锁持有时间长）
        printf("线程 %lu 执行任务...\n", pthread_self());
        task.function(task.arg);
    }

    return NULL;
}

// 提交任务（生产者）
int submit_task(void (*func)(void*), void *arg) 
{
    Task task;
    task.function = func;
    task.arg = arg;

    pthread_mutex_lock(&pool.mutex);

    if (pool.count >= TaskNum - 1) 
    {

        pthread_cond_wait(&pool.not_full, &pool.mutex);
    }

    pool.tas[pool.rear] = task;
    pool.rear = (pool.rear + 1) % TaskNum;
    pool.count++;

    pthread_cond_signal(&pool.not_empty);

    pthread_mutex_unlock(&pool.mutex);
    return 0;
}

// 示例任务
void task_example(void *arg) 
{
    int id = (int)(long)arg;
    printf("任务 %d 开始执行...\n", id);
    sleep(2);
    printf("任务 %d 完成\n", id);
}
int main() 
{
    pool.rear = 0;
    pool.front = 0;
    pool.count = 0;
    pool.shutdown = 0;

    pthread_mutex_init(&pool.mutex, NULL);
    pthread_cond_init(&pool.not_empty, NULL);
    pthread_cond_init(&pool.not_full, NULL);
    printf("创建 %d 个工作线程...\n", NUM_WORKERS);

    for (int i = 0; i < NUM_WORKERS; i++) 
    {
        pthread_create(&pool.threads[i], NULL, worker, NULL);
    }

    // 模拟提交任务
    for (int i = 1; i <= 15; i++) 
    {
        submit_task(task_example, (void*)(long)i);
        printf("主线程提交任务 %d\n", i);
        usleep(200000);  // 模拟提交间隔
    }

    // 等待一段时间后关闭
    sleep(20);
    printf("主线程发送关闭信号...\n");

    pthread_mutex_lock(&pool.mutex);
    pool.shutdown = 1;
    pthread_cond_broadcast(&pool.not_empty);
    pthread_mutex_unlock(&pool.mutex);

    for (int i = 0; i < NUM_WORKERS; i++) 
    {
        pthread_join(pool.threads[i], NULL);
    }

    pthread_mutex_destroy(&pool.mutex);
    pthread_cond_destroy(&pool.not_empty);
    pthread_cond_destroy(&pool.not_full);
    printf("线程池已销毁\n");
    return 0;
}