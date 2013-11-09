#include "stdafx.h"
#include "DataListDlg.h"

#include "../ArxHelper/HelperClass.h"
#include "../MineGE/HelperClass.h"
#include "../GasGE/GasGeologyGE.h"

struct ItemData
{
    int iItem;            // �ڼ��е�����
    AcDbObjectId objId;   // ����id
};

// ����CtrlList��������
struct SortData
{
    int iSubItem;
    CListCtrl* pList;
};

static int nSortOrder = 1;      // ����ķ�ʽ,1����-1����

int CALLBACK SortFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
    ItemData* pData1 = ( ItemData* )lParam1;
    ItemData* pData2 = ( ItemData* )lParam2;
    SortData* pSD = ( SortData* )lParamSort;

    // ������򵥵İ취�������Ǿ�������һ��������
    // �����ַ������򼴿�

    CString s1 = pSD->pList->GetItemText( pData1->iItem, pSD->iSubItem );
    CString s2 = pSD->pList->GetItemText( pData2->iItem, pSD->iSubItem );

    int nRetVal = 0;

    if( pSD->iSubItem == 0 )
    {
        // ��1��(���)
        int n1 = _ttoi( s1 );
        int n2 = _ttoi( s2 );
        nRetVal = ( ( n1 < n2 ) ? -1 : 1 );
    }
    else
    {
        nRetVal = s1.CompareNoCase( s2 );
    }

    return nRetVal * nSortOrder;
}

static void AdjustItemDataNum( CListCtrl& m_list )
{
    // ��Ϊ������ܵ���iItem��ʵ�ʵ���λ�ò�һ�£���Ҫ����
    int n = m_list.GetItemCount();
    for( int i = 0; i < n; i++ )
    {
        ItemData* pData = ( ItemData* )m_list.GetItemData( i );
        pData->iItem = i;
    }
}

static void GetAllRegTypes( AcStringArray& types )
{
    // �������д�GasGeologyGE�����ķǳ�����
    ArxClassHelper::GetArxClassTypes( _T( "GasGeologyGE" ), types, false );
}

static void GetTypeDxfNames( const AcStringArray& types, AcStringArray& dxfNames )
{
    int len = types.length();
    for( int i = 0; i < len; i++ )
    {
        AcRxClass* pClass = AcRxClass::cast( acrxClassDictionary->at( types[i].kACharPtr() ) );
        if( pClass != 0 )
        {
            dxfNames.append( pClass->dxfName() );
        }
    }
}

static void FillTypeList( AcStringArray& types, CComboBox& m_list )
{
    GetAllRegTypes( types );

    AcStringArray dxfNames;
    GetTypeDxfNames( types, dxfNames );

    assert( types.length() == dxfNames.length() );

    int len = dxfNames.length();
    for( int i = 0; i < len; i++ )
    {
        m_list.AddString( dxfNames[i].kACharPtr() );
    }
}

static int FindItemByObjectId( CListCtrl& m_list, const AcDbObjectId& objId )
{
    int row = CB_ERR;
    int n = m_list.GetItemCount();
    for( int i = 0; i < n; i++ )
    {
        ItemData* pData = ( ItemData* )m_list.GetItemData( i );
        if( pData->objId == objId )
        {
            row = i;
            break;
        }
    }
    return row;
}

static void ClearList( CListCtrl& m_list )
{
    //����ɾ��������
    m_list.DeleteAllItems();
    //�õ�����
    int n = m_list.GetHeaderCtrl()->GetItemCount();
    //ɾ��������
    for( int i = 0; i < n; i++ )
    {
        m_list.DeleteColumn( 0 );
    }
}

static void BuildHeadColumns( CListCtrl& m_list, const AcStringArray fields )
{
    int len = fields.length();

    CRect rect;
    m_list.GetClientRect( &rect );

    int n = len + 3;
    if( n > 10 ) n = 10;
    double dw = rect.Width() / n;

    m_list.InsertColumn( 0, _T( "���" ), LVCFMT_LEFT, dw );
    m_list.InsertColumn( 1, _T( "x����" ), LVCFMT_LEFT, dw );
    m_list.InsertColumn( 2, _T( "y����" ), LVCFMT_LEFT, dw );
    for( int i = 0; i < len; i++ )
    {
        m_list.InsertColumn( i + 3, fields[i].kACharPtr(), LVCFMT_LEFT, dw );
    }
}

static void GetInsertPoints( const AcDbObjectIdArray& objIds, AcGePoint3dArray& pts )
{
    AcTransaction* pTrans = actrTransactionManager->startTransaction();

    for( int i = 0; i < objIds.length(); i++ )
    {
        AcDbObject* pObj;
        pTrans->getObject( pObj, objIds[i], AcDb::kForRead );

        GasGeologyGE* pGE = GasGeologyGE::cast( pObj );
        pts.append( pGE->getInsertPt() );
    }

    actrTransactionManager->endTransaction();
}

