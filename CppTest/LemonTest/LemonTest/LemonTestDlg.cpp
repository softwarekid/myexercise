
// LemonTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LemonTest.h"
#include "LemonTestDlg.h"

#undef OUT
#undef IN

#include <lemon/list_graph.h>
#include <lemon/adaptors.h>
#include <lemon/dfs.h>
#include <lemon/bfs.h>
#include <lemon/dijkstra.h>
#include <lemon/connectivity.h>

using namespace lemon;
typedef ListDigraph Digraph;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

// 对话框数据
    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV 支持

// 实现
protected:
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog( CAboutDlg::IDD )
{
}

void CAboutDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP( CAboutDlg, CDialog )
END_MESSAGE_MAP()


// CLemonTestDlg 对话框




CLemonTestDlg::CLemonTestDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( CLemonTestDlg::IDD, pParent )
    , m_xx( 0 )
{
    m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
}

void CLemonTestDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Text( pDX, IDC_EDIT1, m_xx );
}

BEGIN_MESSAGE_MAP( CLemonTestDlg, CDialog )
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_BN_CLICKED( IDC_BUTTON1, &CLemonTestDlg::OnBnClickedButton1 )
    ON_BN_CLICKED( IDC_BUTTON2, &CLemonTestDlg::OnBnClickedButton2 )
    ON_BN_CLICKED( IDC_BUTTON3, &CLemonTestDlg::OnBnClickedButton3 )
    ON_BN_CLICKED( IDC_BUTTON4, &CLemonTestDlg::OnBnClickedButton4 )
    ON_BN_CLICKED( IDC_BUTTON5, &CLemonTestDlg::OnBnClickedButton5 )
    ON_BN_CLICKED( IDC_BUTTON6, &CLemonTestDlg::OnBnClickedButton6 )
    ON_BN_CLICKED( IDC_BUTTON7, &CLemonTestDlg::OnBnClickedButton7 )
    ON_BN_CLICKED( IDC_BUTTON8, &CLemonTestDlg::OnBnClickedButton8 )
    ON_BN_CLICKED( IDC_BUTTON9, &CLemonTestDlg::OnBnClickedButton9 )
    ON_BN_CLICKED( IDC_BUTTON10, &CLemonTestDlg::OnBnClickedButton10 )
    ON_BN_CLICKED( IDC_BUTTON11, &CLemonTestDlg::OnBnClickedButton11 )
    ON_WM_CLOSE()
    ON_BN_CLICKED( IDC_BUTTON12, &CLemonTestDlg::OnBnClickedButton12 )
    ON_BN_CLICKED( IDC_BUTTON13, &CLemonTestDlg::OnBnClickedButton13 )
    ON_CBN_SELCHANGE( IDC_COMBO1, &CLemonTestDlg::OnCbnSelchangeCombo1 )
END_MESSAGE_MAP()


// CLemonTestDlg 消息处理程序

