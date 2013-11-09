#include "stdafx.h"
#include "NormalEvalSettingDlg.h"
#include "QuestionNaireDlg.h"

#include "DataBaseFieldNames.h"
#include "TestHelper.h"
#include "IniFile.h"

#include "../EvalBase/EvalService.h"
#include "../DaoHelper/TableCreator.h"
#include "../DaoHelper/DataWriter.h"
using namespace PJ;

#include "../MineGE/DataListHelper.h"
#include "../minege/DataHelper.h"
//#include "../MineGE/ConstData.h"

// �γ�һ���򵥵�"�ֵ�"��
// �ڹ���Ͳ���regulationListʱ
// ����������ֶβ���
const static TCHAR* REGULATION_INFO[] =
{
    ID,
    NAME_FIELD,
    VERSION_FIELD,
    PUBLISHER_FIELD,
    LINK_TABLE_FIELD
};

static int CStrToInt( CString str )
{
    int clause = _tstoi( ( LPCTSTR )str.GetBuffer() );            // ת����int����
    str.ReleaseBuffer();
    return clause;
}

// ��access��: -1��ʾ�棬0��ʾ��
// ��Ҫת����bool����
static int Access2Bool( int value )
{
    return -1 * value;
}

static CString GetPathDir( const CString& path )
{
    // ����1��ʹ��splitpath��makepath����(ϵͳapi��
    /*
    char path_buffer[_MAX_PATH];
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];

    // ���·��
    _splitpath(	m_outputEvalReportPath.GetBuffer(),
    drive, dir, fname, ext);
    m_outputEvalReportPath.ReleaseBuffer();

    // �ϲ��õ�ini�ļ���·��
    _makepath_s(path_buffer, drive, dir, fname, ".ini");
    */

    // ����2��Ҳ����ʹ��cstring�ķ���
    return path.Left( path.ReverseFind( _T( '\\' ) ) );
}

static bool CreateEmptyFile( const CString& path )
{
    CFile myfile;
    CFileException fileException;
    if( !myfile.Open( path, CFile::modeCreate | CFile::modeWrite, &fileException ) ) // �½��ļ�
    {
        // ���������ļ�ʧ��
        /*CString msg;
        msg.Format("���������ļ�ʧ��!\n�ļ�: %s������: %u", path, fileException.m_cause);
        AfxMessageBox(msg);*/
        return false;
    }
    else
    {
        myfile.Close(); // �ر��ļ�
        return true;
    }
}

static void RemoveFile( const CString& filePath )
{
    CFileFind ff;
    if( ff.FindFile( filePath ) )
    {
        CFile::Remove( filePath );
    }
}

static bool IsFileExist( const CString& filePath )
{
    CFileFind ff;
    return ff.FindFile( filePath );
}

// ����EvalBase��Ŀ�͵�ǰ��Ŀ�е�"�����ż���"���ݽṹ��ͬ
// ��Ҫ���м򵥵�ת��
static void ClauseCollectionTransformBy( const ClauseCollection& c1, EvalClauseCollection& c2 )
{
    for( ClauseCollection::const_iterator itr = c1.begin(); itr != c1.end(); itr++ )
    {
        c2.append( *itr );
    }
}

// �������۵�Ԫ���۽�����ܱ�
static void CreateEvalUnitResultTable( CDaoDatabase* pDB, LPCTSTR tableName )
{
    TableCreator* pTableCreator = new TableCreator( pDB );
    pTableCreator->createTable( tableName );            // ������
    pTableCreator->addPrimaryKeyField( ID );            // id����
    pTableCreator->addIntField( EVAL_RESULT_FIELD );    // ���۽��
    pTableCreator->addStringField( UNIT_FIELD );        // ���۵�Ԫ����
    pTableCreator->closeTable();                        // �رձ�
    delete pTableCreator;
}

// ��"���۽�����ܱ�"����ȡ����(clauseNum)��Ӧ�����۽��(EvalResult)
static EvalResult GetEvalResultByClause( CDaoDatabase* pDB, LPCTSTR tableName, int clauseNum )
{
    CDaoRecordset rs( pDB );
    CString sql;
    sql.Format( _T( "select * from %s where %s=%d" ), tableName, CLAUSE_NUM_FIELD, clauseNum );
    rs.Open( dbOpenDynaset, sql, 0 );

    EvalResult er = UNKNOWN_RESULT;
    rs.MoveFirst () ;
    while( !rs.IsEOF() )
    {
        COleVariant v;
        rs.GetFieldValue( EVAL_RESULT_FIELD , v ); // ��ȡ������

        er = ( EvalResult )v.intVal;

        // �Ƶ���һ����¼
        rs.MoveNext () ;
    }
    // �رռ�¼��
    rs.Close ();

    return er;
}

// �ϲ����������۵�Ԫ�����۽��
static void InsertEvalUnitResult( CDaoDatabase* pDB, LPCTSTR tableName, LPCTSTR unit, EvalResult er )
{
    DataWriter* pDataWriter = new DataWriter( pDB );
    pDataWriter->beginWriteData( tableName );
    pDataWriter->writeIntData( EVAL_RESULT_FIELD, er );
    pDataWriter->writeStringData( UNIT_FIELD, unit );
    pDataWriter->endWriteData();
    delete pDataWriter;
}

