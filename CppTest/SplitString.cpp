#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

typedef vector<string> StringArray;

void SplitCString( const string& content, const string& token, StringArray& cc )
{
    const char* delimeter =  token.c_str();
    char* pch = strtok( ( char* )content.c_str(), delimeter );
    while( pch != NULL )
    {
        cc.push_back( pch );
        pch = strtok( NULL, delimeter );
    }
}

int main()
{
    string s = " wo de zu guo ";
    StringArray sa;
    SplitCString( s, " \t", sa );

    for( StringArray::iterator itr = sa.begin(); itr != sa.end(); ++itr )
    {
        cout << "--" << *itr << "--" << endl;
    }

    const string s2 = "v12";
    cout << s2.substr( 1 ) << endl;

    ifstream inFile( "test.txt" );
    string strLine;
    while( getline( inFile, strLine ) )
    {
        cout << strLine << endl;
    }
    inFile.close();

    return 0;
}
