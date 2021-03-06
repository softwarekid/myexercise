#include "stdafx.h"
#include "TWSDataLink.h"

void TWSDataLink::regDatas()
{
    linkStringData( _T( "名称" ), &name );
    linkDoubleData( _T( "氧气浓度" ), &O2 );
    linkDoubleData( _T( "二氧化碳浓度" ), &CO2 );
    linkDoubleData( _T( "一氧化碳浓度" ), &CO );
    linkDoubleData( _T( "氧化氮浓度" ), &NO );
    linkDoubleData( _T( "二氧化硫浓度" ), &SO2 );
    linkDoubleData( _T( "硫化氢浓度" ), &H2S );
    linkDoubleData( _T( "氨浓度" ), &NH3 );
}