// ��ȡoffice2003�İ�װĿ¼
static bool GetWordExePathFromRegistry( CString& path )
{
    long lRet = 0;
    CString strRegOffice = _T( "Software\\Microsoft\\Office\\11.0\\Word\\InstallRoot" ); // office03�汾
    HKEY   hResult;
    lRet = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, strRegOffice, 0, KEY_QUERY_VALUE, &hResult );
    if( lRet == ERROR_SUCCESS )
    {
        DWORD      WordType =  REG_SZ;
        DWORD      WordData =  100;
        LPTSTR     KeyByte[1024];
        lRet = ::RegQueryValueEx( hResult, ( _T( "Path" ) ), NULL, &WordType, ( LPBYTE )KeyByte, &WordData );
        if( lRet == ERROR_SUCCESS )
        {
            path = ( LPTSTR )KeyByte;
            path.Append( _T( "word.exe" ) ); // �ϲ��õ�word.exe·��
        }
        RegCloseKey( hResult );
    }
    return ( lRet == ERROR_SUCCESS );
}

// ʹ��ShellExecute���ĵ�
static void OpenWordDocument( const CString& filePath )
{
    //CString filePath = _T("C:\\Users\\anheihb03dlj\\Desktop\\��ú��ȫ��̡�103����������㷽��.doc");
    ShellExecute( NULL, _T( "open" ), filePath, NULL, NULL, SW_SHOWNORMAL );
}

IMPLEMENT_DYNAMIC( NormalEvalSettingDlg, CDialog )

NormalEvalSettingDlg::NormalEvalSettingDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( NormalEvalSettingDlg::IDD, pParent )
    , m_regulationInfoFileName( _T( "" ) )
    , m_outputEvalReportPath( _T( "" ) )
{

}

NormalEvalSettingDlg::~NormalEvalSettingDlg()
{
}

void NormalEvalSettingDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Text( pDX, IDC_DB_PATH_EDIT, m_regulationInfoFileName );
    DDX_Control( pDX, IDC_REGULATION_LIST, m_regulationListCtrl );
    DDX_Control( pDX, IDC_EVAL_UNIT_TREE, m_evalUnitTreeCtrl );
    DDX_Control( pDX, IDC_EVAL_TYPE1_RADIO, m_normalEvalTypeRadio );
    DDX_Text( pDX, IDC_OUTPUT_EDIT, m_outputEvalReportPath );
}


BEGIN_MESSAGE_MAP( NormalEvalSettingDlg, CDialog )
    ON_BN_CLICKED( IDC_DB_PATH_SEL_BUTTON, &NormalEvalSettingDlg::OnBnClickedDbPathSelButton )
    ON_BN_CLICKED( IDC_EVAL_TYPE1_RADIO, &NormalEvalSettingDlg::OnBnClickedEvalType1Radio )
    ON_BN_CLICKED( IDC_EVAL_TYPE2_RADIO, &NormalEvalSettingDlg::OnBnClickedEvalType2Radio )
    ON_NOTIFY( LVN_ITEMCHANGED, IDC_REGULATION_LIST, &NormalEvalSettingDlg::OnLvnItemchangedRegulationList )
    ON_BN_CLICKED( IDC_OUTPUT_BUTTON, &NormalEvalSettingDlg::OnBnClickedOutputButton )
    ON_BN_CLICKED( IDOK, &NormalEvalSettingDlg::OnBnClickedOk )
    //ON_BN_CLICKED(IDC_INIT_BTN, &NormalEvalSettingDlg::OnBnClickedInitBtn)
END_MESSAGE_MAP()


// NormalEvalSettingDlg ��Ϣ��������
BOOL NormalEvalSettingDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    initRegulationListCtrl();
    initEvalUnitTreeCtrl();

    m_normalEvalTypeRadio.SetCheck( BST_CHECKED );
    m_evalUnitTreeCtrl.EnableWindow( FALSE );

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

void NormalEvalSettingDlg::OnBnClickedDbPathSelButton()
{
    CFileDialog  openDialog(
        TRUE,
        _T( "mdb" ),
        NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
        _T( "Access2003���ݿ�(*.mdb)|*.mdb||" ) );

    if( IDOK == openDialog.DoModal() )
    {
        CString cFilePath = openDialog.GetPathName();
        m_regulationInfoFileName = cFilePath.GetBuffer();
        cFilePath.ReleaseBuffer();
        UpdateData( FALSE );        // ��ʾ�ڱ༭����

        readRegulationInfoFromDB(); // ��ȡ���ݿ�
    }
}

void NormalEvalSettingDlg::OnBnClickedEvalType1Radio()
{
    // �������ۣ�����treeCtrl
    m_evalUnitTreeCtrl.DeleteAllItems();
    m_evalUnitTreeCtrl.EnableWindow( FALSE );
}

void NormalEvalSettingDlg::OnBnClickedEvalType2Radio()
{
    // �����۵�Ԫ��������
    m_evalUnitTreeCtrl.EnableWindow( TRUE );
    m_regulationListCtrl.SetItemState( 0, LVIS_SELECTED, 3 );
}

void NormalEvalSettingDlg::OnLvnItemchangedRegulationList( NMHDR* pNMHDR, LRESULT* pResult )
{
    NM_LISTVIEW* pNMListView = ( NM_LISTVIEW* )pNMHDR;
    if( pNMListView->iItem != -1 )
    {
        // ������۵�Ԫ��״�ṹ
        m_evalUnitTreeCtrl.DeleteAllItems();

        int row = pNMListView->iItem;
        CString tablename = m_regulationListCtrl.GetItemText( row, 4 ); // ȡtable��
        if( tablename.GetLength() != 0 ) // ��������Ϊ��
        {
            //AfxMessageBox(tablename);
            if( BST_UNCHECKED == m_normalEvalTypeRadio.GetCheck() ) // ���ݱ��������ݿ��ж�ȡ���۵�Ԫ��������Ϣ
            {
                readEvalUnitFromDB( tablename );
            }
        }
    }
    *pResult = 0;
}