BOOL CLemonTestDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // 将“关于...”菜单项添加到系统菜单中。

    // IDM_ABOUTBOX 必须在系统命令范围内。
    ASSERT( ( IDM_ABOUTBOX & 0xFFF0 ) == IDM_ABOUTBOX );
    ASSERT( IDM_ABOUTBOX < 0xF000 );

    CMenu* pSysMenu = GetSystemMenu( FALSE );
    if ( pSysMenu != NULL )
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString( IDS_ABOUTBOX );
        ASSERT( bNameValid );
        if ( !strAboutMenu.IsEmpty() )
        {
            pSysMenu->AppendMenu( MF_SEPARATOR );
            pSysMenu->AppendMenu( MF_STRING, IDM_ABOUTBOX, strAboutMenu );
        }
    }

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon( m_hIcon, TRUE );			// 设置大图标
    SetIcon( m_hIcon, FALSE );		// 设置小图标

    // TODO: 在此添加额外的初始化代码

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CLemonTestDlg::OnSysCommand( UINT nID, LPARAM lParam )
{
    if ( ( nID & 0xFFF0 ) == IDM_ABOUTBOX )
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDialog::OnSysCommand( nID, lParam );
    }
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLemonTestDlg::OnPaint()
{
    if ( IsIconic() )
    {
        CPaintDC dc( this ); // 用于绘制的设备上下文

        SendMessage( WM_ICONERASEBKGND, reinterpret_cast<WPARAM>( dc.GetSafeHdc() ), 0 );

        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics( SM_CXICON );
        int cyIcon = GetSystemMetrics( SM_CYICON );
        CRect rect;
        GetClientRect( &rect );
        int x = ( rect.Width() - cxIcon + 1 ) / 2;
        int y = ( rect.Height() - cyIcon + 1 ) / 2;

        // 绘制图标
        dc.DrawIcon( x, y, m_hIcon );
    }
    else
    {
        CDialog::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLemonTestDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>( m_hIcon );
}

void CLemonTestDlg::OnBnClickedButton1()
{
    Digraph dg;
    Digraph::Node v1 = dg.addNode();
    Digraph::Node v2 = dg.addNode();
    Digraph::Node v3 = dg.addNode();
    Digraph::Node v4 = dg.addNode();
    Digraph::Arc e1 = dg.addArc( v1, v2 );
    Digraph::Arc e2 = dg.addArc( v1, v3 );
    Digraph::Arc e3 = dg.addArc( v2, v3 );
    Digraph::Arc e4 = dg.addArc( v2, v4 );
    Digraph::Arc e5 = dg.addArc( v3, v4 );

    typedef Digraph::ArcMap<CString> EdgeNameMap;
    EdgeNameMap eName( dg );
    eName[e1] = _T( "e1" );
    eName[e2] = _T( "e2" );
    eName[e3] = _T( "e3" );
    eName[e4] = _T( "e4" );
    eName[e5] = _T( "e5" );

    typedef Digraph::NodeMap<CString> NodeNameMap;
    NodeNameMap nName( dg );
    nName[v1] = _T( "v1" );
    nName[v2] = _T( "v2" );
    nName[v3] = _T( "v3" );
    nName[v4] = _T( "v4" );

    bool ret = true;
    typedef Path<Digraph> DPath;
    DPath pp;
    Dfs<Digraph> aDfs( dg );
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // lemon的run(s,t)实现有bug
    // 只能通过run(s)的方法得到dsf tree
    // 然后再得到path，这样的效率就比较差
    // 需要遍历整个图
    // bfs也是如此
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //Bfs<Digraph> bDfs(dg);
    //bool ret = aDfs.run(v1, v4);
    aDfs.run( v1 );
    //bool ret = bDfs.run(v1, v4);
    if( ret )
    {
        if( !aDfs.reached( v4 ) ) return;
        pp = aDfs.path( v4 );
        CString msg;
        //pp = bDfs.path(v3);
        msg.Format( _T( "\n%s->%s的路径:" ), nName[v1], nName[v4] );
        for( DPath::ArcIt e( pp ); e != INVALID; ++e )
        {
            DPath::Arc a = e;
            msg.AppendFormat( _T( "%s->" ), eName[a] );
        }
        msg.AppendFormat( _T( "\n" ) );

        MessageBox( msg );
    }
}
typedef ListDigraph Digraph;

void CLemonTestDlg::OnBnClickedButton2()
{
    Digraph dg;
    Digraph::Node v1 = dg.addNode();
    Digraph::Node v2 = dg.addNode();
    Digraph::Node v3 = dg.addNode();
    Digraph::Node v4 = dg.addNode();
    Digraph::Arc e1 = dg.addArc( v1, v2 );
    Digraph::Arc e2 = dg.addArc( v1, v3 );
    Digraph::Arc e3 = dg.addArc( v2, v3 );
    Digraph::Arc e4 = dg.addArc( v2, v4 );
    Digraph::Arc e5 = dg.addArc( v3, v4 );

    typedef Digraph::ArcMap<CString> EdgeNameMap;
    EdgeNameMap eName( dg );
    eName[e1] = _T( "e1" );
    eName[e2] = _T( "e2" );
    eName[e3] = _T( "e3" );
    eName[e4] = _T( "e4" );
    eName[e5] = _T( "e5" );

    typedef Digraph::NodeMap<CString> NodeNameMap;
    NodeNameMap nName( dg );
    nName[v1] = _T( "v1" );
    nName[v2] = _T( "v2" );
    nName[v3] = _T( "v3" );
    nName[v4] = _T( "v4" );

    typedef Undirector<Digraph> Graph;
    Graph g( dg ); // 转换成无向图
    typedef Path<Undirector<Digraph>> UPath;
    UPath pp;
    Dfs<Graph> aDfs( g );
    // 3种写法
    //aDfs.run(v1);

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // lemon的run(s,t)实现有bug
    // 只能通过run(s)的方法得到dsf tree
    // 然后再得到path，这样的效率就比较差
    // 需要遍历整个图
    // bfs也是如此
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // 目前已修复(从官网下载了一个小patch，然后把lemon的源码重新编译了一次)
    bool ret = aDfs.run( v1, v4 );
    //bool ret = dfs(g).path(pp).run(v1, v4);
    if( ret )
    {
        if( !aDfs.reached( v4 ) ) return;
        pp = aDfs.path( v4 );
        CString msg;
        msg.Format( _T( "\n%s->%s的路径:" ), nName[v1], nName[v4] );
        for( UPath::ArcIt e( pp ); e != INVALID; ++e )
        {
            UPath::Arc a = e;
            //Digraph::Arc b = a;
            msg.AppendFormat( _T( "%s(%s, %s)->" ), eName[a], nName[dg.source( a )], nName[dg.target( a )] );
            //msg.AppendFormat(_T("%s(%s, %s)->"), eName[a], nName[dg.source(b)], nName[dg.target(b)]);
            //msg.AppendFormat(_T("%s(%s, %s)->"), eName[a], nName[g.source(a)], nName[g.target(a)]);
//			msg.AppendFormat(_T("%s(%s, %s)->"), eName[b], nName[g.source(b)], nName[g.target(b)]); // error!
        }
        msg.AppendFormat( _T( "\n" ) );
        MessageBox( msg );
    }
}

void CLemonTestDlg::OnBnClickedButton3()
{
    Digraph dg;
    typedef Digraph::NodeMap<CString> NodeNameMap;
    NodeNameMap name( dg );
    Digraph::Node v1 = dg.addNode();
    Digraph::Node v2 = dg.addNode();
    Digraph::Node v3 = dg.addNode();
    dg.addArc( v1, v2 );
    dg.addArc( v2, v3 );

    name[v1] = _T( "v1" );
    name[v2] = _T( "v2" );
    name[v3] = _T( "v3" );

    CString msg;
    for( Digraph::NodeIt n( dg ); n != INVALID; ++n )
    {
        msg.AppendFormat( _T( "%s\n" ), name[n] );
    }

    MessageBox( msg );
}

void CLemonTestDlg::OnBnClickedButton4()
{
    Digraph dg;
    Digraph::Node v1 = dg.addNode();
    Digraph::Node v2 = dg.addNode();
    Digraph::Node v3 = dg.addNode();
    Digraph::Node v4 = dg.addNode();
    Digraph::Arc e1 = dg.addArc( v1, v2 );
    Digraph::Arc e2 = dg.addArc( v1, v3 );
    Digraph::Arc e3 = dg.addArc( v2, v3 );
    Digraph::Arc e4 = dg.addArc( v2, v4 );
    Digraph::Arc e5 = dg.addArc( v3, v4 );

    typedef Digraph::ArcMap<CString> EdgeNameMap;
    EdgeNameMap eName( dg );
    eName[e1] = _T( "e1" );
    eName[e2] = _T( "e2" );
    eName[e3] = _T( "e3" );
    eName[e4] = _T( "e4" );
    eName[e5] = _T( "e5" );

    typedef Digraph::ArcMap<double> Weight;
    Weight w( dg );
    w[e1] = 1;
    w[e2] = 1;
    w[e3] = 1;
    w[e4] = 1;
    w[e5] = 1;

    typedef Digraph::NodeMap<CString> NodeNameMap;
    NodeNameMap nName( dg );
    nName[v1] = _T( "v1" );
    nName[v2] = _T( "v2" );
    nName[v3] = _T( "v3" );
    nName[v4] = _T( "v4" );

    // Dijkstra最短路径算法
    Dijkstra<Digraph, Weight> dk( dg, w );

    //dk.run(v4);
    dk.run( v1 );
    if( !dk.reached( v4 ) ) return;

    typedef Path<Digraph> DPath;
    DPath p1 = dk.path( v4 );

    CString msg;
    msg.Format( _T( "\n%s->%s的路径:" ), nName[v1], nName[v4] );
    for( DPath::ArcIt e( p1 ); e != INVALID; ++e )
    {
        msg.AppendFormat( _T( "%s(%s, %s)->" ), eName[e], nName[dg.source( e )], nName[dg.target( e )] );
    }
    msg.AppendFormat( _T( "\n" ) );
    MessageBox( msg );
}

static bool ReadWordInstallPath( CString& path )
{
    long lRet = 0;
    CString strRegOffice = _T( "Software\\Microsoft\\Office\\11.0\\Word\\InstallRoot" ); // office03版本
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
            path.Append( _T( "word.exe" ) );
        }

        RegCloseKey( hResult );
    }
    return ( lRet == ERROR_SUCCESS );
}

