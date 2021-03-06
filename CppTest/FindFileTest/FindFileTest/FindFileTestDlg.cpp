
// FindFileTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FindFileTest.h"
#include "FindFileTestDlg.h"

#include <vector>

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


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


// CFindFileTestDlg 对话框




CFindFileTestDlg::CFindFileTestDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( CFindFileTestDlg::IDD, pParent )
{
    m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
}

void CFindFileTestDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP( CFindFileTestDlg, CDialog )
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_BN_CLICKED( IDC_BUTTON1, &CFindFileTestDlg::OnBnClickedButton1 )
    ON_BN_CLICKED( IDC_BUTTON2, &CFindFileTestDlg::OnBnClickedButton2 )
    ON_BN_CLICKED( IDC_BUTTON3, &CFindFileTestDlg::OnBnClickedButton3 )
    ON_BN_CLICKED( IDC_BUTTON4, &CFindFileTestDlg::OnBnClickedButton4 )
END_MESSAGE_MAP()


// CFindFileTestDlg 消息处理程序

BOOL CFindFileTestDlg::OnInitDialog()
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

void CFindFileTestDlg::OnSysCommand( UINT nID, LPARAM lParam )
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

void CFindFileTestDlg::OnPaint()
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
HCURSOR CFindFileTestDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>( m_hIcon );
}

static void GetAllDisks( CStringArray& disks, bool ignore )
{
    TCHAR szBuf[100];
    memset( szBuf, 0, 100 );
    DWORD len = GetLogicalDriveStrings( sizeof( szBuf ) / sizeof( TCHAR ), szBuf );

    for ( TCHAR* s = szBuf; *s; s += _tcslen( s ) + 1 )
    {
        LPCTSTR sDrivePath = s;

        // 磁盘类型
        UINT uDriveType = GetDriveType( sDrivePath );

        // 只考虑普通硬盘分区
        // 忽略光盘、usb等
        if( !ignore || uDriveType == DRIVE_FIXED )
        {
            disks.Add( sDrivePath );
        }
    }
}

static void FindFile( const CString& strPath, const CString& fname, CStringArray& paths )
{
    CString strTmp;
    if ( strPath.Right( 1 ) != _T( '\\' ) )
    {
        strTmp.Format( _T( "%s\\*.*" ), strPath );
    }
    else
    {
        strTmp.Format( _T( "%s*.*" ), strPath );
    }

    CFileFind findFile;
    BOOL bFind = findFile.FindFile( strTmp );
    while( bFind )
    {
        bFind = findFile.FindNextFile();
        if ( fname == findFile.GetFileName() )
        {
            paths.Add( findFile.GetFilePath() );
        }
        if( findFile.IsDirectory() && !findFile.IsDots() )
        {
            FindFile( findFile.GetFilePath(), fname, paths );
        }
    }
}

void CFindFileTestDlg::OnBnClickedButton1()
{
    CString path = _T( "d:\\" );
    CString fname = _T( "dot.exe" );

    CStringArray disks;
    GetAllDisks( disks, true );

    CStringArray paths;
    for( int i = 0; i < disks.GetCount(); i++ )
    {
        //FindFile(path, fname, paths);
        CString msg;
        msg.Format( _T( "搜索%s" ), disks.GetAt( i ) );
        MessageBox( msg );

        FindFile( disks.GetAt( i ), fname, paths );
    }

    CString msg;
    msg.Format( _T( "查找到的文件个数:%d\n" ), paths.GetCount() );
    for( int i = 0; i < paths.GetCount(); i++ )
    {
        msg.AppendFormat( _T( "    %d) %s" ), i + 1, paths.GetAt( i ) );
    }
    MessageBox( msg );
}

void CFindFileTestDlg::OnBnClickedButton2()
{
    CStringArray disks;
    GetAllDisks( disks, false );

    CString msg;
    msg.Format( _T( "搜索到的磁盘分区个数:%d\n" ), disks.GetCount() );
    for( int i = 0; i < disks.GetCount(); i++ )
    {
        msg.AppendFormat( _T( "    %s\n" ), disks.GetAt( i ) );
    }
    MessageBox( msg );
}

// http://www.cnblogs.com/phinecos/archive/2008/06/19/1225224.html
static bool CreateMultipleDirectory( const CString& szPath )
{
    CString strDir( szPath ); //存放要创建的目录字符串
    //确保以'\'结尾以创建最后一个目录
    if ( strDir.GetAt( strDir.GetLength() - 1 ) != _T( '\\' ) )
    {
        strDir.AppendChar( _T( '\\' ) );
    }
    std::vector<CString> vPath;//存放每一层目录字符串
    CString strTemp;//一个临时变量,存放目录字符串
    bool bSuccess = false;//成功标志
    //遍历要创建的字符串
    for ( int i = 0; i < strDir.GetLength(); ++i )
    {
        if ( strDir.GetAt( i ) != _T( '\\' ) )
        {
            //如果当前字符不是'\\'
            strTemp.AppendChar( strDir.GetAt( i ) );
        }
        else
        {
            //如果当前字符是'\\'
            vPath.push_back( strTemp ); //将当前层的字符串添加到数组中
            strTemp.AppendChar( _T( '\\' ) );
        }
    }

    //遍历存放目录的数组,创建每层目录
    std::vector<CString>::const_iterator vIter;
    for ( vIter = vPath.begin(); vIter != vPath.end(); vIter++ )
    {
        //如果CreateDirectory执行成功,返回true,否则返回false
        bSuccess = CreateDirectory( *vIter, NULL ) ? true : false;
    }

    return bSuccess;
}

void CFindFileTestDlg::OnBnClickedButton3()
{
    // CreateDirectory只能创建一个目录
    // 如果有多层目录，需要逐层的创建目录

    CString dir = _T( "d:\\mydir\\xx\\yy" );
//	if(CreateDirectory(dir, NULL))
    if( CreateMultipleDirectory( dir ) )
    {
        MessageBox( _T( "创建文件夹成功" ) );
    }
    else
    {
        MessageBox( _T( "创建文件夹失败" ) );
    }
}

bool IsFileExist( const CString& filePath )
{
    CFileFind ff;
    return ff.FindFile( filePath );
}

void CFindFileTestDlg::OnBnClickedButton4()
{
    //CString dir = _T("C:\\Users\\anheihb03dlj\\Desktop\\data\\");
    //CreateMultipleDirectory(dir);

    //CStdioFile outfile;
    //if(!outfile.Open(dir+_T("testfile.txt"), CFile::modeCreate | CFile::modeWrite))
    //{
    //	MessageBox(_T("创建文件失败"));
    //	return;
    //}

    //// 写入汉字失败???
    //outfile.WriteString(_T("aaa其它的好的"));
    //outfile.WriteString(_T("\nadkfsd1233333333333333"));

    //outfile.Close();

    //MessageBox(_T("创建文件成功"));

    CString appDir = _T( "D:\\Test\\ARXTest\\ARX_FFC\\Win32\\Debug\\" );
    if( IsFileExist( appDir + _T( "ffc.msh" ) ) )
    {
        MessageBox( _T( "存在" ) );
    }
    else
    {
        MessageBox( _T( "不存在" ) );
    }
    //CString str = _T("我的太阳");
    //LPTSTR pBuffer = str.GetBuffer();

    //str.ReleaseBuffer(-1);

    //CString msg;
    //msg.Format(_T("%d\n%d"), str.GetLength(), _tcslen(pBuffer));
    //MessageBox(msg);
}
