#include "StdAfx.h"
#include "SingleTunnelDraw_Jig.h"

SingleTunnelDraw_Jig::SingleTunnelDraw_Jig( const CString& geType, const CString& drawName )
    : MineGEDraw_Jig( geType, drawName )
{
}

Adesk::Boolean SingleTunnelDraw_Jig::doJig( MineGEDraw* pMineGEDraw )
{
    // 转换成特定效果的draw指针对象
    m_pDraw = SingleTunnelDraw::cast( pMineGEDraw );

    setDispPrompt( _T( "\n请选择巷道起点坐标: " ) );
    AcGePoint3d pt;
    DragStatus stat = acquirePoint( pt );

    if ( stat != kNormal ) return Adesk::kFalse;

    m_pDraw->m_startPt = pt;

    setDispPrompt( _T( "\n请选择末点坐标: " ) );
    stat = drag();

    return ( stat == kNormal );
}

AcEdJig::DragStatus SingleTunnelDraw_Jig::getEndPoint()
{
    AcGePoint3d pt;
    AcEdJig::DragStatus stat = acquirePoint( pt, m_pDraw->m_startPt );

    if( stat != kNormal ) return stat;

    if( pt == m_pDraw->m_startPt ) // no change
    {
        stat = kNoChange;
    }
    else
    {
        m_pDraw->m_endPt = pt;
    }

    return stat;
}

AcEdJig::DragStatus SingleTunnelDraw_Jig::sampler()
{
    return getEndPoint();
}

Adesk::Boolean SingleTunnelDraw_Jig::update()
{
    return Adesk::kTrue;
}

AcDbEntity* SingleTunnelDraw_Jig::entity() const
{
    return m_pDraw;
}