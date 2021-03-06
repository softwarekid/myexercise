#include "StdAfx.h"
#include "DoubleTTunnelDraw.h"

ACRX_CONS_DEFINE_MEMBERS ( DoubleTTunnelDraw, DoubleTunnelDraw, 1 )

DoubleTTunnelDraw::DoubleTTunnelDraw( void )
{
}

void DoubleTTunnelDraw::setAllExtraParamsToDefault()
{
    DoubleTunnelDraw::setAllExtraParamsToDefault();
    m_distance = 50;
}

void DoubleTTunnelDraw::configExtraParams()
{

}

void DoubleTTunnelDraw::readExtraParam( DrawParamReader& reader )
{
    DoubleTunnelDraw::readExtraParam( reader );
    reader.readDouble( m_distance );
}

void DoubleTTunnelDraw::writeExtraParam( DrawParamWriter& writer )
{
    DoubleTunnelDraw::writeExtraParam( writer );
    writer.writeDouble( m_distance );
}

void DoubleTTunnelDraw::dealWithStartPointBoundary( const AcGeRay3d& boundaryLine )
{
    DoubleTunnelDraw::dealWithStartPointBoundary( boundaryLine );
}

void DoubleTTunnelDraw::dealWithEndPointBoundary( const AcGeRay3d& boundaryLine )
{
    // 不处理末端闭合
    return;
}

Adesk::Boolean DoubleTTunnelDraw::subWorldDraw( AcGiWorldDraw* mode )
{
    assertReadEnabled();

    Adesk::Boolean b = DoubleTunnelDraw::subWorldDraw( mode );
    if( b )
    {
        AcGePoint3d headPt = caclHeadPoint();
        drawLine( mode, m_leftEndPt, headPt );
        drawLine( mode, m_rightEndPt, headPt );
    }

    //return Adesk::kTrue;
    return b;
}

AcGePoint3d DoubleTTunnelDraw::caclHeadPoint()
{
    AcGeVector3d v = m_endPt - m_startPt;
    v.normalize(); // 单位方向向量
    v = v * m_distance;

    return m_endPt + v;
}