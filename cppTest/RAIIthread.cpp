#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
using namespace std;

atomic<int> gVal_atomic(0);           // 无锁
mutex mtx;
int gVal_mutex = 0;

class MutexLockGuard {                // 自定义RAII
    mutex& _mtx;
public:
    MutexLockGuard(mutex& m) : _mtx(m) { _mtx.lock(); }
    ~MutexLockGuard() { _mtx.unlock(); }
};

void threadFunc() {
    for (int i = 0; i < 10000000; ++i) {
        // 方式1：atomic（最快）
        ++gVal_atomic;

        // 方式2：lock_guard（推荐，最常用）
        lock_guard<mutex> lg(mtx);
        ++gVal_mutex;

        // 方式3：unique_lock（最灵活）
        // unique_lock<mutex> ul(mtx);
        // ++gVal_mutex;
        // ul.unlock();  // 可以手动解锁
    }
}

int main() {
    vector<thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(threadFunc);
    }
    for (auto& t : threads) t.join();

    cout << "atomic结果: " << gVal_atomic << endl;
    cout << "mutex结果: " << gVal_mutex << endl;   // 应为 100000000
}