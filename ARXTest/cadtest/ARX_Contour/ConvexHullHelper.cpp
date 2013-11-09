#include "stdafx.h"
#include "ConvexHullHelper.h"

#include <cmath>
#include <cfloat>
//#include <climits>
#include <algorithm>

/* ��������Ͷ��� */
struct Point
{
    double x;
    double y;
};

#define MAXN 10000
Point points[MAXN];     // ��������
int S[MAXN];            // graham�㷨ʹ��
int D[MAXN * 2];        // melkman�㷨ʹ��
int n;                  // ����ĸ���

/* ��������֮����� */
double dist( Point& a, Point& b )
{
    double x = a.x - b.x;
    double y = a.y - b.y;
    return sqrt( x * x + y * y );
}

double dist_2( Point& a, Point& b )
{
    double x = a.x - b.x;
    double y = a.y - b.y;
    return ( x * x + y * y );
}

/* ��a-c�Ƿ���a-b����� */
/* a-b�Ƕ�ջS�е������㣬c������Ҫ�жϵĵ� */
int isleft( Point& a, Point& b, Point& c )
{
    return ( b.x - a.x ) * ( c.y - a.y ) - ( c.x - a.x ) * ( b.y - a.y );
}

// �������Ƚ�
int cmp_real( double x, double y )
{
    double d = x - y;
    if( d < 0 ) return 1; // d < 0
    //if(d > DBL_EPSILON) return 1; // d > 0
    return 0;   // d == 0
}

int cmp_y( int i, int j )
{
    return cmp_real( points[i].y, points[j].y );
}

// ���ռ��ǵĴ�С��������
// ����Ƕ���ͬ����ôȡ����p0��Զ�ĵ�
int cmp_angle( Point& a, Point& b )
{
    // �������0����ʾ0-b��0-a���ұ�
    // ���С��0����ʾ0-b��0-a�����
    if( isleft( points[0], a, b ) > 0 ) return 1;
    if( isleft( points[0], a, b ) < 0 ) return 0;
    if( dist( points[0], a ) < dist( points[0], b ) ) return 1;
    return 0;
}

// ������x��y����
int cmp_xy( Point& a, Point& b )
{
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    if( dx < 0 ) return 1;
    if( dx < DBL_EPSILON  && dy < 0 ) return 1; // dx == 0
    return 0;
}

//�ҵ�һ������,��Ϊ�㷨����ʼ����
int getStartPoint()
{
    int k = 0;
    for( int i = 0; i < n; ++i )
    {
        double xi = points[i].x;
        double yi = points[i].y;

        double xk = points[k].x;
        double yk = points[k].y;

        if( ( yi < yk ) || ( ( yi == yk ) && ( xi < xk ) ) )
        {
            k = i;
        }
    }
    return k;
}

/* ����Graham�㷨����͹������������ջS�е�λ��:[0, top] */
void graham( int& top )
{
    //�ҵ�һ������,��Ϊ�㷨����ʼ����
    int k = getStartPoint();
    Point tmp = points[0];
    points[0] = points[k];
    points[k] = tmp;

    acutPrintf( _T( "\n��1������(%.3f, %.3f)" ), points[0].x, points[0].y );

    // ��1-n-1�ĵ㰴�ռ���[0, PI]��������
    std::sort( points + 1, points + n, cmp_angle );

    for( int i = 0; i <= 2; ++i )
    {
        S[i] = i;   //��ǰ3������ѹջ
    }

    top = 2;
    for( int i = 3; i < n; i++ )
    {
        /*����µĵ�,�������ջ�е�2�㹹����һ��"��"��, ��ջ��Ԫ�س�ջ. ֱ����ջ�����*/
        while( top > 0 && isleft( points[S[top - 1]], points[S[top]], points[i] ) < 0 )
        {
            top--;
        }

        //acutPrintf(_T("top: %d"), top);
        top++;        // ���µ�ѹջ
        S[top] = i;
    }
}

