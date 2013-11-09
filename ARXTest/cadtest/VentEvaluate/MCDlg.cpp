#include "stdafx.h"
#include "MCDlg.h"

#include "../MineGE/HelperClass.h"
#include "../MineGE/DataLink.h"

class MCDataLink : public DataLink
{
protected:
    virtual void regDatas()
    {
        linkDoubleData( _T( "�����泤��" ), &length );
        linkDoubleData( _T( "���ض���" ), &max_kdj );
        linkDoubleData( _T( "��С�ض���" ), &min_kdj );
        linkDoubleData( _T( "ʵ�ʲɸ�" ), &height );
        linkDoubleData( _T( "���˷���" ), &reasonalVelocity );
        linkDoubleData( _T( "�ɸߵ���ϵ��" ), &k_height );
        linkDoubleData( _T( "���ȵ���ϵ��" ), &k_length );
        linkDoubleData( _T( "�¶ȵ���ϵ��" ), &k_temperature );
    }

public:
    double length;                // �����泤��
    double max_kdj, min_kdj;	  // ���ض������С�ض���
    double height;                // ʵ�ʲɸ�
    double reasonalVelocity;      // ���˷���
    double k_height;              // �ɸߵ���ϵ��
    double k_length;              // ���ȵ���ϵ��
    double k_temperature;         // �¶ȵ���ϵ��
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