void NormalEvalSettingDlg::OnBnClickedOutputButton()
{
    CFileDialog SaveDialog( FALSE, _T( "*doc" ), NULL, OFN_NOCHANGEDIR | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T( "Word2003�ĵ�(*.doc)|*.doc||" ) );

    if( IDOK == SaveDialog.DoModal() )
    {
        CString cFilePath = SaveDialog.GetPathName();
        m_outputEvalReportPath = cFilePath.GetBuffer();
        cFilePath.ReleaseBuffer();
        UpdateData( FALSE ); // ��ʾ���·��
    }
}

void NormalEvalSettingDlg::OnBnClickedOk()
{
    UpdateData( TRUE );

    if( !checkInput() ) return; // �������ʧ��
    if( !checkOutput() ) return; // ������ʧ��

    // ������m_regulationInfoFileName��ͬһĿ¼�µ���ʱ�ļ�
    CString dir = GetPathDir( m_regulationInfoFileName );
    //cleanTempData(dir);

    if( getEvalMethodType() == 0 )
    {
        doNormalEvalWithoutUnit( dir );
    }
    else
    {
        MessageBox( _T( "δʵ��" ) );
        /*EvalUnitInfoMap euim;
        getSelEvalUnitMap(euim);

        if(euim.empty())
        {
        	MessageBox(_T("������ѡ��һ�����۵�Ԫ��������"));
        }
        else
        {
        	doNormalEvalWithUnit(m_outputEvalReportPath, euim);
        }	*/
    }
    if( generateReport( dir ) )
    {
        // �ڹ����Ϣ���ݿ�����Ŀ¼�´����۱���ģ��
        CString filePath;
        filePath.Format( _T( "%s\\%s.doc" ), dir, EVAL_REPORT_TEMPLATE_FILE_NAME );
        OpenWordDocument( filePath );

        MessageBox( _T( "������ɲ����ɱ���!" ) );

        // �����ɵı���
        OpenWordDocument( m_outputEvalReportPath );
    }
    else
    {
        MessageBox( _T( "�������۱���ʧ��!" ) );
    }

    // �����ʱ�ļ�(�ȳ����������֮����ȡ��ע��)
    //cleanTempData(dir);
    //OnOK();
}

bool NormalEvalSettingDlg::checkInput()
{
    if( m_regulationInfoFileName.GetLength() == 0 )
    {
        MessageBox( _T( "��ѡ������Ϣ���ݿ�!" ) );
        return false;
    }

    if( m_regulationListCtrl.GetItemCount() == 0 )
    {
        MessageBox( _T( "�����Ϣ���ݿ���û���κι����Ϣ!" ) );
        return false;
    }

    CString regulationTable = getSelRegulationTableName();
    if( regulationTable.GetLength() == 0 )
    {
        MessageBox( _T( "ѡ�еĹ�̲������κ�������Ϣ!" ) );
        return false;
    }

    if( getEvalMethodType() == 1 )
    {
        if( m_evalUnitTreeCtrl.GetCount() == 0 )
        {
            MessageBox( _T( "û�й������۵�Ԫ��ϵ���޷�ִ������!" ) );
            return false;
        }
    }
    return true;
}

bool NormalEvalSettingDlg::checkOutput()
{
    if( m_outputEvalReportPath.GetLength() == 0 )
    {
        MessageBox( _T( "��ѡ�񱨸����·��!" ) );
        return false;
    }
    return true;
}

void NormalEvalSettingDlg::initRegulationListCtrl()
{
    m_regulationListCtrl.SetExtendedStyle( m_regulationListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT );
    LVCOLUMN lvcol;
    lvcol.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
    lvcol.fmt = LVCFMT_CENTER;
    lvcol.iSubItem = 1;
    lvcol.cx = 80;

    const int len = sizeof( REGULATION_INFO ) / sizeof( REGULATION_INFO[0] );
    for( int i = 0; i < len; i++ )
    {
        lvcol.pszText = const_cast<LPTSTR>( REGULATION_INFO[i] );
        m_regulationListCtrl.InsertColumn( i, &lvcol );
    }
}

void NormalEvalSettingDlg::readRegulationInfoFromDB()
{
    //this->m_regulationListCtrl.DeleteAllItems();

    // ��ʼ��ȡ���ݣ������뵽listctrl
    const int len = sizeof( REGULATION_INFO ) / sizeof( REGULATION_INFO[0] );
    UINT nIndex = 0 ;

    CDaoDatabase db;
    db.Open( m_regulationInfoFileName );
    CDaoRecordset rs( &db );
    try
    {
        CString sql;
        sql.Format( _T( "select * from %s" ), REGULATION_INFO_MAIN_TABLE_NAME ); // ��ȡregulationInfo��
        rs.Open( dbOpenDynaset, sql, 0 );

        rs.MoveFirst () ;
        while( !rs.IsEOF() )
        {
            // ���б��ؼ����Ӽ�¼
            CString idStr;
            idStr.Format( _T( "%d" ), nIndex + 1 );
            this->m_regulationListCtrl.InsertItem( nIndex, idStr );

            for( int i = 0; i < len ; i++ )
            {
                if( i == 0 )
                {
                    m_regulationListCtrl.SetItemText( nIndex, 0, idStr );
                }
                else
                {
                    COleVariant v;
                    rs.GetFieldValue( REGULATION_INFO[i] , v );
                    this->m_regulationListCtrl.SetItemText( nIndex, i, ( LPCTSTR )v.bstrVal );
                }
            }
            // �Ƶ���һ����¼
            rs.MoveNext () ;
            nIndex++ ;
        }
    }
    catch( CDaoException* e )
    {
        //AfxMessageBox(e->m_pErrorInfo->m_strDescription);
        e->Delete();

        // �쳣����
        m_regulationInfoFileName = _T( "" );
        UpdateData( FALSE ); // ����
    }
    // �رռ�¼��
    if( rs.IsOpen() ) rs.Close();
    db.Close();
}

