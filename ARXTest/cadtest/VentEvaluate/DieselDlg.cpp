#include "stdafx.h"
#include "DieselDlg.h"

#include "../MineGE/HelperClass.h"
#include "../MineGE/DataLink.h"

class DieselDataLink : public DataLink
{
protected:
    virtual void regDatas()
    {
        linkIntData( _T( "胶轮车台数" ), &num );
        linkDoubleData( _T( "胶轮车功率" ), &power );
    }

public:
    int num;
    double power;
};

IMPLEMENT_DYNAMIC( DieselDlg, CDialog )

DieselDlg::DieselDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( DieselDlg::IDD, pParent )
    , m_num( 0 )
    , m_power( 0 )
{

}

DieselDlg::~DieselDlg()
{
}

void DieselDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Text( pDX, IDC_DIESEL_PARAM_NUM, m_num );
    DDX_Text( pDX, IDC_DIESEL_PARAM_POWER, m_power );
}


BEGIN_MESSAGE_MAP( DieselDlg, CDialog )
    ON_BN_CLICKED( IDOK, &DieselDlg::OnBnClickedOk )
END_MESSAGE_MAP()


BOOL DieselDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    readPropertyData();

    return TRUE;
}

void DieselDlg::readPropertyData()
{
    DieselDataLink dl;
    dl.setDataSource( m_objId );
    dl.updateData( false );
    m_num = dl.num;
    m_power = dl.power;

    UpdateData( FALSE );
}

void DieselDlg::writePropertyData()
{
    UpdateData( TRUE );

    DieselDataLink dl;
    dl.setDataSource( m_objId );
    dl.num = m_num;
    dl.power = m_power;

    dl.updateData( true );
}

void DieselDlg::OnBnClickedOk()
{
    writePropertyData();
    OnOK();
}