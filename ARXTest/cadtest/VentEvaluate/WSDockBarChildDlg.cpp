#include "stdafx.h"
#include "WSDockBarChildDlg.h"

#include "../MineGE/ConstData.h"
#include "../MineGE/HelperClass.h"
#include "../ArxHelper/HelperClass.h"

#include "AirCompositionDlg.h"
#include "AFCDlg.h"

// 读取矿井的瓦斯等级数据
static int ReadGasLevel()
{
    AcDbObjectId objId;
    if( !SingleDataObjectHelper::GetObjectId( _T( "矿井信息" ), objId ) ) return GL_LOW;

    CString value;
    if( !DataHelper::GetPropertyData( objId, _T( "瓦斯等级" ), value ) ) return GL_LOW;

    // 这里假定数据格式总是正确的({0,1,2}--GAS_LEVEL)
    return _ttoi( value ) % 3;
}

IMPLEMENT_DYNAMIC( WSDockBarChildDlg, PropertyData_DockBarChildDlg )

WSDockBarChildDlg::WSDockBarChildDlg( CWnd* pParent, HINSTANCE hInstance )
    : PropertyData_DockBarChildDlg( WSDockBarChildDlg::IDD, pParent, hInstance )
    , m_cm( 0 )
    , m_hasDangerous( FALSE )
    , m_startZ( _T( "" ) )
    , m_endZ( _T( "" ) )

    , m_inHighGas( FALSE )
{

}

WSDockBarChildDlg::~WSDockBarChildDlg()
{
}

void WSDockBarChildDlg::DoDataExchange( CDataExchange* pDX )
{
    PropertyData_DockBarChildDlg::DoDataExchange( pDX );
    DDX_Text( pDX, IDC_WS_NAME_EDIT, m_name );
    DDX_Text( pDX, IDC_WS_AREA_EDIT, m_area );
    DDX_Text( pDX, IDC_WS_VELOCITY_EDIT, m_velocity );
    DDX_Text( pDX, IDC_WS_Q_EDIT, m_q );
    DDX_Text( pDX, IDC_WS_TEMP_EDIT, m_temp );

    DDX_Radio( pDX, IDC_WS_TEMP_MEASURE_RADIO1, m_cm );
    DDX_Text( pDX, IDC_START_ZLEVEL, m_startZ );
    DDX_Text( pDX, IDC_END_ZLEVEL, m_endZ );
    DDX_Check( pDX, IDC_GAS_DANGEROUS, m_hasDangerous );
    DDX_Check( pDX, IDC_IN_HIGH_GAS, m_inHighGas );
}

BEGIN_MESSAGE_MAP( WSDockBarChildDlg, PropertyData_DockBarChildDlg )
    ON_BN_CLICKED( IDC_WS_AIR_COMPOSITION_BTN, &WSDockBarChildDlg::OnBnClickedAirCompositionBtn )
    ON_BN_CLICKED( IDC_WS_AFC_BTN, &WSDockBarChildDlg::OnBnClickedAfcBtn )
END_MESSAGE_MAP()

BOOL WSDockBarChildDlg::OnInitDialog()
{
    PropertyData_DockBarChildDlg::OnInitDialog();

    return TRUE;
}

void WSDockBarChildDlg::readControlMeasure()
{
    CString v1;
    DataHelper::GetPropertyData( m_objId, _T( "超温控制措施" ), v1 );
    int i1 = 0;
    ArxUtilHelper::StringToInt( v1, i1 );
    m_cm = i1 % 3; // 0-无,1-控制措施,2-停止作业
}

void WSDockBarChildDlg::readSEZLevelData()
{
    CString v1;
    DataHelper::GetPropertyData( m_objId, _T( "始点标高" ), v1 );
    double z1 = 0;
    ArxUtilHelper::StringToDouble( v1, z1 );
    m_startZ.Format( _T( "%.3f" ), z1 );

    CString v2;
    DataHelper::GetPropertyData( m_objId, _T( "末点标高" ), v2 );
    double z2 = 0;
    ArxUtilHelper::StringToDouble( v2, z2 );
    m_endZ.Format( _T( "%.3f" ), z2 );

    CString v3;
    DataHelper::GetPropertyData( m_objId, _T( "是否有突出危险" ), v3 );
    int i3 = 0;
    ArxUtilHelper::StringToInt( v3, i3 );
    i3 = i3 % 2;
    m_hasDangerous = i3;
}

