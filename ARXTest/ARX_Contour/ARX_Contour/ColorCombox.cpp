#include "stdafx.h"
#include "ColorCombox.h"

CColorCombox::CColorCombox()
{

}

CColorCombox::~CColorCombox()
{

}

BEGIN_MESSAGE_MAP( CColorCombox, CComboBox )
    //{{AFX_MSG_MAP(CColorCombox)
    //ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
    ON_CONTROL_REFLECT( CBN_SELENDOK, OnSelendok )
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorCombox message handlers

void CColorCombox::DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct )
{
    //验证是否为组合框控件
    ASSERT( lpDrawItemStruct->CtlType == ODT_COMBOBOX );
    CDC dc ;
    dc.Attach( lpDrawItemStruct->hDC );

    //获取项目区域
    CRect itemRC ( lpDrawItemStruct->rcItem );
    //定义显示颜色的区域
    CRect clrRC = itemRC;
    //定义文本区域
    CRect textRC = itemRC;
    //获取系统文本颜色
    COLORREF clrText = GetSysColor( COLOR_WINDOWTEXT );
    //选中时的文本颜色
    COLORREF clrSelected = GetSysColor( COLOR_HIGHLIGHT );
    //获取窗口背景颜色
    COLORREF clrNormal = GetSysColor( COLOR_WINDOW );
    //获取当前项目索引
    int nIndex = lpDrawItemStruct->itemID;
    //判断项目状态
    int nState = lpDrawItemStruct->itemState;
    if( nState & ODS_SELECTED )	//处于选中状态
    {
        dc.SetTextColor( ( 0x00FFFFFF & ~( clrText ) ) );		//文本颜色取反
        dc.SetBkColor( clrSelected );						//设置文本背景颜色
        dc.FillSolidRect( &clrRC, clrSelected );			//填充项目区域为高亮效果
    }
    else
    {
        dc.SetTextColor( clrText );						//设置正常的文本颜色
        dc.SetBkColor( clrNormal );						//设置正常的文本背景颜色
        dc.FillSolidRect( &clrRC, clrNormal );
    }
    if( nState & ODS_FOCUS )								//如果项目获取焦点，绘制焦点区域
    {
        dc.DrawFocusRect( &itemRC );
    }

    //计算文本区域
    int nclrWidth = itemRC.Width() / 4;
    textRC.left = nclrWidth + 55;

    //计算颜色显示区域
    clrRC.DeflateRect( 2, 2 );
    clrRC.right = nclrWidth + 50;

    //绘制颜色文本并且填充颜色区域
    if ( nIndex != -1 )	//项目不为空
    {
        //获取项目颜色
        COLORREF clrItem = GetItemData( nIndex );
        dc.SetBkMode( TRANSPARENT );
        //获取文本
        CString szText;
        GetLBText( nIndex, szText );
        //输出文本
        dc.DrawText( szText, textRC, DT_LEFT | DT_VCENTER | DT_SINGLELINE );
        dc.FillSolidRect( &clrRC, clrItem );
        //输出颜色
        dc.FrameRect( &clrRC, &CBrush( RGB( 0, 0, 0 ) ) );
    }
    dc.Detach();
}

int CColorCombox::AddItem( LPCTSTR lpszText, COLORREF clrValue )
{
    int nIndex = AddString( lpszText );
    SetItemData( nIndex, clrValue );
    return nIndex;
}

void CColorCombox::InitColorCB()
{
    AddItem( _T( "红" ), RGB( 255, 0, 0 ) );
    AddItem( _T( "黄" ), RGB( 255, 255, 0 ) );
    AddItem( _T( "绿" ), RGB( 0, 255, 0 ) );
    AddItem( _T( "蓝" ), RGB( 0, 0, 255 ) );
    AddItem( _T( "其它" ), RGB( 255, 255, 255 ) );

    SetCurSel( 4 );
}

COLORREF CColorCombox::GetCurColor()
{
    int nIndex = GetCurSel();
    if ( nIndex != -1 )
    {
        return GetItemData( nIndex );
    }
    else
        return -1;
}

void CColorCombox::SetCurColor( COLORREF clr )
{
    int pos = CB_ERR;
    int n = GetCount();
    for( int i = 0; i < n; i++ )
    {
        if( GetItemData( i ) == clr )
        {
            pos = i;
            break;
        }
    }

    if( pos == CB_ERR )
    {
        pos = n - 1;
        SetItemData( pos, clr );
    }

    SetCurSel( pos );
}

void CColorCombox::OnSelendok()
{
    CString lpCaption;
    GetLBText( this->GetCurSel(), lpCaption );
    if( lpCaption.CompareNoCase( _T( "其它" ) ) == 0 )
    {
        CColorDialog crDlg( GetCurColor(), CC_FULLOPEN );
        if( crDlg.DoModal() == IDOK )
        {
            SetItemData( this->GetCurSel(), crDlg.GetColor() );
            Invalidate();
        }
    }
}
