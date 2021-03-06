#include "stdafx.h"
#include "GasPipeDockBarChildDlg.h"

#include "../MineGE/HelperClass.h"

IMPLEMENT_DYNAMIC( GasPipeDockBarChildDlg, PropertyData_DockBarChildDlg )

GasPipeDockBarChildDlg::GasPipeDockBarChildDlg( CWnd* pParent /*=NULL*/ )
    : PropertyData_DockBarChildDlg( GasPipeDockBarChildDlg::IDD, pParent )
    , m_q( _T( "" ) )
    , m_ch4( _T( "" ) )
    , m_o2( _T( "" ) )
{

}

GasPipeDockBarChildDlg::~GasPipeDockBarChildDlg()
{
}

void GasPipeDockBarChildDlg::DoDataExchange( CDataExchange* pDX )
{
    PropertyData_DockBarChildDlg::DoDataExchange( pDX );
    DDX_Text( pDX, IDC_FFC_GAS_PIPE_Q, m_q );
    DDX_Text( pDX, IDC_FFC_GAS_PIPE_CH4, m_ch4 );
    DDX_Text( pDX, IDC_FFC_GAS_PIPE_O2, m_o2 );
}


BEGIN_MESSAGE_MAP( GasPipeDockBarChildDlg, PropertyData_DockBarChildDlg )
END_MESSAGE_MAP()


// GasPipeDockBarChildDlg 消息处理程序

BOOL GasPipeDockBarChildDlg::OnInitDialog()
{
    PropertyData_DockBarChildDlg::OnInitDialog();

    return TRUE;
}

void GasPipeDockBarChildDlg::readPropertyData()
{
    DataHelper::GetPropertyData( m_objId, _T( "抽放量" ), m_q );
    DataHelper::GetPropertyData( m_objId, _T( "甲烷浓度" ), m_ch4 );
    DataHelper::GetPropertyData( m_objId, _T( "氧气浓度" ), m_o2 );

    UpdateData( FALSE );
}

void GasPipeDockBarChildDlg::writePropertyData()
{
    UpdateData( TRUE );

    DataHelper::SetPropertyData( m_objId, _T( "抽放量" ), m_q );
    DataHelper::SetPropertyData( m_objId, _T( "甲烷浓度" ), m_ch4 );
    DataHelper::SetPropertyData( m_objId, _T( "氧气浓度" ), m_o2 );
}