void WSDockBarChildDlg::writeControlMeasure()
{
    CString v1;
    v1.Format( _T( "%d" ), m_cm );
    DataHelper::SetPropertyData( m_objId, _T( "超温控制措施" ), v1 );
}

void WSDockBarChildDlg::writeSEZLevelData()
{
    DataHelper::SetPropertyData( m_objId, _T( "始点标高" ), m_startZ );
    DataHelper::SetPropertyData( m_objId, _T( "末点标高" ), m_endZ );

    CString v1;
    v1.Format( _T( "%d" ), m_hasDangerous );
    DataHelper::SetPropertyData( m_objId, _T( "是否有突出危险" ), v1 );
}

void WSDockBarChildDlg::readInHighGas()
{
    if( !lowGasMine )
    {
        GetDlgItem( IDC_IN_HIGH_GAS )->EnableWindow( FALSE );
        m_inHighGas = FALSE;
    }
    else
    {
        GetDlgItem( IDC_IN_HIGH_GAS )->EnableWindow( TRUE );
        CString v;
        DataHelper::GetPropertyData( m_objId, _T( "是否处于高瓦斯区域" ), v );
        int i = 0;
        ArxUtilHelper::StringToInt( v, i );
        m_inHighGas = ( i != 0 );
    }
}

void WSDockBarChildDlg::writeInHighGas()
{
    if( !lowGasMine )
    {
        m_inHighGas = FALSE;
    }
    else
    {
        CString v;
        v.Format( _T( "%d" ), m_inHighGas );
        DataHelper::SetPropertyData( m_objId, _T( "是否处于高瓦斯区域" ), v );
    }
}

void WSDockBarChildDlg::readPropertyData()
{
    DataHelper::GetPropertyData( m_objId, _T( "名称" ), m_name );
    DataHelper::GetPropertyData( m_objId, _T( "断面面积" ), m_area );
    DataHelper::GetPropertyData( m_objId, _T( "风速" ), m_velocity );
    DataHelper::GetPropertyData( m_objId, _T( "风量" ), m_q );
    DataHelper::GetPropertyData( m_objId, _T( "温度" ), m_temp );

    readControlMeasure();
    readSEZLevelData();

    // 读取矿井瓦斯等级
    lowGasMine = ( ReadGasLevel() == GL_LOW );
    readInHighGas();
}

void WSDockBarChildDlg::writePropertyData()
{
    DataHelper::SetPropertyData( m_objId, _T( "名称" ), m_name );
    DataHelper::SetPropertyData( m_objId, _T( "断面面积" ), m_area );
    DataHelper::SetPropertyData( m_objId, _T( "风速" ), m_velocity );
    DataHelper::SetPropertyData( m_objId, _T( "风量" ), m_q );
    DataHelper::SetPropertyData( m_objId, _T( "温度" ), m_temp );

    writeControlMeasure();
    writeSEZLevelData();
    writeInHighGas();
}

void WSDockBarChildDlg::OnBnClickedAirCompositionBtn()
{
    CAcModuleResourceOverride myResources;

    acDocManager->lockDocument( curDoc() );

    AirCompositionDlg dlg;
    dlg.m_objId = m_objId;
    dlg.DoModal();

    acDocManager->unlockDocument( curDoc() );
}

void WSDockBarChildDlg::OnBnClickedAfcBtn()
{
    UpdateData( TRUE );

    CAcModuleResourceOverride myResources;

    acDocManager->lockDocument( curDoc() );

    AFCDlg dlg;
    dlg.m_objId = m_objId;
    dlg.afc_type = AT_WS;
    dlg.m_lowGasMine = lowGasMine;
    dlg.m_inHighGas = ( m_inHighGas != 0 );
    dlg.DoModal();

    acDocManager->unlockDocument( curDoc() );
}
