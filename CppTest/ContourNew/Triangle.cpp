
//在汇编选项中使用了/Yustdafx.h,迫使汇编器寻找#include "stdafx.h"这一行，
//	在这一行之前的所有头文件不在预汇编，而只预汇编这一行之后的所有头文件，
//	目的在于节约时间。
//	而这一行及这一行以前的预汇编信息从stdafx.pch中读取

#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "Triangle.h"


void CTriangle::read_data( CString Filename )
{
    double xm, ym, zm, id;
    int m;
    FILE* fp;

    xmin = 999999;
    xmax = -999999;
    ymin = 999999;
    ymax = -999999;
    epsilon = 0.001;

    if ( ( fp = fopen( Filename, "r" ) ) == NULL )
    {
        AfxMessageBox( "打开文件出错" );
        exit( 0 );
    }

    m = 0;
    kp = 0;
    while( !feof( fp ) )
    {
        if ( fscanf( fp, "%lf %lf %lf %lf", &xm, &ym, &id, &zm ) != 4 ) break;
        m = m + 1;
        /*double mm = m / 10;
        if (m == floor(mm) * 10)
        {*/
        if ( xm < xmin ) xmin = xm;
        if ( xm > xmax ) xmax = xm;
        if ( ym < ymin ) ymin = ym;
        if ( ym > ymax ) ymax = ym;
        x.Add( xm );
        y.Add( ym );
        z.Add( zm );
        //}
    }
    fclose( fp );

    kp = x.GetSize();
}

//求两个线段地交点,当两条线段正真交叉时,status1返回1,否则返回-1, 其中xc,yc返回交点坐标.
//(x1,y1)-(x2,y2)代表一条线段,(x3,y3)-(x4-y4)代表另一线段.
// status=1 for 内交
// status=0 for 重叠
// status=2 for 平行但不重叠
// status=3 for 交点在的一条线段的端点上， 但不在第二条线段的端点上
// status=4 for 交点在的二条线段的端点上， 但不在第一条线段的端点上
// status=5 for 交点既在的一条线段的端点上， 也在第二条线段的端点上
void CTriangle::tie2( double x1, double y1, double x2, double y2, double x3, double y3,
                      double x4, double y4, double& xc, double& yc, int& status1 )
{
    double a1, b1, c1, d1, d2, d1_d2, d3, d4, d3_d4, a2, b2, c2;
    double dx1, dx2, dy1, dy2, k1, k2, epsilon1;

    status1 = -1;
    epsilon = 0.001;
    epsilon1 = 0.5;
    dx1 = x2 - x1;
    if ( fabs( dx1 ) < epsilon )
    {
        a1 = 1;
        b1 = 0;
        c1 = -x1;
    }
    else
    {
        dy1 = y2 - y1;
        k1 = dy1 / dx1;
        a1 = k1;
        b1 = -1;
        c1 = y1 - k1 * x1;
    }
    dx2 = x4 - x3;
    if ( fabs( dx2 ) < epsilon )
    {
        a2 = 1;
        b2 = 0;
        c2 = -x3;
    }
    else
    {
        dy2 = y4 - y3;
        k2 = dy2 / dx2;
        a2 = k2;
        b2 = -1;
        c2 = y3 - k2 * x3;
    }
    if ( fabs( a2 * b1 - a1 * b2 ) > epsilon )
    {
        xc = ( b2 * c1 - b1 * c2 ) / ( a2 * b1 - a1 * b2 );
        yc = ( a1 * c2 - a2 * c1 ) / ( a2 * b1 - a1 * b2 );

        d1 = ( xc - x1 ) * ( xc - x1 ) + ( yc - y1 ) * ( yc - y1 );
        d1 = sqrt( d1 );
        d2 = ( xc - x2 ) * ( xc - x2 ) + ( yc - y2 ) * ( yc - y2 );
        d2 = sqrt( d2 );
        d1_d2 = ( x1 - x2 ) * ( x1 - x2 ) + ( y1 - y2 ) * ( y1 - y2 );
        d1_d2 = sqrt( d1_d2 );
        d3 = ( xc - x3 ) * ( xc - x3 ) + ( yc - y3 ) * ( yc - y3 );
        d3 = sqrt( d3 );
        d4 = ( xc - x4 ) * ( xc - x4 ) + ( yc - y4 ) * ( yc - y4 );
        d4 = sqrt( d4 );
        d3_d4 = ( x3 - x4 ) * ( x3 - x4 ) + ( y3 - y4 ) * ( y3 - y4 );
        d3_d4 = sqrt( d3_d4 );
        if ( d1 > epsilon1 && d2 > epsilon1 ) //not first line end
        {
            if ( d1 + d2 - d1_d2 < epsilon ) // tie point is in the first segment
            {
                if ( d3 > epsilon1 && d4 > epsilon1 ) // not second line end
                {
                    if ( ( d3 + d4 - d3_d4 ) < epsilon ) status1 = 1; //tie point ine the second line
                }
                else
                    status1 = 4;
            }
        }
        else
            //two case

            if ( d3 > epsilon1 && d4 > epsilon1 )
                status1 = 3;
            else
                status1 = 5;
    }
    else
        //平行
    {
        // overlay
        d1 = fabs( a2 * x1 + b2 * y1 + c2 ) / sqrt( a2 * a2 + b2 * b2 );
        if ( d1 < epsilon )
            status1 = 0;
        else
            status1 = 2;
    }
}



