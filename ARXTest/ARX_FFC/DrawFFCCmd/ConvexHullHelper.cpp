#include "stdafx.h"
#include "ConvexHullHelper.h"

#include <cmath>
#include <cfloat>
//#include <climits>
#include <algorithm>

/* 顶点的类型定义 */
struct Point
{
    double x;
    double y;
};

#define MAXN 10000
Point points[MAXN];     // 顶点数组
int S[MAXN];            // graham算法使用
int D[MAXN * 2];        // melkman算法使用
int n;                  // 顶点的个数

/* 计算两点之间距离 */
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

/* 即a-c是否在a-b的左边 */
/* a-b是堆栈S中的两个点，c则是需要判断的点 */
int isleft( Point& a, Point& b, Point& c )
{
    return ( b.x - a.x ) * ( c.y - a.y ) - ( c.x - a.x ) * ( b.y - a.y );
}

// 浮点数比较
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

// 按照极角的大小递增排序
// 如果角度相同，那么取距离p0最远的点
int cmp_angle( Point& a, Point& b )
{
    // 如果大于0，表示0-b在0-a的右边
    // 如果小于0，表示0-b在0-a的左边
    if( isleft( points[0], a, b ) > 0 ) return 1;
    if( isleft( points[0], a, b ) < 0 ) return 0;
    if( dist( points[0], a ) < dist( points[0], b ) ) return 1;
    return 0;
}

// 按照先x后y排序
int cmp_xy( Point& a, Point& b )
{
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    if( dx < 0 ) return 1;
    if( dx < DBL_EPSILON  && dy < 0 ) return 1; // dx == 0
    return 0;
}

//找第一个顶点,做为算法的起始顶点
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

/* 采用Graham算法计算凸包，并返回在栈S中的位置:[0, top] */
void graham( int& top )
{
    //找第一个顶点,做为算法的起始顶点
    int k = getStartPoint();
    Point tmp = points[0];
    points[0] = points[k];
    points[k] = tmp;

    acutPrintf( _T( "\n第1点坐标(%.3f, %.3f)" ), points[0].x, points[0].y );

    // 将1-n-1的点按照极角[0, PI]进行排序
    std::sort( points + 1, points + n, cmp_angle );

    for( int i = 0; i <= 2; ++i )
    {
        S[i] = i;   //将前3个顶点压栈
    }

    top = 2;
    for( int i = 3; i < n; i++ )
    {
        /*如果新的点,与最近入栈中的2点构成了一个"凹"角, 则将栈顶元素出栈. 直到把栈检查完*/
        while( top > 0 && isleft( points[S[top - 1]], points[S[top]], points[i] ) < 0 )
        {
            top--;
        }

        //acutPrintf(_T("top: %d"), top);
        top++;        // 将新点压栈
        S[top] = i;
    }
}

/* 采用melkman算法计算凸包,并返回在数组D中的区间位置(bot, top) */
void melkman( int& bot, int& top )
{
    // 将1-n-1的点按照xy进行排序
    std::sort( points, points + n, cmp_xy );

    int i, t;

    bot = n - 1;
    top = n;
    D[top++] = 0;
    D[top++] = 1;

    for( i = 2; i < n; i++ )
    {
        //寻找第三个点 要保证3个点不共线！！
        if( isleft( points[D[top - 2]], points[D[top - 1]], points[i] ) != 0 ) break;
        D[top - 1] = i;			//共线就更换顶点
    }

    D[bot--] = i;
    D[top++] = i;		//i是第三个点 不共线！！

    //此时队列中有3个点，要保证3个点a,b,c是成逆时针的，不是就调换ab
    if( isleft( points[D[n]], points[D[n + 1]], points[D[n + 2]] ) < 0 )
    {
        t = D[n];
        D[n] = D[n + 1];
        D[n + 1] = t;
    }

    for( i++; i < n; i++ )
    {
        //如果成立就是i在凸包内，跳过
        if( isleft( points[D[top - 2]], points[D[top - 1]], points[i] ) > 0 &&
                isleft( points[D[bot + 1]], points[D[bot + 2]], points[i] ) > 0 ) continue;

        while( isleft( points[D[top - 2]], points[D[top - 1]], points[i] ) <= 0 ) top--;
        D[top++] = i;

        while( isleft( points[D[bot + 1]], points[D[bot + 2]], points[i] ) <= 0 ) bot++;
        D[bot--] = i;
    }

    // 凸包构造完成，D数组里bot+1至top-1内就是凸包的序列(头尾是同一点)
}

