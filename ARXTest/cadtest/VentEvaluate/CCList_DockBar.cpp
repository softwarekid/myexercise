#include "StdAfx.h"
#include "resource.h"
#include "CCList_DockBar.h"

// 用于对话框大小的配置文件
#include "config.h"

IMPLEMENT_DYNAMIC ( CCList_DockBar, CAcUiDockControlBar )

BEGIN_MESSAGE_MAP( CCList_DockBar, CAcUiDockControlBar )
    ON_WM_CREATE()
    ON_WM_SYSCOMMAND()
    ON_WM_SIZE()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
//----- CCList_DockBar *pInstance = new CCList_DockBar;
//----- pInstance->Create (acedGetAcadFrame (), "My title bar") ;
//----- pInstance->EnableDocking (CBRS_ALIGN_ANY) ;
//----- pInstance->RestoreControlBar () ;

//-----------------------------------------------------------------------------
static CLSID clsCCList_DockBar = {0xb7cf00d7, 0xc985, 0x42ba, { 0xbf, 0x29, 0xcc, 0x5d, 0xd9, 0x9b, 0x1f, 0x6f }} ;


//-----------------------------------------------------------------------------
CCList_DockBar::CCList_DockBar () : CAcUiDockControlBar()
{
}

//-----------------------------------------------------------------------------
CCList_DockBar::~CCList_DockBar ()
{

}

//-----------------------------------------------------------------------------
#ifdef _DEBUG
//- Please uncomment the 2 following lines to avoid linker error when compiling
//- in release mode. But make sure to uncomment these lines only once per project
//- if you derive multiple times from CAdUiDockControlBar/CAcUiDockControlBar
//- classes.

//void CAdUiDockControlBar::AssertValid () const {
//}
#endif

//-----------------------------------------------------------------------------
BOOL CCList_DockBar::Create ( CWnd* pParent, LPCTSTR lpszTitle )
{
    CString strWndClass ;
    strWndClass = AfxRegisterWndClass ( CS_DBLCLKS, LoadCursor ( NULL, IDC_ARROW ) ) ;
    CRect rect ( 0, 0, DOCK_BAR_WIDTH_8, DOCK_BAR_HEIGHT_8 ) ;
    if ( !CAcUiDockControlBar::Create (
                strWndClass, lpszTitle, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
                rect, pParent, 0
            )
       )
        return ( FALSE ) ;

    SetToolID ( &clsCCList_DockBar ) ;


    return ( TRUE ) ;
}

//-----------------------------------------------------------------------------
//----- This member function is called when an application requests the window be
//----- created by calling the Create or CreateEx member function
int CCList_DockBar::OnCreate ( LPCREATESTRUCT lpCreateStruct )
{
    if ( CAcUiDockControlBar::OnCreate ( lpCreateStruct ) == -1 )
        return ( -1 ) ;

    //----- Point to our resource
    CAcModuleResourceOverride resourceOverride;
    //----- Create it and set the parent as the dockctrl bar
    mChildDlg.Create ( IDD_CC_LIST_DLG, this ) ;

    CRect rect ;
    GetWindowRect ( &rect ) ;
    //- Move the window over so we can see the control lines
    mChildDlg.MoveWindow ( 0, 0, rect.Width (), rect.Height (), TRUE ) ;

    return ( 0 ) ;
}

//-----------------------------------------------------------------------------
void CCList_DockBar::SizeChanged ( CRect* lpRect, BOOL bFloating, int flags )
{
    // If valid
    if ( ::IsWindow ( mChildDlg.GetSafeHwnd () ) )
    {
        //----- Always point to our resource to be safe
        CAcModuleResourceOverride resourceOverride ;
        //----- Then update its window size relatively
        mChildDlg.SetWindowPos ( this, lpRect->left , lpRect->top , lpRect->Width (), lpRect->Height (), SWP_NOZORDER ) ;
    }
}

//-----------------------------------------------------------------------------
//-----  Function called when user selects a command from Control menu or when user
//----- selects the Maximize or the Minimize button.
void CCList_DockBar::OnSysCommand ( UINT nID, LPARAM lParam )
{
    CAcUiDockControlBar::OnSysCommand ( nID, lParam ) ;
}

//-----------------------------------------------------------------------------
//----- The framework calls this member function after the window's size has changed
void CCList_DockBar::OnSize ( UINT nType, int cx, int cy )
{
    CAcUiDockControlBar::OnSize ( nType, cx, cy ) ;
    // If valid
    if ( ::IsWindow ( mChildDlg.GetSafeHwnd () ) )
    {
        //----- Always point to our resource to be safe
        CAcModuleResourceOverride resourceOverride ;
        //----- then update its window position relatively
        mChildDlg.MoveWindow ( 0, 0, cx, cy ) ;
    }
}

bool CCList_DockBar::OnClosing()
{
    bool ret = CAdUiDockControlBar::OnClosing();
    //MessageBox(_T("调用AirSetDockBar::OnClosing()"));

    /* 手动添加 */
    mChildDlg.OnClosing();

    return ret;
}