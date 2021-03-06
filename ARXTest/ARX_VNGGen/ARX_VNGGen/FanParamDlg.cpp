#include "stdafx.h"
#include "FanParamDlg.h"
#include "DefParam.h"

IMPLEMENT_DYNAMIC( FanParamDlg, CDialog )

FanParamDlg::FanParamDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( FanParamDlg::IDD, pParent )
    , m_radius( 0 )
    , m_distance( 0 )
{

}

FanParamDlg::~FanParamDlg()
{
}

void FanParamDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Text( pDX, IDC_FAN_RADIUS, m_radius );
    DDX_Text( pDX, IDC_FAN_DISTANCE, m_distance );
}


BEGIN_MESSAGE_MAP( FanParamDlg, CDialog )
    ON_BN_CLICKED( IDOK, &FanParamDlg::OnBnClickedOk )
    ON_BN_CLICKED( IDC_FAN_DEF_BTN, &FanParamDlg::OnBnClickedFanDefBtn )
END_MESSAGE_MAP()


BOOL FanParamDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_radius = fp.m_radius;
    m_distance = fp.m_distance;

    UpdateData( FALSE );

    return TRUE;
}

void FanParamDlg::OnBnClickedOk()
{
    if( UpdateData( TRUE ) )
    {
        fp.m_radius = m_radius;
        fp.m_distance = m_distance;

        OnOK();
    }
}

void FanParamDlg::OnBnClickedFanDefBtn()
{
    m_radius = DEF_FAN_RADIUS;
    m_distance = DEF_FAN_DISTANCE;
    UpdateData( FALSE );
}
