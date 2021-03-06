#include "stdafx.h"
#include "PowderDlg.h"

#include "../MineGE/HelperClass.h"
#include "../MineGE/DataLink.h"

class PowderDataLink : public DataLink
{
protected:
    virtual void regDatas()
    {
        linkDoubleData( _T( "���" ), &m_volume );
    }

public:
    double m_volume;
};

IMPLEMENT_DYNAMIC( PowderDlg, CDialog )

PowderDlg::PowderDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( PowderDlg::IDD, pParent )
{

}

PowderDlg::~PowderDlg()
{
}

void PowderDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Text( pDX, IDC_POWDER_VOLUM, m_volume );
}

BEGIN_MESSAGE_MAP( PowderDlg, CDialog )
    ON_BN_CLICKED( IDOK, &PowderDlg::OnBnClickedOk )
END_MESSAGE_MAP()

void PowderDlg::readPropertyData()
{
    PowderDataLink dl;
    dl.setDataSource( m_objId );
    dl.updateData( false );

    m_volume = dl.m_volume;

    UpdateData( FALSE );
}

void PowderDlg::writePropertyData()
{
    UpdateData( TRUE );

    PowderDataLink dl;
    dl.setDataSource( m_objId );

    dl.m_volume = m_volume;
    dl.updateData( true );
}

void PowderDlg::OnBnClickedOk()
{
    writePropertyData();
    OnOK();
}

BOOL PowderDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    readPropertyData();

    return TRUE;
}
