// hildDlg1.cpp : 实现文件
//

#include "stdafx.h"
#include "SwitchDlgTest.h"
#include "ChildDlg1.h"


// ChildDlg1 对话框

IMPLEMENT_DYNAMIC( ChildDlg1, CDialog )

ChildDlg1::ChildDlg1( CWnd* pParent /*=NULL*/ )
    : CDialog( ChildDlg1::IDD, pParent )
{

}

ChildDlg1::~ChildDlg1()
{
}

void ChildDlg1::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
}


BEGIN_MESSAGE_MAP( ChildDlg1, CDialog )
END_MESSAGE_MAP()


// ChildDlg1 消息处理程序
