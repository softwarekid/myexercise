#include "StdAfx.h"

#include "VNG_Edge.h"
#include "DrawTool.h"

#define PI 3.1415926535897932384626433832795

Adesk::UInt32 VNG_Edge::kCurrentVersionNumber = 1 ;

ACRX_DXF_DEFINE_MEMBERS (
    VNG_Edge, ModelGE,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    网络图分支, VNGAPP
)

VNG_Edge::VNG_Edge () : ModelGE ()
{
}

VNG_Edge::~VNG_Edge ()
{
}

Acad::ErrorStatus VNG_Edge::dwgOutFields ( AcDbDwgFiler* pFiler ) const
{
    assertReadEnabled () ;
    //----- Save parent class information first.
    Acad::ErrorStatus es = ModelGE::dwgOutFields ( pFiler ) ;
    if ( es != Acad::eOk )
        return ( es ) ;
    //----- Object version number needs to be saved first
    if ( ( es = pFiler->writeUInt32 ( VNG_Edge::kCurrentVersionNumber ) ) != Acad::eOk )
        return ( es ) ;

    pFiler->writePoint3d( m_spt );
    pFiler->writePoint3d( m_ept );
    pFiler->writePoint3d( m_pt );

    pFiler->writeDouble( m_arrowWidth );
    pFiler->writeDouble( m_arrowLength );

    pFiler->writeBool( m_needEdge );
    pFiler->writePoint3d( m_tpt );
    pFiler->writeDouble( m_textHeight );
    pFiler->writeInt32( m_id );

    return ( pFiler->filerStatus () ) ;
}

Acad::ErrorStatus VNG_Edge::dwgInFields ( AcDbDwgFiler* pFiler )
{
    assertWriteEnabled () ;
    //----- Read parent class information first.
    Acad::ErrorStatus es = ModelGE::dwgInFields ( pFiler ) ;
    if ( es != Acad::eOk )
        return ( es ) ;
    //----- Object version number needs to be read first
    Adesk::UInt32 version = 0 ;
    if ( ( es = pFiler->readUInt32 ( &version ) ) != Acad::eOk )
        return ( es ) ;
    if ( version > VNG_Edge::kCurrentVersionNumber )
        return ( Acad::eMakeMeProxy ) ;

    pFiler->readPoint3d( &m_spt );
    pFiler->readPoint3d( &m_ept );
    pFiler->readPoint3d( &m_pt );

    pFiler->readDouble( &m_arrowWidth );
    pFiler->readDouble( &m_arrowLength );

    pFiler->readBool( &m_needEdge );
    pFiler->readPoint3d( &m_tpt );
    pFiler->readDouble( &m_textHeight );
    long L;
    pFiler->readInt32( &L );
    m_id = ( int )L;

    return ( pFiler->filerStatus () ) ;
}

static double CacArcAngle( const AcGePoint3d& spt, const AcGePoint3d& ept, const AcGePoint3d& pt, double L )
{
    AcGeCircArc3d arc( spt, pt, ept );
    int c = ( arc.normal().z > 0 ? 1 : -1 ); // 弧线方向

    double dc = 0.95 * 0.5;
    double da = acos( dc * L / arc.radius() );
    AcGeVector3d v = pt - arc.center();
    v.rotateBy( c * ( PI - da ), AcGeVector3d::kZAxis );

    return v.angleTo( AcGeVector3d::kXAxis, -AcGeVector3d::kZAxis );
}

Adesk::Boolean VNG_Edge::subWorldDraw ( AcGiWorldDraw* mode )
{
    assertReadEnabled () ;

    AcGiSubEntityTraits& traits = mode->subEntityTraits();
    Adesk::UInt16 cl = traits.color();
    if( getDataObject().isNull() ) traits.setColor( 2 ); // 虚分支用黄色高亮

    // 绘制分支(样条曲线)
    DrawArc( mode, m_spt, m_pt, m_ept, false );

    // 绘制箭头
    DrawArrow( mode, m_pt, CacArcAngle( m_spt, m_ept, m_pt, m_arrowLength ), m_arrowWidth, m_arrowLength );

    if( m_needEdge )
    {
        CString idStr;
        idStr.Format( _T( "%d" ), m_id );
        CString str;
        str.Format( _T( "e%s" ), MakeLowerText( idStr ) );
        DrawMText( mode, m_tpt, 0, str, m_textHeight );
    }

    // 复原颜色
    traits.setColor( cl );

    return Adesk::kTrue;
    //return (ModelGE::subWorldDraw (mode)) ;
}

Acad::ErrorStatus VNG_Edge::subTransformBy( const AcGeMatrix3d& xform )
{
    // 构造弧线，进行变换
    m_spt.transformBy( xform );
    m_ept.transformBy( xform );
    m_pt.transformBy( xform );

    if( m_needEdge )
    {
        m_tpt.transformBy( xform );
    }

    return Acad::eOk;
}

Acad::ErrorStatus VNG_Edge::subGetOsnapPoints (
    AcDb::OsnapMode osnapMode,
    int gsSelectionMark,
    const AcGePoint3d& pickPoint,
    const AcGePoint3d& lastPoint,
    const AcGeMatrix3d& viewXform,
    AcGePoint3dArray& snapPoints,
    AcDbIntArray& geomIds ) const
{
    assertReadEnabled () ;

    // 弧线的捕捉点
    if( osnapMode == AcDb::kOsModeEnd )
    {
        snapPoints.append( m_spt );
        snapPoints.append( m_ept );
        snapPoints.append( m_pt );
        if( m_needEdge )
        {
            snapPoints.append( m_tpt );
        }
    }

    return Acad::eOk;
}

Acad::ErrorStatus VNG_Edge::subGetGripPoints(
    AcGePoint3dArray& gripPoints,
    AcDbIntArray& osnapModes,
    AcDbIntArray& geomIds ) const
{
    assertReadEnabled () ;

    gripPoints.append( m_spt );
    gripPoints.append( m_ept );
    gripPoints.append( m_pt );

    if( m_needEdge )
    {
        gripPoints.append( m_tpt );
    }

    return Acad::eOk;
}

Acad::ErrorStatus VNG_Edge::subMoveGripPointsAt ( const AcDbIntArray& indices, const AcGeVector3d& offset )
{
    assertWriteEnabled () ;

    int n = indices.length();
    for( int i = 0; i < n; i++ )
    {
        int idx = indices[i];
        if( idx == 0 )
        {
            m_spt += offset;
        }
        else if( idx == 1 )
        {
            m_ept += offset;
        }
        else if( idx == 2 )
        {
            m_pt += offset;
        }
        else if( m_needEdge && idx == 3 )
        {
            m_tpt += offset;
        }
    }

    return Acad::eOk;
}