#include "stdafx.h"

#include <fstream>
// 需要包含<fstream>才能使用
#include "AcFStream.h"

#include "CADDrawHelper.h"

struct Edge
{
    int s, t;
};

struct Triangle
{
    int t1, t2, t3;
};

typedef AcArray<Edge> EdgeArray;
typedef AcArray<Triangle> TriangleArray;

static void ReadPoints( AcIfstream& inFile, AcGePoint3dArray& pts )
{
    // 1) 读取第1行
    // v -节点个数
    // n - 维数
    // a - 属性个数
    // b - 边界标记
    int nv, n, a, b;
    inFile >> nv >> n >> a >> b;

    for( int i = 0; i < nv; i++ )
    {
        int ni;
        double x, y;
        inFile >> ni >> x >> y;

        pts.append( AcGePoint3d( x, y, 0 ) );

        for( int j = 0; j < a; j++ )
        {
            double aa;
            inFile >> aa;
        }

        if( b != 0 )
        {
            double bb;
            inFile >> bb;
        }
    }
}

static void ReadSegments( AcIfstream& inFile, EdgeArray& sa )
{
    int ns, b;
    inFile >> ns >> b;

    for( int i = 0; i < ns; i++ )
    {
        int ei;
        inFile >> ei;

        int s, t;
        inFile >> s >> t;

        Edge seg = {s, t};
        sa.append( seg );

        if( b != 0 )
        {
            int bb;
            inFile >> bb;
        }
    }
}

static void ReadHoles( AcIfstream& inFile, AcGePoint3dArray& holes )
{
    int nh;
    inFile >> nh;

    for( int i = 0; i < nh; i++ )
    {
        int hi;
        inFile >> hi;

        double x, y;
        inFile >> x >> y;

        holes.append( AcGePoint3d( x, y, 0 ) );
    }
}

static void ParseInPolyFile( const CString& path, AcGePoint3dArray& pts, EdgeArray& sa, AcGePoint3dArray& holes )
{
    acutPrintf( _T( "\n从路径: %s读取文件..." ), path );

    AcIfstream inFile( path );
    if( !inFile ) return;

    TCHAR ext[_MAX_EXT];
    _tsplitpath( path, NULL, NULL, NULL, ext );

    ReadPoints( inFile, pts );
    if( CString( ext ) == _T( ".poly" ) )
    {
        ReadSegments( inFile, sa );
        ReadHoles( inFile, holes );
    }

    inFile.close();
}

static bool GenTriangle( const CString& triangleExePath, const CString& path )
{
    TCHAR ext[_MAX_EXT];
    _tsplitpath( path, NULL, NULL, NULL, ext );

    CString cmdLine;
    if( CString( ext ) == _T( ".poly" ) )
    {
        cmdLine.Format( _T( " -pcez_Q %s" ), path );
    }
    else if( CString( ext ) == _T( ".node" ) )
    {
        cmdLine.Format( _T( " -e_Q %s" ), path );
    }
    else
    {
        return false;
    }

    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    memset( &si, 0, sizeof( si ) );
    si.cb = sizeof( si );
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW;

    BOOL ret = CreateProcess( ( LPTSTR )( LPCTSTR )triangleExePath, ( LPTSTR )( LPCTSTR )cmdLine, NULL, FALSE, NULL, NULL, NULL, NULL, &si, &pi );
    if( ret )
    {
        WaitForSingleObject( pi.hProcess, INFINITE );
        CloseHandle( pi.hThread );
        CloseHandle( pi.hProcess );
    }
    return ( ret == TRUE );
}

static void DrawPoints( const AcGePoint3dArray& pts )
{
    int n = pts.length();
    for( int i = 0; i < n; i++ )
    {
        DrawCircle( pts[i], 0.005 );
    }
}