void CLemonTestDlg::OnBnClickedButton5()
{
    CString path;
    if( ReadWordInstallPath( path ) )
    {
        CString msg;
        msg.Format( _T( "获取word安装路径成功\n%s" ), path );
        MessageBox( msg );
    }
    else
    {
        MessageBox( _T( "获取路径失败" ) );
    }

    CString filePath = _T( "C:\\Users\\anheihb03dlj\\Desktop\\《煤矿安全规程》103条需风量计算方法.doc" );
    ShellExecute( NULL, _T( "open" ), filePath, NULL, NULL, SW_HIDE/*SW_SHOWNORMAL*/ );
}

static CString GetPathDir( const CString& path )
{
    // 方法2：也可以使用cstring的方法
    return path.Left( path.ReverseFind( _T( '\\' ) ) );
}

void CLemonTestDlg::OnBnClickedButton6()
{
    CString path = _T( "C:\\Users\\anheihb03dlj\\Desktop\\数据模型修改.txt" );
    MessageBox( GetPathDir( path ) );
}

static void SplitCString( const CString& content, const CString& token, CStringArray& cc )
{
    int curPos = 0;
    CString str = content.Tokenize( token, curPos );
    while ( str != _T( "" ) )
    {
        cc.Add( str );
        str = content.Tokenize( token, curPos );
    };
}

