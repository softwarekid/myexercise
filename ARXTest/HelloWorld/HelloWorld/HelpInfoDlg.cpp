// HelpInfoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "HelpInfoDlg.h"


// HelpInfoDlg �Ի���

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
    // ���Ʊ���
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
    DrawTextString( pDC, _T( "��ͨ�����ϵͳJLMVSS Ver1.0" ), rect, _T( "����" ), RGB( 0, 0, 255 ), 40, true );
}

void HelpInfoDlg::DrawTitle2( CDC* pDC )
{
    CRect rect;
    GetDlgItem( IDC_TITLE2 )->GetWindowRect( &rect );
    ScreenToClient( &rect );

    // ����������3��
    int d = rect.Height() / 3;

    CRect block_rect;
    block_rect.top = rect.top;
    block_rect.bottom = rect.top + d;
    block_rect.left = rect.left;
    block_rect.right = rect.right;
    DrawTextString(
        pDC,
        _T( "�Ϻ�����������˾" ),
        block_rect,
        _T( "����" ),
        RGB( 0, 162, 232 ),
        25,
        true );

    block_rect.top = block_rect.bottom;
    block_rect.bottom = block_rect.top + d;
    DrawTextString(
        pDC,
        _T( "�������̼�����ѧ��ȫ��ѧ�빤��ѧԺ" ),
        block_rect,
        _T( "����" ),
        RGB( 0, 162, 232 ),
        25,
        true );

    block_rect.top = block_rect.bottom;
    block_rect.bottom = block_rect.top + d;
    DrawTextString(
        pDC,
        _T( "��������" ),
        block_rect,
        _T( "����" ),
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

    DrawTextString( pDC, _T( "��Ŀ����֧�֣�������13904988193, 1j1961@vip.sina.com��" ), rect, _T( "����" ), RGB( 255, 0, 0 ), 20, true );

    //// ����������2��
    //int d = rect.Height()/2;

    //CRect block_rect;
    //block_rect.top = rect.top;
    //block_rect.bottom = rect.top+d;
    //block_rect.left = rect.left;
    //block_rect.right = rect.right;
    //DrawTextString(pDC, _T("��ͨ�����ϵͳJLMVSS Ver1.0"), rect, _T("����"), RGB(255, 0, 0), 20, true);

    //block_rect.top = block_rect.bottom;
    //block_rect.bottom = block_rect.top+d;
    //DrawTextString(pDC, _T("��ͨ�����ϵͳJLMVSS Ver1.0"), rect, _T("����"), RGB(255, 0, 0), 20, true);
}