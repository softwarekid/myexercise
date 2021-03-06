#include "stdafx.h"
#include "DHC_AFCDlg.h"

#include "PowderDlg.h"
#include "ChargeDlg.h"
#include "MachineRoomDlg.h"

#include "../MineGE/HelperClass.h"
#include "../ArxHelper/HelperClass.h"

static ROOM_TYPE GetRoomType( const AcDbObjectId& objId )
{
    ROOM_TYPE rt;
    if( ArxUtilHelper::IsEqualType( _T( "PowderStorage" ), objId ) )
    {
        rt = RT_POWDER;
    }
    else if( ArxUtilHelper::IsEqualType( _T( "ChargeRoom" ), objId ) )
    {
        rt = RT_CHARGE;
    }
    else if( ArxUtilHelper::IsEqualType( _T( "MachineRoom" ), objId ) )
    {
        rt = RT_MACHINE;
    }
    return rt;
}

IMPLEMENT_DYNAMIC( DHC_AFCDlg, CDialog )

DHC_AFCDlg::DHC_AFCDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( DHC_AFCDlg::IDD, pParent )
{

}

DHC_AFCDlg::~DHC_AFCDlg()
{
}

void DHC_AFCDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
}


BEGIN_MESSAGE_MAP( DHC_AFCDlg, CDialog )
    ON_BN_CLICKED( IDC_DHC_AFC_POWDER_BTN, &DHC_AFCDlg::OnBnClickedDhcAfcPowderBtn )
    ON_BN_CLICKED( IDC_DHC_AFC_CHARGE_BTN, &DHC_AFCDlg::OnBnClickedDhcAfcChargeBtn )
    ON_BN_CLICKED( IDC_DHC_AFC_MACHINE_BTN, &DHC_AFCDlg::OnBnClickedDhcAfcMachineBtn )
END_MESSAGE_MAP()


// DHC_AFCDlg 消息处理程序

BOOL DHC_AFCDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    ROOM_TYPE rt = GetRoomType( m_objId );
    switch( rt )
    {
    case RT_POWDER:
        GetDlgItem( IDC_DHC_AFC_POWDER_BTN )->EnableWindow( TRUE );
        GetDlgItem( IDC_DHC_AFC_CHARGE_BTN )->EnableWindow( FALSE );
        GetDlgItem( IDC_DHC_AFC_MACHINE_BTN )->EnableWindow( FALSE );
        break;
    case RT_CHARGE:
        GetDlgItem( IDC_DHC_AFC_POWDER_BTN )->EnableWindow( FALSE );
        GetDlgItem( IDC_DHC_AFC_CHARGE_BTN )->EnableWindow( TRUE );
        GetDlgItem( IDC_DHC_AFC_MACHINE_BTN )->EnableWindow( FALSE );
        break;
    case RT_MACHINE:
        GetDlgItem( IDC_DHC_AFC_POWDER_BTN )->EnableWindow( FALSE );
        GetDlgItem( IDC_DHC_AFC_CHARGE_BTN )->EnableWindow( FALSE );
        GetDlgItem( IDC_DHC_AFC_MACHINE_BTN )->EnableWindow( TRUE );
        break;
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void DHC_AFCDlg::OnBnClickedDhcAfcPowderBtn()
{
    CAcModuleResourceOverride myResources;

    PowderDlg dlg;
    dlg.m_objId = m_objId;
    dlg.DoModal();
}

void DHC_AFCDlg::OnBnClickedDhcAfcChargeBtn()
{
    CAcModuleResourceOverride myResources;

    ChargeDlg dlg;
    dlg.m_objId = m_objId;
    dlg.DoModal();
}

void DHC_AFCDlg::OnBnClickedDhcAfcMachineBtn()
{
    CAcModuleResourceOverride myResources;

    MachineRoomDlg dlg;
    dlg.m_objId = m_objId;
    dlg.DoModal();
}
