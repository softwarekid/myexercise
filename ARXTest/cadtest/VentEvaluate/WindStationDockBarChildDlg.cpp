#include "stdafx.h"
#include "WindStationDockBarChildDlg.h"

#include "../MineGE/HelperClass.h"
#include "../ArxHelper/HelperClass.h"
#include "../VentNetworkAlgo/VNAlgoHelper.h"

/* ȫ�ֺ���(ʵ����Tool.cpp��) */
extern void FindUseAirPlaces( AcDbObjectIdArray& objIds, int& p1, int& p2, int& p3 );

IMPLEMENT_DYNAMIC( WindStationDockBarChildDlg, CAcUiDialog )

WindStationDockBarChildDlg::WindStationDockBarChildDlg( CWnd* pParent, HINSTANCE hInstance )
    : CAcUiDialog( WindStationDockBarChildDlg::IDD, pParent, hInstance ),
      m_hasStation( FALSE )
      , m_name( _T( "" ) )
      , m_area( _T( "" ) )
      , m_v( _T( "" ) )
      , m_q( _T( "" ) )
{

}

WindStationDockBarChildDlg::~WindStationDockBarChildDlg()
{
    if( curDoc() != 0 )
    {
        updateLastSelect();
        clearAll();
    }
}

void WindStationDockBarChildDlg::DoDataExchange( CDataExchange* pDX )
{
    CAcUiDialog::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_WIND_STATION_LIST, m_list );
    DDX_Check( pDX, IDC_WIND_STATION_CHECK1, m_hasStation );
    DDX_Text( pDX, IDC_STATION_NAME, m_name );
    DDX_Text( pDX, IDC_STATION_AREA, m_area );
    DDX_Text( pDX, IDC_STATION_VELOCITY, m_v );
    DDX_Text( pDX, IDC_STATION_Q, m_q );
}


BEGIN_MESSAGE_MAP( WindStationDockBarChildDlg, CAcUiDialog )
    ON_MESSAGE( WM_ACAD_KEEPFOCUS, OnAcadKeepFocus )  // Needed for modeless dialog.
    ON_NOTIFY( LVN_ITEMCHANGED, IDC_WIND_STATION_LIST, &WindStationDockBarChildDlg::OnLvnItemchangedAirList )
    ON_BN_CLICKED( IDC_WIND_STATION_UPDATE_BTN, &WindStationDockBarChildDlg::OnBnClickedAirUpdateBtn )
    ON_BN_CLICKED( IDC_WIND_STATION_SEL_BTN, &WindStationDockBarChildDlg::OnBnClickedAirSelBtn )
    ON_BN_CLICKED( IDC_STATION_READ_BTN, &WindStationDockBarChildDlg::OnBnClickedStationReadBtn )
    ON_BN_CLICKED( IDC_STATION_ZOOM_BTN, &WindStationDockBarChildDlg::OnBnClickedStationZoomBtn )
END_MESSAGE_MAP()


