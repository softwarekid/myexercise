#include "StdAfx.h"
#include "TriContour.h"

#define PI 3.1415926535897932384626433832795

/* 全局函数(ARX_Smooth.cpp) */
extern void ARX_SmoothPoints( const AcGePoint3dArray& inPts, AcGePoint3dArray& outPts );
/* 全局函数(ARX_Label.cpp) */
extern void ARX_Label( const AcGePoint3dArray& pts, AcGePoint3d& tpt, double& angle );

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
}

static void GetTextAndWidth( double z, double height, CString& text, double& width )
{
    AcDbMText mt;
    mt.setLocation( AcGePoint3d::kOrigin );
    mt.setRotation( 0 );
    mt.setTextHeight( height );
    mt.setAttachment( AcDbMText::kMiddleCenter ); // 默认居中

    // 获取文字内容
    text.Format( _T( " %.3f " ), z );
    mt.setContents( text );

    // 获取文字宽度
    width = mt.actualWidth();
}

static void DrawZValue( AcGiWorldDraw* mode, double z, const AcGePoint3d& tpt, double angle, double height )
{
    AcGeVector3d v( AcGeVector3d::kXAxis );
    v.rotateBy( angle, AcGeVector3d::kZAxis );
    if( v.x < 0 )
    {
        angle += PI;
    }

    CString text;
    text.Format( _T( " %.5f " ), z );

    AcDbMText mt;
    mt.setLocation( tpt );
    mt.setTextHeight( height );
    mt.setAttachment( AcDbMText::kMiddleCenter );
    mt.setRotation( angle );
    mt.setBackgroundFill( true );
    mt.setContents( text );

    // 经过测试发现，AcDbMText调用worldDraw的同时会修改mode的一些属性
    mt.setColorIndex( mode->subEntityTraits().color() );

    mt.worldDraw( mode );
}

static void DrawSmoothPoints( AcGiWorldDraw* mode, const AcGePoint3dArray& pts )
{
    // 使用样条函数进行光滑
    AcGePoint3dArray spline_pts;
    ARX_SmoothPoints( pts, spline_pts );

    // 绘制cad样条曲线spline
    AcDbSpline sp( spline_pts );
    sp.worldDraw( mode );
}

Adesk::UInt32 TriContour::kCurrentVersionNumber = 1 ;

ACRX_DXF_DEFINE_MEMBERS (
    TriContour, AcDbEntity,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    等值线,	CONTOURAPP )

TriContour::TriContour () : AcDbEntity (), m_bSmooth( true ), m_textHeight( 1.2 )
{
}

TriContour::~TriContour ()
{
}

Acad::ErrorStatus TriContour::dwgOutFields ( AcDbDwgFiler* pFiler ) const
{
    assertReadEnabled () ;
    //----- Save parent class information first.
    Acad::ErrorStatus es = AcDbEntity::dwgOutFields ( pFiler ) ;
    if ( es != Acad::eOk )
        return ( es ) ;
    //----- Object version number needs to be saved first
    if ( ( es = pFiler->writeUInt32 ( TriContour::kCurrentVersionNumber ) ) != Acad::eOk )
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

    // 写入标注位置
    pFiler->writePoint3d( m_tpt );

    // 写入标注方向
    pFiler->writeDouble( m_angle );

    // 写入标注文字大小
    pFiler->writeDouble( m_textHeight );

    return ( pFiler->filerStatus () ) ;
}

Acad::ErrorStatus TriContour::dwgInFields ( AcDbDwgFiler* pFiler )
{
    assertWriteEnabled () ;
    //----- Read parent class information first.
    Acad::ErrorStatus es = AcDbEntity::dwgInFields ( pFiler ) ;
    if ( es != Acad::eOk )
        return ( es ) ;
    //----- Object version number needs to be read first
    Adesk::UInt32 version = 0 ;
    if ( ( es = pFiler->readUInt32 ( &version ) ) != Acad::eOk )
        return ( es ) ;
    if ( version > TriContour::kCurrentVersionNumber )
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

    // 读取标注位置
    pFiler->readPoint3d( &m_tpt );

    // 读取标注方向
    pFiler->readDouble( &m_angle );

    // 读取标注文字高度
    pFiler->readDouble( &m_textHeight );

    return ( pFiler->filerStatus() ) ;
}

Adesk::Boolean TriContour::subWorldDraw( AcGiWorldDraw* mode )
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

    // 标注文字
    DrawZValue( mode, m_zValue, m_tpt, m_angle, m_textHeight );

    return Adesk::kTrue;
}

