#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

// 抽象观察者类
class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const string& message) = 0;
};

// 抽象主题类（被观察者）
class Subject {
protected:
    vector<Observer*> observers;
    
public:
    virtual ~Subject() = default;
    
    // 添加观察者
    virtual void attach(Observer* observer) {
        observers.push_back(observer);
    }
    
    // 移除观察者
    virtual void detach(Observer* observer) {
        auto it = find(observers.begin(), observers.end(), observer);
        if (it != observers.end()) {
            observers.erase(it);
        }
    }
    
    // 通知所有观察者
    virtual void notify(const string& message) {
        for (auto observer : observers) {
            observer->update(message);
        }
    }
};

// 具体主题类 - 图形数据管理器
class FigureManager : public Subject {
private:
    string status;
    
public:
    void setStatus(const string& newStatus) {
        status = newStatus;
        notify("FigureManager status changed to: " + status);
    }
    
    string getStatus() const {
        return status;
    }
};

// 具体观察者 - 控制台显示器
class ConsoleDisplay : public Observer {
private:
    string name;
    
public:
    ConsoleDisplay(const string& observerName) : name(observerName) {}
    
    void update(const string& message) override {
        cout << "[" << name << "] Received notification: " << message << endl;
    }
};

// 具体观察者 - 日志记录器
class Logger : public Observer {
private:
    string logFile;
    
public:
    Logger(const string& file) : logFile(file) {}
    
    void update(const string& message) override {
        cout << "[LOGGER] Writing to log: " << message << " in file: " << logFile << endl;
    }
};

// 具体观察者 - 统计计算器
class StatisticsCalculator : public Observer {
public:
    void update(const string& message) override {
        cout << "[STATS] Calculating statistics for: " << message << endl;
        int wordCount = 0;
        for (char c : message) {
            if (c == ' ') wordCount++;
        }
        cout << "[STATS] Word count: " << wordCount + 1 << endl;
    }
};

// 使用示例
void testObserverPattern() {
    cout << "=== 观察者模式测试 ===" << endl;
    
    // 创建主题对象
    FigureManager* manager = new FigureManager();
    
    // 创建观察者对象
    ConsoleDisplay* console1 = new ConsoleDisplay("Console1");
    ConsoleDisplay* console2 = new ConsoleDisplay("Console2");
    Logger* logger = new Logger("observer.log");
    StatisticsCalculator* stats = new StatisticsCalculator();
    
    // 注册观察者
    manager->attach(console1);
    manager->attach(console2);
    manager->attach(logger);
    manager->attach(stats);
    
    // 更改状态，触发通知
    cout << "\nSetting status to 'Ready':" << endl;
    manager->setStatus("Ready");
    
    cout << "\nRemoving Console2 and setting status to 'Processing':" << endl;
    manager->detach(console2);  // 移除一个观察者
    manager->setStatus("Processing");
    
    cout << "\nSetting status to 'Completed':" << endl;
    manager->setStatus("Completed");
    
    // 清理资源
    delete console1;
    delete console2;
    delete logger;
    delete stats;
    delete manager;
}

// 模板化的观察者模式（更通用的实现）
template<typename T>
class GenericObserver {
public:
    virtual ~GenericObserver() = default;
    virtual void update(const T& data) = 0;
};

template<typename T>
class GenericSubject {
protected:
    vector<GenericObserver<T>*> observers;
    
public:
    virtual ~GenericSubject() {
        // 注意：这里不应该删除观察者对象，因为它们可能在其他地方使用
    }
    
    void attach(GenericObserver<T>* observer) {
        observers.push_back(observer);
    }
    
    void detach(GenericObserver<T>* observer) {
        auto it = find(observers.begin(), observers.end(), observer);
        if (it != observers.end()) {
            observers.erase(it);
        }
    }
    
    void notify(const T& data) {
        for (auto observer : observers) {
            observer->update(data);
        }
    }
};

// 使用模板版本的例子
class TemperatureSensor : public GenericSubject<double> {
private:
    double temperature;
    
public:
    void setTemperature(double temp) {
        temperature = temp;
        notify(temperature);
    }
    
    double getTemperature() const { return temperature; }
};

class TemperatureDisplay : public GenericObserver<double> {
private:
    string name;
    
public:
    TemperatureDisplay(const string& displayName) : name(displayName) {}
    
    void update(const double& temp) override {
        cout << "[" << name << "] Current temperature: " << temp << "°C" << endl;
    }
};

class TemperatureAlarm : public GenericObserver<double> {
public:
    void update(const double& temp) override {
        if (temp > 100.0) {
            cout << "[ALARM] High temperature warning: " << temp << "°C!" << endl;
        } else if (temp < 0.0) {
            cout << "[ALARM] Low temperature warning: " << temp << "°C!" << endl;
        }
    }
};

void testGenericObserver() {
    cout << "\n=== 泛型观察者模式测试 ===" << endl;
    
    TemperatureSensor sensor;
    TemperatureDisplay display("Living Room Display");
    TemperatureDisplay kitchenDisplay("Kitchen Display");
    TemperatureAlarm alarm;
    
    sensor.attach(&display);
    sensor.attach(&kitchenDisplay);
    sensor.attach(&alarm);
    
    sensor.setTemperature(25.5);
    sensor.setTemperature(105.0);
    sensor.setTemperature(-5.0);
}

int main() {
    testObserverPattern();
    testGenericObserver();
    return 0;
}