#include "StdAfx.h"
#include "CompassBlockDraw.h"

ACRX_CONS_DEFINE_MEMBERS ( CompassBlockDraw, BlockDraw, 1 )

CompassBlockDraw::CompassBlockDraw () : BlockDraw ()
{
    m_blockName = _T( "ָ����" );
}

CompassBlockDraw::~CompassBlockDraw ()
{
}

void CompassBlockDraw::regPropertyDataNames( AcStringArray& names ) const
{
    names.append( _T( "����" ) );
    names.append( _T( "ƫ�ƽǶ�" ) );
}

void CompassBlockDraw::readPropertyDataFromGE( const AcStringArray& values )
{
    BlockDraw::readPropertyDataFromGE( values );
    // �޸�m_angle
    // ��2������(����Ϊ1)Ϊƫ�ƽǶ�
    m_angle = _tstof( values[1].kACharPtr() );
}