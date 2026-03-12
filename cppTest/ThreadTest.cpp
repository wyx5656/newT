#include <iostream>
#include <thread>
#include <functional>
using namespace std;

void threadFunc(int x) {
    cout << "普通函数执行，x = " << x 
         << " 线程id: " << this_thread::get_id() << endl;
}

// 2. 函数指针
void (*pFunc)(int) = threadFunc;

// 3. 函数引用
void (&rFunc)(int) = threadFunc;

class Example {
public:
    void operator()(int x) {
        cout << "函数对象执行，x = " << x << endl;
    }
};

int main() {
    cout << "主线程id: " << this_thread::get_id() << endl;

    // 1. 普通函数
    thread t1(threadFunc, 1);
    // 2. 函数指针
    thread t2(pFunc, 2);
    // 3. 函数引用
    thread t3(rFunc, 3);
    // 4. 函数对象
    Example ex;
    thread t4(ex, 4);
    // 5. lambda
    thread t5([](int x){ cout << "lambda 执行，x = " << x << endl; }, 5);
    // 6. std::function
    function<void(int)> f = [](int x){ cout << "function 执行，x = " << x << endl; };
    thread t6(f, 6);
    // 7. bind
    thread t7(bind(threadFunc, 7));

    t1.join(); t2.join(); t3.join(); t4.join();
    t5.join(); t6.join(); t7.join();
}