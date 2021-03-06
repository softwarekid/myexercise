#include "stdafx.h"
#include "MineInfoDlg.h"

#include "../MineGE/HelperClass.h"
#include "../ArxHelper/HelperClass.h"

IMPLEMENT_DYNAMIC( MineInfoDlg, CDialog )

MineInfoDlg::MineInfoDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( MineInfoDlg::IDD, pParent )
    , m_designDate( COleDateTime::GetCurrentTime() )
    , m_workDate( COleDateTime::GetCurrentTime() )
    , m_name( _T( "" ) )
    , m_company( _T( "" ) )
    , m_quality( _T( "" ) )
    , m_totalQ( _T( "" ) )
{

}

MineInfoDlg::~MineInfoDlg()
{
}

void MineInfoDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_DateTimeCtrl( pDX, IDC_MINE_DESIGN_DATE, m_designDate );
    DDX_DateTimeCtrl( pDX, IDC_MINE_WORK_DATE, m_workDate );
    DDX_Text( pDX, IDC_MINE_NAME, m_name );
    DDX_Text( pDX, IDC_MINE_COMPANY, m_company );
    DDX_Text( pDX, IDC_MINE_QUALITY, m_quality );
    DDX_Control( pDX, IDC_MINE_GAS_LEVEL, m_gasLevel );
    DDX_Control( pDX, IDC_MINE_VENT, m_vent );
    DDX_Control( pDX, IDC_MINE_INFO_SLIDER1, m_slider1 );
    DDX_Control( pDX, IDC_MINE_INFO_SLIDER2, m_slider2 );
    DDX_Text( pDX, IDC_TOTAL_Q, m_totalQ );
}


BEGIN_MESSAGE_MAP( MineInfoDlg, CDialog )
    ON_BN_CLICKED( IDOK, &MineInfoDlg::OnBnClickedOk )
    ON_WM_HSCROLL()
END_MESSAGE_MAP()


// MineInfoDlg 消息处理程序

static void GetTimeFromData( const AcDbObjectId& objId, const CString& field, COleDateTime& mt )
{
    CString value;
    DataHelper::GetPropertyData( objId, field, value );
    mt.ParseDateTime( value );
}

static void SetTimeToData( const AcDbObjectId& objId, const CString& field, const COleDateTime& mt )
{
    CString value = ( COleVariant )mt;
    DataHelper::SetPropertyData( objId, field, value );
}

BOOL MineInfoDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    readPropertyData();

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void MineInfoDlg::OnBnClickedOk()
{
    writePropertyData();

    OnOK();
}

void MineInfoDlg::readPropertyData()
{
    AcDbObjectId objId;
    SingleDataObjectHelper::GetObjectId( _T( "矿井信息" ), objId );

    DataHelper::GetPropertyData( objId, _T( "名称" ), m_name );
    DataHelper::GetPropertyData( objId, _T( "隶属集团公司" ), m_company );

    GetTimeFromData( objId, _T( "设计日期" ), m_designDate );
    GetTimeFromData( objId, _T( "投产日期" ), m_workDate );

    DataHelper::GetPropertyData( objId, _T( "年产量" ), m_quality );
    DataHelper::GetPropertyData( objId, _T( "总进风量" ), m_totalQ );

    m_gasLevel.AddString( _T( "低瓦斯" ) );
    m_gasLevel.AddString( _T( "高瓦斯" ) );
    m_gasLevel.AddString( _T( "煤与瓦斯突出" ) );
    CString v1;
    DataHelper::GetPropertyData( objId, _T( "瓦斯等级" ), v1 );
    int gl = 0;
    ArxUtilHelper::StringToInt( v1, gl );
    gl = gl % 3; // 范围：0-2
    m_gasLevel.SetCurSel( gl );

    m_vent.AddString( _T( "抽出式" ) );
    m_vent.AddString( _T( "压入式" ) );
    CString v2;
    DataHelper::GetPropertyData( objId, _T( "通风方式" ), v2 );
    int vt = 0;
    ArxUtilHelper::StringToInt( v2, vt );
    vt = vt % 2; // 范围：{0, 1}
    m_vent.SetCurSel( vt );

    m_slider1.SetRange( 115, 190, TRUE ); // 1.15 ~ 1.9, 放大100倍
    m_slider2.SetRange( 115, 130, TRUE ); // 1.15 ~ 1.3, 放大100倍

    CString v3;
    DataHelper::GetPropertyData( objId, _T( "通风系数" ), v3 );
    double c3 = 0;
    ArxUtilHelper::StringToDouble( v3, c3 );
    int pos3 = ( int )( c3 * 100 );
    initSlider( m_slider1, 115, 190, pos3, IDC_MINE_INFO_COEFF1 );

    CString v4;
    DataHelper::GetPropertyData( objId, _T( "通风需风系数" ), v4 );
    double c4 = 0;
    ArxUtilHelper::StringToDouble( v4, c4 );
    int pos4 = ( int )( c4 * 100 );
    initSlider( m_slider2, 115, 130, pos4, IDC_MINE_INFO_COEFF2 );

    UpdateData( FALSE );
}

void MineInfoDlg::writePropertyData()
{
    UpdateData( TRUE );

    AcDbObjectId objId;
    SingleDataObjectHelper::GetObjectId( _T( "矿井信息" ), objId );

    DataHelper::SetPropertyData( objId, _T( "名称" ), m_name );
    DataHelper::SetPropertyData( objId, _T( "隶属集团公司" ), m_company );

    SetTimeToData( objId, _T( "设计日期" ), m_designDate );
    SetTimeToData( objId, _T( "投产日期" ), m_workDate );

    DataHelper::SetPropertyData( objId, _T( "年产量" ), m_quality );
    DataHelper::SetPropertyData( objId, _T( "总进风量" ), m_totalQ );

    CString v1;
    v1.Format( _T( "%d" ), m_gasLevel.GetCurSel() );
    DataHelper::SetPropertyData( objId, _T( "瓦斯等级" ), v1 );

    CString v2;
    v2.Format( _T( "%d" ), m_vent.GetCurSel() );
    DataHelper::SetPropertyData( objId, _T( "通风方式" ), v2 );

    CString v3;
    GetDlgItem( IDC_MINE_INFO_COEFF1 )->GetWindowText( v3 );
    DataHelper::SetPropertyData( objId, _T( "通风系数" ), v3 );

    CString v4;
    GetDlgItem( IDC_MINE_INFO_COEFF2 )->GetWindowText( v4 );
    DataHelper::SetPropertyData( objId, _T( "通风需风系数" ), v4 );
}

void MineInfoDlg::initSlider( CSliderCtrl& sl, int minR, int maxR, int pos, long id )
{
    if( pos < minR || pos > maxR ) pos = minR;
    sl.SetPos( pos );
    updateCoeff( id, pos );
}

void MineInfoDlg::updateCoeff( long id, int v )
{
    CString str;
    str.Format( _T( "%.2f" ), v * 0.01 );
    GetDlgItem( id )->SetWindowText( str );
}

void MineInfoDlg::OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar )
{
    switch( pScrollBar->GetDlgCtrlID() )
    {
    case IDC_MINE_INFO_SLIDER1:
        updateCoeff( IDC_MINE_INFO_COEFF1, m_slider1.GetPos() );
        break;

    case IDC_MINE_INFO_SLIDER2:
        updateCoeff( IDC_MINE_INFO_COEFF2, m_slider2.GetPos() );
        break;
    }

    CDialog::OnHScroll( nSBCode, nPos, pScrollBar );
}