void CLemonTestDlg::OnBnClickedButton7()
{
    CStdioFile inFile;
    if( !inFile.Open( _T( "D:\\xx.txt" ), CFile::modeRead | CFile::typeText ) ) return;

    // 读取图元类型和字段名称
    CString geType;
    CStringArray fields;

    CString strLine;
    inFile.ReadString( strLine );
    geType = strLine.Trim();

    // 读取字段
    CString strLine2;
    inFile.ReadString( strLine2 );
    SplitCString( strLine2, _T( " \t" ), fields );
}

// GUID字符串前缀
#define GUID_PREFIX _T("JLDATA")

// GUID字符串格式
#define GUID_FORMAT _T("{%08lX-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}")

// 生成GUID字符串
static bool NewGUID( CString& strGUID )
{
    GUID m_guid;
    bool ret = ( S_OK == ::CoCreateGuid( &m_guid ) );
    if( ret )
    {
        strGUID.Format( _T( "%s_" ), GUID_PREFIX ); // 添加前缀
        // 追加GUID字符串
        strGUID.AppendFormat( GUID_FORMAT,
                              m_guid.Data1, m_guid.Data2, m_guid.Data3,
                              m_guid.Data4[0], m_guid.Data4[1], m_guid.Data4[2],
                              m_guid.Data4[3], m_guid.Data4[4], m_guid.Data4[5],
                              m_guid.Data4[6], m_guid.Data4[7] );
    }
    return ret;
}

void CLemonTestDlg::OnBnClickedButton8()
{
    CString strGUID;
    NewGUID( strGUID );
    MessageBox( strGUID );
}

