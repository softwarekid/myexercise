#include "StdAfx.h"
#include "WSDataLink.h"

WSDataLink::WSDataLink( void )
{
}

WSDataLink::~WSDataLink( void )
{
}

void WSDataLink::regDatas()
{
    linkStringData( _T( "����" ), &name );
    linkBoolData( _T( "�Ƿ���ͻ��Σ��" ), &hasRisk );
    linkDoubleData( _T( "ʼ����" ), &iz );
    linkDoubleData( _T( "ĩ����" ), &oz );
}