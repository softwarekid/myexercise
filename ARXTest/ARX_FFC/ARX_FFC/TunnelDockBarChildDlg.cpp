#include "stdafx.h"
#include "TunnelDockBarChildDlg.h"

#include "../MineGE/HelperClass.h"

IMPLEMENT_DYNAMIC( TunnelDockBarChildDlg, PropertyData_DockBarChildDlg )

TunnelDockBarChildDlg::TunnelDockBarChildDlg( CWnd* pParent /*=NULL*/ )
    : PropertyData_DockBarChildDlg( TunnelDockBarChildDlg::IDD, pParent )
    , m_area( _T( "" ) )
    , m_q( _T( "" ) )
    , m_ch4( _T( "" ) )
    , m_o2( _T( "" ) )
{

}

TunnelDockBarChildDlg::~TunnelDockBarChildDlg()
{
}

void TunnelDockBarChildDlg::DoDataExchange( CDataExchange* pDX )
{
    PropertyData_DockBarChildDlg::DoDataExchange( pDX );
    DDX_Text( pDX, IDC_FFC_TUNNEL_AREA, m_area );
    DDX_Text( pDX, IDC_FFC_TUNNEL_Q, m_q );
    DDX_Text( pDX, IDC_FFC_TUNNEL_CH4, m_ch4 );
    DDX_Text( pDX, IDC_FFC_TUNNEL_O2, m_o2 );
}


BEGIN_MESSAGE_MAP( TunnelDockBarChildDlg, PropertyData_DockBarChildDlg )
END_MESSAGE_MAP()

BOOL TunnelDockBarChildDlg::OnInitDialog()
{
    PropertyData_DockBarChildDlg::OnInitDialog();

    return TRUE;
}

void TunnelDockBarChildDlg::readPropertyData()
{
    DataHelper::GetPropertyData( m_objId, _T( "断面面积" ), m_area );
    DataHelper::GetPropertyData( m_objId, _T( "风量" ), m_q );
    DataHelper::GetPropertyData( m_objId, _T( "甲烷浓度" ), m_ch4 );
    DataHelper::GetPropertyData( m_objId, _T( "氧气浓度" ), m_o2 );

    UpdateData( FALSE );
}

void TunnelDockBarChildDlg::writePropertyData()
{
    UpdateData( TRUE );

    DataHelper::SetPropertyData( m_objId, _T( "断面面积" ), m_area );
    DataHelper::SetPropertyData( m_objId, _T( "风量" ), m_q );
    DataHelper::SetPropertyData( m_objId, _T( "甲烷浓度" ), m_ch4 );
    DataHelper::SetPropertyData( m_objId, _T( "氧气浓度" ), m_o2 );
}