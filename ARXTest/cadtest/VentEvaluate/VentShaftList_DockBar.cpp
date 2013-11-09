#include "StdAfx.h"
#include "resource.h"
#include "VentShaftList_DockBar.h"

// 对话框大小配置文件
#include "config.h"

IMPLEMENT_DYNAMIC ( VentShaftList_DockBar, CAcUiDockControlBar )

BEGIN_MESSAGE_MAP( VentShaftList_DockBar, CAcUiDockControlBar )
    ON_WM_CREATE()
    ON_WM_SYSCOMMAND()
    ON_WM_SIZE()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
//----- VentShaftList_DockBar *pInstance = new VentShaftList_DockBar;
//----- pInstance->Create (acedGetAcadFrame (), "My title bar") ;
//----- pInstance->EnableDocking (CBRS_ALIGN_ANY) ;
//----- pInstance->RestoreControlBar () ;

//-----------------------------------------------------------------------------
static CLSID clsVentShaftList_DockBar = {0x936e0b2c, 0xf7ff, 0x4ef2, { 0xbb, 0x33, 0x68, 0xf6, 0x8d, 0xd5, 0x61, 0x73 }} ;


//-----------------------------------------------------------------------------
VentShaftList_DockBar::VentShaftList_DockBar () : CAcUiDockControlBar()
{
}

//-----------------------------------------------------------------------------
VentShaftList_DockBar::~VentShaftList_DockBar ()
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
BOOL VentShaftList_DockBar::Create ( CWnd* pParent, LPCTSTR lpszTitle )
{
    CString strWndClass ;
    strWndClass = AfxRegisterWndClass ( CS_DBLCLKS, LoadCursor ( NULL, IDC_ARROW ) ) ;
    CRect rect ( 0, 0, DOCK_BAR_WIDTH_7, DOCK_BAR_HEIGHT_7 ) ;
    if ( !CAcUiDockControlBar::Create (
                strWndClass, lpszTitle, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
                rect, pParent, 0
            )
       )
        return ( FALSE ) ;

    SetToolID ( &clsVentShaftList_DockBar ) ;

    // TODO: Add your code here

    return ( TRUE ) ;
}

//-----------------------------------------------------------------------------
//----- This member function is called when an application requests the window be
//----- created by calling the Create or CreateEx member function
int VentShaftList_DockBar::OnCreate ( LPCREATESTRUCT lpCreateStruct )
{
    if ( CAcUiDockControlBar::OnCreate ( lpCreateStruct ) == -1 )
        return ( -1 ) ;

    //----- Point to our resource
    CAcModuleResourceOverride resourceOverride;
    //----- Create it and set the parent as the dockctrl bar
    mChildDlg.Create ( IDD_VENT_SHAFT_LIST_DLG, this ) ;

    CRect rect ;
    GetWindowRect ( &rect ) ;
    //- Move the window over so we can see the control lines
    mChildDlg.MoveWindow ( 0, 0, rect.Width (), rect.Height (), TRUE ) ;

    return ( 0 ) ;
}

//-----------------------------------------------------------------------------
void VentShaftList_DockBar::SizeChanged ( CRect* lpRect, BOOL bFloating, int flags )
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
void VentShaftList_DockBar::OnSysCommand ( UINT nID, LPARAM lParam )
{
    CAcUiDockControlBar::OnSysCommand ( nID, lParam ) ;
}

//-----------------------------------------------------------------------------
//----- The framework calls this member function after the window's size has changed
void VentShaftList_DockBar::OnSize ( UINT nType, int cx, int cy )
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

bool VentShaftList_DockBar::OnClosing()
{
    bool ret = CAdUiDockControlBar::OnClosing();
    //MessageBox(_T("调用AirSetDockBar::OnClosing()"));

    /* 手动添加 */
    mChildDlg.OnClosing();

    return ret;
}