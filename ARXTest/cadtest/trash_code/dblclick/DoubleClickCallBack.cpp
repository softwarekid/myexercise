#include "stdafx.h"
#include "DoubleClickCallBack.h"
#include "ArxUtilHelper.h"

// Ĭ�ϻص�����
void MineGE_DoNothingDoubleClick( const AcDbObjectId& objId )
{
    // do nothing
}

/* ȫ�ֵ�˫���ص����� */
DoubleClickFunc global_mine_ge_dcf = &MineGE_DoNothingDoubleClick;

void RegDoubleClickFunc( DoubleClickFunc dcf )
{
    if( dcf == 0 )
    {
        global_mine_ge_dcf = &MineGE_DoNothingDoubleClick;
    }
    else
    {
        global_mine_ge_dcf = dcf;
    }
}

/* ȫ�ֺ���(����˫���ص�����) */
void CallDoubleClickFunc()
{
    //acutPrintf(_T("\n˫���Զ���..."));
    AcDbObjectIdArray objIds;
    ArxUtilHelper::GetPickSetEntity( objIds );
    if( objIds.length() != 1 ) return;

    global_mine_ge_dcf( objIds[0] );
}