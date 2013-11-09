#include "stdafx.h"
#include "GambitParamDlg.h"

#include "GambitParamHelper.h"

IMPLEMENT_DYNAMIC( GambitParamDlg, CDialog )

GambitParamDlg::GambitParamDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( GambitParamDlg::IDD, pParent )
    , m_width( 8 )
{

}

GambitParamDlg::~GambitParamDlg()
{
}

void GambitParamDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Text( pDX, IDC_FFC_GAMBIT_WIDTH, m_width );
    DDV_MinMaxDouble( pDX, m_width, 2, 40 );
}


BEGIN_MESSAGE_MAP( GambitParamDlg, CDialog )
    ON_BN_CLICKED( IDOK, &GambitParamDlg::OnBnClickedOk )
END_MESSAGE_MAP()


// GambitParamDlg 消息处理程序

void GambitParamDlg::OnBnClickedOk()
{
    if( UpdateData( TRUE ) )
    {
        GambitParam gp;
        gp.width = m_width;
        GambitParamHelper::WriteGambitParam( gp );

        OnOK();
    }
}

BOOL GambitParamDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    GambitParam gp;
    GambitParamHelper::ReadGambitParam( gp );
    m_width = gp.width;

    UpdateData( FALSE );

    return TRUE;
}
