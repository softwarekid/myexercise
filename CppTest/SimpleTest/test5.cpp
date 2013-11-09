#include <iostream>
#include <string>
#include <fstream>
using namespace std;

int main()
{
    string inFilePath = "ddddd.txt";
    string outFilePath = "aaa.rtf";
    ifstream inFile( inFilePath.c_str() );
    if( !inFile ) return 0;

    ofstream outFile( outFilePath.c_str() );
    while( !inFile.eof() )
    {
        string value;
        getline( inFile, value );
        outFile << value;
    }
    inFile.close();
    outFile.close();

    return 0;
}
