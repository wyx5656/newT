#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define INITIAL_CAPACITY 8
typedef struct {
    size_t size;
    size_t capacity;
    size_t front;
    size_t rear;
    int *data;
} Queue;

// 初始化队列
Queue* queue_create() {
    Queue *q = (Queue*)malloc(sizeof(Queue));
    if (!q) return NULL;

    q->data = (int*)malloc(INITIAL_CAPACITY * sizeof(int));
    if (!q->data) {
        free(q);
        return NULL;
    }

    q->capacity = INITIAL_CAPACITY;
    q->front = 0;
    q->rear = 0;
    q->size = 0;
    return q;
}

// 销毁队列
void queue_destroy(Queue *q) {
    if (!q) return;
    free(q->data);
    free(q);
}

// 判断是否为空
bool queue_is_empty(Queue *q) {
    return q->size == 0;
}

// 判断是否已满（在扩容版本中很少用到）
bool queue_is_full(Queue *q) {
    return q->size == q->capacity;
}

// 扩容（容量翻倍）
static bool queue_resize(Queue *q, size_t new_capacity) {
    if (new_capacity <= q->size) return false;

    int *new_data = (int*)malloc(new_capacity * sizeof(int));
    if (!new_data) return false;

    // 把旧数据线性拷贝到新数组（从 front 开始）
    size_t i = 0;
    size_t idx = q->front;
    while (i < q->size) {
        new_data[i++] = q->data[idx];
        idx = (idx + 1) % q->capacity;
    }

    free(q->data);
    q->data = new_data;
    q->capacity = new_capacity;
    q->front = 0;
    q->rear = q->size;  // 新的 rear 指向 size 位置

    return true;
}

// 入队（从队尾）
bool queue_enqueue(Queue *q, int value) {
    // 容量不足时扩容（通常翻倍）
    if (q->size == q->capacity) {
        size_t new_cap = q->capacity == 0 ? INITIAL_CAPACITY : q->capacity * 2;
        if (!queue_resize(q, new_cap)) {
            printf("扩容失败\n");
            return false;
        }
    }

    q->data[q->rear] = value;
    q->rear = (q->rear + 1) % q->capacity;
    q->size++;
    return true;
}

// 出队（从队头）
bool queue_dequeue(Queue *q, int *value) {
    if (queue_is_empty(q)) {
        printf("队列为空\n");
        return false;
    }

    *value = q->data[q->front];
    q->front = (q->front + 1) % q->capacity;
    q->size--;

    // 可选：缩容（容量过大时缩小，通常缩到 1/4 时减半）
    // 这里简单实现：当 size <= capacity/4 且 capacity > INITIAL_CAPACITY 时缩容
    if (q->size <= q->capacity / 4 && q->capacity > INITIAL_CAPACITY) {
        size_t new_cap = q->capacity / 2;
        queue_resize(q, new_cap);  // 缩容
    }

    return true;
}
// 获取队头元素（不删除）
bool queue_front(Queue *q, int *value) {
    if (queue_is_empty(q)) return false;
    *value = q->data[q->front];
    return true;
}

// 获取当前元素个数
size_t queue_size(Queue *q) {
    return q->size;
}

size_t queue_capacity(Queue *q) {
    return q->capacity;
}

// 打印队列内容（从头到尾）
void queue_print(Queue *q) {
    if (queue_is_empty(q)) {
        printf("队列为空\n");
        return;
    }

    printf("队列（%zu/%zu）：", q->size, q->capacity);
    size_t i = q->front;
    for (size_t count = 0; count < q->size; count++) {
        printf("%d ", q->data[i]);
        i = (i + 1) % q->capacity;
    }
    printf("\n");
}

// 测试代码
int main() {
    Queue *q = queue_create();
    if (!q) {
        printf("队列创建失败\n");
        return 1;
    }

    printf("初始容量: %zu\n", queue_capacity(q));

    // 入队测试
    for (int i = 1; i <= 20; i++) {
        queue_enqueue(q, i * 10);
        printf("入队 %d 后，", i * 10);
        queue_print(q);
    }

    // 出队测试
    printf("\n开始出队：\n");
    for (int i = 0; i < 15; i++) {
        int val;
        if (queue_dequeue(q, &val)) {
            printf("出队 %d 后，", val);
            queue_print(q);
        }
    }

    queue_destroy(q);
    return 0;
}