#include "stdafx.h"
#include "MpnwDlg.h"

#include "../MineGE/HelperClass.h"
#include "../MineGE/DataLink.h"

class MpnwDataLink : public DataLink
{
protected:
    virtual void regDatas()
    {
        linkIntData( _T( "工作面最多人数" ), &maxPeopleNum );
        linkDoubleData( _T( "一次爆破炸药最大用量" ), &maxWeight );
    }

public:
    int maxPeopleNum;             // 工作面最多人数
    double maxWeight;             // 一次爆破炸药最大用量

};

IMPLEMENT_DYNAMIC( MpnwDlg, CDialog )

MpnwDlg::MpnwDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( MpnwDlg::IDD, pParent )
    , maxPeopleNum( 0 )
    , maxWeight( 0 )
{

}

MpnwDlg::~MpnwDlg()
{
}

void MpnwDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Text( pDX, IDC_MPN_EDIT, maxPeopleNum );
    DDX_Text( pDX, IDC_MW_EDIT, maxWeight );
}

BEGIN_MESSAGE_MAP( MpnwDlg, CDialog )
    ON_BN_CLICKED( IDOK, &MpnwDlg::OnBnClickedOk )
END_MESSAGE_MAP()

BOOL MpnwDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    readPropertyData();

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void MpnwDlg::OnBnClickedOk()
{
    writePropertyData();

    OnOK();
}

void MpnwDlg::readPropertyData()
{
    MpnwDataLink dl;
    dl.setDataSource( m_objId );
    dl.updateData( false );

    maxPeopleNum = dl.maxPeopleNum;
    maxWeight = dl.maxWeight;

    UpdateData( FALSE );
}

void MpnwDlg::writePropertyData()
{
    UpdateData( TRUE );

    MpnwDataLink dl;
    dl.maxPeopleNum = maxPeopleNum;
    dl.maxWeight = maxWeight;

    dl.setDataSource( m_objId );
    dl.updateData( true );
}