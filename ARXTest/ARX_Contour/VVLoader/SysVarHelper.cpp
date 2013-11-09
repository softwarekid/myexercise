#include "StdAfx.h"
#include "SysVarHelper.h"

#include "TolSetter.h"
#include "PickAdd.h"
#include "SelectPreview.h"

// ϵͳ����������ȫ�ֱ���
TolSetter* pTS = 0;
// ���������ͣ�Ƿ������ʾ
SelectPreview* pSelectPreview = 0;
// ����ÿ��ֻ��ѡ��һ��ͼԪ
PickAdd* pPickAdd = 0;

void SysVarHelper::ModifyTolerance()
{
    if( pTS == 0 )
    {
        pTS = new TolSetter( GLOBAL_TOLERANCE );
    }
}

void SysVarHelper::RestoreTolerace()
{
    if( pTS != 0 )
    {
        delete pTS;
        pTS = 0;
    }
}

void SysVarHelper::ModifySelectPreview()
{
    if( pSelectPreview == 0 )
    {
        pSelectPreview = new SelectPreview( 0 );
    }
}

void SysVarHelper::RestoreSelectPreview()
{
    if( pSelectPreview != 0 )
    {
        delete pSelectPreview;
        pSelectPreview = 0;
    }
}

void SysVarHelper::ModifyPickAdd()
{
    if( pPickAdd == 0 )
    {
        pPickAdd = new PickAdd( 0 );
    }
}

void SysVarHelper::RestorePickAdd()
{
    if( pPickAdd != 0 )
    {
        delete pPickAdd;
        pPickAdd = 0;
    }
}