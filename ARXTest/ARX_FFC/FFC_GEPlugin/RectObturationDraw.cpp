#include "StdAfx.h"
#include "RectObturationDraw.h"

#include "DrawTool.h"

ACRX_CONS_DEFINE_MEMBERS ( RectObturationDraw, MineGEDraw, 1 )

RectObturationDraw::RectObturationDraw()
{
}

RectObturationDraw::~RectObturationDraw( void )
{
}

void RectObturationDraw::setAllExtraParamsToDefault()
{
    const double phi = 1.61803399;
    m_length = phi * FFC_TUNNEL_WIDTH;
    m_thick = 0.25 * FFC_TUNNEL_WIDTH;
}

void RectObturationDraw::configExtraParams()
{

}

void RectObturationDraw::updateExtraParams()
{

}

void RectObturationDraw::writeKeyParam( DrawParamWriter& writer )
{
    writer.writePoint( m_pt );
    writer.writeDouble( m_angle );
}

void RectObturationDraw::readKeyParam( DrawParamReader& reader )
{
    reader.readPoint( m_pt );
    reader.readDouble( m_angle );
}

void RectObturationDraw::readExtraParam( DrawParamReader& reader )
{
    reader.readDouble( m_length );
    reader.readDouble( m_thick );
}

void RectObturationDraw::writeExtraParam( DrawParamWriter& writer )
{
    writer.writeDouble( m_length );
    writer.writeDouble( m_thick );
}

Adesk::Boolean RectObturationDraw::subWorldDraw( AcGiWorldDraw* mode )
{
    assertReadEnabled () ;

    DrawRect( mode, m_pt, m_angle, m_thick, m_length, true );

    return Adesk::kTrue;
}

Acad::ErrorStatus RectObturationDraw::subTransformBy( const AcGeMatrix3d& xform )
{
    // 插入点变换
    m_pt.transformBy( xform );

    AcGeVector3d v( AcGeVector3d::kXAxis );
    v.rotateBy( m_angle, AcGeVector3d::kZAxis );
    v.transformBy( xform );

    m_angle = v.angleTo( AcGeVector3d::kXAxis, -AcGeVector3d::kZAxis ); // 变换后的旋转角度

    return Acad::eOk;
}

Acad::ErrorStatus RectObturationDraw::subGetOsnapPoints (
    AcDb::OsnapMode osnapMode,
    int gsSelectionMark,
    const AcGePoint3d& pickPoint,
    const AcGePoint3d& lastPoint,
    const AcGeMatrix3d& viewXform,
    AcGePoint3dArray& snapPoints,
    AcDbIntArray& geomIds ) const
{
    assertReadEnabled () ;
    // 捕捉2种类型的点：端点和中心点
    if( osnapMode != AcDb::kOsModeCen )
        return Acad::eOk;

    Acad::ErrorStatus es = Acad::eOk;

    if( osnapMode == AcDb::kOsModeCen )
    {
        snapPoints.append( m_pt );
    }

    return es;
}

Acad::ErrorStatus RectObturationDraw::subGetGripPoints( AcGePoint3dArray& gripPoints, AcDbIntArray& osnapModes, AcDbIntArray& geomIds ) const
{
    assertReadEnabled () ;

    gripPoints.append( m_pt );

    return Acad::eOk;
}

Acad::ErrorStatus RectObturationDraw::subMoveGripPointsAt( const AcDbIntArray& indices, const AcGeVector3d& offset )
{
    assertWriteEnabled () ;

    for( int i = 0; i < indices.length(); i++ )
    {
        int idx = indices.at( i );
        if ( idx == 0 )
        {
            m_pt += offset; 			// 插入点偏移
        }
    }
    return Acad::eOk;
}
