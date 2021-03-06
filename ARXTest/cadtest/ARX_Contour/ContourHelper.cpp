#include "StdAfx.h"
#include "ContourHelper.h"
#include "TriContour.h"
#include "CADDrawHelper.h"

#include "ContourParamHelper.h"

#include <algorithm>
#include <vector>
#include <numeric>

#include "../ArxHelper/HelperClass.h"

// 三角形网格法生成等值线
#include "../Contour/Contour.h"
#pragma comment(lib, "Contour.lib")

/* 全局函数(实现在KrigingInterpolater.cpp) */
extern double KrigingInterpolater( const AcGePoint3dArray& datas, const AcGePoint3d& pt, double dSemivariance );
extern void KrigingInterpolater_More( const AcGePoint3dArray& datas, AcGePoint3dArray& pts, int nDiameter, double dSemivariance );

static void DrawOrginData( const AcGePoint3dArray& datas, double textHeight )
{
    int len = datas.length();
    for( int i = 0; i < len; i++ )
    {
        AcGePoint3d pt = datas[i];

        CString text;
        text.Format( _T( "%.3f" ), pt.z );
        pt.z = 0;

        DrawMText( pt, 0, text, textHeight / 2 );
        DrawEllipse( pt, textHeight, textHeight );
    }
}

// pts.length() == nDiameter*nDiameter
// 坐标点的z值保存使用插值算法计算的值
static void DrawGrid( int nDiameter, const AcGePoint3dArray& pts, double textHeight )
{
    // 绘制原始数据
    // 绘制网格
    for( int i = 0; i < nDiameter; i++ )
    {
        for( int j = 0; j < nDiameter; j++ )
        {
            AcGePoint3d pt = pts[i * nDiameter + j];

            CString text;
            text.Format( _T( "%.3f" ), pt.z );

            pt.z = 0;
            DrawMText( pt, 0, text, textHeight ); // 标注文字
        }

        AcGePoint3dArray tempPts;
        for( int j = 0; j < nDiameter; j++ )
        {
            AcGePoint3d pt = pts[i * nDiameter + j];
            pt.z = 0;

            tempPts.append( pt );
        }
        DrawLines( tempPts );

        tempPts.removeAll();
        for( int j = 0; j < nDiameter; j++ )
        {
            AcGePoint3d pt = pts[j * nDiameter + i];
            pt.z = 0;

            tempPts.append( pt );
        }
        DrawLines( tempPts );
    }
}

/*
 * 网格计算的思路
 * 指定不规则多边形，划分网格
 * 网格中可能存在不规则的形状，例如三角形，四边形等
 * 计算每一个多边形的平均z值(通常直接采用平均方法，采用插值方法计算多边形顶点的z值，然后求平均)
 * 计算不规则多边形中的所有"网格"的面积*平均z值，并求总和，然后除以整个多边形的面积
 * 得到多边形总的平均z值
 *
 * 下面的代码思路：
 * 1) 将所有的多边形都处理成矩形，显然每一个小矩形的面积都是相同的
 * 2) 将网格划分成(n+1)*(n+1)个，取每一个小矩形的中心点进行插值，作为矩形的平均值
 * 3) 由于矩形的面积是相同的，可以简化成所有中心点z值求和取平均值
 *
 * 计算很可能存在误差
 ------------------------------------------------
 |		-		|		-		|		-		|
 ------------------------------------------------
 |		-		|		-		|		-		|
 ------------------------------------------------
 |		-		|		-		|		-		|
 ------------------------------------------------
 |		-		|		-		|		-		|
 ------------------------------------------------
 如上，要计算3*3的网格，实际将网格划分成4*4
 */