void NormalEvalSettingDlg::initEvalUnitTreeCtrl()
{
    // ����3̬treectrl��ͼ��
    m_imgState.Create( IDB_BITMAP2, 13, 1, RGB( 255, 255, 255 ) );
    m_imgList.Create( IDB_BITMAP1, 16, 1, RGB( 255, 255, 255 ) );

    m_evalUnitTreeCtrl.SetImageList( &m_imgList, TVSIL_NORMAL );
    m_evalUnitTreeCtrl.SetImageList( &m_imgState, TVSIL_STATE );
}

void NormalEvalSettingDlg::readEvalUnitFromDB( const CString& tableName )
{
    EvalUnitInfoMap euim;
    initEvalUntiHierarchy( tableName, euim );
    if( euim.empty() ) return;

    caclEvalUnitHierarchy( euim );
    buildTree( euim ); // ������
}

void NormalEvalSettingDlg::initEvalUntiHierarchy( const CString& tableName, EvalUnitInfoMap& euim )
{
    CDaoDatabase db;
    db.Open( m_regulationInfoFileName );
    CDaoRecordset rs( &db );
    try
    {
        // �����Ƿ����
        CString sql;
        sql.Format( _T( "select * from %s" ), tableName );
        rs.Open( dbOpenDynaset, sql, 0 );
        rs.MoveFirst () ;

        while( !rs.IsEOF() )
        {
            CString parent_unit, unit;
            int clauseNum;
            COleVariant v;

            rs.GetFieldValue( PARENT_UNIT_FIELD, v );
            parent_unit.Format( _T( "%s" ), v.bstrVal ); // �õ�parent_unit

            rs.GetFieldValue( UNIT_FIELD, v );
            unit.Format( _T( "%s" ), v.bstrVal ); // �õ�unit

            rs.GetFieldValue( CLAUSE_NUM_FIELD, v );
            clauseNum = v.intVal;

            if( ( parent_unit.GetLength() != 0 ) && ( euim.find( parent_unit ) == euim.end() ) )
            {
                EvalUnitInfo unknown_info; // δ֪�ڵ���Ϣ
                unknown_info.parent_unit = _T( "" );
                unknown_info.hierarchy_num = 0;
                euim.insert( EvalUnitInfoMap::value_type( parent_unit, unknown_info ) );
            }

            EvalUnitInfoMap::iterator itr = euim.find( unit );
            if( itr == euim.end() )
            {
                EvalUnitInfo info;
                info.parent_unit = parent_unit;
                info.hierarchy_num = 0;
                info.clauses.push_back( clauseNum );
                euim.insert( EvalUnitInfoMap::value_type( unit, info ) );
            }
            else
            {
                if( itr->second.parent_unit.GetLength() == 0 ) // ��ʾ��δ����
                {
                    itr->second.parent_unit = parent_unit;
                }
                itr->second.clauses.push_back( clauseNum );
            }

            // �Ƶ���һ����¼
            rs.MoveNext () ;
        }
    }
    catch( CDaoException* e )
    {
        //AfxMessageBox(e->m_pErrorInfo->m_strDescription);
        e->Delete();
    }

    // �رռ�¼��
    if( rs.IsOpen() ) rs.Close();
    db.Close();
}

void NormalEvalSettingDlg::caclEvalUnitHierarchy( EvalUnitInfoMap& euim )
{
    while( true )
    {
        bool hasMoreToRevise = false;
        for( EvalUnitInfoMap::iterator itr = euim.begin(); itr != euim.end(); itr++ )
        {
            if( itr->second.parent_unit.GetLength() == 0 ) continue;
            EvalUnitInfo info = itr->second;
            int num = info.hierarchy_num;
            int parent_num = euim[info.parent_unit].hierarchy_num;
            if( parent_num >= num )
            {
                itr->second.hierarchy_num = parent_num + 1;
                hasMoreToRevise = true;
            }
        }
        if( !hasMoreToRevise ) break;
    }
}

void NormalEvalSettingDlg::buildTree( EvalUnitInfoMap& euim )
{
    TV_INSERTSTRUCT tvinsert;

    tvinsert.hParent = NULL;
    tvinsert.hInsertAfter = TVI_LAST;
    tvinsert.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_STATE;
    tvinsert.item.hItem = NULL;
    tvinsert.item.state = INDEXTOSTATEIMAGEMASK( 1 );
    tvinsert.item.stateMask = TVIS_STATEIMAGEMASK;
    tvinsert.item.cchTextMax = 6;
    tvinsert.item.iSelectedImage = 1;
    tvinsert.item.cChildren = 0;
    tvinsert.item.lParam = 0;

    buildSubTree( euim, tvinsert, _T( "" ), 0 );
}

void NormalEvalSettingDlg::buildSubTree( EvalUnitInfoMap& euim, TV_INSERTSTRUCT tvinsert, const CString& parent_unit, int hierarchy_num )
{
    bool hasMoreNodes = false;
    for( EvalUnitInfoMap::iterator itr = euim.begin(); itr != euim.end(); itr++ )
    {
        EvalUnitInfo info = itr->second;
        if( info.hierarchy_num == hierarchy_num && info.parent_unit == parent_unit )
        {
            hasMoreNodes = true;

            tvinsert.item.pszText = ( LPTSTR )( LPCTSTR )itr->first;
            tvinsert.item.iImage = 0;

            TV_INSERTSTRUCT childTVInsert = tvinsert;
            childTVInsert.hParent = m_evalUnitTreeCtrl.InsertItem( &tvinsert );
            buildSubTree( euim, childTVInsert, itr->first, hierarchy_num + 1 );
        }
    }

    // ���Ը�����Ҫ����ѡ��Ƿ���Ҫ��ʾҶ�ӽڵ㣨����������ţ�
    if( !hasMoreNodes ) insertLeafNode( euim, tvinsert, parent_unit );
}