static void DrawSegments( const AcGePoint3dArray& pts, const EdgeArray& sa )
{
    int n = sa.length();
    for( int i = 0; i < n; i++ )
    {
        AcGePoint3dArray linePts;
        int s = sa[i].s;
        int t = sa[i].t;
        linePts.append( pts[s - 1] );
        linePts.append( pts[t - 1] );
        DrawLines( linePts );
    }
}

static void DrawHoles( const AcGePoint3dArray& holes )
{
    int n = holes.length();
    for( int i = 0; i < n; i++ )
    {
        DrawCircle( holes[i], 0.01 );
    }
}

static void DrawEdges( const AcGePoint3dArray& pts, const EdgeArray& ea )
{
    DrawSegments( pts, ea );
}

static void ReadEdges( AcIfstream& inFile, EdgeArray& ea )
{
    int ne, b;
    inFile >> ne >> b;

    for( int i = 0; i < ne; i++ )
    {
        int ei;
        inFile >> ei;

        int s, t;
        inFile >> s >> t;

        //acutPrintf(_T("\n%d %d %d"), ei, s, t);

        Edge e = {s, t};
        ea.append( e );

        if( b != 0 )
        {
            int bb;
            inFile >> bb;
        }
    }
}

static void ReadTriangles( AcIfstream& inFile, TriangleArray& ta )
{
    int nt, v, b;
    inFile >> nt >> v >> b;

    for( int i = 0; i < nt; i++ )
    {
        int ti;
        inFile >> ti;

        int t1, t2, t3;
        inFile >> t1 >> t2 >> t3;

        Triangle t = {t1, t2, t3};
        ta.append( t );

        if( b != 0 )
        {
            int bb;
            inFile >> bb;
        }
    }
}

static void ParseOutPolyFile( const CString& path, TriangleArray& ta, EdgeArray& ea )
{
    TCHAR drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
    _tsplitpath( path, drive, dir, fname, ext );

    AcIfstream inFile;

    CString edgeFilePath;
    edgeFilePath.Format( _T( "%s%s%s.1.edge" ), drive, dir, fname );
    acutPrintf( _T( "\n从路径: %s读取.edge文件..." ), edgeFilePath );

    inFile.open( edgeFilePath );
    ReadEdges( inFile, ea );
    inFile.close();

    CString triangleFilePath;
    triangleFilePath.Format( _T( "%s%s%s.1.ele" ), drive, dir, fname );
    acutPrintf( _T( "\n从路径: %s读取.ele文件..." ), triangleFilePath );

    inFile.open( triangleFilePath );
    ReadTriangles( inFile, ta );
    inFile.close();
}

void DrawData( const CString& path )
{
    // 注：最好是将hole放在一个封闭图形内(三角形或四边形)
    // 这样不会出什么问题
    AcGePoint3dArray pts;
    EdgeArray sa;
    AcGePoint3dArray holes;

    ParseInPolyFile( path, pts, sa, holes );
    acutPrintf( _T( "\n节点个数:%d" ), pts.length() );
    acutPrintf( _T( "\n隔断个数:%d" ), sa.length() );
    acutPrintf( _T( "\n孔洞个数:%d" ), holes.length() );

    DrawPoints( pts );
    DrawSegments( pts, sa );
    DrawHoles( holes );
}

void DrawTriangle( const CString& path )
{
    // 注：最好是将hole放在一个封闭图形内(三角形或四边形)
    // 这样不会出什么问题
    CString triangleExePath = _T( "D:\\Test\\CppTest\\triangle\\Release\\triangle.exe" );

    AcGePoint3dArray pts;
    EdgeArray sa;
    AcGePoint3dArray holes;
    ParseInPolyFile( path, pts, sa, holes );

    if( GenTriangle( triangleExePath, path ) )
    {
        TriangleArray ta;
        EdgeArray ea;
        ParseOutPolyFile( path, ta, ea );

        acutPrintf( _T( "\n分支个数:%d" ), ea.length() );
        acutPrintf( _T( "\n三角形个数:%d" ), ta.length() );
        DrawEdges( pts, ea );
    }
}