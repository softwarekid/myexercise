#include "StdAfx.h"
#include "NullJointDraw.h"

ACRX_CONS_DEFINE_MEMBERS ( NullJointDraw, MineGEDraw, 1 )

NullJointDraw::NullJointDraw()
{
}

NullJointDraw::~NullJointDraw( void )
{
}

void NullJointDraw::setAllExtraParamsToDefault()
{
}

void NullJointDraw::configExtraParams()
{

}

void NullJointDraw::updateExtraParams()
{

}

void NullJointDraw::readKeyParam( DrawParamReader& reader )
{
    reader.readPoint( m_insertPt );
}

void NullJointDraw::writeKeyParam( DrawParamWriter& writer )
{
    writer.writePoint( m_insertPt );
}

void NullJointDraw::readExtraParam( DrawParamReader& reader )
{
}

void NullJointDraw::writeExtraParam( DrawParamWriter& writer )
{
}

Adesk::Boolean NullJointDraw::subWorldDraw( AcGiWorldDraw* mode )
{
    assertReadEnabled () ;

    //return MineGEDraw::subWorldDraw(mode);
    return Adesk::kTrue;
}

Acad::ErrorStatus NullJointDraw::subTransformBy( const AcGeMatrix3d& xform )
{
    //m_insertPt.transformBy(xform);	// 插入点变换
    return Acad::eOk;
}

//- Osnap points protocol
Acad::ErrorStatus NullJointDraw::subGetOsnapPoints (
    AcDb::OsnapMode osnapMode,
    int gsSelectionMark,
    const AcGePoint3d& pickPoint,
    const AcGePoint3d& lastPoint,
    const AcGeMatrix3d& viewXform,
    AcGePoint3dArray& snapPoints,
    AcDbIntArray& geomIds ) const
{
    assertReadEnabled () ;

    //// 只捕捉1种类型的点：中心点
    //if(osnapMode != AcDb::kOsModeCen) return Acad::eOk;

    //if(osnapMode == AcDb::kOsModeCen)
    //{
    //	snapPoints.append(m_insertPt);
    //}

    return Acad::eOk;
}

//- Grip points protocol
Acad::ErrorStatus NullJointDraw::subGetGripPoints (
    AcGePoint3dArray& gripPoints, AcDbIntArray& osnapModes, AcDbIntArray& geomIds ) const
{
    assertReadEnabled () ;

    //gripPoints.append(m_insertPt);

    return Acad::eOk;
}

Acad::ErrorStatus NullJointDraw::subMoveGripPointsAt ( const AcDbIntArray& indices, const AcGeVector3d& offset )
{
    assertWriteEnabled () ;

    //for(int i=0; i<indices.length(); i++)
    //{
    //	int idx = indices.at(i);
    //	if (idx==0) {
    //		m_insertPt += offset; // 始节点偏移
    //	}
    //}
    return Acad::eOk;
}