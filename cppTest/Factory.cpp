#include <iostream>
#include <cmath>
using namespace std;
class Figure
{
public:
    virtual void display() = 0;
    virtual double area() = 0;
    virtual ~Figure() {}
};

class Rectangle : public Figure
{
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

class Circle : public Figure
{
private:
    double radius;
public:
    Circle(double r = 1.0) : radius(r) {}
    
    void display() override {
        cout << "Circle: radius = " << radius << endl;
    }
    
    double area() override {
        return M_PI * radius * radius;
    }
};

class Factory
{
public:
    static Figure* create(const string& name)
    {
        if(name == "rectangle") return new Rectangle();
        if (name == "circle") return new Circle(5);
        return nullptr;
    }
};

/* 
class Factory
{
public:
    virtual Figure* create() = 0;
    virtual ~Factory() {};
};

class RectangleFactory
:public Factory
{
public:
    Figure* crea
}
    */