#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>
using namespace std;

class TaskQueue {
private:
    size_t _capacity;
    queue<function<void()>> _que;   // 存任务（函数）
    mutex _mutex;
    condition_variable _notEmpty;
    condition_variable _notFull;
    bool _flag = true;              // 控制退出
public:
    TaskQueue(size_t cap) : _capacity(cap) {}

    void push(function<void()> task) {
        unique_lock<mutex> ul(_mutex);
        while (_que.size() == _capacity) _notFull.wait(ul);
        _que.push(task);
        _notEmpty.notify_one();
    }

    function<void()> pop() {
        unique_lock<mutex> ul(_mutex);
        while (_que.empty() && _flag) {
            _notEmpty.wait(ul);
        }
        if (_flag) {
            auto task = _que.front(); _que.pop();
            _notFull.notify_one();
            return task;
        }
        return nullptr;   // 线程池要退出
    }

    void wakeup() {       // 解决第2个问题
        _flag = false;
        _notEmpty.notify_all();
    }

    bool empty() {
        lock_guard<mutex> lock(_mutex);
        return _que.empty();
    }
};

class ThreadPool {
private:
    vector<thread> _threads;
    TaskQueue _taskQue;
    size_t _threadNum;
    bool _isExit = false;
public:
    ThreadPool(size_t threadNum, size_t queSize)
        : _threadNum(threadNum), _taskQue(queSize) {}

    void start() {
        for (size_t i = 0; i < _threadNum; ++i) {
            _threads.emplace_back(&ThreadPool::doTask, this);
        }
    }

    void addTask(function<void()> task) {
        _taskQue.push(task);
    }

    void stop() {
        // 解决第1个问题：任务没执行完不能退出
        while (!_taskQue.empty()) {   // 关键！
            this_thread::sleep_for(chrono::seconds(1));
        }

        _isExit = true;
        _taskQue.wakeup();            // 解决第2个问题

        for (auto& th : _threads) {
            th.join();
        }
    }

private:
    void doTask() {
        while (!_isExit) {
            auto task = _taskQue.pop();
            if (task) {
                task();               // 执行任务
            }
        }
    }
};

// 测试任务
class MyTask {
public:
    void process(int id) {
        cout << "任务 " << id << " 执行中... (线程id: " 
             << this_thread::get_id() << ")" << endl;
        this_thread::sleep_for(chrono::milliseconds(500));
    }
};

int main() {
    ThreadPool pool(4, 10);   // 4个线程，队列容量10
    pool.start();

    MyTask task;
    for (int i = 0; i < 20; ++i) {
        pool.addTask([=, &task](){ task.process(i); });
    }

    pool.stop();              // 安全退出
    cout << "线程池已安全退出！" << endl;
    return 0;
}