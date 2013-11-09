// CoolBar.cpp : implementation file
//

#include "stdafx.h"
#include "contour.h"
#include "CoolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCoolBar dialog


CCoolBar::CCoolBar( CWnd* pParent /*=NULL*/ )
    : CDialog( CCoolBar::IDD, pParent )
{
    //{{AFX_DATA_INIT(CCoolBar)
    m_staticxyz = _T( "" );
    //}}AFX_DATA_INIT
}


void CCoolBar::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    //{{AFX_DATA_MAP(CCoolBar)
    DDX_Text( pDX, IDC_STATICXYZ, m_staticxyz );
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP( CCoolBar, CDialog )
    //{{AFX_MSG_MAP(CCoolBar)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCoolBar message handlers
