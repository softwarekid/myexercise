#include "stdafx.h"
#include "AFCDlg.h"

#include "WS_AFCDlg.h"
#include "TWS_AFCDlg.h"
#include "OT_AFCDlg.h"
#include "DHC_AFCDlg.h"

#include "../MineGE/HelperClass.h"
#include "../MineGE/DataLink.h"

class AFCDataLink : public DataLink
{
protected:
    virtual void regDatas()
    {
        linkDoubleData( _T( "������" ), &userAllocateQ );
    }

public:
    double userAllocateQ;
};

IMPLEMENT_DYNAMIC( AFCDlg, CDialog )

AFCDlg::AFCDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( AFCDlg::IDD, pParent )
    , userAllocateQ( 0 )
{

}

AFCDlg::~AFCDlg()
{
}

void AFCDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Text( pDX, IDC_AFC_ALLOCATE_Q_EDIT, userAllocateQ );
    DDV_MinMaxDouble( pDX, userAllocateQ, 0, DBL_MAX );
}


BEGIN_MESSAGE_MAP( AFCDlg, CDialog )
    ON_BN_CLICKED( IDC_AFC_BTN, &AFCDlg::OnBnClickedAfcBtn )
END_MESSAGE_MAP()

BOOL AFCDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    readPropertyData();

    return TRUE;
}

void AFCDlg::readPropertyData()
{
    AFCDataLink dl;
    dl.setDataSource( m_objId );
    dl.updateData( false );

    userAllocateQ = dl.userAllocateQ;

    UpdateData( FALSE );
}

void AFCDlg::writePropertyData()
{
    UpdateData( TRUE );

    AFCDataLink dl;
    dl.userAllocateQ = userAllocateQ;

    dl.setDataSource( m_objId );
    dl.updateData( true );
}

void AFCDlg::OnBnClickedAfcBtn()
{
    CAcModuleResourceOverride myResources;

    if( afc_type == AT_WS )
    {
        WS_AFCDlg dlg;
        dlg.m_objId = m_objId;
        dlg.m_lowGasMine = m_lowGasMine;
        dlg.m_inHighGas = m_inHighGas;
        dlg.DoModal();
    }
    else if( afc_type == AT_TWS )
    {
        TWS_AFCDlg dlg;
        dlg.m_objId = m_objId;
        dlg.DoModal();
    }
    else if( afc_type == AT_OT )
    {
        OT_AFCDlg dlg;
        dlg.m_objId = m_objId;
        dlg.m_isDiesel = m_isDiesel;
        dlg.DoModal();
    }
    else if( afc_type = AT_DHC )
    {
        DHC_AFCDlg dlg;
        dlg.m_objId = m_objId;
        dlg.DoModal();
    }
}
void AFCDlg::OnCancel()
{
    if( UpdateData( TRUE ) )
    {
        writePropertyData();
        CDialog::OnCancel();
    }
}
