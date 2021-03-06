
// RunFluentDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RunFluentApp.h"
#include "RunFluentDlg.h"

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


// CRunFluentDlg 对话框




CRunFluentDlg::CRunFluentDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( CRunFluentDlg::IDD, pParent )
{
    m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
}

void CRunFluentDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP( CRunFluentDlg, CDialog )
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_BN_CLICKED( IDC_BUTTON1, &CRunFluentDlg::OnBnClickedButton1 )
    ON_BN_CLICKED( IDC_BUTTON2, &CRunFluentDlg::OnBnClickedButton2 )
    ON_BN_CLICKED( IDC_BUTTON3, &CRunFluentDlg::OnBnClickedButton3 )
    ON_BN_CLICKED( IDC_BUTTON4, &CRunFluentDlg::OnBnClickedButton4 )
    ON_BN_CLICKED( IDC_BUTTON5, &CRunFluentDlg::OnBnClickedButton5 )
END_MESSAGE_MAP()


// CRunFluentDlg 消息处理程序

BOOL CRunFluentDlg::OnInitDialog()
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

void CRunFluentDlg::OnSysCommand( UINT nID, LPARAM lParam )
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

void CRunFluentDlg::OnPaint()
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
HCURSOR CRunFluentDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>( m_hIcon );
}

#include "FFC_Def.h"
#include "SysTool.h"
extern bool RunFluent( const CString& appDir );
extern bool RunGambit( const CString& appDir );
extern bool RunTecplot( const CString& appDir, int type );

void CRunFluentDlg::OnBnClickedButton1()
{
    CString appDir = GetAppPathDir();
    if( RunFluent( appDir ) )
    {
        MessageBox( _T( "运行Fluent成功" ) );
    }
    else
    {
        MessageBox( _T( "运行Fluent失败" ) );
    }
}

void CRunFluentDlg::OnBnClickedButton2()
{
    CString appDir = GetAppPathDir();
    if( RunGambit( appDir ) )
    {
        MessageBox( _T( "运行Gambit成功" ) );
    }
    else
    {
        MessageBox( _T( "运行Gambit失败" ) );
    }
}

void CRunFluentDlg::OnBnClickedButton3()
{
    CString appDir = GetAppPathDir();

    if( RunTecplot( appDir, TST_VELOCITY_NEPHOGRAM ) )
    {
        MessageBox( _T( "运行Tecplot成功" ) );
    }
    else
    {
        MessageBox( _T( "运行Tecplot失败" ) );
    }
}

void CRunFluentDlg::OnBnClickedButton4()
{
    //KillProcess(_T("notepad.exe"));
    KillProcess( _T( "gambit.exe" ) );
    KillProcess( _T( "exceed.exe" ) );
    KillProcess( _T( "fluent.exe" ) );
    KillProcess( _T( "fl6326s.exe" ) );
}

static void xxx()
{
    //TCHAR varName[] = _T("Path");
    TCHAR varName[] = _T( "testxx" );

    HKEY hkUser;
    if( ERROR_SUCCESS != RegOpenCurrentUser( KEY_ALL_ACCESS, &hkUser ) )
    {
        //AfxMessageBox(_T())
        return;
    }

    HKEY hkResult;
    if( ERROR_SUCCESS != RegOpenKeyEx( hkUser, _T( "Environment" ), 0, KEY_ALL_ACCESS, &hkResult ) )
    {
        AfxMessageBox( _T( "访问注册表出错！" ) );
        return;
    }

    DWORD BufferSize = 8192;
    DWORD cbData;
    DWORD dwRet;

    VOID* PerfData = malloc( BufferSize );
    cbData = BufferSize;

    dwRet = RegQueryValueEx( hkResult,
                             varName,
                             NULL,
                             NULL,
                             ( LPBYTE ) PerfData,
                             &cbData );

    while ( dwRet == ERROR_MORE_DATA )
    {
        BufferSize += 4096;
        PerfData = realloc( PerfData, BufferSize );
        cbData = BufferSize;

        dwRet = RegQueryValueEx( hkResult,
                                 varName,
                                 NULL,
                                 NULL,
                                 ( LPBYTE ) PerfData,
                                 &cbData );

    }

    AfxMessageBox( ( LPCTSTR )PerfData );

    LPTSTR javaHome = _T( ";ddd" );

    TCHAR* temp = new TCHAR[cbData + sizeof( javaHome )];

    _tcscpy_s( temp, cbData, ( LPTSTR )PerfData );

    LPTSTR path = _tcscat( temp, javaHome );

    RegSetValueEx( hkResult,
                   varName,
                   0,
                   REG_EXPAND_SZ,                // REG_SZ
                   ( LPBYTE ) path,
                   _tcsclen( path )*sizeof( TCHAR ) );

    DWORD dwResult;

    SendMessageTimeout( HWND_BROADCAST, WM_SETTINGCHANGE, 0, LPARAM( "Environment" ), SMTO_ABORTIFHUNG, 5000, &dwResult );

    delete temp;
}

void CRunFluentDlg::OnBnClickedButton5()
{
    //TCHAR name[] = _T("Path");
    //TCHAR value1[1000];

    //DWORD dwRet = GetEnvironmentVariable(name, value1, 1000);

    //TCHAR name2[] = _T("myvar");
    //TCHAR value2[100];

    //SetEnvironmentVariable(name2, _T("d:\\xxx"));
    ///*DWORD dwRet = */GetEnvironmentVariable(name2, value2, 100);

    //MessageBox(value2);

    xxx();
}
