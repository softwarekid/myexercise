#include <iostream>
#include <map>
#include <string>
using namespace std;

struct IntData
{
    IntData( int aa, int bb ) : a( aa ), b( bb ) {}
    int a;
    int b;
};

int main()
{
    typedef map<string, IntData*> IntDataMap;
    IntDataMap idm;
    idm.insert( IntDataMap::value_type( "a", new IntData( 1, 2 ) ) );
    idm.insert( IntDataMap::value_type( "b", new IntData( 1, 2 ) ) );
    idm.insert( IntDataMap::value_type( "c", new IntData( 1, 2 ) ) );
    idm.insert( IntDataMap::value_type( "d", new IntData( 1, 2 ) ) );

    IntData* pData = idm["b"];
    cout << pData->a << "\t" << pData->b << endl;
    pData->a = 11;
    pData->b = 22;
    cout << pData->a << "\t" << pData->b << endl;
    return 0;
}
