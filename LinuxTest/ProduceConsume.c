#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 10
#define PRODUCER_COUNT 3
#define CONSUMER_COUNT 1

int buffer[BUFFER_SIZE];
int front = 0, rear = 0, count = 0;

// 同步原语
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t notFull = PTHREAD_COND_INITIALIZER; //非满，唤醒生产者
pthread_cond_t notEmpty = PTHREAD_COND_INITIALIZER;//非空，唤醒消费者

void* producer(void* arg)
{
    int id = (int)(long)arg;
    int item = 0;

    while(1)
    {
        item = rand() % 100;

        pthread_mutex_lock(&mutex);

        while(BUFFER_SIZE == count)
        {
            printf("生产者 %d：缓冲区满，等待...\n", id);
            pthread_cond_wait(&notFull, &mutex);
            sleep(1);
        }

        // 生产
        buffer[rear] = item;
        rear = (rear + 1) % BUFFER_SIZE;
        count++;

        printf("生产者 %d 生产了 %d，当前队列有 %d 个\n", id, item, count);

        pthread_cond_signal(&notEmpty);
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 2);
    }

    return NULL;
}

void* consumer(void* arg)
{
    int id = (int)(long)arg;
    int item = 0;

    while(1)
    {

        pthread_mutex_lock(&mutex);

        while(0 == count)
        {
            printf("消费者 %d：缓冲区空，等待...\n", id);
            sleep(1);
            pthread_cond_wait(&notEmpty, &mutex);
        }

        // 生产
        item = buffer[front];
        front = (front + 1) % BUFFER_SIZE;
        count--;

        printf("消费者 %d 消费了 %d，当前队列有 %d 个\n", id, item, count);

        pthread_cond_signal(&notFull);
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 3);
    }

    return NULL;
}

int main() {
    pthread_t producers[PRODUCER_COUNT];
    pthread_t consumers[CONSUMER_COUNT];

    srand(time(NULL));

    // 创建生产者
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_create(&producers[i], NULL, producer, (void*)(long)i);
    }

    // 创建消费者
    for (int i = 0; i < CONSUMER_COUNT; i++) {
        pthread_create(&consumers[i], NULL, consumer, (void*)(long)i);
    }

    sleep(10);

    pthread_cancel(consumers[0]);
    // 等待（这里简化，实际可加退出机制）
    for (int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_join(producers[i], NULL);
    }
    for (int i = 0; i < CONSUMER_COUNT; i++) {
        pthread_join(consumers[i], NULL);
    }

    return 0;
}