double ContourHelper::GridCacl( const AcGePoint3dArray& datas, const AcGePoint3d& minPt, const AcGePoint3d& maxPt, int nDiameter, double dSemivariance )
{
    // 每一个小网格的宽(dx)和高(dy)
    double dx = ( maxPt.x - minPt.x ) / ( nDiameter + 1 );
    double dy = ( maxPt.y - minPt.y ) / ( nDiameter + 1 );

    // 字体高度
    double c = 0.2;
    double textHeight = int( min( dx, dy ) * c );

    AcGePoint3d basePt = minPt;
    basePt.x = basePt.x + dx / 2;
    basePt.y = basePt.y + dy / 2;

    // 准备网格坐标数据
    AcGePoint3dArray pts;
    for( int i = 0; i < nDiameter; i++ )
    {
        for( int j = 0; j < nDiameter; j++ )
        {
            // 坐标系从左下角开始
            double xpos = basePt.x + j * dx;
            double ypos = basePt.y + i * dy;
            pts.append( AcGePoint3d( xpos, ypos, 0 ) );
        }
    }

    // 使用Kriging插值算法，返回值保存在pts的z值
    KrigingInterpolater_More( datas, pts, nDiameter, dSemivariance );

    // 绘制原始数据
    DrawOrginData( datas, 4 );
    DrawGrid( nDiameter, pts, textHeight );

    // 计算平均值
    double sum = 0;
    int len = pts.length();
    for( int i = 0; i < len; i++ )
    {
        sum += pts[i].z;
    }
    return sum / len;
}

double ContourHelper::PointCacl( const AcGePoint3dArray& datas, const AcGePoint3d& pt, double dSemivariance )
{
    return KrigingInterpolater( datas, pt, dSemivariance );
}

static void GetMinMaxZValue( const AcGePoint3dArray& datas, double& minZ, double& maxZ )
{
    using namespace std;
    int len = datas.length();
    vector<double> zs;
    vector<double>::iterator itr;
    for( int i = 0; i < len; i++ )
    {
        zs.push_back( datas[i].z );
    }
    itr = min_element( zs.begin(), zs.end() );
    minZ = *itr;

    itr = max_element( zs.begin(), zs.end() );
    maxZ = *itr;
}

static bool CreateTriContour( double z, const AcGePoint3dArray& pts, bool bSmooth, double textHeight )
{
    TriContour* pTC = new TriContour();
    pTC->setZValue( z );

    int n = pts.length();
    for( int i = 0; i < n; i++ )
    {
        pTC->addPoint( pts[i] );
    }
    pTC->label(); // 计算标注参数(位置及方向)
    pTC->smooth( bSmooth );
    pTC->setTextHeight( textHeight );

    // 添加到数据库
    bool ret = ArxUtilHelper::PostToModelSpace( pTC );
    if( ret )
    {
        pTC->close();
    }
    else
    {
        delete pTC;
    }
    return ret;
}

static void PointToAcGePoint( const PointArray& pa, AcGePoint3dArray& pts )
{
    for( int i = 0; i < pa.size(); i++ )
    {
        pts.append( AcGePoint3d( pa[i].x, pa[i].y, 0 ) );
    }
}

static void DrawContourHelper( const DoubleArray& zValues, const PointArray& cnpts, const ContourArray& cna )
{
    // 获取等值线全局参数
    bool bSmooth;
    double textHeight;
    ContourParamHelper::ReadParams( bSmooth, textHeight );

    //acutPrintf(_T("\n"));
    for( int i = 0; i < ( int )cna.size(); i++ )
    {
        DT_Contour z_contour = cna[i];
        int zi = z_contour.zi;
        int cnpts_num = z_contour.cnpts_num;
        //acutPrintf(_T("第%d条等值线, z=%.3f\n"), i, zValues[zi]);

        // 定位到第i条等值线位置
        int s = 0;
        for( int j = 0; j < i; j++ )
        {
            s += cna[j].cnpts_num;
        }
        int t = s + cna[i].cnpts_num;

        PointArray pa;
        for( int j = s; j < t; j++ )
        {
            pa.push_back( cnpts[j] );
            //acutPrintf(_T("(%.2f, %.2f)  "), cnpts[j].x, cnpts[j].y);
        }
        //acutPrintf(_T("\n"));

        if( pa.empty() ) continue;

        AcGePoint3dArray pts;
        PointToAcGePoint( pa, pts );
        CreateTriContour( zValues[zi], pts, bSmooth, textHeight );
    }
}

