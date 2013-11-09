// 测试拷贝构造函数和赋值运算符
#include <iostream>

using namespace std;

class A
{
public:
    A( int i ): m_i( i )
    {
        cout << "调用构造函数" << endl;
    }
    A( const A& a )
    {
        cout << "调用拷贝构造函数" << endl;
        m_i = a.m_i;
    }

    A& operator=( const A& a )
    {
        cout << "调用赋值运算符重载" << endl;
        this->m_i = a.m_i;
        return *this;
    }
private:
    int m_i;
};

void f1( const A& a )
{
    cout << "调用函数f1()，使用const引用" << endl;
}

void f2( A a )
{
    cout << "调用函数f2(),使用复制" << endl;
}

void f3( A& a )
{
    cout << "调用函数f3(),使用引用" << endl;
}

A f4()
{
    cout << "调用函数f4()，直接返回整数" << endl;
    return 1;
}

A f5()
{
    cout << "调用函数f5()，直接返回局部对象" << endl;
    A a( 11 );
    return a;
}

A f6()
{
    cout << "调用函数f6()，直接返回临时对象" << endl;
    return A( 11 );
}

int main()
{
    cout << "-----------------------" << endl;
    A a( 10 );
    cout << "-----------------------" << endl;
    A b = a;
    cout << "-----------------------" << endl;
    A c( a );
    cout << "-----------------------" << endl;
    c = b;
    cout << "-----------------------" << endl;

    cout << endl;
    cout << "************************" << endl;
    A d = 200;
    cout << "************************" << endl;
    A e( 300 );
    cout << "************************" << endl;
    e = 1000;
    cout << "************************" << endl;

    cout << endl;
    cout << "-----------------------" << endl;
    f1( c );
    cout << "-----------------------" << endl;
    f2( c );
    cout << "-----------------------" << endl;
    f1( 120 );
    cout << "-----------------------" << endl;
    f2( 100 );
    cout << "-----------------------" << endl;
    //f3(1000); // 取消注释，得到C2664 error

    A f = f4();
    cout << "-----------------------" << endl;
    A g = f5();
    cout << "-----------------------" << endl;
    A h = f6();
    cout << "-----------------------" << endl;
    return 0;
}
