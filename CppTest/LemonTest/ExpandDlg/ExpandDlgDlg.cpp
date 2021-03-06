
// ExpandDlgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ExpandDlg.h"
#include "ExpandDlgDlg.h"

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


// CExpandDlgDlg 对话框




CExpandDlgDlg::CExpandDlgDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( CExpandDlgDlg::IDD, pParent )
{
    m_hIcon = AfxGetApp()->LoadIcon( IDR_MAINFRAME );
}

void CExpandDlgDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP( CExpandDlgDlg, CDialog )
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_BN_CLICKED( IDC_BUTTON2, &CExpandDlgDlg::OnBnClickedButton2 )
    ON_BN_CLICKED( IDC_BUTTON3, &CExpandDlgDlg::OnBnClickedButton3 )
END_MESSAGE_MAP()


// CExpandDlgDlg 消息处理程序

BOOL CExpandDlgDlg::OnInitDialog()
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

    m_childDlg.Create( IDD_DIALOG1, this );
    //SetWindowLong(m_childDlg.GetSafeHwnd(), GWL_STYLE, WS_CHILD | WS_VISIBLE);

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CExpandDlgDlg::OnSysCommand( UINT nID, LPARAM lParam )
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

void CExpandDlgDlg::OnPaint()
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
HCURSOR CExpandDlgDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>( m_hIcon );
}


void CExpandDlgDlg::OnBnClickedButton2()
{
    CRect rect;
    GetDlgItem( IDC_POS )->GetWindowRect( &rect );
    ScreenToClient( &rect );

    CString msg;
    msg.Format( _T( "left:%d, right:%d, top:%d, bottom:%d, width:%d, height:%d" ), rect.left, rect.right, rect.top, rect.bottom, rect.Width(), rect.Height() );

    CRect dlgRect;
    m_childDlg.GetWindowRect( &dlgRect );
    msg.AppendFormat( _T( "\nleft:%d, right:%d, top:%d, bottom:%d, width:%d, height:%d" ), dlgRect.left, dlgRect.right, dlgRect.top, dlgRect.bottom, dlgRect.Width(), dlgRect.Height() );

    ScreenToClient( &dlgRect );
    msg.AppendFormat( _T( "\n[C]left:%d, right:%d, top:%d, bottom:%d, width:%d, height:%d" ), dlgRect.left, dlgRect.right, dlgRect.top, dlgRect.bottom, dlgRect.Width(), dlgRect.Height() );

    MessageBox( msg );

    dlgRect.left = rect.left;
    dlgRect.top = rect.bottom;
    dlgRect.right += rect.left;
    dlgRect.bottom += rect.bottom;

    CRect thisDlgRect;
    GetWindowRect( &thisDlgRect );
    thisDlgRect.bottom += dlgRect.Height();
    SetWindowPos( NULL, 0, 0, thisDlgRect.Width(), thisDlgRect.Height(), SWP_NOMOVE | SWP_NOZORDER );

    m_childDlg.MoveWindow( &dlgRect );
    m_childDlg.ShowWindow( SW_SHOW );
}

#include "TestDlg.h"

void CExpandDlgDlg::OnBnClickedButton3()
{
    TestDlg dlg;
    dlg.DoModal();
}
