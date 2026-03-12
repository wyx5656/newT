#include <iostream>
#include <string>
using namespace std;

// 抽象产品类
class Figure {
public:
    virtual void display() = 0;
    virtual double area() = 0;
    virtual ~Figure() {}
};

// 具体产品类
class Rectangle : public Figure {
private:
    double width, height;
public:
    Rectangle(double w = 1.0, double h = 1.0) : width(w), height(h) {}
    
    void display() override {
        cout << "Rectangle: " << width << " x " << height << endl;
    }
    
    double area() override {
        return width * height;
    }
};

class Circle : public Figure {
private:
    double radius;
public:
    Circle(double r = 1.0) : radius(r) {}
    
    void display() override {
        cout << "Circle: radius = " << radius << endl;
    }
    
    double area() override {
        const double PI = 3.14159265359;
        return PI * radius * radius;
    }
};

// 抽象工厂类
class FigureFactory {
public:
    virtual Figure* create() = 0;
    virtual ~FigureFactory() {}
};

// 具体工厂类
class RectangleFactory : public FigureFactory {
public:
    Figure* create() override {
        return new Rectangle();
    }
};

class CircleFactory : public FigureFactory {
public:
    Figure* create() override {
        return new Circle();
    }
};

// 使用示例
void testFactoryMethod() {
    cout << "=== 工厂方法模式测试 ===" << endl;
    
    FigureFactory* rectFactory = new RectangleFactory();
    FigureFactory* circleFactory = new CircleFactory();
    
    Figure* rect = rectFactory->create();
    Figure* circle = circleFactory->create();
    
    rect->display();
    cout << "Area: " << rect->area() << endl;
    
    circle->display();
    cout << "Area: " << circle->area() << endl;
    
    delete rect;
    delete circle;
    delete rectFactory;
    delete circleFactory;
}