void NormalEvalSettingDlg::insertLeafNode( EvalUnitInfoMap& euim, TV_INSERTSTRUCT tvinsert, const CString& parent_unit )
{
    // ����Ҷ��
    for( EvalUnitInfoMap::iterator itr = euim.begin(); itr != euim.end(); itr++ )
    {
        if( itr->first == parent_unit )
        {
            ClauseCollection::iterator list_itr_begin = itr->second.clauses.begin();
            ClauseCollection::iterator list_itr_end = itr->second.clauses.end();
            ClauseCollection::iterator list_itr;
            for( list_itr = list_itr_begin; list_itr != list_itr_end; list_itr++ )
            {
                CString num;
                num.Format( _T( "%d" ), *list_itr ); // ������
                tvinsert.item.pszText = ( LPTSTR )( LPCTSTR )num;

                HTREEITEM hItem = m_evalUnitTreeCtrl.InsertItem( &tvinsert );
            }
        }
    }
}

int NormalEvalSettingDlg::getEvalMethodType()
{
    if( BST_CHECKED == m_normalEvalTypeRadio.GetCheck() )
        return 0;
    else
        return 1;
}

CString NormalEvalSettingDlg::getSelRegulationTableName()
{
    int row = 0;
    for( int i = 0; i < m_regulationListCtrl.GetItemCount(); i++ )
    {
        if( LVIS_SELECTED == m_regulationListCtrl.GetItemState( i, LVIS_SELECTED ) )
        {
            row = i;
            break;
        }
    }
    return m_regulationListCtrl.GetItemText( row, 4 ); // ��ȡtable name
}

void NormalEvalSettingDlg::getClauseCollection( ClauseCollection& clauses )
{
    CString tableName = getSelRegulationTableName(); // ��ǰѡ��Ĺ�̶�Ӧ�����ݿ��

    CDaoDatabase db;
    db.Open( m_regulationInfoFileName );
    CDaoRecordset rs( &db );
    try
    {
        CString sql;
        sql.Format( _T( "select * from %s" ), tableName );  // ��ȡ��̱�--tablename
        rs.Open( dbOpenDynaset, sql, 0 );

        rs.MoveFirst () ;
        while( !rs.IsEOF() )
        {
            COleVariant v;
            rs.GetFieldValue( CLAUSE_NUM_FIELD , v ); // ��ȡ������
            clauses.push_back( v.intVal );

            rs.MoveNext () ;
        }
    }
    catch( CDaoException* e )
    {
        //AfxMessageBox(e->m_pErrorInfo->m_strDescription);
        e->Delete();
        clauses.clear(); // �����쳣�����
    }
    // �رռ�¼��
    if( rs.IsOpen() ) rs.Close();
    db.Close();
}

void NormalEvalSettingDlg::getSelEvalUnitMap( EvalUnitInfoMap& euim )
{
    HTREEITEM hItem = m_evalUnitTreeCtrl.GetRootItem();
    while( hItem != NULL )
    {
        visitTree( euim, m_evalUnitTreeCtrl, hItem );
        hItem = m_evalUnitTreeCtrl.GetNextSiblingItem( hItem ); // ͬһ��ĸ��ڵ�
    }
}

void NormalEvalSettingDlg::visitTree( EvalUnitInfoMap& euim, CTreeCtrl& tree, HTREEITEM hItem )
{
    int nState = tree.GetItemState( hItem, TVIS_STATEIMAGEMASK ) >> 12;  		// ���ڵ��״̬
    if( nState < 2 ) return;	                                                  // û��ѡ��ڵ�

    CString unit = tree.GetItemText( hItem );                                   // ���۵�Ԫ

    CString parent_unit;                                                        // �����۵�Ԫ
    HTREEITEM hParentItem = tree.GetParentItem( hItem );
    if( hParentItem != NULL ) parent_unit = tree.GetItemText( hParentItem );

    if( tree.ItemHasChildren( hItem ) )
    {
        if( nState == 3 )                                                       // �ڵ�ȫ��ѡ��
        {
            EvalUnitInfo info;
            info.parent_unit = parent_unit;
            euim.insert( EvalUnitInfoMap::value_type( unit, info ) );
        }

        HTREEITEM hChildItem = tree.GetChildItem( hItem );                      // �ݹ�����ӽڵ�
        while( hChildItem != NULL )
        {
            visitTree( euim, tree, hChildItem );
            hChildItem = tree.GetNextItem( hChildItem, TVGN_NEXT );
        }
    }
    else if( parent_unit.GetLength() != 0 )
    {
        // Ҷ�ӽڵ㣬Ҳ���Ǿ����������
        euim[parent_unit].clauses.push_back( CStrToInt( unit ) );
    }
}

void NormalEvalSettingDlg::doNormalEvalWithoutUnit( const CString& outputDirPath )
{
    // �ռ���ѡ��Ĺ��������������
    CString regulationName = getSelRegulationTableName();
    if( regulationName.GetLength() == 0 ) return ;

    EvalService* pEvalService = EvalService::CreateInstance();

    // ���ز��
    // Ŀǰ��ʱ��ô����
    // �����ڿ��ǲ������...
    pEvalService->loadPlugin( BuildPath( GetAppPathDir(), _T( "cmsr2010.arx" ) ) );

    // �������۽�����ݿ�·��
    // �����ݿ��뱨�����ͬһ��Ŀ¼��
    CString path;
    path.Format( _T( "%s\\%s.mdb" ), outputDirPath, EVAL_RESULT_DATA_BASE_NAME );
    pEvalService->setEvalResultDataBasePath( path );

    typedef std::deque<int> ClauseCollection;
    ClauseCollection clauses;
    getClauseCollection( clauses );  // �ռ���Ҫ���۵������ţ�Ŀǰ��ʵ���Ǵ����ݿ��ж�ȡ���еı�ţ�

    EvalClauseCollection clauses2;
    ClauseCollectionTransformBy( clauses, clauses2 );    // ʹ�õ����ݽṹ��ͬ���򵥵�ת��һ��
    pEvalService->doRegEval( regulationName, clauses2 ); // ִ�����ۣ��������д�뵽���ݿ�

    EvalService::DestroyInstance();

    // ���û������������۽���ϲ�
    //mergeEvalResultByUser(path, m_regulationInfoFileName, regulationName);
}

