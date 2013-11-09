#include "stdafx.h"
#include "LocalFanDlg.h"

#include "../MineGE/HelperClass.h"
#include "../MineGE/DataLink.h"

class LocalFanDataLink : public DataLink
{
protected:
    virtual void regDatas()
    {
        linkIntData( _T( "局部通风机台数" ), &fanNum );
        linkDoubleData( _T( "局部通风机实际吸风量" ), &q_fan );
    }

public:
    int fanNum;                   // 局部通风机台数
    double q_fan;                 // 局部通风机实际吸风量
};


IMPLEMENT_DYNAMIC( LocalFanDlg, CDialog )

LocalFanDlg::LocalFanDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( LocalFanDlg::IDD, pParent )
    , fanNum( 0 )
    , q_fan( 0 )
{

}

LocalFanDlg::~LocalFanDlg()
{
}

void LocalFanDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Text( pDX, IDC_LOCAL_FAN_NUM_EDIT, fanNum );
    DDX_Text( pDX, IDC_LOCAL_FAN_Q_EDIT, q_fan );
}


BEGIN_MESSAGE_MAP( LocalFanDlg, CDialog )
    ON_BN_CLICKED( IDOK, &LocalFanDlg::OnBnClickedOk )
END_MESSAGE_MAP()

BOOL LocalFanDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    readPropertyData();

    return TRUE;
}

void LocalFanDlg::OnBnClickedOk()
{
    writePropertyData();

    OnOK();
}

void LocalFanDlg::readPropertyData()
{
    LocalFanDataLink dl;
    dl.setDataSource( m_objId );
    dl.updateData( false );

    fanNum = dl.fanNum;
    q_fan = dl.q_fan;

    UpdateData( FALSE );
}

void LocalFanDlg::writePropertyData()
{
    UpdateData( TRUE );

    LocalFanDataLink dl;
    dl.fanNum = fanNum;
    dl.q_fan = q_fan;

    dl.setDataSource( m_objId );
    dl.updateData( true );
}