//////////////////////////////////////////////////////////////////////
// CTriangle Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTriangle::CTriangle(): m_faults()
{
    kp = 0;
    mydc = NULL;
}


CTriangle::~CTriangle()
{
    int i;
    for ( i = 0; i < m_trisets.GetSize(); i++ )
        delete m_trisets[i];
    m_trisets.RemoveAll();
    for( i = 0; i < m_faults.GetSize(); i++ )
        delete m_faults[i];
    m_faults.RemoveAll();

    faultsRgn.DeleteObject();
}

// 最后一个参数bNeedSetMapMode决定是否重新设置坐标系统
void CTriangle::DrawTriangle( CDC* dc )
{
    int i, k;
    mydc = dc;


    // 初始化边界数据并画边界
    SPoint tmpSPt;
    tmpSPt.x = xmin;
    tmpSPt.y = ymin;
    this->m_Boundary.Add( tmpSPt );
    tmpSPt.x = xmax;
    tmpSPt.y = ymin;
    this->m_Boundary.Add( tmpSPt );
    tmpSPt.x = xmax;
    tmpSPt.y = ymax;
    this->m_Boundary.Add( tmpSPt );
    tmpSPt.x = xmin;
    tmpSPt.y = ymax;
    this->m_Boundary.Add( tmpSPt );
    /*	if(faultsRgn.m_hObject == NULL)
    		faultsRgn.CreateRectRgn( 0, 0, 0, 0 );
    	DrawBoundary(dc);*/


    //画散点
    CString theStr;
    CFont font;
    CPen myPen( PS_SOLID, 2, RGB( 0, 0, 222 ) );
    CPen* oldPen = dc->SelectObject( &myPen );

    font.CreateFont( 900, 400, 0, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, "Arial" );
    CFont* oldfont = dc->SelectObject( &font );
    for ( i = 0; i < kp; i++ )
    {
        dc->Ellipse( ( int )( x[i] - xmin - 200 ), ( int )( y[i] - ymin + 200 ), ( int )( x[i] - xmin + 200 ), ( int )( y[i] - ymin - 200 ) );
        //theStr.Format("%d", i);
        //dc->TextOut(x[i]-xmin, y[i]-ymin, theStr); //标点号
    }
    dc->SelectObject( oldPen );
    dc->SelectObject( oldfont );
    //画断层线
    DrawFaults( true, dc );

    /////////////////////////////////////
    //设置断层区域
    //int iSize=this->m_Boundary.GetSize(); //处理边界区域
    //CRgn boundRgn;
    //CPoint *tmpPt = new CPoint[iSize];
    //for (i=0; i<iSize; i++)
    //{
    //	tmpPt[i].x=m_Boundary.ElementAt(i).x;
    //	tmpPt[i].y=m_Boundary.ElementAt(i).y;
    //	tmpPt[i]-=CSize(xmin, ymin);
    //}
    //boundRgn.CreatePolygonRgn(tmpPt, iSize, ALTERNATE);
    //delete []tmpPt;

    //const int DofCircle=50;	//处理断层区域
    //int faultsCount, xyCount;
    //CRgn *ptRgn = 0;
    //faultsCount = m_faults.GetSize();
    //if(faultsCount>0)
    //{
    //	ptRgn = new CRgn[faultsCount];
    //}

    //for (i=0; i<faultsCount; i++)
    //{
    //	xyCount = ((CFault *)m_faults[i])->x.GetSize();
    //	tmpPt = new CPoint[xyCount];
    //	for ( k=0; k<xyCount; k++)
    //	{
    //		tmpPt[k].x=((CFault *)m_faults[i])->x[k]-xmin;
    //		tmpPt[k].y=((CFault *)m_faults[i])->y[k]-ymin;
    //		dc->Ellipse(tmpPt[k].x-DofCircle/2, tmpPt[k].y-DofCircle/2, tmpPt[k].x+DofCircle/2, tmpPt[k].y+DofCircle/2);
    //	}
    //	ptRgn[i].CreatePolygonRgn(tmpPt, xyCount, ALTERNATE);
    //	delete []tmpPt;
    //}

    //if (faultsCount>0) //合并断层区域和边界区域
    //{
    //	faultsRgn.CopyRgn(ptRgn);
    //	for(i=1; i<faultsCount; i++)
    //	{
    //		faultsRgn.CombineRgn(&faultsRgn, ptRgn+i, RGN_OR);
    //	}
    //	faultsRgn.CombineRgn(&faultsRgn, &boundRgn, RGN_AND);
    //}
    //else
    //	faultsRgn.CreateRectRgn( 0, 0, 0, 0 );

    //boundRgn.DeleteObject();
    //if(faultsCount>0)
    //{
    //	for(i=0; i<faultsCount; i++)
    //		ptRgn[i].DeleteObject();
    //	delete []ptRgn;

    //	// 填充断层区域
    //	CBrush pb;
    //	pb.CreateSolidBrush(RGB(128,128,128));
    //	dc->FillRgn(&faultsRgn, &pb);
    //	////// 设置断层区域完毕
    //}



    //在窗口上追踪并画三角形
#if defined(XLD_READ)
    //AfxMessageBox("Hello, Is reading.....");
    CFile theFile;
    theFile.Open( "m_trisets.dat", CFile::modeRead );
    CArchive ar( &theFile, CArchive::load );
    this->Serialize( ar );
    //AfxMessageBox("Hello, read finished!");
#else
    CreateTriSetEx( dc );
#endif
    DrawTris( dc ); //画所有的三角形


    //给断层插入z值
    int j, dotscount;
    int faultscount = m_faults.GetSize();
    SDot destDot;
    destDot.enumDotSource = DOTSRC_FAULT;
    for( i = 0; i < faultscount; i++ )
    {
        dotscount = ( ( CFault* )m_faults[i] )->iDots;
        destDot.iFaultID = i;
        for( j = 0; j < dotscount; j++ )
        {
            destDot.index = j;
            InsertZinDot( destDot );
        }
    }
    //插入完毕
}



