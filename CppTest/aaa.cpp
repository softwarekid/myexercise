#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
    int n, i, j, k, x, c[100];
    char a[201][1000];
    memset( c, 0, sizeof( c ) );
    scanf( "%d", &n );
    for( i = 0; i < 2 * n; i++ )
        scanf( "%s", a[i] );
    for( i = 0; i < 2 * n; i += 2 )
    {
        for( j = 0, k = 0; a[i][j] != '\0' || a[i + 1][k] != '\0'; j++, k++ )
        {
            if( a[i][j] != a[i + 1][k] )
            {
                if( a[i][j + 1] == a[i + 1][k] )
                {
                    c[i] += 1;
                    for( x = j; a[i][x] != '\0'; x++ )
                    {
                        a[i][x] = a[i][x + 1];

                    }
                    j = j - 1;
                }
                if( a[i][j] == a[i + 1][k + 1] )
                {
                    c[i] += 1;
                    for( x = k; a[i + 1][x] != '\0'; x++ )
                    {
                        a[i + 1][x] = a[i + 1][x + 1];

                    }
                    k = k - 1;
                }
            }
        }
        if( j != k )
            c[i] += abs( k - j );
    }
    for( i = 0; i < 2 * n; i += 2 )
        printf( "%d\n", c[i] );
    return 0;

}