void NormalEvalSettingDlg::doNormalEvalWithUnit( const CString& outputEvalReportPath, EvalUnitInfoMap& euim )
{
    CString regulationName = getSelRegulationTableName();
    if( regulationName.GetLength() == 0 ) return ;

    EvalService* pEvalService = EvalService::CreateInstance();

    // ���ز��
    // Ŀǰ��ʱ��ô����
    // �����ٿ��ǲ������...
    pEvalService->loadPlugin( BuildPath( GetAppPathDir(), _T( "cmsr2010.arx" ) ) );

    // �������۽�����ݿ�·��
    // �����ݿ��뱨�����ͬһ��Ŀ¼��
    CString path;
    path.Format( _T( "%s\\%s.mdb" ), GetPathDir( outputEvalReportPath ), EVAL_RESULT_DATA_BASE_NAME );
    pEvalService->setEvalResultDataBasePath( path );

    // 1��ִ�о������������
    doNormalEvalWithUnit_firstStep( path, regulationName, euim );

    EvalService::DestroyInstance();

    // ���û������������۽���ϲ�
    //mergeEvalResultByUser(path, m_regulationInfoFileName, regulationName);

    // �������۵�Ԫ�����۽�����ܱ�
    CDaoDatabase db;
    db.Open( path );
    CreateEvalUnitResultTable( &db, EVAL_UNIT_RESULT_TABLE_NAME );

    // 2���������۵�Ԫ�����۽��
    EvalResultMap er_map;
    doNormalEvalWithUnit_secondStep( &db, euim, er_map );

    // 3�������۵�Ԫ�����۽��д�뵽���ݿ���
    doNormalEvalWithUnit_thirdStep( &db, er_map );

    db.Close();
}

void NormalEvalSettingDlg::doNormalEvalWithUnit_firstStep( LPCTSTR dbPath, const CString& regulationTable, EvalUnitInfoMap& euim )
{
    EvalService* pEvalService = EvalService::GetInstance();

    // ���۵�ʱ������ɾ��ԭ�е����ݿ⣬
    // ��֤���ݿ�ʼ��ʱ�����µ�
    pEvalService->setEvalResultDataBasePath( dbPath );         // �������۽�����ݿ�·��

    EvalClauseCollection clauses;
    for( EvalUnitInfoMap::iterator itr = euim.begin(); itr != euim.end(); itr++ )
    {
        EvalClauseCollection temp_clauses;
        ClauseCollectionTransformBy( itr->second.clauses, temp_clauses );
        clauses.append( temp_clauses );
    }
    pEvalService->doRegEval( regulationTable, clauses );      // ִ�����ۣ��������д�뵽���ݿ�
}

void NormalEvalSettingDlg::doNormalEvalWithUnit_secondStep( CDaoDatabase* pDB, EvalUnitInfoMap& euim, EvalResultMap& er_map )
{
    for( EvalUnitInfoMap::iterator itr = euim.begin(); itr != euim.end(); itr++ )
    {
        CString unit = itr->first;
        CString parent_unit = itr->second.parent_unit;

        if( er_map.find( unit ) == er_map.end() ) er_map.insert( EvalResultMap::value_type( unit, Qualified ) );

        ClauseCollection& clauses = itr->second.clauses;
        if( clauses.empty() ) continue;

        EvalResult er = er_map[unit];
        for( ClauseCollection::iterator c_itr = clauses.begin(); c_itr != clauses.end(); c_itr++ )
        {
            er = DiffEvalResult( GetEvalResultByClause( pDB, TOTAL_EVAL_RESULT_TABLE_NAME, *c_itr ), er );
        }
        er_map[unit] = er;

        if( euim.find( parent_unit ) != euim.end() )
        {
            // ���������۵�Ԫ�Ľ��
            EvalResultMap::iterator itr2 = er_map.find( parent_unit );
            if( itr2 == er_map.end() )
            {
                er_map.insert( EvalResultMap::value_type( parent_unit, DiffEvalResult( er_map[unit], Qualified ) ) );
            }
            else
            {
                itr2->second = DiffEvalResult( er_map[unit], itr2->second );
            }
        }
    }
}

void NormalEvalSettingDlg::doNormalEvalWithUnit_thirdStep( CDaoDatabase* pDB, EvalResultMap& er_map )
{
    for( EvalResultMap::iterator itr = er_map.begin(); itr != er_map.end(); itr++ )
    {
        // ���뵽���ݿ���
        InsertEvalUnitResult( pDB, EVAL_UNIT_RESULT_TABLE_NAME, itr->first, er_map[itr->first] );
    }
}

