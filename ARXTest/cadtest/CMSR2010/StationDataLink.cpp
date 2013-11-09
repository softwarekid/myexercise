#include "StdAfx.h"
#include "StationDataLink.h"

void StationDataLink::regDatas()
{
    linkBoolData( _T( "是否按规定测风" ), &hasDone );
}

StationTunnelDataLink::StationTunnelDataLink() : DataLink(), hasWindStation( false ), hasDone( false )
{

}

void StationTunnelDataLink::regDatas()
{
    linkStringData( _T( "名称" ), &name );
}