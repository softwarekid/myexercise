#include "stdafx.h"
#include "ObturationDockBarChildDlg.h"

#include "../MineGE/HelperClass.h"

IMPLEMENT_DYNAMIC( ObturationDockBarChildDlg, PropertyData_DockBarChildDlg )

ObturationDockBarChildDlg::ObturationDockBarChildDlg( CWnd* pParent /*=NULL*/ )
    : PropertyData_DockBarChildDlg( ObturationDockBarChildDlg::IDD, pParent )
    , m_air_leak( _T( "" ) )
{

}

ObturationDockBarChildDlg::~ObturationDockBarChildDlg()
{
}

void ObturationDockBarChildDlg::DoDataExchange( CDataExchange* pDX )
{
    PropertyData_DockBarChildDlg::DoDataExchange( pDX );
    DDX_Text( pDX, IDC_FFC_OBTURATION_AIR_LEAK, m_air_leak );
}


BEGIN_MESSAGE_MAP( ObturationDockBarChildDlg, PropertyData_DockBarChildDlg )
END_MESSAGE_MAP()


BOOL ObturationDockBarChildDlg::OnInitDialog()
{
    PropertyData_DockBarChildDlg::OnInitDialog();

    return TRUE;
}

void ObturationDockBarChildDlg::readPropertyData()
{
    DataHelper::GetPropertyData( m_objId, _T( "©����" ), m_air_leak );

    UpdateData( FALSE );
}

void ObturationDockBarChildDlg::writePropertyData()
{
    UpdateData( TRUE );

    DataHelper::SetPropertyData( m_objId, _T( "©����" ), m_air_leak );
}