void NormalEvalSettingDlg::mergeEvalResultByUser( const CString& evalResultDBPath, const CString& regulationDBPath, const CString& regulationTable )
{
    // �����Ϣ���ݿ�
    CDaoDatabase regulationDB;
    regulationDB.Open( regulationDBPath );

    // ���۽�����ݿ�
    CDaoDatabase evalResultDB;
    evalResultDB.Open( evalResultDBPath );
    CDaoRecordset rs( &evalResultDB );
    try
    {
        CString sql;
        sql.Format( _T( "select * from %s" ), TOTAL_EVAL_RESULT_TABLE_NAME );
        rs.Open( dbOpenDynaset, sql, 0 );
        rs.MoveFirst();
        while( !rs.IsEOF() )
        {
            COleVariant v1, v2;
            rs.GetFieldValue( CLAUSE_NUM_FIELD , v1 );      // ��ȡ������

            rs.GetFieldValue( CLAUSE_IMPL_STATE_FIELD, v2 ); // ��ȡ����ʵ��״̬
            if( UNKNOWN_RESULT == v2.intVal )               // ����û��ʵ��
            {
                int qnr = getInitQuestionNaireResult( &regulationDB, regulationTable, v1.intVal );
                if( qnr != -1 )
                {
                    rs.Edit();
                    CString value;
                    value.Format( _T( "%d" ), ConvertBoolToEvalResult( qnr ) );
                    rs.SetFieldValue( EVAL_RESULT_FIELD, ( LPCTSTR )value );
                    rs.Update();
                }
            }
            rs.MoveNext();
        }
    }
    catch( CDaoException* e )
    {
        //AfxMessageBox(e->m_pErrorInfo->m_strDescription);
        e->Delete();
    }
    // �رռ�¼��
    if( rs.IsOpen() ) rs.Close();
    evalResultDB.Close();
    regulationDB.Close();
}

int NormalEvalSettingDlg::getInitQuestionNaireResult(
    CDaoDatabase* pRegulationDB,
    const CString& regulationTable,
    int clauseNum )
{
    int qnr = -1; // δ֪
    CDaoRecordset rs( pRegulationDB );
    try
    {
        CString sql;
        sql.Format( _T( "select * from %s where %s=%d" ), regulationTable, CLAUSE_NUM_FIELD, clauseNum );
        rs.Open( dbOpenDynaset, sql, 0 );

        rs.MoveFirst();
        while( !rs.IsEOF() )
        {
            COleVariant v;
            rs.GetFieldValue( INIT_QUSESTION_NAIRE_RESULT_FIELD, v );
            qnr = Access2Bool( v.boolVal ); // ת������
            rs.MoveNext () ;
        }
    }
    catch( CDaoException* e )
    {
        //AfxMessageBox(e->m_pErrorInfo->m_strDescription);
        e->Delete();
    }
    // �رռ�¼��
    if( rs.IsOpen() ) rs.Close();

    return qnr;
}

void NormalEvalSettingDlg::OnBnClickedInitBtn()
{
    UpdateData( TRUE );
    if( !checkInput() ) return;

    QuestionNaireDlg qnd;
    qnd.m_dbPath = m_regulationInfoFileName;
    qnd.m_mainTalbe = REGULATION_INFO_MAIN_TABLE_NAME;
    qnd.m_regulationTable = getSelRegulationTableName();
    qnd.DoModal();
}

static bool WriteCoalMineInfo( CIniFile& iniFile )
{
    // ��ȡ"����Ϣ"���ݶ���
    AcDbObjectId objId;
    if( !SingleDataObjectHelper::GetObjectId( COAL_MINE_SECTION, objId ) ) return false;

    // "����"
    CString v1;
    if( !DataHelper::GetPropertyData( objId, COAL_MINE_NAME_KEY, v1 ) ) return false;

    // "�������Ź�˾"
    CString v2;
    if( !DataHelper::GetPropertyData( objId, COMPANY_NAME_KEY, v2 ) ) return false;

    iniFile.SetKeyValue( COAL_MINE_SECTION, COAL_MINE_NAME_KEY, v1 );
    iniFile.SetKeyValue( COAL_MINE_SECTION, COMPANY_NAME_KEY, v2 );

    return true;
}

