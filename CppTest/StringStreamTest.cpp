#include <iostream>
#include <sstream>
#include <string>
using namespace std;


int main()
{
    stringstream ss;
    ss << "hello";
    ss << " world";
    //ss<<" c++"<<endl;
    ss << " c++";
    cout << ss.str() << endl;
    return 0;
}

