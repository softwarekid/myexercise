#include "stdafx.h"
#include "PropertyDataDlg.h"

#include "../MineGE/HelperClass.h"
#include "../PropData/HelperClass.h"

IMPLEMENT_DYNAMIC( PropertyDataDlg, CDialog )

PropertyDataDlg::PropertyDataDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( PropertyDataDlg::IDD, pParent )
{
    m_showAll = true; // 默认显示全部数据
}

PropertyDataDlg::~PropertyDataDlg()
{
}

void PropertyDataDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP( PropertyDataDlg, CDialog )
    ON_BN_CLICKED( IDOK, &PropertyDataDlg::OnBnClickedOk )
END_MESSAGE_MAP()


// PropertyDataDlg 消息处理程序

BOOL PropertyDataDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // 创建, 定位, 显示CMFCPropertyGridCtrl
    CRect rect;
    GetDlgItem( IDC_PROP_POS )->GetWindowRect( &rect );
    ScreenToClient( &rect );
    m_propertyDataList.Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER, rect, this, ( UINT ) - 1 );

    // 获取类型名称
    CString type;
    DataHelper::GetTypeName( m_objId, type );

    // 获取字段
    AcStringArray fileds;
    if( m_showAll )
    {
        FieldHelper::GetAllFields( type, fileds );
    }
    else
    {
        fileds.append( m_fields );
    }

    // 创建属性数据控件
    PropertyDataUpdater::BuildPropGridCtrl( &m_propertyDataList, type, fileds );

    // 填充属性数据
    bool ret = PropertyDataUpdater::ReadDataFromGE( &m_propertyDataList, m_objId );

    // 填充数据失败
    if( !ret )
    {
        m_propertyDataList.EnableWindow( FALSE );
        m_propertyDataList.ShowWindow( SW_HIDE );
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常: OCX 属性页应返回 FALSE
}

void PropertyDataDlg::OnBnClickedOk()
{
    UpdateData( TRUE ); // 更新控件

    // 更新图元的属性数据
    PropertyDataUpdater::WriteDataToGE( &m_propertyDataList, m_objId );

    OnOK();
}

void PropertyDataDlg::setMineGE( const AcDbObjectId& objId )
{
    m_objId = objId;
}

void PropertyDataDlg::addField( const CString& field )
{
    m_fields.append( field );
}

void PropertyDataDlg::showAllData( bool bFlag )
{
    m_showAll = bFlag;
}