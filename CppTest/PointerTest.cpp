#include <iostream>
using namespace std;

int main()
{
    int a = 3;
    int* p = &a;

    cout << a << endl;
    cout << &a << endl;
    cout << p << endl;
    cout << *p << endl;

    *p = 10;
    cout << a << endl;
    cout << &a << endl;
    cout << p << endl;
    cout << *p << endl;


    cout << endl;

    int* q = p;
    cout << *q << endl;
    cout << q << endl;
    *q = 20;

    cout << endl;
    cout << a << endl;
    cout << &a << endl;
    cout << p << endl;
    cout << *p << endl;
    cout << endl;
    cout << *q << endl;
    cout << q << endl;
    cout << endl;

    *( &a ) = 100;
    cout << a << endl;
    return 0;
}
