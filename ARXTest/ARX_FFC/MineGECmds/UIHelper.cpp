#include "StdAfx.h"
#include "UIHelper.h"

#include "DrawManagerDlg.h"
#include "FieldManagerDlg.h"
#include "DataObjectStateDlg.h"
#include "VarListDlg.h"

#include "PropertyDataDlgHelper.h"

#include "../ArxHelper/HelperClass.h"

void UIHelper::ShowDrawManagerDlg()
{
    // 切换资源
    CAcModuleResourceOverride myResources;

    DrawManagerDlg dmd;
    dmd.DoModal(); // 可视化效果切换和配置
}

void UIHelper::ShowFieldManagerDlg()
{
    // 切换资源
    CAcModuleResourceOverride myResources;

    FieldManagerDlg fmd;
    fmd.DoModal(); // 属性数据字段管理
}

//void UIHelper::ShowPropertyDataDlg()
//{
//
//}

void UIHelper::ShowVarListDlg()
{
    CAcModuleResourceOverride myResources;

    VarListDlg vld;
    vld.DoModal();
}

void UIHelper::ShowDataObjectStateDlg()
{
    CAcModuleResourceOverride myResources;

    DataObjectStateDlg dosd;
    dosd.DoModal();
}

void UIHelper::DisplayData()
{
    // 显示图元的属性数据
    AcDbObjectId objId = ArxUtilHelper::SelectObject( _T( "请选择一个图元:" ) );
    if( objId.isNull() ) return;

    PropertyDataDlgHelper::DisplayPropertyDataDlg( objId );
}

void UIHelper::DisplayDataByDoubleClick()
{
    //acutPrintf(_T("\n双击自定义..."));
    AcDbObjectIdArray objIds;
    ArxUtilHelper::GetPickSetEntity( objIds );
    if( objIds.length() != 1 ) return;

    // 显示属性对话框
    PropertyDataDlgHelper::DisplayPropertyDataDlg( objIds[0] );
}