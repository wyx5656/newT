#include <stdio.h>
#include <func.h>
//全局互斥锁
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int a = 0;
void* start_routine(void *arg)
{
    int num = (int)(long)arg;
    printf("我是子线程，线程ID = %lu，接收到的参数 = %d\n", 
           pthread_self(), num);
    sleep(2);
    char* result = malloc(strlen("Hello from thread") + 1);
    if (NULL == result)
    {
        return("malloc fail");
        return NULL;
    }

    strcpy(result, "Hello from thread");
    printf("子线程即将退出\n");
    return (void*)result;
} 

void* start_routine1(void *arg)
{
    char* num = arg;
    printf("我是子线程%s，线程ID = %lu\n", 
           num, pthread_self());
    sleep(2);
    printf("子线程即将退出\n");
    return 0;
}

void cheanup_handler(void *arg)
{
    printf("清理函数被调用，释放资源...\n");
    pthread_mutex_unlock((pthread_mutex_t*)arg);
}

void* start_routine_with_lock(void* arg)
{
    //设置清理函数
    pthread_cleanup_push(cheanup_handler, &mutex);

    //加锁
    pthread_mutex_lock(&mutex);
    printf("线程 %lu 获得锁，开始执行任务...\n", pthread_self());
       // 睡眠3秒
    printf("线程 %lu 开始睡眠3秒...\n", pthread_self());
    sleep(3); 

    // 正常解锁（如果未被取消）
    pthread_mutex_unlock(&mutex);
    
    // 清理函数出栈，参数为0表示不执行清理函数
    pthread_cleanup_pop(0);

    return NULL;
}

void* addNum(void* arg)
{
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < 50000; i++)
    {
        a++;
        printf("线程 %lu 增加计数器，当前值: %d\n", pthread_self(), a);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}
int main()
{
    pthread_t tid;
    int err = pthread_create(&tid, NULL, start_routine, (void*)123);
    if (err) {
        error(1, err, "pthread_create");
    }
    
    printf("我是主线程，创建了子线程 ID = %lu\n", tid);

    void* retval;
    pthread_join(tid, &retval);
    char* thread_result = (char*)retval;
    if(thread_result != NULL) {
        printf("主线程：子线程已结束，返回字符串 = %s\n", thread_result);
        free(thread_result);
        thread_result = NULL;
    } else {
        printf("主线程：子线程返回NULL\n");
    }

    pthread_t tid1;
    pthread_t tid2;
    err = pthread_create(&tid1,NULL, start_routine1, "A");
    if (err) {
        error(1, err, "pthread_create");
    }

    err = pthread_create(&tid2,NULL, start_routine1, "B");
    if (err) {
        error(1, err, "pthread_create");
    }
    pthread_join(tid1, &retval);
    pthread_join(tid2, &retval);

    //创建一个线程，里面加锁 → 睡眠 3 秒 → 解锁。
    //用 pthread_cancel 在睡眠期间取消它，看清理函数是否被调用。
    pthread_t lock_thread;
    pthread_create(&lock_thread, NULL, start_routine_with_lock, NULL);
    if (err) {
        error(1, err, "pthread_create with lock");
    }
    // 给线程一些时间来获取锁并开始睡眠
    sleep(1);
    
    printf("主线程准备取消带锁的线程...\n");
    // 取消线程，此时应该触发清理函数
    err = pthread_cancel(lock_thread);
    if (err) {
        error(1, err, "pthread_cancel");
    }
    
    // 等待线程结束
    pthread_join(lock_thread, &retval);
    printf("带锁线程已被取消，程序继续...\n");

    //写一个线程安全的计数器，多个线程同时++一个全局变量
    pthread_t add1;
    pthread_t add2;
    pthread_t add3;
    pthread_create(&add1, NULL, addNum, NULL);
    if (err) {
        error(1, err, "pthread_create with lock");
    }
    pthread_create(&add2, NULL, addNum, NULL);
    if (err) {
        error(1, err, "pthread_create with lock");
    }
    pthread_create(&add3, NULL, addNum, NULL);
    if (err) {
        error(1, err, "pthread_create with lock");
    }

    pthread_join(add1, &retval);
    pthread_join(add2, &retval);
    pthread_join(add3, &retval);

    printf("最终计数器值: %d\n", a);

    return 0;
}