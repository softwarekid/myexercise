#include "stdafx.h"
#include "AirSetDlg.h"

#include "../MineGE/HelperClass.h"

IMPLEMENT_DYNAMIC( AirSetDlg, CDialog )

AirSetDlg::AirSetDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( AirSetDlg::IDD, pParent )
{

}

AirSetDlg::~AirSetDlg()
{
}

void AirSetDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_AIR_LIST, m_list );
}


BEGIN_MESSAGE_MAP( AirSetDlg, CDialog )
    ON_NOTIFY( LVN_ITEMCHANGED, IDC_AIR_LIST, &AirSetDlg::OnLvnItemchangedAirList )
END_MESSAGE_MAP()


// AirSetDlg 消息处理程序

BOOL AirSetDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // 创建, 定位, 显示CMFCPropertyGridCtrl
    CRect rect;
    GetDlgItem( IDC_AIR_POS )->GetWindowRect( &rect );
    ScreenToClient( &rect );
    m_propertyDataList.Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rect, this, ( UINT ) - 1 );

    m_propertyDataList.EnableWindow( FALSE );
    m_propertyDataList.ShowWindow( SW_HIDE );

    m_lastPos = -1;

    initTunnelList();

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void AirSetDlg::readPropertData( const AcDbObjectId& objId )
{
    AcDbObjectId dObjId;
    bool ret = DataHelper::GetDataObject( objId, dObjId );
    if( ret )
    {
        ret = PropertyDataUpdater::ReadPartialDataFromGE( &m_propertyDataList, dObjId, m_fields );
    }

    if( !ret )
    {
        m_propertyDataList.EnableWindow( FALSE );
        m_propertyDataList.ShowWindow( SW_HIDE );
    }
}

void AirSetDlg::writePropertData( const AcDbObjectId& objId )
{
    AcDbObjectId dObjId;
    bool ret = DataHelper::GetDataObject( objId, dObjId );
    PropertyDataUpdater::WriteDataToGE( &m_propertyDataList, dObjId );
}

void AirSetDlg::initFields()
{
    m_fields.removeAll();
    m_fields.append( _T( "氧气浓度" ) );
    m_fields.append( _T( "二氧化碳浓度" ) );
    m_fields.append( _T( "一氧化碳浓度" ) );
    m_fields.append( _T( "氧化氮浓度" ) );
    m_fields.append( _T( "二氧化硫浓度" ) );
    m_fields.append( _T( "硫化氢浓度" ) );
    m_fields.append( _T( "氨浓度" ) );
}

void AirSetDlg::initTunnelList()
{
    //m_lastPos = -1;

    DrawHelper::FindMineGEs( _T( "WorkSurface" ), m_objIds );
    DrawHelper::FindMineGEs( _T( "TTunnel" ), m_objIds );

    m_list.SetExtendedStyle( m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
    CRect rect;
    m_list.GetClientRect( &rect );
    double c = 0.3;
    m_list.InsertColumn( 0, _T( "序号" ), LVCFMT_LEFT, rect.Width()*c );
    m_list.InsertColumn( 1, _T( "名称" ), LVCFMT_LEFT, rect.Width() * ( 1 - c ) );

    int len = m_objIds.length();
    for ( int i = 0; i < len; i++ )
    {
        // 序号
        CString num;
        num.Format( _T( "%d" ), i + 1 );
        m_list.InsertItem( i, num );
        //m_list.SetItemData(i, i);

        m_list.SetItemText( i, 0, num );

        // 井巷名称
        CString name;
        DataHelper::GetPropertyData( m_objIds[i], _T( "名称" ), name );
        m_list.SetItemText( i, 1, name );
    }
    //if(len>0) m_list.SetHotItem(0);
}

void AirSetDlg::OnLvnItemchangedAirList( NMHDR* pNMHDR, LRESULT* pResult )
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>( pNMHDR );

    if( pNMLV->iItem != -1 )
    {
        // 第几个图元
        int row = pNMLV->iItem;

        if( m_lastPos != -1 )
        {
            writePropertData( m_objIds[m_lastPos] );
        }
        m_lastPos = row;

        ArxUtilHelper::ZoomToEntity( m_objIds[row] );
        /*	CString msg;
        	msg.Format(_T("选中第%d个"), row);
        	MessageBox(msg);*/

        m_propertyDataList.EnableWindow( TRUE );
        m_propertyDataList.ShowWindow( SW_SHOW );

        readPropertData( m_objIds[row] );
        //m_propertyDataList.EnableHeaderCtrl(FALSE); // 不显示标题
    }
    else
    {
        m_propertyDataList.EnableWindow( FALSE );
        m_propertyDataList.ShowWindow( SW_HIDE );
    }

    *pResult = 0;
}