static void DrawTriangle( const PointArray& pts, const EdgeArray& segments )
{
    for( int i = 0; i < segments.size(); i++ )
    {
        DT_Edge e = segments[i];
        DT_Point p1 = pts[e.s], p2 = pts[e.t];
        DrawLine( AcGePoint3d( p1.x, p1.y, 0 ), AcGePoint3d( p2.x, p2.y, 0 ) );
    }
}

static void GenerateContour( const AcGePoint3dArray& datas, const DoubleArray& zValues )
{
    PointArray pts;
    for( int i = 0; i < datas.length(); i++ )
    {
        DT_Point p = {datas[i].x, datas[i].y, datas[i].z};
        pts.push_back( p );
    }

    // 1) 三角网格剖分
    EdgeArray segments;
    PointArray holes;
    EdgeArray ea;
    TriangleArray ta;
    CDT_LIB( pts, segments, holes, ea, ta );

    // 2) 追踪等值线
    PointArray cnpts;
    ContourArray cna;
    TraceContours( pts, ea, ta, zValues, cnpts, cna );

    DrawOrginData( datas, 4 );
    DrawTriangle( pts, ea );
    DrawContourHelper( zValues, cnpts, cna );
}

void ContourHelper::DrawCountour( const AcGePoint3dArray& datas, double dz )
{
    // 初始化z值
    DoubleArray zValues;

    double minZ, maxZ;
    GetMinMaxZValue( datas, minZ, maxZ );
    const double eplison = 0.001;
    //double dz = (maxZ-minZ)/n;
    for( double z = minZ; ( maxZ - z - eplison ) > 0; z += dz )
    {
        zValues.push_back( z );
    }

    GenerateContour( datas, zValues );
}

void ContourHelper::DrawCountour2( const AcGePoint3dArray& datas, int n )
{
    // 初始化z值
    DoubleArray zValues;
    double minZ, maxZ;
    GetMinMaxZValue( datas, minZ, maxZ );
    const double eplison = 0.001;
    double dz = ( maxZ - minZ ) / n;
    for( double z = minZ; ( maxZ - z - eplison ) > 0; z += dz )
    {
        zValues.push_back( z );
    }

    GenerateContour( datas, zValues );
}

static void DrawBoundaryHelper( const PointArray& bpts, const IntArray& bpos )
{
    for( int i = 0; i < ( int )bpos.size(); i++ )
    {
        int s = 0;
        for( int j = 0; j < i; j++ )
        {
            s += bpos[j];
        }
        int t = s + bpos[i];

        PointArray pa;
        for( int j = s; j < t; j++ )
        {
            pa.push_back( bpts[j] );
        }

        AcGePoint3dArray pts;
        PointToAcGePoint( pa, pts );

        // 绘制多边形边界
        DrawPLines( pts, true );
    }
}

void ContourHelper::DrawBoundary( const AcGePoint3dArray& datas )
{
    PointArray pts;
    for( int i = 0; i < datas.length(); i++ )
    {
        DT_Point p = {datas[i].x, datas[i].y, datas[i].z};
        pts.push_back( p );
    }

    // 1) 三角网格剖分
    EdgeArray segments;
    PointArray holes;
    EdgeArray ea;
    TriangleArray ta;
    CDT_LIB( pts, segments, holes, ea, ta );

    // 搜索边界
    PointArray bpts;
    IntArray bpos;
    SearchBoundary( pts, ea, ta, bpts, bpos );

    // 绘制边界
    DrawBoundaryHelper( bpts, bpos );
}