BOOL WindStationDockBarChildDlg::OnInitDialog()
{
    CAcUiDialog::OnInitDialog();

    // ����listctrl�ı���ͷ
    m_list.SetExtendedStyle( m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
    CRect rect;
    m_list.GetClientRect( &rect );
    double c = 0.3;
    m_list.InsertColumn( 0, _T( "���" ), LVCFMT_LEFT, rect.Width()*c );
    m_list.InsertColumn( 1, _T( "�������" ), LVCFMT_LEFT, rect.Width() * ( 1 - c ) );

    // ���
    clearAll();

    initTunnelList();

    //MessageBox(_T("��ʼ�����"));

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

void WindStationDockBarChildDlg::readPropertyData( const AcDbObjectId& objId )
{
    int pos = m_objIds.find( objId );
    if( pos < 0 ) return;

    AcDbObjectIdArray stations;
    DrawHelper::GetTagGEById2( m_hosts[pos], _T( "WindStation" ), stations );
    if( stations.isEmpty() )
    {
        m_hasStation = FALSE;
    }
    else
    {
        m_hasStation = TRUE;

        DataHelper::GetPropertyData( stations[0], _T( "����" ), m_name );
        DataHelper::GetPropertyData( stations[0], _T( "���Զ������" ), m_area );
        DataHelper::GetPropertyData( stations[0], _T( "���Է���" ), m_v );
        DataHelper::GetPropertyData( stations[0], _T( "���Է���" ), m_q );
    }
    UpdateData( FALSE );
}

void WindStationDockBarChildDlg::writePropertyData( const AcDbObjectId& objId )
{
    UpdateData( TRUE );

    int pos = m_objIds.find( objId );
    if( pos < 0 ) return;

    AcDbObjectIdArray stations;
    DrawHelper::GetTagGEById2( m_hosts[pos], _T( "WindStation" ), stations );
    if( stations.isEmpty() ) return;

    if( m_hasStation )
    {
        CString v1;
        v1.Format( _T( "%d" ), 1 ); // Ĭ�϶��ǰ��չ涨���
        DataHelper::SetPropertyData( stations[0], _T( "�Ƿ񰴹涨���" ), v1 );

        DataHelper::SetPropertyData( stations[0], _T( "����" ), m_name );
        DataHelper::SetPropertyData( stations[0], _T( "���Զ������" ), m_area );
        DataHelper::SetPropertyData( stations[0], _T( "���Է���" ), m_v );
        DataHelper::SetPropertyData( stations[0], _T( "���Է���" ), m_q );
    }
}

static void FindUseAirPlacesAndReasons( AcDbObjectIdArray& objIds, AcStringArray& reasons )
{
    int p1, p2, p3;
    FindUseAirPlaces( objIds, p1, p2, p3 );
    for( int i = 0; i < p1; i++ )
    {
        reasons.append( _T( "�زɹ�����" ) );
    }
    for( int i = p1; i < p2; i++ )
    {
        reasons.append( _T( "���������" ) );
    }
    for( int i = p2; i < p3; i++ )
    {
        reasons.append( _T( "����" ) );
    }
    int len = objIds.length();
    for( int i = p3; i < len; i++ )
    {
        reasons.append( _T( "��Ҫ���ء��������ش�����÷����" ) );
    }
}

static void FindWindStationHosts( const AcDbObjectIdArray& objIds, AcDbObjectIdArray& hosts )
{
    FWSHelper fws;
    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        hosts.append( fws.doFWS( objIds[i] ) );
    }
}

void WindStationDockBarChildDlg::initTunnelList()
{
    // ������ǰ�ĵ�
    acDocManager->lockDocument( curDoc() );

    // �����÷�ص��Լ�Ե��
    FindUseAirPlacesAndReasons( m_objIds, m_reasons );
    // �����÷�ص�Ĳ��վ���ڵ��������
    FindWindStationHosts( m_objIds, m_hosts );

    // ��¼��ɫ
    ArxEntityHelper::GetEntitiesColor( m_objIds, m_colors );

    // ������ǰ�ĵ�
    acDocManager->unlockDocument( curDoc() );

    //assert(m_objIds.length() == m_reasons.length());
    int len = m_objIds.length();
    for ( int i = 0; i < len; i++ )
    {
        // ���
        CString num;
        num.Format( _T( "%d" ), i + 1 );
        m_list.InsertItem( i, num );
        //m_list.SetItemData(i, i);

        m_list.SetItemText( i, 0, num );
        m_list.SetItemText( i, 1, m_reasons[i].kACharPtr() );
    }

    // ѡ�е�1��
    if( len > 0 )
    {
        m_list.SetItemState( 0, LVIS_SELECTED, LVIS_SELECTED );
        m_listFinishInitial = true;
    }
}

//- Needed for modeless dialogs to keep focus.
//- Return FALSE to not keep the focus, return TRUE to keep the focus
LRESULT WindStationDockBarChildDlg::OnAcadKeepFocus ( WPARAM wParam, LPARAM lParam )
{
    return ( TRUE ) ;
}

//- As this dialog is a child dialog we need to disable ok and cancel
BOOL WindStationDockBarChildDlg::OnCommand ( WPARAM wParam, LPARAM lParam )
{
    switch ( wParam )
    {
    case IDOK:
    case IDCANCEL:
        return ( FALSE );
    }
    return ( CAcUiDialog::OnCommand ( wParam, lParam ) ) ;
}

void WindStationDockBarChildDlg::OnSize ( UINT nType, int cx, int cy )
{
    CAcUiDialog::OnSize ( nType, cx, cy ) ;
    //- Now update the dialog
    MoveWindow( 0, 0, cx, cy );
}

void WindStationDockBarChildDlg::clearAllData()
{
    m_hasStation = FALSE;

    m_name = _T( "" );
    m_area = _T( "" );
    m_v = _T( "" );
    m_q = _T( "" );
}

void WindStationDockBarChildDlg::clearAll()
{
    // ������ǰ�ĵ�
    acDocManager->lockDocument( curDoc() );
    // �ָ���ɫ
    ArxEntityHelper::SetEntitiesColor2( m_objIds, m_colors );

    acDocManager->unlockDocument( curDoc() );

    m_list.DeleteAllItems();
    m_objIds.removeAll();
    m_colors.removeAll();
    m_lastPos = -1;
    m_listFinishInitial = false;
    m_reasons.removeAll();
    m_hosts.removeAll();

    clearAllData();
}

void WindStationDockBarChildDlg::OnBnClickedAirUpdateBtn()
{
    // �������ѡ���һ��
    updateLastSelect();

    // ���
    clearAll();

    // ��ʼ��list
    initTunnelList();
}

void WindStationDockBarChildDlg::OnClosing()
{
    // �������ѡ���һ��
    updateLastSelect();

    // ���
    clearAll();
}

void WindStationDockBarChildDlg::OnLvnItemchangedAirList( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( !m_listFinishInitial ) return;

    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>( pNMHDR );

    if( pNMLV->iItem != -1 )
    {
        // ������ǰ�ĵ�
        acDocManager->lockDocument( curDoc() );

        // �ڼ���ͼԪ
        int row = pNMLV->iItem;

        if( m_lastPos != -1 )
        {
            // ��������
            writePropertyData( m_objIds[m_lastPos] );

            // �ָ���һ��ѡ��ͼԪ����ɫ
            ArxEntityHelper::SetEntityColor( m_objIds[m_lastPos], m_colors[m_lastPos] );
        }

        clearAllData();
        // ��ȡ����
        readPropertyData( m_objIds[row] );

        // ���Ų��û���ɫ������ʾ
        ArxEntityHelper::ZoomToEntity( m_objIds[row] );
        ArxEntityHelper::SetEntityColor( m_objIds[row], 2 );

        m_lastPos = row;

        acDocManager->unlockDocument( curDoc() );
    }

    *pResult = 0;
}

void WindStationDockBarChildDlg::updateLastSelect()
{
    int row = LB_ERR;
    for( int i = 0; i < m_list.GetItemCount(); i++ )
    {
        if( LVIS_SELECTED == m_list.GetItemState( i, LVIS_SELECTED ) )
        {
            row = i;
            break;
        }
    }

    if( row != LB_ERR )
    {
        //UpdateData(TRUE);

        // ������ǰ�ĵ�
        acDocManager->lockDocument( curDoc() );
        // ��������
        writePropertyData( m_objIds[row] );

        acDocManager->unlockDocument( curDoc() );
    }
}

void WindStationDockBarChildDlg::OnBnClickedAirSelBtn()
{
    // ʰȡ��ť
    //BeginEditorCommand();
    AcDbObjectId objId = ArxUtilHelper::SelectObject( _T( "��ѡ��һ�����: " ) );
    //CompleteEditorCommand();
    if( !objId.isNull() )
    {
        //MessageBox(_T("ʰȡ�ɹ�"));
        int pos = m_objIds.find( objId );
        if( pos != -1 )
        {
            //�������Զ���������,��ѡ��
            m_list.EnsureVisible( pos, FALSE );
            m_list.SetItemState( pos, LVIS_SELECTED, LVIS_SELECTED );

            // ȡ��ѡ��
            //m_list.SetItemState(pos, NULL, NULL);
        }
    }
    else
    {
        MessageBox( _T( "ʰȡʧ��" ) );
    }
}

void WindStationDockBarChildDlg::OnBnClickedStationReadBtn()
{
    int row = LB_ERR;
    for( int i = 0; i < m_list.GetItemCount(); i++ )
    {
        if( LVIS_SELECTED == m_list.GetItemState( i, LVIS_SELECTED ) )
        {
            row = i;
            break;
        }
    }
    if( row == LB_ERR ) return;

    if( !m_hasStation )
    {
        CString msg;
        msg.Format( _T( "�������ò��վ!!!\n����ʾ��\n����������ġ����վ����ť���Ʋ��վ" ) );
        MessageBox( msg );
        return;
    }

    // ��ȡ���վ��������ġ�����������������١���������������
    AcDbObjectId objId = m_objIds[row];

    // ������ǰ�ĵ�
    acDocManager->lockDocument( curDoc() );

    //DataHelper::GetPropertyData(objId, _T("����"), m_name);
    DataHelper::GetPropertyData( objId, _T( "�������" ), m_area );
    DataHelper::GetPropertyData( objId, _T( "����" ), m_v );
    DataHelper::GetPropertyData( objId, _T( "����" ), m_q );

    acDocManager->unlockDocument( curDoc() );

    UpdateData( FALSE );
}

void WindStationDockBarChildDlg::OnBnClickedStationZoomBtn()
{
    UpdateData( TRUE );

    int row = LB_ERR;
    for( int i = 0; i < m_list.GetItemCount(); i++ )
    {
        if( LVIS_SELECTED == m_list.GetItemState( i, LVIS_SELECTED ) )
        {
            row = i;
            break;
        }
    }
    if( row == LB_ERR ) return;

    if( !m_hasStation )
    {
        MessageBox( _T( "�õص�û�����ò��վ!!!" ) );
    }
    else
    {
        ArxEntityHelper::ZoomToEntity( m_hosts[row] );
    }
}