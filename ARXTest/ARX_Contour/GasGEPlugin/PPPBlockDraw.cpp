#include "StdAfx.h"
#include "PPPBlockDraw.h"
#include "DrawTool.h"

ACRX_CONS_DEFINE_MEMBERS( PPPBlockDraw, BlockDraw, 1 )

PPPBlockDraw::PPPBlockDraw () : BlockDraw ()
{
    m_blockName = _T( "动力现象点" );
}

void PPPBlockDraw::regPropertyDataNames( AcStringArray& names ) const
{
    names.append( _T( "突出煤岩量" ) );
    names.append( _T( "涌出瓦斯量" ) );
    names.append( _T( "标高" ) );
    names.append( _T( "发生年月" ) );
}

void PPPBlockDraw::readPropertyDataFromGE( const AcStringArray& values )
{
    BlockDraw::readPropertyDataFromGE( values );

    m_attValues[0].append( _T( "t" ) );

    AcString str;
    str.format( _T( "m%s" ), MakeUpperText( _T( "3" ) ) );
    m_attValues[1].append( str );
}