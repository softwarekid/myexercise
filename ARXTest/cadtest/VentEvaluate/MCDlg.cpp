#include "stdafx.h"
#include "MCDlg.h"

#include "../MineGE/HelperClass.h"
#include "../MineGE/DataLink.h"

class MCDataLink : public DataLink
{
protected:
    virtual void regDatas()
    {
        linkDoubleData( _T( "工作面长度" ), &length );
        linkDoubleData( _T( "最大控顶距" ), &max_kdj );
        linkDoubleData( _T( "最小控顶距" ), &min_kdj );
        linkDoubleData( _T( "实际采高" ), &height );
        linkDoubleData( _T( "适宜风速" ), &reasonalVelocity );
        linkDoubleData( _T( "采高调整系数" ), &k_height );
        linkDoubleData( _T( "长度调整系数" ), &k_length );
        linkDoubleData( _T( "温度调整系数" ), &k_temperature );
    }

public:
    double length;                // 工作面长度
    double max_kdj, min_kdj;	  // 最大控顶距和最小控顶距
    double height;                // 实际采高
    double reasonalVelocity;      // 适宜风速
    double k_height;              // 采高调整系数
    double k_length;              // 长度调整系数
    double k_temperature;         // 温度调整系数
};

IMPLEMENT_DYNAMIC( MCDlg, CDialog )

MCDlg::MCDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( MCDlg::IDD, pParent )
    , length( 0 )
{

}

MCDlg::~MCDlg()
{
}

void MCDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Text( pDX, IDC_MC_LENGTH_EDIT, length );
    DDX_Text( pDX, IDC_MC_MIN_KDJ_EDIT, min_kdj );
    DDX_Text( pDX, IDC_MC_MAX_KDJ_EDIT, max_kdj );
    DDX_Text( pDX, IDC_MC_HEIGHT_EDIT, height );
    DDX_Text( pDX, IDC_MC_VELOCITY_EDIT, reasonalVelocity );
    DDX_Text( pDX, IDC_MC_k_HEIGHT_EDIT, k_height );
    DDX_Text( pDX, IDC_MC_K_LENGTH_EDIT, k_length );
    DDX_Text( pDX, IDC_MC_K_TEMP_EDIT, k_temperature );
}


BEGIN_MESSAGE_MAP( MCDlg, CDialog )
    ON_BN_CLICKED( IDOK, &MCDlg::OnBnClickedOk )
END_MESSAGE_MAP()

BOOL MCDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    readPropertyData();

    return TRUE;
}

void MCDlg::OnBnClickedOk()
{
    writePropertyData();

    OnOK();
}

void MCDlg::readPropertyData()
{
    MCDataLink dl;
    dl.setDataSource( m_objId );
    dl.updateData( false );

    length = dl.length;
    max_kdj = dl.max_kdj;
    min_kdj = dl.min_kdj;
    height = dl.height;
    reasonalVelocity = dl.reasonalVelocity;
    k_height = dl.k_height;
    k_length = dl.k_length;
    k_temperature = dl.k_temperature;

    UpdateData( FALSE );
}

void MCDlg::writePropertyData()
{
    UpdateData( TRUE );

    MCDataLink dl;
    dl.length = length;
    dl.max_kdj = max_kdj;
    dl.min_kdj = min_kdj;
    dl.height = height;
    dl.reasonalVelocity = reasonalVelocity;
    dl.k_height = k_height;
    dl.k_length = k_length;
    dl.k_temperature = k_temperature;

    dl.setDataSource( m_objId );
    dl.updateData( true );
}