// 最近点对算法,采用分治算法
int CPY[MAXN], CPlen; // 数组Y'以及长度
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
    //acutPrintf(_T("\n计算左侧距离:%.15f, p1=%d, p2=%d"), d1, lp1, lp2);

    //for(i=mid; i<=R /*&& !cmp_real(points[i].x, points[mid].x)*/; i++);
    int rp1, rp2;
    double d2 = closestPoint_recursion( mid, R, rp1, rp2 );
    //acutPrintf(_T("\n计算右侧距离:%.15f, p1=%d, p2=%d"), d2, rp1, rp2);

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

    //acutPrintf(_T("\n比较左右两侧，得到距离:%.15f, p1=%d, p2=%d"), ret, p1, p2);

    CPlen = 0;
    for( i = L; i <= R; i++ )
    {
        double dx = ( points[i].x - points[mid].x );
        //acutPrintf(_T("\n区间:(%d, %d)的第%d个点与中间点:%d的x轴距离:%.15f"), L, R, i, mid, dx);
        if( ( dx * dx - ret ) < 0 )
        {
            //acutPrintf(_T("\n第%d个点添加到Y中"), i);
            CPY[CPlen] = i;
            CPlen++;
        }
    }
    //acutPrintf(_T("\nY数组长度:%d"), CPlen);

    std::sort( CPY, CPY + CPlen, cmp_y );

    for( i = 0; i < CPlen; i++ )
    {
        //acutPrintf(_T("\nY中第%d个点坐标(%.15f, %.15f)"), i, points[CPY[i]].x, points[CPY[i]].y);
        int cnt = 1;
        // 至多有8个点cnt = [0, 7]
        for( j = i + 1; j < CPlen && cnt <= 7; j++ )
        {
            // 不需要开方
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
    // 预排序(先x后y)
    std::sort( points, points + n, cmp_xy );

    for( int i = 0; i < n; i++ )
    {
        //acutPrintf(_T("\n坐标:%.15f, %.15f"), points[i].x, points[i].y);
    }
    acutPrintf( _T( "\n" ) );
    int p1, p2;
    double d = closestPoint_recursion( 0, n - 1, p1, p2 );

    pt1.set( points[p1].x, points[p1].y );
    pt2.set( points[p2].x, points[p2].y );
    //acutPrintf(_T("\n最近距离:%.15f"), d);
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
    // 至少有3个点数据
    if( V.length() < 3 )
    {
        //acutPrintf(_T("\n点的个数%d < 3"), V.length());
        return false;
    }

    // 从V中读取数据到全局变量points中
    if( !ReadPoints( V ) )
    {
        //acutPrintf(_T("\n点的个数%d 超过 %d"), V.length(), MAXN);
        return false;
    }

    // 使用GrahamScan算法计算凸包，保存在stack数组中
    // 返回数组的最大索引位置top
    int top;
    graham( top );

    //acutPrintf(_T("\n返回的S数组的最大索引:%d"), top);

    // 从S数组中获取凸包
    Graham_GetHullPoints( top, H );

    return true;
}

bool Melkman_ConvexHull_2D( const AcGePoint2dArray& V, AcGePoint2dArray& H )
{
    // 至少有3个点数据
    if( V.length() < 3 )
    {
        //acutPrintf(_T("\n点的个数%d < 3"), V.length());
        return false;
    }

    // 从V中读取数据到全局变量points中
    if( !ReadPoints( V ) )
    {
        //acutPrintf(_T("\n点的个数%d 超过 %d"), V.length(), MAXN);
        return false;
    }

    // 使用melkman算法计算凸包，保存在D数组中
    // 返回数组的底部bot和顶部top
    int bot, top;
    melkman( bot, top );

    //acutPrintf(_T("\n返回的D数组的底部:%d，顶部:%d"), bot, top);

    // 从stack数组中获取凸包
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
        //acutPrintf(_T("\n点的个数%d 超过 %d"), V.length(), MAXN);
        return false;
    }

    // 计算最近点对
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