void CLemonTestDlg::OnBnClickedButton9()
{
    Digraph dg;
    Digraph::Node v1 = dg.addNode();
    Digraph::Node v2 = dg.addNode();
    Digraph::Node v3 = dg.addNode();
    Digraph::Node v4 = dg.addNode();
    Digraph::Arc e1 = dg.addArc( v1, v2 );
    Digraph::Arc e2 = dg.addArc( v1, v3 );
    Digraph::Arc e3 = dg.addArc( v2, v3 );
    Digraph::Arc e4 = dg.addArc( v2, v4 );
    Digraph::Arc e5 = dg.addArc( v3, v4 );

    typedef Digraph::ArcMap<CString> EdgeNameMap;
    EdgeNameMap eName( dg );
    eName[e1] = _T( "e1" );
    eName[e2] = _T( "e2" );
    eName[e3] = _T( "e3" );
    eName[e4] = _T( "e4" );
    eName[e5] = _T( "e5" );

    typedef Digraph::NodeMap<CString> NodeNameMap;
    NodeNameMap nName( dg );
    nName[v1] = _T( "v1" );
    nName[v2] = _T( "v2" );
    nName[v3] = _T( "v3" );
    nName[v4] = _T( "v4" );

    Dfs<Digraph> aDfs( dg );
    aDfs.run( v1 );

    typedef ReverseDigraph<Digraph> RDiagraph;
    RDiagraph rdg( dg );
    Dfs<RDiagraph> bDfs( rdg );
    bDfs.run( v4 );

    CString msg;
    for( Digraph::NodeIt n( dg ); n != INVALID; ++n )
    {
        msg.AppendFormat( _T( "%s: %d %d\n" ), nName[n], aDfs.dist( n ), bDfs.dist( n ) );
    }

    MessageBox( msg );
}

void CLemonTestDlg::OnBnClickedButton10()
{
    Digraph dg;
    Digraph::Node v1 = dg.addNode();
    Digraph::Node v2 = dg.addNode();
    Digraph::Node v3 = dg.addNode();
    Digraph::Node v4 = dg.addNode();
    Digraph::Arc e1 = dg.addArc( v1, v2 ); // v1->v2
    Digraph::Arc e2 = dg.addArc( v3, v1 ); // v1->v3
    Digraph::Arc e3 = dg.addArc( v3, v2 ); // v3->v2
    Digraph::Arc e4 = dg.addArc( v2, v4 ); // v2->v4
    Digraph::Arc e5 = dg.addArc( v4, v3 ); // v3->v4

    typedef Digraph::ArcMap<CString> EdgeNameMap;
    EdgeNameMap eName( dg );
    eName[e1] = _T( "e1" );
    eName[e2] = _T( "e2" );
    eName[e3] = _T( "e3" );
    eName[e4] = _T( "e4" );
    eName[e5] = _T( "e5" );

    typedef Digraph::NodeMap<CString> NodeNameMap;
    NodeNameMap nName( dg );
    nName[v1] = _T( "v1" );
    nName[v2] = _T( "v2" );
    nName[v3] = _T( "v3" );
    nName[v4] = _T( "v4" );

    typedef Undirector<Digraph> UGraph;
    UGraph ug( dg );

    Dfs<UGraph> aDfs( ug );
    if( aDfs.run( v4, v1 ) )
    {
        typedef Path<UGraph> UPath;
        UPath p = aDfs.path( v1 );

        CString msg;
        for( UPath::ArcIt e( p ); e != INVALID; ++e )
        {
            Digraph::Node u = ug.source( e );
            Digraph::Node v = ug.target( e );
            UGraph::Arc ue = e;    // ArcIt派生自Arc
            Digraph::Arc ee = ue; // UGraph::Arc可转换成Digraph::Arc
            msg.AppendFormat( _T( "分支:%s(%s, %s)\n" ), eName[ee], nName[u], nName[v] );
        }

        MessageBox( msg );
    }
    else
    {
        MessageBox( _T( "不可达" ) );
    }
}

BOOL WCharToMByte( const wchar_t* wch, char*& ch )
{
    int size = WideCharToMultiByte( CP_ACP, 0, wch, -1, NULL, 0, NULL, NULL );
    /*char* */
    ch = new char[size + 1];
    BOOL ret = WideCharToMultiByte( CP_ACP, 0, wch, -1, ch, size, NULL, NULL );
    if( !ret )
    {
        delete []ch;
        ch = 0;
    }
    return ret;
}