/* ����melkman�㷨����͹��,������������D�е�����λ��(bot, top) */
void melkman( int& bot, int& top )
{
    // ��1-n-1�ĵ㰴��xy��������
    std::sort( points, points + n, cmp_xy );

    int i, t;

    bot = n - 1;
    top = n;
    D[top++] = 0;
    D[top++] = 1;

    for( i = 2; i < n; i++ )
    {
        //Ѱ�ҵ������� Ҫ��֤3���㲻���ߣ���
        if( isleft( points[D[top - 2]], points[D[top - 1]], points[i] ) != 0 ) break;
        D[top - 1] = i;			//���߾͸�������
    }

    D[bot--] = i;
    D[top++] = i;		//i�ǵ������� �����ߣ���

    //��ʱ��������3���㣬Ҫ��֤3����a,b,c�ǳ���ʱ��ģ����Ǿ͵���ab
    if( isleft( points[D[n]], points[D[n + 1]], points[D[n + 2]] ) < 0 )
    {
        t = D[n];
        D[n] = D[n + 1];
        D[n + 1] = t;
    }

    for( i++; i < n; i++ )
    {
        //�����������i��͹���ڣ�����
        if( isleft( points[D[top - 2]], points[D[top - 1]], points[i] ) > 0 &&
                isleft( points[D[bot + 1]], points[D[bot + 2]], points[i] ) > 0 ) continue;

        while( isleft( points[D[top - 2]], points[D[top - 1]], points[i] ) <= 0 ) top--;
        D[top++] = i;

        while( isleft( points[D[bot + 1]], points[D[bot + 2]], points[i] ) <= 0 ) bot++;
        D[bot--] = i;
    }

    // ͹��������ɣ�D������bot+1��top-1�ھ���͹��������(ͷβ��ͬһ��)
}

// �������㷨,���÷����㷨
int CPY[MAXN], CPlen; // ����Y'�Լ�����
double closestPoint_recursion( int L, int R, int& p1, int& p2 )
{
    //acutPrintf(_T("\nL=%d, R=%d"), L, R);

    if( L >= R ) return DBL_MAX;
    if( R - L == 1 )
    {
        p1 = L;
        p2 = R;
        return dist_2( points[L], points[R] );
    }

    int mid = ( L + R ) >> 1;
    int i, j;
    //for(i=mid; i>=L /*&& !cmp_real(points[i].x, points[mid].x)*/; i--);
    //acutPrintf(_T("\nmid=%d"), mid);

    int lp1, lp2;
    double d1 = closestPoint_recursion( L, mid, lp1, lp2 );
    //acutPrintf(_T("\n����������:%.15f, p1=%d, p2=%d"), d1, lp1, lp2);

    //for(i=mid; i<=R /*&& !cmp_real(points[i].x, points[mid].x)*/; i++);
    int rp1, rp2;
    double d2 = closestPoint_recursion( mid, R, rp1, rp2 );
    //acutPrintf(_T("\n�����Ҳ����:%.15f, p1=%d, p2=%d"), d2, rp1, rp2);

    double ret;
    if( ( d1 - d2 ) < 0 ) // d1 < d2
    {
        p1 = lp1;
        p2 = lp2;
        ret = d1;
    }
    else             // d1 >= d2
    {
        p1 = rp1;
        p2 = rp2;
        ret = d2;
    }

    //acutPrintf(_T("\n�Ƚ��������࣬�õ�����:%.15f, p1=%d, p2=%d"), ret, p1, p2);

    CPlen = 0;
    for( i = L; i <= R; i++ )
    {
        double dx = ( points[i].x - points[mid].x );
        //acutPrintf(_T("\n����:(%d, %d)�ĵ�%d�������м��:%d��x�����:%.15f"), L, R, i, mid, dx);
        if( ( dx * dx - ret ) < 0 )
        {
            //acutPrintf(_T("\n��%d�������ӵ�Y��"), i);
            CPY[CPlen] = i;
            CPlen++;
        }
    }
    //acutPrintf(_T("\nY���鳤��:%d"), CPlen);

    std::sort( CPY, CPY + CPlen, cmp_y );

    for( i = 0; i < CPlen; i++ )
    {
        //acutPrintf(_T("\nY�е�%d��������(%.15f, %.15f)"), i, points[CPY[i]].x, points[CPY[i]].y);
        int cnt = 1;
        // ������8����cnt = [0, 7]
        for( j = i + 1; j < CPlen && cnt <= 7; j++ )
        {
            // ����Ҫ����
            double d = dist_2( points[CPY[i]], points[CPY[j]] );
            if( ( d - ret ) < 0 ) // d < ret
            {
                ret = d;
                p1 = CPY[i];
                p2 = CPY[j];
            }
            cnt++;
        }
    }
    return ret;
}

void closestPoint( AcGePoint2d& pt1, AcGePoint2d& pt2 )
{
    // Ԥ����(��x��y)
    std::sort( points, points + n, cmp_xy );

    for( int i = 0; i < n; i++ )
    {
        //acutPrintf(_T("\n����:%.15f, %.15f"), points[i].x, points[i].y);
    }
    acutPrintf( _T( "\n" ) );
    int p1, p2;
    double d = closestPoint_recursion( 0, n - 1, p1, p2 );

    pt1.set( points[p1].x, points[p1].y );
    pt2.set( points[p2].x, points[p2].y );
    //acutPrintf(_T("\n�������:%.15f"), d);
}

static bool ReadPoints( const AcGePoint2dArray& V )
{
    n = V.length();
    if( n > MAXN ) return false;

    for( int i = 0; i < n; i++ )
    {
        points[i].x = V[i].x;
        points[i].y = V[i].y;
    }
    return true;
}

