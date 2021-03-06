#include "StdAfx.h"
#include "UIHelper.h"

#include "TunnelDockBarChildDlg.h"
#include "WSDockBarChildDlg.h"
#include "ObturationDockBarChildDlg.h"
#include "GasPipeDockBarChildDlg.h"
#include "NitrogenDockBarChildDlg.h"
#include "SourceAirLeakDockBarChildDlg.h"
#include "SinkAirLeakDockBarChildDlg.h"

#include "DataDlgManager.h"

#include "GoafGasDlg.h"
#include "FFCDialog.h"

#include "GambitParamDlg.h"

DataDlgManager* pDDM = 0;

static void ChildOnCreate( PropertyData_DockBarChildDlg* pDlg, long idd, CWnd* pParent )
{
    CAcModuleResourceOverride resourceOverride;
    pDlg->Create( idd, pParent );
    pDlg->ShowWindow( SW_HIDE );
}

static void CreateDlg( CMDIFrameWnd* pAcadFrame, const CString& type, long id, PropertyData_DockBarChildDlg* pDlg )
{
    ChildOnCreate( pDlg, id, pAcadFrame );
    pDDM->regDlg( type, pDlg );
}

static void RegDialogs( CMDIFrameWnd* pAcadFrame )
{
    pDDM = new DataDlgManager();
    if( pDDM != 0 )
    {
        CreateDlg( pAcadFrame, _T( "Tunnel" ), IDD_FFC_TUNNEL_PARAM_DLG, new TunnelDockBarChildDlg() );
        CreateDlg( pAcadFrame, _T( "WorkSurface" ), IDD_FFC_WS_PARAM_DLG, new WSDockBarChildDlg() );
        CreateDlg( pAcadFrame, _T( "Obturation" ), IDD_FFC_OBTURATION_PARAM_DLG, new ObturationDockBarChildDlg() );
        CreateDlg( pAcadFrame, _T( "GasPipe" ), IDD_FFC_GAS_PIPE_PARAM_DLG, new GasPipeDockBarChildDlg() );
        CreateDlg( pAcadFrame, _T( "NitrogenPipe" ), IDD_FFC_NITROGEN_PARAM_DLG, new NitrogenDockBarChildDlg() );
        CreateDlg( pAcadFrame, _T( "SourceAirLeak" ), IDD_FFC_SOURCE_AIR_LEAK_PARAM_DLG, new SourceAirLeakDockBarChildDlg() );
        CreateDlg( pAcadFrame, _T( "SinkAirLeak" ), IDD_FFC_SINK_AIR_LEAK_PARAM_DLG, new SinkAirLeakDockBarChildDlg() );
    }
}
void UIHelper::CreateDataDlgManager()
{
    CAcModuleResourceOverride myResources;
    RegDialogs( acedGetAcadFrame() );
}

void UIHelper::DestroyDataDlgManager()
{
    delete pDDM;
    pDDM = 0;
}

void UIHelper::LoadData( const AcDbObjectId& objId )
{
    if( pDDM != 0 )
    {
        pDDM->load( objId );
    }
}

void UIHelper::ShowGoafGasDlg()
{
    // 切换资源
    CAcModuleResourceOverride myResources;

    GoafGasDlg dlg;
    dlg.DoModal();
}

void UIHelper::ShowFFCDlg()
{
    // 切换资源
    CAcModuleResourceOverride myResources;

    FFCDialog dlg;
    dlg.DoModal();
}

void UIHelper::ShowGambitParamDlg()
{
    // 切换资源
    CAcModuleResourceOverride myResources;

    GambitParamDlg dlg;
    dlg.DoModal();
}