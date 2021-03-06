#include "StdAfx.h"
#include "SingleObturationDraw.h"

#include "DrawTool.h"

ACRX_CONS_DEFINE_MEMBERS ( SingleObturationDraw, MineGEDraw, 1 )

SingleObturationDraw::SingleObturationDraw()
{
}

SingleObturationDraw::~SingleObturationDraw( void )
{
}

void SingleObturationDraw::setAllExtraParamsToDefault()
{
    const double phi = 1.61803399;
    m_length = 40 * phi;
}

void SingleObturationDraw::configExtraParams()
{

}

void SingleObturationDraw::updateExtraParams()
{

}

void SingleObturationDraw::writeKeyParam( DrawParamWriter& writer )
{
    writer.writePoint( m_pt );
    writer.writeDouble( m_angle );
}

void SingleObturationDraw::readKeyParam( DrawParamReader& reader )
{
    reader.readPoint( m_pt );
    reader.readDouble( m_angle );
}

void SingleObturationDraw::readExtraParam( DrawParamReader& reader )
{
    reader.readDouble( m_length );
}

void SingleObturationDraw::writeExtraParam( DrawParamWriter& writer )
{
    writer.writeDouble( m_length );
}

Adesk::Boolean SingleObturationDraw::subWorldDraw( AcGiWorldDraw* mode )
{
    assertReadEnabled () ;

    AcGeVector3d v( AcGeVector3d::kXAxis );
    v.rotateBy( m_angle + PI * 0.5, AcGeVector3d::kZAxis );

    DrawLine( mode, m_pt, m_pt + v * m_length * 0.5 );

    v.rotateBy( PI, AcGeVector3d::kZAxis );
    DrawLine( mode, m_pt, m_pt + v * m_length * 0.5 );

    return Adesk::kTrue;
}

Acad::ErrorStatus SingleObturationDraw::subTransformBy( const AcGeMatrix3d& xform )
{
    // 插入点变换
    m_pt.transformBy( xform );

    AcGeVector3d v( AcGeVector3d::kXAxis );
    v.rotateBy( m_angle, AcGeVector3d::kZAxis );
    v.transformBy( xform );

    m_angle = v.angleTo( AcGeVector3d::kXAxis, -AcGeVector3d::kZAxis ); // 变换后的旋转角度

    return Acad::eOk;
}

Acad::ErrorStatus SingleObturationDraw::subGetOsnapPoints (
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

Acad::ErrorStatus SingleObturationDraw::subGetGripPoints( AcGePoint3dArray& gripPoints, AcDbIntArray& osnapModes, AcDbIntArray& geomIds ) const
{
    assertReadEnabled () ;

    gripPoints.append( m_pt );

    return Acad::eOk;
}

Acad::ErrorStatus SingleObturationDraw::subMoveGripPointsAt( const AcDbIntArray& indices, const AcGeVector3d& offset )
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
