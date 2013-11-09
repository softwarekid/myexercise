// HelpInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "HelpInfoDlg.h"


// HelpInfoDlg 对话框

IMPLEMENT_DYNAMIC( HelpInfoDlg, CDialog )

HelpInfoDlg::HelpInfoDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( HelpInfoDlg::IDD, pParent )
{

}

HelpInfoDlg::~HelpInfoDlg()
{
}

void HelpInfoDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
}


BEGIN_MESSAGE_MAP( HelpInfoDlg, CDialog )
    ON_WM_PAINT()
END_MESSAGE_MAP()

static void DrawTextString( CDC* pDC, const CString& str, CRect& rect, const CString& fontname, COLORREF crColor, double height, bool bBold )
{
    // 绘制标题
    CFont* pFontOld = ( CFont* ) pDC->SelectStockObject( DEFAULT_GUI_FONT );

    CFont font;
    font.CreateFont(
        height,
        0,
        0,
        0,
        ( bBold ? FW_BOLD : FW_NORMAL ),
        FALSE,
        FALSE,
        FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY,
        VARIABLE_PITCH | FF_SCRIPT,
        fontname );

    pDC->SetTextColor( crColor );
    //pDC->SetBkColor(RGB(255,255,0));

    pDC->SelectObject( &font );

    pDC->DrawText( str, &rect, DT_CENTER | /*DT_VCENTER | */DT_WORDBREAK );

    pDC->SelectObject ( pFontOld );
}

void HelpInfoDlg::OnPaint()
{
    CPaintDC dc( this ); // device context for painting

    dc.SetBkMode( TRANSPARENT );

    DrawTitle1( &dc );
    DrawTitle2( &dc );
    DrawTitle3( &dc );
    DrawTitle4( &dc );
}

void HelpInfoDlg::DrawTitle1( CDC* pDC )
{
    CRect rect;
    GetDlgItem( IDC_TITLE1 )->GetWindowRect( &rect );
    ScreenToClient( &rect );
    DrawTextString( pDC, _T( "矿井通风仿真系统JLMVSS Ver1.0" ), rect, _T( "宋体" ), RGB( 0, 0, 255 ), 40, true );
}

void HelpInfoDlg::DrawTitle2( CDC* pDC )
{
    CRect rect;
    GetDlgItem( IDC_TITLE2 )->GetWindowRect( &rect );
    ScreenToClient( &rect );

    // 将矩形区域3分
    int d = rect.Height() / 3;

    CRect block_rect;
    block_rect.top = rect.top;
    block_rect.bottom = rect.top + d;
    block_rect.left = rect.left;
    block_rect.right = rect.right;
    DrawTextString(
        pDC,
        _T( "上海基蓝软件公司" ),
        block_rect,
        _T( "宋体" ),
        RGB( 0, 162, 232 ),
        25,
        true );

    block_rect.top = block_rect.bottom;
    block_rect.bottom = block_rect.top + d;
    DrawTextString(
        pDC,
        _T( "辽宁工程技术大学安全科学与工程学院" ),
        block_rect,
        _T( "宋体" ),
        RGB( 0, 162, 232 ),
        25,
        true );

    block_rect.top = block_rect.bottom;
    block_rect.bottom = block_rect.top + d;
    DrawTextString(
        pDC,
        _T( "联合研制" ),
        block_rect,
        _T( "宋体" ),
        RGB( 0, 162, 232 ),
        25,
        true );
}

void HelpInfoDlg::DrawTitle3( CDC* pDC )
{
    CRect rect;
    GetDlgItem( IDC_TITLE3 )->GetWindowRect( &rect );
    ScreenToClient( &rect );
    DrawTextString( pDC, _T( "Copyright(c) 2000-2011" ), rect, _T( "Arial" ), RGB( 0, 162, 232 ), 30, true );
}

void HelpInfoDlg::DrawTitle4( CDC* pDC )
{
    CRect rect;
    GetDlgItem( IDC_TITLE4 )->GetWindowRect( &rect );
    ScreenToClient( &rect );

    DrawTextString( pDC, _T( "项目技术支持：刘剑【13904988193, 1j1961@vip.sina.com】" ), rect, _T( "宋体" ), RGB( 255, 0, 0 ), 20, true );

    //// 将矩形区域2分
    //int d = rect.Height()/2;

    //CRect block_rect;
    //block_rect.top = rect.top;
    //block_rect.bottom = rect.top+d;
    //block_rect.left = rect.left;
    //block_rect.right = rect.right;
    //DrawTextString(pDC, _T("矿井通风仿真系统JLMVSS Ver1.0"), rect, _T("宋体"), RGB(255, 0, 0), 20, true);

    //block_rect.top = block_rect.bottom;
    //block_rect.bottom = block_rect.top+d;
    //DrawTextString(pDC, _T("矿井通风仿真系统JLMVSS Ver1.0"), rect, _T("宋体"), RGB(255, 0, 0), 20, true);
}