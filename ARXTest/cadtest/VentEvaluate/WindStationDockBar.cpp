#include "StdAfx.h"
#include "resource.h"
#include "WindStationDockBar.h"

#include "config.h"

IMPLEMENT_DYNAMIC ( WindStationDockBar, CAcUiDockControlBar )

BEGIN_MESSAGE_MAP( WindStationDockBar, CAcUiDockControlBar )
    ON_WM_CREATE()
    ON_WM_SYSCOMMAND()
    ON_WM_SIZE()
END_MESSAGE_MAP()

//-----------------------------------------------------------------------------
//----- WindStationDockBar *pInstance = new WindStationDockBar;
//----- pInstance->Create (acedGetAcadFrame (), "My title bar") ;
//----- pInstance->EnableDocking (CBRS_ALIGN_ANY) ;
//----- pInstance->RestoreControlBar () ;

//-----------------------------------------------------------------------------
static CLSID clsWindStationDockBar = {0x2a2252a7, 0xec81, 0x4716, { 0x8a, 0xb9, 0xc2, 0xd1, 0xd0, 0x22, 0xfe, 0xce }} ;


//-----------------------------------------------------------------------------
WindStationDockBar::WindStationDockBar () : CAcUiDockControlBar()
{
}

//-----------------------------------------------------------------------------
WindStationDockBar::~WindStationDockBar ()
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
BOOL WindStationDockBar::Create ( CWnd* pParent, LPCTSTR lpszTitle )
{
    CString strWndClass ;
    strWndClass = AfxRegisterWndClass ( CS_DBLCLKS, LoadCursor ( NULL, IDC_ARROW ) ) ;
    CRect rect ( 0, 0, DOCK_BAR_WIDTH_3, DOCK_BAR_HEIGHT_3 ) ;
    if ( !CAcUiDockControlBar::Create (
                strWndClass, lpszTitle, WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
                rect, pParent, 0
            )
       )
        return ( FALSE ) ;

    SetToolID ( &clsWindStationDockBar ) ;

    // TODO: Add your code here

    return ( TRUE ) ;
}

//-----------------------------------------------------------------------------
//----- This member function is called when an application requests the window be
//----- created by calling the Create or CreateEx member function
int WindStationDockBar::OnCreate ( LPCREATESTRUCT lpCreateStruct )
{
    if ( CAcUiDockControlBar::OnCreate ( lpCreateStruct ) == -1 )
        return ( -1 ) ;

    //----- Point to our resource
    CAcModuleResourceOverride resourceOverride;
    //----- Create it and set the parent as the dockctrl bar

    mChildDlg.Create ( IDD_WIND_STATION_DLG, this ) ;

    CRect rect ;
    GetWindowRect ( &rect ) ;
    //- Move the window over so we can see the control lines
    mChildDlg.MoveWindow ( 0, 0, rect.Width (), rect.Height (), TRUE ) ;

    return ( 0 ) ;
}

//-----------------------------------------------------------------------------
void WindStationDockBar::SizeChanged ( CRect* lpRect, BOOL bFloating, int flags )
{
    // If valid
    if ( ::IsWindow ( mChildDlg.GetSafeHwnd () ) )
    {
        //----- Always point to our resource to be safe
        CAcModuleResourceOverride resourceOverride ;
        //----- Then update its window size relatively
        mChildDlg.SetWindowPos ( this, lpRect->left, lpRect->top, lpRect->Width (), lpRect->Height (), SWP_NOZORDER ) ;
    }
}

//-----------------------------------------------------------------------------
//-----  Function called when user selects a command from Control menu or when user
//----- selects the Maximize or the Minimize button.
void WindStationDockBar::OnSysCommand ( UINT nID, LPARAM lParam )
{
    CAcUiDockControlBar::OnSysCommand ( nID, lParam ) ;
}

//-----------------------------------------------------------------------------
//----- The framework calls this member function after the window's size has changed
void WindStationDockBar::OnSize ( UINT nType, int cx, int cy )
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

bool WindStationDockBar::OnClosing()
{
    bool ret = CAdUiDockControlBar::OnClosing();
    //MessageBox(_T("调用WindStationDockBar::OnClosing()"));

    /* 手动添加 */
    mChildDlg.OnClosing();

    return ret;
}