//----- AcDbEntity protocols
Acad::ErrorStatus TriContour::subTransformBy( const AcGeMatrix3d& xform )
{
    return AcDbEntity::subTransformBy( xform );
}

Acad::ErrorStatus TriContour::subGetOsnapPoints (
    AcDb::OsnapMode osnapMode,
    int gsSelectionMark,
    const AcGePoint3d& pickPoint,
    const AcGePoint3d& lastPoint,
    const AcGeMatrix3d& viewXform,
    AcGePoint3dArray& snapPoints,
    AcDbIntArray& geomIds ) const
{
    assertReadEnabled () ;

    if( osnapMode == AcDb::kOsModeEnd )
    {
        snapPoints.append( m_pts.first() );
        snapPoints.append( m_pts.last() );
    }
    return Acad::eOk;
    //return AcDbEntity::subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds);
}

//- Grip points protocol
Acad::ErrorStatus TriContour::subGetGripPoints ( AcGePoint3dArray& gripPoints, AcDbIntArray& osnapModes, AcDbIntArray& geomIds ) const
{
    assertReadEnabled () ;

    gripPoints.append( m_tpt );

    return Acad::eOk;
}

static bool CaclLabelAngle( const AcGePoint3dArray& pts, const AcGePoint3d& pt, double& angle )
{
    double minDist;
    int pos = -1;
    for( int i = 0; i < pts.length() - 1; i++ )
    {
        AcGeLineSeg3d line( pts[i], pts[i + 1] );
        AcGePoint3d closePt = line.closestPointTo( pt );
        if( closePt != pts[i] && closePt != pts[i + 1] )
        {
            double dist = closePt.distanceTo( pt );
            if( pos == -1 )
            {
                minDist = dist;
                pos = i;
            }
            else if( dist < minDist )
            {
                minDist = dist;
                pos = i;
            }
        }
    }
    bool ret = ( pos != -1 );
    if( ret )
    {
        AcGeVector3d v = pts[pos + 1] - pts[pos];
        angle = v.angleTo( AcGeVector3d::kXAxis, -AcGeVector3d::kZAxis );
    }
    return ret;
}

Acad::ErrorStatus TriContour::subMoveGripPointsAt ( const AcDbIntArray& indices, const AcGeVector3d& offset )
{
    assertWriteEnabled () ;

    for( int i = 0; i < indices.length(); i++ )
    {
        int idx = indices.at( i );
        // 始节点
        if ( idx == 0 )
        {
            m_tpt += offset;
            double angle = m_angle;
            CaclLabelAngle( m_pts, m_tpt, angle );
            acutPrintf( _T( "\n变换前角度:%.3f, 变换后角度:%.3f" ), m_angle, angle );
            m_angle = angle;
        }
    }
    return Acad::eOk;
}

void TriContour::setZValue( double z )
{
    assertWriteEnabled();
    m_zValue = z;
}

void TriContour::addPoint( const AcGePoint3d& pt )
{
    assertWriteEnabled();
    m_pts.append( pt );
}

void TriContour::smooth( bool bSmooth )
{
    assertWriteEnabled();
    this->m_bSmooth = bSmooth;
}

void TriContour::label()
{
    assertWriteEnabled();
    ARX_Label( m_pts, m_tpt, m_angle );
}

void TriContour::setTextHeight( double height )
{
    assertWriteEnabled();
    this->m_textHeight = height;
}

Acad::ErrorStatus TriContour::subExplode( AcDbVoidPtrArray& entitySet ) const
{
    Acad::ErrorStatus retCode = AcDbEntity::subExplode ( entitySet ) ;

    if( m_bSmooth )
    {
        // 使用样条函数进行光滑
        AcGePoint3dArray spline_pts;
        ARX_SmoothPoints( m_pts, spline_pts );

        // 绘制cad样条曲线spline
        AcDbSpline* pSpline = new AcDbSpline( spline_pts );
        entitySet.append( pSpline );
    }
    else
    {
        int n = m_pts.length();
        AcDbPolyline* pPolyLine = new AcDbPolyline( n );
        for( int i = 0; i < n; i++ )
        {
            AcGePoint2d p2d( m_pts[i].x, m_pts[i].y );
            pPolyLine->addVertexAt( i, p2d );
        }
        entitySet.append( pPolyLine );
    }

    return Acad::eOk;
}