static bool WriteEvalProjectInfo( CIniFile& iniFile )
{
    AcDbObjectId objId;
    if( !SingleDataObjectHelper::GetObjectId( EVAL_PROJECT_INFO_SECTION, objId ) ) return false;

    // ���������ݡ�
    CString v1;
    if( !DataHelper::GetPropertyData( objId, EVAL_REGULATION_KEY, v1 ) ) return false;

    // "�ƶ����۷�����ʼʱ��"
    CString v2;
    if( !DataHelper::GetPropertyData( objId, PREPARE_SOLUTION_BEGIN_KEY, v2 ) ) return false;

    // "�ƶ����۷�������ʱ��"
    CString v3;
    if( !DataHelper::GetPropertyData( objId, PREPARE_SOLUTION_END_KEY, v3 ) ) return false;

    // "�ռ��������Ͽ�ʼʱ��"
    CString v4;
    if( !DataHelper::GetPropertyData( objId, PREPARE_INFO_BEGIN_KEY, v4 ) ) return false;

    // "�ռ��������Ͻ���ʱ��"
    CString v5;
    if( !DataHelper::GetPropertyData( objId, PREPARE_INFO_END_KEY, v5 ) ) return false;

    // "ǩ�����ۺ�ͬ��ʼʱ��"
    CString v6;
    if( !DataHelper::GetPropertyData( objId, PREPARE_CONTRACT_BEGIN_KEY, v6 ) ) return false;

    // "ǩ�����ۺ�ͬ����ʱ��"
    CString v7;
    if( !DataHelper::GetPropertyData( objId, PREPARE_CONTRACT_END_KEY, v7 ) ) return false;

    // "���������᳹��ʼʱ��"
    CString v8;
    if( !DataHelper::GetPropertyData( objId, SURVEY_LEARN_BEGIN_KEY, v8 ) ) return false;

    // "���������᳹����ʱ��"
    CString v9;
    if( !DataHelper::GetPropertyData( objId, SURVEY_LEARN_END_KEY, v9 ) ) return false;

    // "��չ���۹�����ʼʱ��"
    CString v10;
    if( !DataHelper::GetPropertyData( objId, WORK_DO_BEGIN_KEY, v10 ) ) return false;

    // "��չ���۹�������ʱ��"
    CString v11;
    if( !DataHelper::GetPropertyData( objId, WORK_DO_END_KEY, v11 ) ) return false;

    // "�������۱��濪ʼʱ��"
    CString v12;
    if( !DataHelper::GetPropertyData( objId, WORK_REPORT_BEGIN_KEY, v12 ) ) return false;

    // "�������۱������ʱ��"
    CString v13;
    if( !DataHelper::GetPropertyData( objId, WORK_REPORT_END_KEY, v13 ) ) return false;

    iniFile.SetKeyValue( EVAL_PROJECT_INFO_SECTION, EVAL_REGULATION_KEY, v1 );
    iniFile.SetKeyValue( EVAL_PROJECT_INFO_SECTION, PREPARE_SOLUTION_BEGIN_KEY, v2 );
    iniFile.SetKeyValue( EVAL_PROJECT_INFO_SECTION, PREPARE_SOLUTION_END_KEY, v3 );
    iniFile.SetKeyValue( EVAL_PROJECT_INFO_SECTION, PREPARE_INFO_BEGIN_KEY, v4 );
    iniFile.SetKeyValue( EVAL_PROJECT_INFO_SECTION, PREPARE_INFO_END_KEY, v5 );
    iniFile.SetKeyValue( EVAL_PROJECT_INFO_SECTION, PREPARE_CONTRACT_BEGIN_KEY, v6 );
    iniFile.SetKeyValue( EVAL_PROJECT_INFO_SECTION, PREPARE_CONTRACT_END_KEY, v7 );
    iniFile.SetKeyValue( EVAL_PROJECT_INFO_SECTION, SURVEY_LEARN_BEGIN_KEY, v8 );
    iniFile.SetKeyValue( EVAL_PROJECT_INFO_SECTION, SURVEY_LEARN_END_KEY, v9 );
    iniFile.SetKeyValue( EVAL_PROJECT_INFO_SECTION, WORK_DO_BEGIN_KEY, v10 );
    iniFile.SetKeyValue( EVAL_PROJECT_INFO_SECTION, WORK_DO_END_KEY, v11 );
    iniFile.SetKeyValue( EVAL_PROJECT_INFO_SECTION, WORK_REPORT_BEGIN_KEY, v12 );
    iniFile.SetKeyValue( EVAL_PROJECT_INFO_SECTION, WORK_REPORT_END_KEY, v13 );

    return true;
}

static bool WriteExtraData( CIniFile& iniFile )
{
    // 1��д�����Ϣ
    if( !WriteCoalMineInfo( iniFile ) ) return false;

    // 2��д��������Ŀ��Ϣ
    return WriteEvalProjectInfo( iniFile );
}

bool NormalEvalSettingDlg::writeEvalDataIniFile( const CString& outputPath, bool regulationImpl )
{
    // ���������ļ�֮ǰɾ���Ѵ��ڵ������ļ�
    CString path;
    path.Format( _T( "%s\\%s.ini" ), outputPath, EVAL_DATA_INFO_FILE_NAME );
    if( !CreateEmptyFile( path ) )
    {
        MessageBox( _T( "���������ļ�ʧ��!" ) );
        return false;
    }

    CIniFile iniFile;
    iniFile.SetPath( path );

    // ���۷���
    CString value;
    value.Format( _T( "%d" ), getEvalMethodType() );
    iniFile.SetKeyValue( EVAL_METHOD_SECTION, EVAL_METHOD_KEY, value );

    // �����Ϣ
    iniFile.SetKeyValue( REGULATION_INFO_SECTION, REGULATION_DATABASE_PATH_KEY, m_regulationInfoFileName );
    iniFile.SetKeyValue( REGULATION_INFO_SECTION, REGULATION_TABLE_KEY, getSelRegulationTableName() );

    // ���۽��
    iniFile.SetKeyValue( EVAL_RESULT_SECTION, EVAL_RESULT_KEY, ( regulationImpl ? "1" : "0" ) );

    // ���۱���
    iniFile.SetKeyValue( EVAL_REPORT_FILE_NAME_SECTION, EVAL_REPORT_FILE_NAME_KEY, m_outputEvalReportPath );

    bool ret = WriteExtraData( iniFile );
    // ����Ҫд����������Ϣ
    // ����Ϣ��������Ŀ��Ϣ
    if( !ret )
    {
        RemoveFile( path ); // д������ʧ�ܣ�ɾ��ini�ļ�
    }
    return ret;
}

void NormalEvalSettingDlg::cleanTempData( const CString& outputPath )
{
    CString path;
    path.Format( _T( "%s\\%s.ini" ), outputPath, EVAL_DATA_INFO_FILE_NAME );
    RemoveFile( path );

    path.Format( _T( "%s\\%s.mdb" ), outputPath, EVAL_RESULT_DATA_BASE_NAME );
    RemoveFile( path );

    path.Format( _T( "%s\\%s.mdb" ), outputPath, _T( "afc" ) );
    RemoveFile( path );
}

bool NormalEvalSettingDlg::generateReport( const CString& outputPath )
{
    CString path;
    path.Format( _T( "%s\\%s.mdb" ), outputPath, EVAL_RESULT_DATA_BASE_NAME );
    bool regulationImpl = IsFileExist( path );
    // ���۽�����ݿⲻ���ڣ���ʾ���û��ʵ��
    if( !regulationImpl ) return false;

    return writeEvalDataIniFile( outputPath, regulationImpl );
}