#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
using namespace std;

class TaskQueue {
private:
    size_t _capacity;
    queue<int> _que;
    mutex _mutex;
    condition_variable _notEmpty;   // 消费者等待
    condition_variable _notFull;    // 生产者等待
public:
    TaskQueue(size_t cap) : _capacity(cap) {}

    void push(const int& value) {
        unique_lock<mutex> ul(_mutex);           // RAII加锁
        while (_que.size() == _capacity) {       // 必须用while！防止虚假唤醒
            _notFull.wait(ul);                   // 仓库满 → 生产者睡觉
        }
        _que.push(value);
        _notEmpty.notify_one();                  // 唤醒一个消费者
    }

    int pop() {
        unique_lock<mutex> ul(_mutex);
        while (_que.empty()) {                   // 必须用while！
            _notEmpty.wait(ul);                  // 仓库空 → 消费者睡觉
        }
        int tmp = _que.front();
        _que.pop();
        _notFull.notify_one();                   // 唤醒一个生产者
        return tmp;
    }
};

// 生产者类
class Producer {
public:
    void produce(TaskQueue& tq) {
        for (int i = 0; i < 20; ++i) {
            tq.push(i);
            cout << "生产者生产了: " << i << endl;
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
};

// 消费者类
class Consumer {
public:
    void consume(TaskQueue& tq) {
        for (int i = 0; i < 20; ++i) {
            int val = tq.pop();
            cout << "消费者消费了: " << val << endl;
            this_thread::sleep_for(chrono::milliseconds(150));
        }
    }
};

int main() {
    TaskQueue tq(10);          // 仓库容量10
    Producer pr;
    Consumer co;

    thread pro(&Producer::produce, &pr, ref(tq));   // 引用传递
    thread con(&Consumer::consume, &co, ref(tq));

    pro.join();
    con.join();
    return 0;
}