#include "stdafx.h"
#include "AirCompositionDlg.h"

#include "../MineGE/HelperClass.h"
#include "../MineGE/DataLink.h"

// 采掘工作面模型--煤安第100条
class TWSDataLink : public DataLink
{
protected:
    virtual void regDatas()
    {
        linkDoubleData( _T( "氧气浓度" ), &O2 );
        linkDoubleData( _T( "二氧化碳浓度" ), &CO2 );
        linkDoubleData( _T( "一氧化碳浓度" ), &CO );
        linkDoubleData( _T( "氧化氮浓度" ), &NO );
        linkDoubleData( _T( "二氧化硫浓度" ), &SO2 );
        linkDoubleData( _T( "硫化氢浓度" ), &H2S );
        linkDoubleData( _T( "氨浓度" ), &NH3 );
    }

    // 为了简单起见，所有的数据字段都为public类型，便于访问
public:
    double O2;         // 氧气浓度
    double CO2;        // 二氧化碳浓度
    double CO;         // 一氧化碳浓度
    double NO;         // 氧化氮浓度（在煤安的第100条中有涉及到）
    double SO2;        // 二氧化硫浓度（同上）
    double H2S;        // 硫化氢浓度（同上）
    double NH3;        // 氨浓度（同上）
};

IMPLEMENT_DYNAMIC( AirCompositionDlg, CDialog )

AirCompositionDlg::AirCompositionDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( AirCompositionDlg::IDD, pParent )
    , O2( 0 )
    , CO2( 0 )
    , CO( 0 )
    , NO( 0 )
    , SO2( 0 )
    , H2S( 0 )
    , NH3( 0 )
{

}

AirCompositionDlg::~AirCompositionDlg()
{
}

void AirCompositionDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Text( pDX, IDC_AIR_O2_EDIT, O2 );
    DDV_MinMaxDouble( pDX, O2, 0, 100 );
    DDX_Text( pDX, IDC_AIR_CO2_EDIT, CO2 );
    DDV_MinMaxDouble( pDX, CO2, 0, 100 );
    DDX_Text( pDX, IDC_AIR_CO_EDIT, CO );
    DDV_MinMaxDouble( pDX, CO, 0, 100 );
    DDX_Text( pDX, IDC_AIR_NO_EDIT, NO );
    DDV_MinMaxDouble( pDX, NO, 0, 100 );
    DDX_Text( pDX, IDC_AIR_SO2_EDIT, SO2 );
    DDV_MinMaxDouble( pDX, SO2, 0, 100 );
    DDX_Text( pDX, IDC_AIR_H2S_EDIT, H2S );
    DDV_MinMaxDouble( pDX, H2S, 0, 100 );
    DDX_Text( pDX, IDC_AIR_NH3_EDIT, NH3 );
    DDV_MinMaxDouble( pDX, NH3, 0, 100 );
}


BEGIN_MESSAGE_MAP( AirCompositionDlg, CDialog )
    ON_BN_CLICKED( IDOK, &AirCompositionDlg::OnBnClickedOk )
END_MESSAGE_MAP()


BOOL AirCompositionDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    readPropertyData();

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void AirCompositionDlg::OnBnClickedOk()
{
    writePropertyData();

    OnOK();
}

void AirCompositionDlg::readPropertyData()
{
    TWSDataLink dl;
    dl.setDataSource( m_objId );
    dl.updateData( false );

    O2 = dl.O2;
    CO2 = dl.CO2;
    CO = dl.CO;
    NO = dl.NO;
    SO2 = dl.SO2;
    H2S = dl.H2S;
    NH3 = dl.NH3;

    UpdateData( FALSE );
}

void AirCompositionDlg::writePropertyData()
{
    UpdateData( TRUE );

    TWSDataLink dl;
    dl.O2 = O2;
    dl.CO2 = CO2;
    dl.CO = CO;
    dl.NO = NO;
    dl.SO2 = SO2;
    dl.H2S = H2S;
    dl.NH3 = NH3;

    dl.setDataSource( m_objId );
    dl.updateData( true );
}