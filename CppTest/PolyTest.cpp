#include <iostream>
#include <vector>

using namespace std;

typedef vector<double> DoubleArray;

// m�ĵ�n�ף���n(n-1)(n-2)...(n-m+1)
int factorial( int m, int n )
{
    if( n > m ) return 0;

    int k = m;
    int c = 1;
    for( int j = 0; j < n; j++ )
    {
        c *= k;
        k--;
    }
    return c;
}

double CaclPolyomial( const DoubleArray& a, int n, double q )
{
    double s = 0;
    double t = 1;

    int len = a.size();
    for( int i = n; i < len; i++ )
    {
        s += factorial( i, n ) * a[i] * t;
        t *= q;
    }
    return s;
}

int main()
{
    DoubleArray a;
    a.push_back( 1046.3 );
    a.push_back( 5 );
    a.push_back( -0.85 );
    a.push_back( 0 );
    a.push_back( 0 );
    a.push_back( 0 );

    cout << CaclPolyomial( a, 0, 20 ) << endl;
    cout << CaclPolyomial( a, 1, 20 ) << endl;
    cout << CaclPolyomial( a, 2, 20 ) << endl;
    cout << CaclPolyomial( a, 3, 20 ) << endl;
    cout << CaclPolyomial( a, 4, 20 ) << endl;

    cout << CaclPolyomial( a, 0, 18.049 ) << endl;
    cout << CaclPolyomial( a, 1, 18.049 ) << endl;
    cout << CaclPolyomial( a, 2, 18.049 ) << endl;
    cout << CaclPolyomial( a, 3, 18.049 ) << endl;
    cout << CaclPolyomial( a, 4, 18.049 ) << endl;

    cout << CaclPolyomial( a, 0, 20.8828 ) << endl;

    return 0;
}
