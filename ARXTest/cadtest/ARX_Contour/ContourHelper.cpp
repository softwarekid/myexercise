#include "StdAfx.h"
#include "ContourHelper.h"
#include "TriContour.h"
#include "CADDrawHelper.h"

#include "ContourParamHelper.h"

#include <algorithm>
#include <vector>
#include <numeric>

#include "../ArxHelper/HelperClass.h"

// �������������ɵ�ֵ��
#include "../Contour/Contour.h"
#pragma comment(lib, "Contour.lib")

/* ȫ�ֺ���(ʵ����KrigingInterpolater.cpp) */
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
// ������zֵ����ʹ�ò�ֵ�㷨�����ֵ
static void DrawGrid( int nDiameter, const AcGePoint3dArray& pts, double textHeight )
{
    // ����ԭʼ����
    // ��������
    for( int i = 0; i < nDiameter; i++ )
    {
        for( int j = 0; j < nDiameter; j++ )
        {
            AcGePoint3d pt = pts[i * nDiameter + j];

            CString text;
            text.Format( _T( "%.3f" ), pt.z );

            pt.z = 0;
            DrawMText( pt, 0, text, textHeight ); // ��ע����
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
 * ��������˼·
 * ָ�����������Σ���������
 * �����п��ܴ��ڲ��������״�����������Σ��ı��ε�
 * ����ÿһ������ε�ƽ��zֵ(ͨ��ֱ�Ӳ���ƽ�����������ò�ֵ�����������ζ����zֵ��Ȼ����ƽ��)
 * ���㲻���������е�����"����"�����*ƽ��zֵ�������ܺͣ�Ȼ�������������ε����
 * �õ�������ܵ�ƽ��zֵ
 *
 * ����Ĵ���˼·��
 * 1) �����еĶ���ζ������ɾ��Σ���Ȼÿһ��С���ε����������ͬ��
 * 2) �����񻮷ֳ�(n+1)*(n+1)����ȡÿһ��С���ε����ĵ���в�ֵ����Ϊ���ε�ƽ��ֵ
 * 3) ���ھ��ε��������ͬ�ģ����Լ򻯳��������ĵ�zֵ���ȡƽ��ֵ
 *
 * ����ܿ��ܴ������
 ------------------------------------------------
 |		-		|		-		|		-		|
 ------------------------------------------------
 |		-		|		-		|		-		|
 ------------------------------------------------
 |		-		|		-		|		-		|
 ------------------------------------------------
 |		-		|		-		|		-		|
 ------------------------------------------------
 ���ϣ�Ҫ����3*3������ʵ�ʽ����񻮷ֳ�4*4
 */
double ContourHelper::GridCacl( const AcGePoint3dArray& datas, const AcGePoint3d& minPt, const AcGePoint3d& maxPt, int nDiameter, double dSemivariance )
{
    // ÿһ��С����Ŀ�(dx)�͸�(dy)
    double dx = ( maxPt.x - minPt.x ) / ( nDiameter + 1 );
    double dy = ( maxPt.y - minPt.y ) / ( nDiameter + 1 );

    // ����߶�
    double c = 0.2;
    double textHeight = int( min( dx, dy ) * c );

    AcGePoint3d basePt = minPt;
    basePt.x = basePt.x + dx / 2;
    basePt.y = basePt.y + dy / 2;

    // ׼��������������
    AcGePoint3dArray pts;
    for( int i = 0; i < nDiameter; i++ )
    {
        for( int j = 0; j < nDiameter; j++ )
        {
            // ����ϵ�����½ǿ�ʼ
            double xpos = basePt.x + j * dx;
            double ypos = basePt.y + i * dy;
            pts.append( AcGePoint3d( xpos, ypos, 0 ) );
        }
    }

    // ʹ��Kriging��ֵ�㷨������ֵ������pts��zֵ
    KrigingInterpolater_More( datas, pts, nDiameter, dSemivariance );

    // ����ԭʼ����
    DrawOrginData( datas, 4 );
    DrawGrid( nDiameter, pts, textHeight );

    // ����ƽ��ֵ
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
    pTC->label(); // �����ע����(λ�ü�����)
    pTC->smooth( bSmooth );
    pTC->setTextHeight( textHeight );

    // ���ӵ����ݿ�
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
    // ��ȡ��ֵ��ȫ�ֲ���
    bool bSmooth;
    double textHeight;
    ContourParamHelper::ReadParams( bSmooth, textHeight );

    //acutPrintf(_T("\n"));
    for( int i = 0; i < ( int )cna.size(); i++ )
    {
        DT_Contour z_contour = cna[i];
        int zi = z_contour.zi;
        int cnpts_num = z_contour.cnpts_num;
        //acutPrintf(_T("��%d����ֵ��, z=%.3f\n"), i, zValues[zi]);

        // ��λ����i����ֵ��λ��
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

    // 1) ���������ʷ�
    EdgeArray segments;
    PointArray holes;
    EdgeArray ea;
    TriangleArray ta;
    CDT_LIB( pts, segments, holes, ea, ta );

    // 2) ׷�ٵ�ֵ��
    PointArray cnpts;
    ContourArray cna;
    TraceContours( pts, ea, ta, zValues, cnpts, cna );

    DrawOrginData( datas, 4 );
    DrawTriangle( pts, ea );
    DrawContourHelper( zValues, cnpts, cna );
}

void ContourHelper::DrawCountour( const AcGePoint3dArray& datas, double dz )
{
    // ��ʼ��zֵ
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
    // ��ʼ��zֵ
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

        // ���ƶ���α߽�
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

    // 1) ���������ʷ�
    EdgeArray segments;
    PointArray holes;
    EdgeArray ea;
    TriangleArray ta;
    CDT_LIB( pts, segments, holes, ea, ta );

    // �����߽�
    PointArray bpts;
    IntArray bpos;
    SearchBoundary( pts, ea, ta, bpts, bpos );

    // ���Ʊ߽�
    DrawBoundaryHelper( bpts, bpos );
}