void CLemonTestDlg::OnBnClickedButton11()
{
    wchar_t* wch = _T( "翔翔糖糖" );
    char* ch;
    if( !WCharToMByte( wch, ch ) )
    {
        MessageBox( _T( "转换失败" ) );
    }
    else
    {
        //AfxMessageBoxA()
    }
    //int size=WideCharToMultiByte(CP_ACP,0,wch,-1,NULL,0,NULL,NULL);
    //char *ch=new char[size+1];
    //if(!WideCharToMultiByte(CP_ACP,0,wch,-1,ch,size,NULL,NULL))
    //{
    //	MessageBox(_T("转换失败"));
    //	//return false;
    //}
    //else
    //{

    //}
}

void CLemonTestDlg::OnClose()
{
    //MessageBox(_T("close"));

    CDialog::OnClose();
}

void CLemonTestDlg::OnCancel()
{
    //MessageBox(_T("onCancel"));

    CDialog::OnCancel();
}

void CLemonTestDlg::OnBnClickedButton12()
{
    CRect rect;
    GetDlgItem( IDC_RECT_PIC )->GetWindowRect( &rect );

    CString msg;
    msg.Format( _T( "left:%d, right:%d, top:%d, bottom:%d, width:%d, height:%d" ), rect.left, rect.right, rect.top, rect.bottom, rect.Width(), rect.Height() );

    ScreenToClient( &rect );
    msg.AppendFormat( _T( "\n[C]left:%d, right:%d, top:%d, bottom:%d, width:%d, height:%d" ), rect.left, rect.right, rect.top, rect.bottom, rect.Width(), rect.Height() );

    CRect dlgRect;
    GetWindowRect( &dlgRect );
    msg.AppendFormat( _T( "\n\nleft:%d, right:%d, top:%d, bottom:%d, width:%d, height:%d" ), dlgRect.left, dlgRect.right, dlgRect.top, dlgRect.bottom, dlgRect.Width(), dlgRect.Height() );

    //ScreenToClient(&dlgRect);
    //msg.AppendFormat(_T("\n[C]left:%d, right:%d, top:%d, bottom:%d, width:%d, height:%d"), dlgRect.left, dlgRect.right, dlgRect.top, dlgRect.bottom, dlgRect.Width(), dlgRect.Height());

    //MessageBox(msg);

    dlgRect.bottom += 20;
    rect.bottom += 100;
    SetWindowPos( NULL, dlgRect.left, dlgRect.top, dlgRect.Width(), dlgRect.Height(), SWP_NOMOVE | SWP_NOZORDER );
    GetDlgItem( IDC_RECT_PIC )->SetWindowPos( NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOMOVE | SWP_NOZORDER );
}

void CLemonTestDlg::OnBnClickedButton13()
{
    Digraph dg;
    Digraph::Node v1 = dg.addNode();
    Digraph::Node v2 = dg.addNode();
    Digraph::Node v3 = dg.addNode();
    Digraph::Node v4 = dg.addNode();
    Digraph::Arc e1 = dg.addArc( v1, v2 ); // v1->v2
    Digraph::Arc e2 = dg.addArc( v1, v3 ); // v1->v3
    Digraph::Arc e3 = dg.addArc( v3, v2 ); // v3->v2
    Digraph::Arc e4 = dg.addArc( v2, v4 ); // v2->v4
    Digraph::Arc e5 = dg.addArc( v3, v4 ); // v3->v4

    typedef Digraph::NodeMap<CString> NodeNameMap;
    NodeNameMap nName( dg );
    nName[v1] = _T( "v1" );
    nName[v2] = _T( "v2" );
    nName[v3] = _T( "v3" );
    nName[v4] = _T( "v4" );

    typedef Digraph::NodeMap<int> NodeOrder;
    NodeOrder order( dg );
    topologicalSort( dg, order );

    CString msg;
    for( Digraph::NodeIt n( dg ); n != INVALID; ++n )
    {
        msg.AppendFormat( _T( "%s order=%d\n" ), nName[n], order[n] );
    }
    MessageBox( msg );
}

void CLemonTestDlg::OnCbnSelchangeCombo1()
{
    // TODO: 在此添加控件通知处理程序代码
}
