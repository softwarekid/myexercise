#include "StdAfx.h"
#include "TunnelNameDataLink.h"

TunnelNameDataLink::TunnelNameDataLink( void )
{
}

TunnelNameDataLink::~TunnelNameDataLink( void )
{
}

void TunnelNameDataLink::regDatas()
{
    linkStringData( _T( "����" ), &name );
}