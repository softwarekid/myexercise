#include "stdafx.h"

#include "PropertyDataDlgHelper.h"
#include "../MineGE/DoubleClickCallBack.h"

void DCF_MineGECmds( const AcDbObjectId& objId )
{
    // ��ʾ���ԶԻ���
    PropertyDataDlgHelper::DisplayPropertyDataDlg( objId );
}

REG_DOUBLE_CLICK_FUNC( DCF_MineGECmds )
