#include "stdafx.h"
#include "TWSDataLink.h"

void TWSDataLink::regDatas()
{
    linkStringData( _T( "����" ), &name );
    linkDoubleData( _T( "����Ũ��" ), &O2 );
    linkDoubleData( _T( "������̼Ũ��" ), &CO2 );
    linkDoubleData( _T( "һ����̼Ũ��" ), &CO );
    linkDoubleData( _T( "������Ũ��" ), &NO );
    linkDoubleData( _T( "��������Ũ��" ), &SO2 );
    linkDoubleData( _T( "����Ũ��" ), &H2S );
    linkDoubleData( _T( "��Ũ��" ), &NH3 );
}