static void Graham_GetHullPoints( int top, AcGePoint2dArray& H )
{
    for( int i = 0; i <= top; ++i )
    {
        double x = points[S[i]].x;
        double y = points[S[i]].y;
        H.append( AcGePoint2d( x, y ) );
    }
}

static void Melkman_GetHullPoints( int bot, int top, AcGePoint2dArray& H )
{
    for( int i = bot + 1; i < top - 1; i++ )
    {
        double x = points[D[i]].x;
        double y = points[D[i]].y;
        H.append( AcGePoint2d( x, y ) );
    }
}

static AcGePoint2d Point3D_To_2D( const AcGePoint3d& pt )
{
    return AcGePoint2d( pt.x, pt.y );
}

static AcGePoint3d Point2D_To_3D( const AcGePoint2d& pt )
{
    return AcGePoint3d( pt.x, pt.y, 0 );
}

bool Graham_ConvexHull_2D( const AcGePoint2dArray& V, AcGePoint2dArray& H )
{
    // ������3��������
    if( V.length() < 3 )
    {
        //acutPrintf(_T("\n��ĸ���%d < 3"), V.length());
        return false;
    }

    // ��V�ж�ȡ���ݵ�ȫ�ֱ���points��
    if( !ReadPoints( V ) )
    {
        //acutPrintf(_T("\n��ĸ���%d ���� %d"), V.length(), MAXN);
        return false;
    }

    // ʹ��GrahamScan�㷨����͹����������stack������
    // ����������������λ��top
    int top;
    graham( top );

    //acutPrintf(_T("\n���ص�S������������:%d"), top);

    // ��S�����л�ȡ͹��
    Graham_GetHullPoints( top, H );

    return true;
}

bool Melkman_ConvexHull_2D( const AcGePoint2dArray& V, AcGePoint2dArray& H )
{
    // ������3��������
    if( V.length() < 3 )
    {
        //acutPrintf(_T("\n��ĸ���%d < 3"), V.length());
        return false;
    }

    // ��V�ж�ȡ���ݵ�ȫ�ֱ���points��
    if( !ReadPoints( V ) )
    {
        //acutPrintf(_T("\n��ĸ���%d ���� %d"), V.length(), MAXN);
        return false;
    }

    // ʹ��melkman�㷨����͹����������D������
    // ��������ĵײ�bot�Ͷ���top
    int bot, top;
    melkman( bot, top );

    //acutPrintf(_T("\n���ص�D����ĵײ�:%d������:%d"), bot, top);

    // ��stack�����л�ȡ͹��
    Melkman_GetHullPoints( bot, top, H );

    return true;
}

static bool ConvexHull_3D( const AcGePoint3dArray& V, AcGePoint3dArray& H, bool useGrahamOrMelkman )
{
    AcGePoint2dArray V_2d, H_2d;
    int len = V.length();
    for( int i = 0; i < len; i++ )
    {
        V_2d.append( Point3D_To_2D( V[i] ) );
    }

    bool ret;
    if( useGrahamOrMelkman )
    {
        ret = Graham_ConvexHull_2D( V_2d, H_2d );
    }
    else
    {
        ret = Melkman_ConvexHull_2D( V_2d, H_2d );
    }

    if( ret )
    {
        H.removeAll();
        int len = H_2d.length();
        for( int i = 0; i < len; i++ )
        {
            H.append( Point2D_To_3D( H_2d[i] ) );
        }
    }
    return ret;
}

bool Graham_ConvexHull_3D( const AcGePoint3dArray& V, AcGePoint3dArray& H )
{
    return ConvexHull_3D( V, H, true );
}

bool Melkman_ConvexHull_3D( const AcGePoint3dArray& V, AcGePoint3dArray& H )
{
    return ConvexHull_3D( V, H, false );
}

bool ClosestPoint_2D( const AcGePoint2dArray& V, AcGePoint2d& pt1, AcGePoint2d& pt2 )
{
    if( !ReadPoints( V ) )
    {
        //acutPrintf(_T("\n��ĸ���%d ���� %d"), V.length(), MAXN);
        return false;
    }

    // ����������
    closestPoint( pt1, pt2 );

    return true;
}

bool ClosestPoint_3D( const AcGePoint3dArray& V, AcGePoint3d& pt1, AcGePoint3d& pt2 )
{
    AcGePoint2dArray V_2d;
    int len = V.length();
    for( int i = 0; i < len; i++ )
    {
        V_2d.append( Point3D_To_2D( V[i] ) );
    }

    AcGePoint2d pt1_2d, pt2_2d;
    bool ret = ClosestPoint_2D( V_2d, pt1_2d, pt2_2d );
    if( ret )
    {
        pt1 = Point2D_To_3D( pt1_2d );
        pt2 = Point2D_To_3D( pt2_2d );
    }
    return ret;
}