#include "StdAfx.h"
#include "LiftShaftDataLink.h"

void LiftShaftDataLink::regDatas()
{
    linkStringData( _T( "名称" ), &name );
    linkDoubleData( _T( "风速" ), &v );
    linkIntData( _T( "功能类型" ), &ft );
    linkIntData( _T( "井筒附加属性" ), &sp );
}