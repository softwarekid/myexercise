#include "StdAfx.h"
#include "StationDataLink.h"

void StationDataLink::regDatas()
{
    linkBoolData( _T( "�Ƿ񰴹涨���" ), &hasDone );
}

StationTunnelDataLink::StationTunnelDataLink() : DataLink(), hasWindStation( false ), hasDone( false )
{

}

void StationTunnelDataLink::regDatas()
{
    linkStringData( _T( "����" ), &name );
}