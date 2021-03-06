#include "stdafx.h"
#include "ExpandDlg.h"
#include "TestDlg.h"

IMPLEMENT_DYNAMIC( TestDlg, CDialog )

TestDlg::TestDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( TestDlg::IDD, pParent )
{

}

TestDlg::~TestDlg()
{
    for ( int i = 0; i < 10; i++ )
        delete m_pData[i];
}

void TestDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_LIST1, m_list );
}

BEGIN_MESSAGE_MAP( TestDlg, CDialog )
    ON_NOTIFY( LVN_COLUMNCLICK, IDC_LIST1, &TestDlg::OnLvnColumnclickList1 )
END_MESSAGE_MAP()


int CALLBACK SortFunc( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
{
    int nRetVal = 0;

    PITEMDATA pData1 = ( PITEMDATA )lParam1;
    PITEMDATA pData2 = ( PITEMDATA )lParam2;

    switch( lParamSort )
    {
    case 0:	// Last Name
        nRetVal = _tcscmp( pData1->pszLastName,
                           pData2->pszLastName );
        break;

    case 1:	// First Name
        nRetVal = _tcscmp( pData1->pszFirstName,
                           pData2->pszFirstName );
        break;

    case 2: // Term
        nRetVal = _tcscmp( pData1->pszTerm, pData2->pszTerm );
        break;

    default:
        break;
    }

    return nRetVal;
}

// TestDlg 消息处理程序

void TestDlg::OnLvnColumnclickList1( NMHDR* pNMHDR, LRESULT* pResult )
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>( pNMHDR );

    m_list.SortItems( SortFunc, pNMLV->iSubItem );

    *pResult = 0;
}

static CString strData[10][3] =
{
    { _T( "Washington" ), _T( "George" ), _T( "1789-1797" ) },
    { _T( "Adams" ), _T( "John" ), _T( "1797-1801" ) },
    { _T( "Jefferson" ), _T( "Thomas" ), _T( "1801-1809" ) },
    { _T( "Madison" ), _T( "James" ), _T( "1809-1817" ) },
    { _T( "Monroe" ), _T( "James" ), _T( "1817-1825" ) },
    { _T( "Adams" ), _T( "John Quincy" ), _T( "1825-1829" ) },
    { _T( "Jackson" ), _T( "Andrew" ), _T( "1829-1837" ) },
    { _T( "Van Buren" ), _T( "Martin" ), _T( "1837-1841" ) },
    { _T( "Harrison" ), _T( "William Henry" ), _T( "1841" ) },
    { _T( "Tyler" ), _T( "John" ), _T( "1841-1845" ) }
};

BOOL TestDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_list.SetExtendedStyle( m_list.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

    m_list.InsertColumn( 0, _T( "Last Name" ), LVCFMT_LEFT, 100 );
    m_list.InsertColumn( 1, _T( "First Name" ), LVCFMT_LEFT, 100 );
    m_list.InsertColumn( 2, _T( "Term" ), LVCFMT_LEFT, 100 );

    for ( int i = 0; i < 10; i++ )
    {
        m_pData[i] = new ITEMDATA;
        m_pData[i]->pszLastName = ( LPTSTR )( LPCTSTR )strData[i][0];
        m_pData[i]->pszFirstName = ( LPTSTR )( LPCTSTR )strData[i][1];
        m_pData[i]->pszTerm = ( LPTSTR )( LPCTSTR )strData[i][2];

        m_list.InsertItem( i, strData[i][0] );
        m_list.SetItemText( i, 1, strData[i][1] );
        m_list.SetItemText( i, 2, strData[i][2] );
        m_list.SetItemData( i, ( LPARAM )m_pData[i] );
    }

    return TRUE;
}
