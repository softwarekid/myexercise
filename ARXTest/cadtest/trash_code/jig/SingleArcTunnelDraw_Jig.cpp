#include "StdAfx.h"
#include "SingleArcTunnelDraw_Jig.h"

SingleArcTunnelDraw_Jig::SingleArcTunnelDraw_Jig( const CString& geType, const CString& drawName )
    : MineGEDraw_Jig( geType, drawName )
{
}

Adesk::Boolean SingleArcTunnelDraw_Jig::doJig( MineGEDraw* pMineGEDraw )
{
    // 转换成特定效果的draw指针对象
    m_pDraw = SingleArcTunnelDraw::cast( pMineGEDraw );

    setUserInputControls( ( UserInputControls )( kAcceptMouseUpAsPoint | kDontUpdateLastPoint ) );

    setDispPrompt( _T( "\n请选择巷道起点坐标: " ) );
    AcGePoint3d pt;
    DragStatus stat = acquirePoint( pt );
    if ( stat != kNormal ) return Adesk::kFalse;

    m_pDraw->m_startPt = pt;
    m_pDraw->m_endPt = pt; // 这个很重要，避免绘制无谓的效果

    gs = SEPOINT;
    setDispPrompt( _T( "\n请选择巷道第2点坐标: " ) );
    stat = drag();
    if ( ( stat != kNoChange ) && ( stat != kNormal ) ) return Adesk::kFalse;

    gs = EPOINT;
    setDispPrompt( _T( "\n请选择巷道末点坐标: " ) );
    stat = drag();

    return ( stat == kNormal );
}

AcEdJig::DragStatus SingleArcTunnelDraw_Jig::sampler()
{
    if( gs == SEPOINT )
        return getSecondPoint();
    else if( gs == EPOINT )
        return getEndPoint();
    else
        return kNormal;
}

Adesk::Boolean SingleArcTunnelDraw_Jig::update()
{
    m_pDraw->m_thirdPt = m_secondPt;

    return Adesk::kTrue;
}

AcDbEntity* SingleArcTunnelDraw_Jig::entity() const
{
    return m_pDraw;
}

AcEdJig::DragStatus SingleArcTunnelDraw_Jig::getEndPoint()
{
    AcGePoint3d pt;
    AcEdJig::DragStatus stat = acquirePoint( pt, m_secondPt );
    if( stat != kNormal ) return stat;

    if( pt == m_secondPt )
    {
        stat = kNoChange;
    }
    else
    {
        m_pDraw->m_endPt = pt;
    }
    return stat;
}

AcEdJig::DragStatus SingleArcTunnelDraw_Jig::getSecondPoint()
{
    AcGePoint3d pt;
    AcEdJig::DragStatus stat = acquirePoint( pt, m_pDraw->m_startPt );
    if( stat != kNormal ) return stat;

    if( pt == m_pDraw->m_startPt || pt == m_secondPt )
    {
        stat = kNoChange;
    }
    else
    {
        m_secondPt = pt;
    }

    // 在获得第2点的时候不进行绘制
    return kNoChange;
}