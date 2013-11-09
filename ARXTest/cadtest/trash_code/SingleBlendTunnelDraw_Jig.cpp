#include "StdAfx.h"
#include "SingleBlendTunnelDraw_Jig.h"

SingleBlendTunnelDraw_Jig::SingleBlendTunnelDraw_Jig( const CString& geType, const CString& drawName )
    : MineGEDraw_Jig( geType, drawName )
{
}

Adesk::Boolean SingleBlendTunnelDraw_Jig::doJig( MineGEDraw* pMineGEDraw )
{
    // ת�����ض�Ч����drawָ�����
    m_pDraw = SingleBlendTunnelDraw::cast( pMineGEDraw );

    m_pDraw->m_blendPts.removeAll(); // ���ԭ�е��۵�

    setUserInputControls( ( UserInputControls )( kAcceptMouseUpAsPoint | kDontUpdateLastPoint ) );

    setDispPrompt( _T( "\n��ѡ������������: " ) );
    AcGePoint3d pt;
    DragStatus stat = acquirePoint( pt );
    if ( stat != kNormal ) return Adesk::kFalse;

    m_pDraw->m_startPt = pt;
    m_pDraw->m_endPt = pt;
    m_prevPt = pt;

    int i = 0;
    const int maxCount = 8; // ���յ���
    while( i < maxCount )
    {
        setDispPrompt( _T( "\n��ѡ�������һ������: " ) );
        stat = drag();

        if( stat != kNormal )
        {
            m_pDraw->m_endPt = m_prevPt;
            break;
        }

        i++;

        if( i > 1 )
        {
            m_pDraw->m_blendPts.append( m_prevPt );
            m_prevPt = m_pDraw->m_endPt;
        }
    }

    return !( i < 1 && stat != kNormal );
}

AcEdJig::DragStatus SingleBlendTunnelDraw_Jig::sampler()
{
    return getNextPoint();
}

Adesk::Boolean SingleBlendTunnelDraw_Jig::update()
{
    return Adesk::kTrue;
}

AcDbEntity* SingleBlendTunnelDraw_Jig::entity() const
{
    return m_pDraw;
}

AcEdJig::DragStatus SingleBlendTunnelDraw_Jig::getNextPoint()
{
    setUserInputControls( ( UserInputControls )( kAcceptMouseUpAsPoint | kDontUpdateLastPoint ) );

    AcGePoint3d pt;
    AcEdJig::DragStatus stat = acquirePoint( pt, m_prevPt );
    if( stat != kNormal ) return stat;

    if( pt == m_prevPt || pt == m_pDraw->m_endPt )
    {
        stat = kNoChange;
    }
    else
    {
        m_pDraw->m_endPt = pt;
    }

    return stat;
}