static void FillDatas( CListCtrl& m_list, const AcStringArray fields, const AcDbObjectIdArray& objIds )
{
    if( objIds.isEmpty() ) return;

    AcGePoint3dArray pts;
    GetInsertPoints( objIds, pts );

    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        m_list.InsertItem( i, _T( "xx" ) );

        CString num;
        num.Format( _T( "%d" ), i + 1 );
        m_list.SetItemText( i, 0, num );

        ItemData* pData = new ItemData();
        pData->iItem = i;
        pData->objId = objIds[i];
        m_list.SetItemData( i, ( LPARAM )pData ); // ��������

        // д��xֵ
        CString x;
        x.Format( _T( "%.3f" ), pts[i].x );
        m_list.SetItemText( i, 1, x );

        // д��yֵ
        CString y;
        y.Format( _T( "%.3f" ), pts[i].y );
        m_list.SetItemText( i, 2, y );

        int n = fields.length();
        for( int j = 0; j < n; j++ )
        {
            //m_list.SetItemData(i, i);
            CString value;
            DataHelper::GetPropertyData( objIds[i], fields[j].kACharPtr(), value );
            m_list.SetItemText( i, j + 3, value );
        }
    }
}

static void ClearItemData( CListCtrl& m_list )
{
    int n = m_list.GetItemCount();
    for( int i = 0; i < n; i++ )
    {
        ItemData* pData = ( ItemData* )m_list.GetItemData( i );
        delete pData;
    }
}

/*
 * �����ĵ���acedSSSetFirst�Լ�acedSSGetFirst
 * Ҫ��ʹ����������Ӧ�ÿ���ACRX_CMD_USEPICKSET��ACRX_CMD_REDRAWѡ��
 * �����Խ����ʾ��ò�Ʋ�ʹ��Ҳ����??????
 */
static bool SelectEntity( const AcDbObjectId& objId )
{
    //acedSSSetFirst(NULL, NULL);

    if( objId.isNull() ) return false;

    ads_name ename;
    if( Acad::eOk != acdbGetAdsName( ename, objId ) ) return false;;

    ads_name ss;
    if( RTNORM != acedSSAdd( ename, NULL, ss ) ) return false; // ����ѡ��

    acedSSSetFirst( ss, NULL ); // ����ѡ��
    acedSSFree( ss );           // �ͷ�ѡ��

    return true;
}

static void ZoomToEntity( const AcDbObjectId& objId )
{
    // �ڷ�ģ̬�Ի������޷�ʹ��
    //ads_name en;
    //if(Acad::eOk != acdbGetAdsName(en, objId)) return;
    //acedCommand(RTSTR, _T("ZOOM"), RTSTR, _T("O"), RTENAME, en, RTSTR, _T(""), 0);

    // ��ʱʹ��sendStringToExecute������Ŷ�λ����
    CString cmd;
    cmd.Format( _T( "ZOOM O \003" ) ); // ���ո����ѡ�����Ȼ��esc(��ֹ����Ŀո��ظ�ִ������)
    acDocManager->sendStringToExecute( curDoc(), cmd, true, false, false );
}

IMPLEMENT_DYNAMIC( DataListDlg, DockBarChildDlg )

DataListDlg::DataListDlg( CWnd* pParent /*=NULL*/ )
    : DockBarChildDlg( DataListDlg::IDD, pParent )
{
}

DataListDlg::~DataListDlg()
{
}

void DataListDlg::DoDataExchange( CDataExchange* pDX )
{
    DockBarChildDlg::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_TYPE_COMBO, m_typeList );
    DDX_Control( pDX, IDC_DATA_LIST, m_geList );
}


BEGIN_MESSAGE_MAP( DataListDlg, DockBarChildDlg )
    ON_BN_CLICKED( IDC_UPDATE_BTN, &DataListDlg::OnBnClickedUpdateBtn )
    ON_BN_CLICKED( IDC_HILIGHT_SELECT_BTN, &DataListDlg::OnBnClickedHilightSelectBtn )
    ON_NOTIFY( LVN_ITEMCHANGED, IDC_DATA_LIST, &DataListDlg::OnLvnItemchangedDataList )
    ON_NOTIFY( LVN_COLUMNCLICK, IDC_DATA_LIST, &DataListDlg::OnLvnColumnclickGeList )
    ON_BN_CLICKED( IDC_SELECT_BTN, &DataListDlg::OnBnClickedSelectBtn )
END_MESSAGE_MAP()

// DataListDlg ��Ϣ��������

