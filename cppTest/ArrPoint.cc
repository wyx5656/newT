#include <iostream>
using namespace std;

void test()
{
    int arr[5] = {1,2,3,4,5};
    cout << arr << endl;
    cout << arr + 1 << endl;
    cout << &arr << endl;
    cout << &arr + 1 << endl;

    int(*p)[5] = &arr;
    for (int i = 0; i < 5; i++)
    {
        cout << (*p)[i] << " ";
    }
    cout << endl;
}

void test1()
{
    int num =1, num2 = 2, num3 = 3;
    int *p1 = &num;
    int *p2 = &num2;
    int *p3 = &num3;
    int* arr[3] = {p1,p2,p3};
    for(int i = 0; i < 3; ++i)
    {
        cout << *arr[i] << " ";
    }

    cout << endl;
}

void test2()
{
    int num =100;
    int *p = &num;
    cout << &p << endl;
    cout << p << endl;
    cout << &num << endl;

    int & ref = num;
    cout << &ref << endl;
    cout << ref << endl;
}

void test3()
{
    const int num = 200;
    const int *p = &num;
    int *p1 = const_cast<int*>(p);

    *p1 = 10;
    cout << *p1 << endl;
    cout << num << endl;
    cout << p1 << endl;
    cout << &num << endl;
}
int main()
{
    test3();
    return 0;
}