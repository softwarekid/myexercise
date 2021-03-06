#include <iostream>

using namespace std;

/*
 * 析构函数只要最顶层的基类声明了virtual，那么
 * 子类都自动的继承virtual的特性
 */
class Test
{
public:
    virtual ~Test()
    {
        cout << "Test::~Test()" << endl;
    }
};

class Test1 : public Test
{
public:
    ~Test1()
    {
        cout << "Test1::~Test1()" << endl;
    }
};

class Test2 : public Test
{
public:
    ~Test2()
    {
        cout << "Test2::~Test2()" << endl;
    }
};

class Test11 : public Test1
{
public:
    ~Test11()
    {
        cout << "Test11::~Test11()" << endl;
    }
};


int main()
{
    Test1* pTest1 = new Test11();
    delete pTest1;
    return 0;
}