BOOL DataListDlg::OnInitDialog()
{
    DockBarChildDlg::OnInitDialog();

    m_geList.SetExtendedStyle( m_geList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

    m_types.removeAll();
    m_typeList.ResetContent();

    FillTypeList( m_types, m_typeList );

    return TRUE;
}

void DataListDlg::OnClosing()
{
    //m_typeList.ResetContent();
    ClearItemData( m_geList );
    ClearList( m_geList );
}

void DataListDlg::OnBnClickedUpdateBtn()
{
    // ɾ�����е�����(item data)
    ClearItemData( m_geList );

    // �������
    ClearList( m_geList );

    // ��ȡ����
    CString type = m_types[m_typeList.GetCurSel()].kACharPtr();

    acDocManager->lockDocument( curDoc() );

    // ��ȡ���е��ֶ�
    AcStringArray fields;
    FieldHelper::GetAllFields( type, fields );

    // ����Column
    BuildHeadColumns( m_geList, fields );

    // ������������=type��ͼԪ
    AcDbObjectIdArray objIds;
    DrawHelper::FindMineGEs( type, objIds );

    //CString msg;
    //msg.Format(_T("�ֶθ���:%d\tcolumn:%d"), fields.length(),  m_geList.GetHeaderCtrl()->GetItemCount());
    //MessageBox(msg);

    m_initFinished = false;

    // �������
    FillDatas( m_geList, fields, objIds );

    // ���ݳ�ʼ�����
    m_initFinished = true;

    acDocManager->unlockDocument( curDoc() );
}

void DataListDlg::OnBnClickedHilightSelectBtn()
{
    int row = LB_ERR;
    for( int i = 0; i < m_geList.GetItemCount(); i++ )
    {
        if( LVIS_SELECTED == m_geList.GetItemState( i, LVIS_SELECTED ) )
        {
            row = i;
            break;
        }
    }
    if( row == LB_ERR )
    {
        MessageBox( _T( "��ѡ��һ������" ) );
    }
    else
    {
        ItemData* pData = ( ItemData* )m_geList.GetItemData( row );

        acDocManager->lockDocument( curDoc() );

        SelectEntity( pData->objId );

        acDocManager->unlockDocument( curDoc() );

        acedGetAcadFrame()->SetFocus();
    }
}

void DataListDlg::OnLvnItemchangedDataList( NMHDR* pNMHDR, LRESULT* pResult )
{
    if( !m_initFinished ) return;

    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>( pNMHDR );

    int row = pNMLV->iItem;
    if( row != LB_ERR )
    {
        ItemData* pData = ( ItemData* )m_geList.GetItemData( row );
        //acutPrintf(_T("\nid:%d"), pData->objId);

        acDocManager->lockDocument( curDoc() );

        if( SelectEntity( pData->objId ) )
        {
            ZoomToEntity( pData->objId );
        }

        acDocManager->unlockDocument( curDoc() );
    }

    *pResult = 0;
}

void DataListDlg::OnLvnColumnclickGeList( NMHDR* pNMHDR, LRESULT* pResult )
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>( pNMHDR );

    static int ncurSortCol = -1;      // ���浱ǰ��������

    /*
     * һ��ʼ��ʾΪ-1����ʾ��δ���κ�������
     * ����ǰ��������������ͬ��������������������ʽ��
     * ��������뵱ǰ�����в�ͬ���У���ı������򣬲�������ʽ��Ϊ����
     */
    if( ncurSortCol == pNMLV->iSubItem )
    {
        nSortOrder *= -1;
    }
    else
    {
        ncurSortCol = pNMLV->iSubItem;
        nSortOrder = 1;
    }
    AdjustItemDataNum( m_geList );

    SortData sd = {pNMLV->iSubItem, &m_geList};
    m_geList.SortItems( SortFunc, ( LPARAM )&sd ); // ����

    *pResult = 0;
}

void DataListDlg::OnBnClickedSelectBtn()
{
    int pos = m_typeList.GetCurSel();
    if( pos == LB_ERR ) return;

    CString name;
    m_typeList.GetLBText( pos, name );

    GetDlgItem( IDC_SELECT_BTN )->EnableWindow( FALSE );

    CString msg;
    msg.Format( _T( "��ѡ��һ��%s: " ), name );
    AcDbObjectId objId = ArxUtilHelper::SelectObject( msg );

    if( objId.isNull() || !ArxUtilHelper::IsEqualType( m_types[pos].kACharPtr(), objId ) )
    {
        MessageBox( _T( "ʰȡʧ��" ) );
    }
    else
    {
        int pos = FindItemByObjectId( m_geList, objId );
        if( pos != CB_ERR )
        {
            m_geList.EnsureVisible( pos, FALSE );
            m_geList.SetItemState( pos, LVIS_SELECTED, LVIS_SELECTED );
        }
    }
    GetDlgItem( IDC_SELECT_BTN )->EnableWindow( TRUE );
}