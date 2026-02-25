#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define THREADCOUNT 5
#define INCREMENTTIMES 100000

int counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexA = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexB = PTHREAD_MUTEX_INITIALIZER;

void* increment(void *arg)
{
    for(int i = 0; i < INCREMENTTIMES; i++)
    {
        //pthread_mutex_lock(&mutex);

        counter++;
        //pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void cleanup_unlock(void *arg)
{
    pthread_mutex_t *mutex = (pthread_mutex_t*)arg;

    pthread_mutex_unlock(mutex);
}
void* thread1Func(void *arg)
{
    pthread_cleanup_push(cleanup_unlock, &mutexA);
    pthread_mutex_lock(&mutexA);
    printf("1 持有了A\n");
    sleep(1);
    pthread_mutex_lock(&mutexB);
    printf("1 持有了B\n");
    pthread_mutex_unlock(&mutexB);
    printf("1 释放了B\n");
    pthread_mutex_unlock(&mutexA);
    printf("1 释放了A\n");

    pthread_cleanup_pop(0);

    return NULL;
}

void* thread2Func(void *arg)
{
    pthread_mutex_lock(&mutexA);
    printf("2 持有了B\n");
    sleep(1);
    pthread_mutex_lock(&mutexB);
    printf("2 持有了A\n");
    pthread_mutex_unlock(&mutexA);
    printf("2 释放了A\n");
    pthread_mutex_unlock(&mutexB);
    printf("2 释放了B\n");

    return NULL;
}
int main()
{
    pthread_t threads[THREADCOUNT];

    printf("启动 %d 个线程，每个线程加 %d 次...\n", THREADCOUNT, INCREMENTTIMES);

    for (int i = 0; i < THREADCOUNT; i++)
    {
        pthread_create(&threads[i], NULL, increment, NULL);
    }

    for (int i = 0; i < THREADCOUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("最终计数器值：%d\n", counter);           // 应该正好是 500000
    printf("预期值：%d\n", THREADCOUNT * INCREMENTTIMES);

    pthread_t thread1;
    pthread_t thread2;
    pthread_create(&thread1, NULL,thread1Func, NULL);
    pthread_create(&thread2, NULL,thread2Func, NULL);

    pthread_cancel(thread1);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutexA);
    pthread_mutex_destroy(&mutexB);

    return 0;
}
