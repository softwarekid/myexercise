#include "StdAfx.h"
#include "UIHelper.h"

#include "DataList_DockBar.h"

#include "GraphParamDlg.h"
#include "NodeParamDlg.h"
#include "EdgeParamDlg.h"
#include "GateParamDlg.h"
#include "FanParamDlg.h"

#include "PropertyDataDlg.h"
#include "VNGGenDlg.h"

#include "VNG_ParamHelper.h"

#include "../ArxHelper/HelperClass.h"

/* 实现在ReadFieldHelper.cpp */
extern void ReadFieldsById( const AcDbObjectId& objId, AcStringArray& fields );

/* 实现在ARX_VNGGen.cpp */
extern void GetDatasByFields( const AcDbObjectId& objId, const AcStringArray& fields, AcStringArray& values );

/* 实现在UpdateParam.cpp */
extern void UpdateAllNodesParam( NodeParam& np );
extern void UpdateAllEdgesParam( EdgeParam& ep );
extern void UpdateAllGatesParam( GateParam& gp );
extern void UpdateAllFansParam( FanParam& fp );

DataList_DockBar* pDataList_DockBar = 0;

void UIHelper::ShowDataListDockBar()
{
    CAcModuleResourceOverride myResources;

    CMDIFrameWnd* pAcadFrame = acedGetAcadFrame();

    if( pDataList_DockBar == 0 )
    {
        pDataList_DockBar = new DataList_DockBar();
        pDataList_DockBar->Create ( pAcadFrame, _T( "图形数据列表" ) ) ;
        pDataList_DockBar->EnableDocking ( CBRS_ALIGN_ANY ) ;
        pDataList_DockBar->RestoreControlBar () ;
    }
    pAcadFrame->ShowControlBar( pDataList_DockBar, TRUE, FALSE );
}

void UIHelper::DestroyDataListDockBar()
{
    if( pDataList_DockBar != 0 )
    {
        pDataList_DockBar->DestroyWindow();
        delete pDataList_DockBar;
        pDataList_DockBar = 0;
    }
}

void UIHelper::ShowPropertyDlgByDoubleClick()
{
    AcDbObjectIdArray objIds;
    ArxUtilHelper::GetPickSetEntity( objIds );
    if( objIds.length() != 1 ) return;

    AcDbObjectId objId = objIds[0];

    AcStringArray fields;
    ReadFieldsById( objId, fields );
    if( fields.isEmpty() )
    {
        AfxMessageBox( _T( "没有数据可以显示" ) );
    }
    else
    {
        AcStringArray values;
        GetDatasByFields( objId, fields, values );

        CAcModuleResourceOverride myResources;

        // 显示属性对话框
        PropertyDataDlg dlg;
        dlg.m_fields.append( fields );
        dlg.m_values.append( values );
        dlg.DoModal();
    }
}

void UIHelper::ShowVNGGenDlg()
{
    CAcModuleResourceOverride myResources;
    VNGGenDlg dlg;
    if( IDOK == dlg.DoModal() )
    {
        // 执行命令：zoom e
        acDocManager->sendStringToExecute( curDoc(), _T( "ZOOM E " ), true, false, false );
    }
}

void UIHelper::SetNodeParam()
{
    NodeParam np;
    VNG_ParamHelper::ReadNodeParams( np );

    CAcModuleResourceOverride myResources;
    NodeParamDlg dlg;
    dlg.np = np;
    if( IDOK == dlg.DoModal() )
    {
        np = dlg.np;
        VNG_ParamHelper::WriteNodeParams( np );
        UpdateAllNodesParam( dlg.np );
    }
}

void UIHelper::SetEdgeParam()
{
    EdgeParam ep;
    VNG_ParamHelper::ReadEdgeParams( ep );

    CAcModuleResourceOverride myResources;
    EdgeParamDlg dlg;
    dlg.ep = ep;
    if( IDOK == dlg.DoModal() )
    {
        ep = dlg.ep;
        VNG_ParamHelper::WriteEdgeParams( ep );
        UpdateAllEdgesParam( dlg.ep );
    }
}

void UIHelper::SetGateParam()
{
    GateParam gp;
    VNG_ParamHelper::ReadGateParams( gp );

    CAcModuleResourceOverride myResources;
    GateParamDlg dlg;
    dlg.gp = gp;
    if( IDOK == dlg.DoModal() )
    {
        gp = dlg.gp;
        VNG_ParamHelper::WriteGateParams( gp );
        UpdateAllGatesParam( dlg.gp );
    }
}

void UIHelper::SetFanParam()
{
    FanParam fp;
    VNG_ParamHelper::ReadFanParams( fp );

    CAcModuleResourceOverride myResources;
    FanParamDlg dlg;
    dlg.fp = fp;
    if( IDOK == dlg.DoModal() )
    {
        fp = dlg.fp;
        VNG_ParamHelper::WriteFanParams( fp );
        UpdateAllFansParam( dlg.fp );
    }
}