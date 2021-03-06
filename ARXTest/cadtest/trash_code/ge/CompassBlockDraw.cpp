#include "StdAfx.h"
#include "CompassBlockDraw.h"

ACRX_CONS_DEFINE_MEMBERS ( CompassBlockDraw, BlockDraw, 1 )

CompassBlockDraw::CompassBlockDraw () : BlockDraw ()
{
    m_blockName = _T( "指北针" );
}

CompassBlockDraw::~CompassBlockDraw ()
{
}

void CompassBlockDraw::regPropertyDataNames( AcStringArray& names ) const
{
    names.append( _T( "名称" ) );
    names.append( _T( "偏移角度" ) );
}

void CompassBlockDraw::readPropertyDataFromGE( const AcStringArray& values )
{
    BlockDraw::readPropertyDataFromGE( values );
    // 修改m_angle
    // 第2个数据(索引为1)为偏移角度
    m_angle = _tstof( values[1].kACharPtr() );
}