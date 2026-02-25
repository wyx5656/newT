#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

#define THREAD_POOL_SIZE 4      // 线程池大小
#define TASK_QUEUE_SIZE 20      // 任务队列最大容量

typedef struct task
{
    void (*function)(void*);
    void *arg;
    struct task *next;
} Task;

typedef struct threadpool
{
    pthread_t* threads;//线程数组
    Task *task_queue_head;//任务队列头
    Task *task_queue_tail; //任务队列尾
    int task_count;        //当前任务数
    bool shutdown;      //关闭标志
    pthread_mutex_t mutex;
    pthread_cond_t not_empty; //队列非空
    pthread_cond_t not_full; //队列不满
} TheadPool;


bool thread_pool_submit(TheadPool* pool, void (*function)(void*), void* arg)
{
    pthread_mutex_lock(&pool->mutex);
    while(TASK_QUEUE_SIZE == pool->task_count && false == pool->shutdown)
    {
        pthread_cond_wait(&pool->not_full, &pool->mutex);
    }

    if(true == pool->shutdown)
    {
        pthread_mutex_unlock(&pool->mutex);
        return false;
    }

    Task *task = malloc(sizeof(Task));
    if(NULL == task)
    {
        pthread_mutex_unlock(&pool->mutex);
        return false;
    }

    task->function = function;
    task->arg = arg;
    task->next = NULL;

    if (pool->task_queue_tail != NULL)
    {
        pool->task_queue_tail->next = task;
    }
    else
    {
        pool->task_queue_head = task;
    }

    pool->task_queue_tail = task;
    pool->task_count++;

    pthread_cond_signal(&pool->not_empty);
    pthread_mutex_unlock(&pool->mutex);
    return true;
}

void *worker_thread(void* arg)
{
    TheadPool *pool = (TheadPool*)arg;
    while(1)
    {
        pthread_mutex_lock(&pool->mutex);
        while(0 == pool->task_count && false == pool->shutdown)
        {
            pthread_cond_wait(&pool->not_empty, &pool->mutex);
        }

        if (true == pool->shutdown && 0 == pool->task_count)
        {
            pthread_mutex_unlock(&pool->mutex);
            return NULL;
        }

        Task *task = pool->task_queue_head;
        pool->task_queue_head = task->next;
        if (NULL == pool->task_queue_head)
        {
            pool->task_queue_tail = NULL;
        }

        pool->task_count--;

        pthread_cond_signal(&pool->not_full);
        pthread_mutex_unlock(&pool->mutex);

        printf("线程 %lu 执行任务， \n", pthread_self());
        task->function(task->arg);
        free(task);
    }
    
    return NULL;
}

TheadPool* thread_pool_create(int num_threads)\
{
    TheadPool* pool = malloc(sizeof(TheadPool));
    if (NULL == pool)
    {
        printf("malloc fail\n");
        return NULL;
    }

    pool->threads = malloc(sizeof(pthread_t) * num_threads);
    if (NULL == pool->threads)
    {
        free(pool);
        printf("malloc fail\n");
        return NULL;
    }

    pool->task_queue_head = pool->task_queue_tail = NULL;
    pool->task_count = 0;
    pool->shutdown = false;

    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->not_empty, NULL);
    pthread_cond_init(&pool->not_full, NULL);

    for (int i = 0; i < num_threads; i++)
    {
        pthread_create(&pool->threads[i], NULL, worker_thread, pool);
    }

    return pool;
}

void task_example(void *arg)
{
    int id = (int)(long)arg;
    printf("任务 %d 开始执行 \n",id);
    sleep(rand() % 3 + 1);
    printf("任务 %d 完成\n", id);
    return;
}

void thread_pool_destroy(TheadPool *pool)
{
    if (NULL == pool) return;
    pthread_mutex_lock(&pool->mutex);
    pool->shutdown = true;
    pthread_cond_broadcast(&pool->not_empty);
    pthread_mutex_unlock(&pool->mutex);

    for (int i = 0; i < THREAD_POOL_SIZE; i++)
    {
        pthread_join(pool->threads[i], NULL);
    }

    Task *task = pool->task_queue_head;
    while(task != NULL)
    {
        Task *next = task->next;
        free(task);
        task = next;
    }

    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->not_empty);
    pthread_cond_destroy(&pool->not_full);

    free(pool->threads);
    free(pool);

    printf("线程已销毁\n");
}
int main()
{
    srand(time(NULL));

    TheadPool *pool = thread_pool_create(THREAD_POOL_SIZE);
    if (NULL == pool)
    {
        printf("线程池创建失败\n");
        return 1;
    }

    printf("线程池创建成功，提交任务\n");
    for (int i = 1; i <= 20; i++)
    {
        thread_pool_submit(pool, task_example, (void*)(long)i);
        usleep(100000);
    }

    sleep(15);
    thread_pool_destroy(pool);

    return 0;
}

/*
写一个线程安全的队列（有界 / 无界）
实现一个简单的线程池（至少包含 submit 和 shutdown）
生产者消费者问题变种：
多生产者多消费者
优先级队列
固定窗口大小的缓冲区

死锁的 4 个必要条件 + 如何避免
pthread_cond_wait 为什么必须用 while 而不是 if（虚假唤醒）
线程取消时如何保证资源释放（清理函数）
线程池的优点 & 适用场景
*/