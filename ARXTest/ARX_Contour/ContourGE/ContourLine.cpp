#include "StdAfx.h"
#include "ContourLine.h"

/* 全局函数(ARX_Smooth.cpp) */
extern void ARX_SmoothPoints( const AcGePoint3dArray& inPts, AcGePoint3dArray& outPts );

static void DrawLinePoints( AcGiWorldDraw* mode, const AcGePoint3dArray& pts )
{
    int len = pts.length();
    for( int i = 0; i < len - 1; i++ )
    {
        AcGePoint3dArray tpts;
        tpts.append( pts[i] );
        tpts.append( pts[i + 1] );
        mode->geometry().worldLine( tpts.asArrayPtr() );
    }

    /*for(int i=0;i<len;i++)
    {
    	mode->geometry().circle(pts[i], 4, AcGeVector3d::kZAxis);
    }*/
}

static void DrawSmoothPoints( AcGiWorldDraw* mode, const AcGePoint3dArray& pts )
{
    // 使用样条函数进行光滑
    AcGePoint3dArray spline_pts;
    ARX_SmoothPoints( pts, spline_pts );

    DrawLinePoints( mode, spline_pts );
}

Adesk::UInt32 ContourLine::kCurrentVersionNumber = 1 ;

ACRX_DXF_DEFINE_MEMBERS (
    ContourLine, ContourGE,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    等值线,	CONTOURAPP )

ContourLine::ContourLine () : ContourGE (), m_bSmooth( true )
{
}

ContourLine::~ContourLine ()
{
}

Acad::ErrorStatus ContourLine::dwgOutFields ( AcDbDwgFiler* pFiler ) const
{
    assertReadEnabled () ;
    //----- Save parent class information first.
    Acad::ErrorStatus es = ContourGE::dwgOutFields ( pFiler ) ;
    if ( es != Acad::eOk )
        return ( es ) ;
    //----- Object version number needs to be saved first
    if ( ( es = pFiler->writeUInt32 ( ContourLine::kCurrentVersionNumber ) ) != Acad::eOk )
        return ( es ) ;

    // 写入z值
    pFiler->writeDouble( m_zValue );

    // 写入坐标个数
    int len = m_pts.length();
    pFiler->writeInt32( len );

    // 写入坐标
    for( int i = 0; i < len; i++ )
    {
        pFiler->writePoint3d( m_pts[i] );
    }

    return ( pFiler->filerStatus () ) ;
}

Acad::ErrorStatus ContourLine::dwgInFields ( AcDbDwgFiler* pFiler )
{
    assertWriteEnabled () ;
    //----- Read parent class information first.
    Acad::ErrorStatus es = ContourGE::dwgInFields ( pFiler ) ;
    if ( es != Acad::eOk )
        return ( es ) ;
    //----- Object version number needs to be read first
    Adesk::UInt32 version = 0 ;
    if ( ( es = pFiler->readUInt32 ( &version ) ) != Acad::eOk )
        return ( es ) ;
    if ( version > ContourLine::kCurrentVersionNumber )
        return ( Acad::eMakeMeProxy ) ;

    // 读取z值
    pFiler->readDouble( &m_zValue );

    // 读取坐标个数
    long len = 0;
    pFiler->readInt32( &len );

    // 读取坐标
    m_pts.removeAll();
    for( int i = 0; i < ( int )len; i++ )
    {
        AcGePoint3d pt;
        pFiler->readPoint3d( &pt );
        m_pts.append( pt );
    }

    return ( pFiler->filerStatus() ) ;
}

Adesk::Boolean ContourLine::subWorldDraw( AcGiWorldDraw* mode )
{
    assertReadEnabled () ;

    // 绘制等值线
    if( m_bSmooth )
    {
        DrawSmoothPoints( mode, m_pts );
    }
    else
    {
        DrawLinePoints( mode, m_pts );
    }

    return Adesk::kTrue;
}

void ContourLine::smooth( bool bSmooth )
{
    assertWriteEnabled();
    this->m_bSmooth = bSmooth;
}

double ContourLine::getZValue() const
{
    assertReadEnabled();
    return m_zValue;
}

void ContourLine::setZValues( double z )
{
    assertWriteEnabled();
    this->m_zValue = z;
}

void ContourLine::getPoints( AcGePoint3dArray& cnpts ) const
{
    assertReadEnabled();
    cnpts.append( m_pts );
}

void ContourLine::setPoints( const AcGePoint3dArray& cnpts )
{
    assertWriteEnabled();
    if( cnpts.length() > 1 )
    {
        m_pts.removeAll();
        m_pts.append( cnpts );
    }
}