#include "StdAfx.h"
#include "CircularJointDraw.h"

#include "CircularJointDraw_ConfigDlg.h"

ACRX_CONS_DEFINE_MEMBERS ( CircularJointDraw, MineGEDraw, 1 )

CircularJointDraw::CircularJointDraw()
{
}

CircularJointDraw::~CircularJointDraw( void )
{
}

void CircularJointDraw::setAllExtraParamsToDefault()
{
    m_radius = 30;
}

void CircularJointDraw::configExtraParams()
{
    // 切换资源
    CAcModuleResourceOverride myResources;

    CircularJointDraw_ConfigDlg dlg;
    dlg.m_strRadius.Format( _T( "%.3f" ), m_radius );
    dlg.DoModal();

    if( dlg.m_strRadius.GetLength() > 0 )
    {
        double radius;
        if( RTNORM == acdbDisToF( dlg.m_strRadius, -1, &radius ) )
        {
            m_radius = radius;
        }
    }
}

void CircularJointDraw::updateExtraParams()
{

}

void CircularJointDraw::readKeyParam( DrawParamReader& reader )
{
    reader.readPoint( m_insertPt );
}

void CircularJointDraw::writeKeyParam( DrawParamWriter& writer )
{
    writer.writePoint( m_insertPt );
}

void CircularJointDraw::readExtraParam( DrawParamReader& reader )
{
    reader.readDouble( m_radius );
}

void CircularJointDraw::writeExtraParam( DrawParamWriter& writer )
{
    writer.writeDouble( m_radius );
}

void CircularJointDraw::drawLine( AcGiWorldDraw* mode )
{
    AcGeVector3d v( AcGeVector3d::kXAxis );
    v *= m_radius;

    for( int i = 0; i < 4; i++ )
    {
        AcGePoint3dArray pts;
        pts.append( m_insertPt );
        pts.append( m_insertPt + v );
        mode->geometry().worldLine( pts.asArrayPtr() );

        v.rotateBy( PI / 2, AcGeVector3d::kZAxis );
    }
}

/*
 * 在绘制闭合图形时，AcGiFillType默认为kAcGiFillAlways (始终填充)
 * 闭合图形包括：圆、多边形、网格等
 * 参见：AcGiSubEntityTraits::fillType()方法说明
 * 例如，绘制一个圆，当前颜色是黑底白色，那么采用自定义实体绘制的圆有2种情况:
 *	    1) arx程序加载的情况下-- 白边+黑底填充(正常效果，和cad的圆是一样的)
 *		2) arx程序卸载，cad采用代理实体模式显示图元 -- 白边+白底填充
 * 具体参见：绘制填充圆的一些说明.doc
 */
void CircularJointDraw::drawCircle( AcGiWorldDraw* mode )
{
    AcGiSubEntityTraits& traits = mode->subEntityTraits();
    AcGiFillType ft = traits.fillType();
    traits.setFillType( kAcGiFillNever ); // 不填充

    mode->geometry().circle( m_insertPt, m_radius, AcGeVector3d::kZAxis );

    // 恢复属性
    traits.setFillType( ft );
}

Adesk::Boolean CircularJointDraw::subWorldDraw( AcGiWorldDraw* mode )
{
    assertReadEnabled () ;

    drawCircle( mode );
    drawLine( mode );

    return Adesk::kTrue;
}

Acad::ErrorStatus CircularJointDraw::subTransformBy( const AcGeMatrix3d& xform )
{
    //m_insertPt.transformBy(xform);	// 插入点变换
    return Acad::eOk;
}

//- Osnap points protocol
Acad::ErrorStatus CircularJointDraw::subGetOsnapPoints (
    AcDb::OsnapMode osnapMode,
    int gsSelectionMark,
    const AcGePoint3d& pickPoint,
    const AcGePoint3d& lastPoint,
    const AcGeMatrix3d& viewXform,
    AcGePoint3dArray& snapPoints,
    AcDbIntArray& geomIds ) const
{
    assertReadEnabled () ;

    // 只捕捉1种类型的点：中心点
    if( osnapMode != AcDb::kOsModeCen ) return Acad::eOk;

    if( osnapMode == AcDb::kOsModeCen )
    {
        snapPoints.append( m_insertPt );
    }

    return Acad::eOk;
}

//- Grip points protocol
Acad::ErrorStatus CircularJointDraw::subGetGripPoints (
    AcGePoint3dArray& gripPoints, AcDbIntArray& osnapModes, AcDbIntArray& geomIds ) const
{
    assertReadEnabled () ;

    gripPoints.append( m_insertPt );

    return Acad::eOk;
}

Acad::ErrorStatus CircularJointDraw::subMoveGripPointsAt ( const AcDbIntArray& indices, const AcGeVector3d& offset )
{
    assertWriteEnabled () ;

    for( int i = 0; i < indices.length(); i++ )
    {
        int idx = indices.at( i );
        if ( idx == 0 )
        {
            m_insertPt += offset; // 始节点偏移
        }
    }
    return Acad::eOk;
}