// 画所有三角形
// 此函数替换了两处Draw函数, 一处在draw_triangle_Click函数中,另外一处在Function02函数中
void CTriangle::DrawTris( CDC* dc, int n )
{
    for( int k = 0; k < m_trisets.GetSize(); k++ )
        for ( int i = 0; i < m_trisets[k]->ibcount; i++ )
            Draw( k, i, dc );
}

// 画第L号三角形
//k 为三角形集合序号， L为某一个集合内的索引号
void CTriangle::Draw( const int k, const int L, CDC* dc )
{
    //CPen pen(PS_SOLID, 1, RGB(220,220,220));
    CPen pen( PS_SOLID, 1, RGB( 225, 225, 224 ) );
    CPen* oldpen;
    oldpen = dc->SelectObject( &pen );

    SDot ib1L, ib2L, ib3L;
    ib1L = ( ( ( CTriSet* )m_trisets[k] )->theTriSet ).GetAt( L ).ib1;
    ib2L = ( ( ( CTriSet* )m_trisets[k] )->theTriSet ).GetAt( L ).ib2;
    ib3L = ( ( ( CTriSet* )m_trisets[k] )->theTriSet ).GetAt( L ).ib3;

    CPoint Points[4];
    SDot2XY( ib1L, Points[0] );
    Points[0] -= CSize( xmin, ymin );
    SDot2XY( ib2L, Points[1] );
    Points[1] -= CSize( xmin, ymin );
    SDot2XY( ib3L, Points[2] );
    Points[2] -= CSize( xmin, ymin );
    SDot2XY( ib1L, Points[3] );
    Points[3] -= CSize( xmin, ymin );

    dc->Polyline( Points, 4 ); //444444444444444

    dc->SelectObject( oldpen );
    pen.DeleteObject();

    CString theStr;
    theStr.Format( "%d", L );

    COLORREF bkc = dc->SetBkColor( RGB( 255, 0, 0 ) );
    //标三角形号
    //dc->TextOut((Points[0].x+Points[1].x+Points[2].x)/3,(Points[0].y+Points[1].y+Points[2].y)/3, theStr);

    dc->SetBkColor( bkc );
}



void CTriangle::DrawBoundary( CDC* dc )
{
    int iSize = this->m_Boundary.GetSize();
    CPoint* tmpPt = new CPoint[iSize];
    for ( int i = 0; i < iSize; i++ )
    {
        tmpPt[i].x = m_Boundary.ElementAt( i ).x;
        tmpPt[i].y = m_Boundary.ElementAt( i ).y;
        tmpPt[i] -= CSize( xmin, ymin );
    }
    dc->Polygon( tmpPt, iSize );

    delete []tmpPt;
}


void CTriangle::Serialize( CArchive& ar )
{
    CObject::Serialize( ar );
    m_trisets.Serialize( ar );
}



void CTriangle::InitialTriangle()
{
    CSomeFuncs somefuncs;

    //读断层线数据
    Set_faults( "D:\\fault.dat" );

    //读散点数据
    read_data( "D:\\Contour.dat" );

    //设置xmin, xmax, ymin, ymax
    xmax = xmin = x[0]; //初始化
    ymax = ymin = y[0];
    //somefuncs.SearchMaxMin(x, kp, xmax, xmin, true);
    //somefuncs.SearchMaxMin(y, kp, ymax, ymin, true);
    somefuncs.SearchMaxMin( x, xmax, xmin, true );
    somefuncs.SearchMaxMin( y, ymax, ymin, true );
    for( int i = 0; i < m_faults.GetSize(); i++ )
    {
        somefuncs.SearchMaxMin( m_faults[i]->x, xmax, xmin, true );
        somefuncs.SearchMaxMin( m_faults[i]->y, ymax, ymin, true );
    }
}
