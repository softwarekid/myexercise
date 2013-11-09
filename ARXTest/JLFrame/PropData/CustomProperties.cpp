#include "StdAfx.h"
#include "CustomProperties.h"

//COleVariant(short nSrc, VARTYPE vtSrc = VT_I2);
//COleVariant(long lSrc, VARTYPE vtSrc = VT_I4);
//COleVariant(double dblSrc);
//COleVariant(const COleDateTime& timeSrc);

// 判断字符串是否是一个合法的整数串
// 格式[+/-][0-9]
static bool IsInteger( LPCTSTR pSrc )
{
    if( *pSrc == _T( '+' ) || *pSrc == _T( '-' ) ) pSrc++;
    for( ; *pSrc != _T( '\0' ) && _istdigit( *pSrc ); pSrc++ );
    return ( *pSrc == _T( '\0' ) );
}

// 判断字符串是否是一个合法的浮点数串
// 格式[+/-][0-9][.][0-9]
static bool IsNumeric( LPCTSTR pSrc )
{
    if( *pSrc == _T( '+' ) || *pSrc == _T( '-' ) ) pSrc++;
    for( bool bp = false; _istdigit( *pSrc ) || ( *pSrc == _T( '.' ) && bp == false ); )
    {
        if( *pSrc++ == _T( '.' ) ) bp = true;
    }
    return ( *pSrc == _T( '\0' ) );
}

CIntProp::CIntProp( const CString& strName, const COleVariant& varValue, int nMinValue, int nMaxValue, LPCTSTR lpszDescr ) :
    CMFCPropertyGridProperty( strName, varValue, lpszDescr )
{
    m_nMinValue = nMinValue;
    m_nMaxValue = nMaxValue;
}

BOOL CIntProp::OnUpdateValue()
{
    ASSERT_VALID( this );
    ASSERT_VALID( m_pWndInPlace );
    ASSERT_VALID( m_pWndList );
    ASSERT( ::IsWindow( m_pWndInPlace->GetSafeHwnd() ) );

    BOOL bRet = TRUE;
    CString strText;
    m_pWndInPlace->GetWindowText( strText );

    BOOL bIsChanged = FormatProperty() != strText;
    if ( bIsChanged )
    {
        // 包含非法字符，不是整数
        if( !IsInteger( strText ) )
        {
            static BOOL bRecursedHere = FALSE;
            if ( bRecursedHere ) return TRUE;
            bRecursedHere = TRUE;

            CString msg;
            msg.Format( _T( "包含非法字符\n正确格式为[+/-][0-9]\n[]表示可选\n例如-1024, +104, 20" ) );
            AfxMessageBox( msg );

            bRecursedHere = FALSE;
            return FALSE;
        }
        else
        {
            // 转换成整数
            // 还可使用_stscanf_s方法,参考msdn以及afxpropertygridctrl.cpp中TextVar()方法中的实现
            // msdn搜索atoi
            int nItem = _ttoi( strText );
            if ( ( nItem < m_nMinValue ) || ( nItem > m_nMaxValue ) )
            {
                static BOOL bRecursedHere = FALSE;
                if ( bRecursedHere ) return TRUE;
                bRecursedHere = TRUE;

                CString strMessage;
                strMessage.Format( _T( "超出取值范围：[%d, %d]" ), m_nMinValue, m_nMaxValue );
                AfxMessageBox( strMessage );

                bRecursedHere = FALSE;
                return FALSE;
            }
        }

        bRet = CMFCPropertyGridProperty::OnUpdateValue();

        if ( m_pParent != NULL )
        {
            m_pWndList->OnPropertyChanged( m_pParent );
        }
    }

    return bRet;
}


CNumericProp::CNumericProp( const CString& strName, const COleVariant& varValue, double dMinValue, double dMaxValue, unsigned short precise, LPCTSTR lpszDescr )
    : CMFCPropertyGridProperty( strName, varValue, lpszDescr )
{
    m_dMinValue = dMinValue;
    m_dMaxValue = dMaxValue;
    m_precise = precise;
}

BOOL CNumericProp::OnUpdateValue()
{
    ASSERT_VALID( this );
    ASSERT_VALID( m_pWndInPlace );
    ASSERT_VALID( m_pWndList );
    ASSERT( ::IsWindow( m_pWndInPlace->GetSafeHwnd() ) );

    BOOL bRet = TRUE;
    CString strText;
    m_pWndInPlace->GetWindowText( strText );

    BOOL bIsChanged = FormatProperty() != strText;
    if ( bIsChanged )
    {
        // 不是合法的浮点数字符串
        if( !IsNumeric( strText ) )
        {
            static BOOL bRecursedHere = FALSE;
            if ( bRecursedHere ) return TRUE;
            bRecursedHere = TRUE;

            CString msg;
            msg.Format( _T( "包含非法字符\n正确格式为[+/-][0-9][.][0-9]\n[]表示可选\n例如-10.24, +100.4, 20.5, 0.410" ) );
            AfxMessageBox( msg );

            bRecursedHere = FALSE;
            return FALSE;
        }
        else
        {
            // 转换成浮点数
            // 还可使用_stscanf_s方法,参考msdn以及afxpropertygridctrl.cpp中TextVar()方法中的实现
            // msdn搜索atof, 查看文章"How to convert a string to a floating-point number in C++"
            // 对atof和sscanf方法有简单的对比说明
            double d = _tstof( strText );
            if ( ( d < m_dMinValue ) || ( d > m_dMaxValue ) )
            {
                static BOOL bRecursedHere = FALSE;
                if ( bRecursedHere ) return TRUE;
                bRecursedHere = TRUE;

                CString msg;
                msg.Format( _T( "超出取值范围：[%f, %f]" ), m_dMinValue, m_dMaxValue );
                AfxMessageBox( msg );

                bRecursedHere = FALSE;
                return FALSE;
            }
        }

        bRet = CMFCPropertyGridProperty::OnUpdateValue();

        if ( m_pParent != NULL )
        {
            m_pWndList->OnPropertyChanged( m_pParent );
        }
    }

    return bRet;
}

IntStrProp::IntStrProp( const CString& name, int value, const AcDbIntArray& intValues, const AcStringArray& strValues, LPCTSTR lpszDescr )
    : CMFCPropertyGridProperty( name, ( long )value, lpszDescr ), m_value( value )
{
    m_intValues.append( intValues );
    m_strValues.append( strValues );
    int len = m_strValues.length();
    for( int i = 0; i < len; i++ )
    {
        AddOption( m_strValues[i].kACharPtr() );
    }
}

CString IntStrProp::Int2Str( int intValue )
{
    ASSERT( m_intValues.length() == m_strValues.length() );

    int index = m_intValues.find( intValue );
    if( index < 0 ) index = 0;
    return m_strValues[index].kACharPtr();
}

int IntStrProp::Str2Int( const CString& strValue )
{
    ASSERT( m_intValues.length() == m_strValues.length() );

    int index = m_strValues.find( strValue );
    if( index < 0 ) index = 0;
    return m_intValues[index];
}

CString IntStrProp::FormatProperty()
{
    ASSERT_VALID( this );
    return Int2Str( m_value );
}

BOOL IntStrProp::OnUpdateValue()
{
    ASSERT_VALID( this );
    ASSERT_VALID( m_pWndList );
    ASSERT_VALID( m_pWndInPlace );
    ASSERT( ::IsWindow( m_pWndInPlace->GetSafeHwnd() ) );

    CString strText;
    m_pWndInPlace->GetWindowText( strText );

    int iCur = m_value;
    m_value = Str2Int( strText );

    if ( iCur != m_value )
    {
        // 更新数据
        SetValue( ( long )m_value );
        m_pWndList->OnPropertyChanged( this );
    }
    return TRUE;
}

BEGIN_MESSAGE_MAP( MyDateTimeCtrl, CDateTimeCtrl )
    ON_NOTIFY_REFLECT( DTN_DATETIMECHANGE, &MyDateTimeCtrl::OnDtnDatetimechange )
    //ON_NOTIFY_REFLECT(NM_KILLFOCUS, &MyDateTimeCtrl::OnNMKillfocus)
    //ON_NOTIFY_REFLECT(DTN_CLOSEUP, &MyDateTimeCtrl::OnDtnCloseup)
END_MESSAGE_MAP()

void MyDateTimeCtrl::OnDtnDatetimechange( NMHDR* pNMHDR, LRESULT* pResult )
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>( pNMHDR );

    //MessageBox(_T("OnDtnDatetimechange"));
    m_pProp->OnUpdateValue();
    m_pProp->Redraw();
    *pResult = 0;
}

//void MyDateTimeCtrl::OnNMKillfocus(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	//MessageBox(_T("OnNMKillfocus"));
//	*pResult = 0;
//}
//
//void MyDateTimeCtrl::OnDtnCloseup(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	//MessageBox(_T("OnDtnCloseup"));
//	*pResult = 0;
//}

DateTimeProperty::DateTimeProperty( const CString& strName, const COleDateTime& timeSrc, LPCTSTR lpszDescr /*= NULL*/ )
    : CMFCPropertyGridProperty( strName, ( COleVariant )timeSrc, lpszDescr )
{
    AllowEdit( FALSE );
}

CString DateTimeProperty::FormatProperty()
{
    COleDateTime dt( GetValue().date );
    // 如果包含了汉字，返回空串
    //return dt.Format(_T("%Y年%B月%d日"));
    //return dt.Format(VAR_DATEVALUEONLY);
    CString str;
    str.Format( _T( "%d年%d月%d日" ), dt.GetYear(), dt.GetMonth(), dt.GetDay() );
    return str;
}

CWnd* DateTimeProperty::CreateInPlaceEdit( CRect rectEdit, BOOL& bDefaultFormat )
{
    MyDateTimeCtrl* pDateTimeCtrl = new MyDateTimeCtrl( this );
    rectEdit.right += 5;
    rectEdit.bottom += 5;
    // 使用DTS_LONGDATEFORMAT样式与FormatProperty()的实现保持一致
    pDateTimeCtrl->Create( WS_VISIBLE | WS_CHILD | DTS_LONGDATEFORMAT, rectEdit, m_pWndList, AFX_PROPLIST_ID_INPLACE );
    pDateTimeCtrl->SetTime( GetValue().date );
    bDefaultFormat = TRUE;
    return pDateTimeCtrl;
}

BOOL DateTimeProperty::OnUpdateValue()
{
    ASSERT_VALID( this );
    ASSERT_VALID( m_pWndInPlace );
    ASSERT_VALID( m_pWndList );
    ASSERT( ::IsWindow( m_pWndInPlace->GetSafeHwnd() ) );

    COleDateTime oldDate( GetValue().date ); // 旧的日期

    MyDateTimeCtrl* pDateTimeCtrl = ( MyDateTimeCtrl* ) m_pWndInPlace;
    COleDateTime newDate;
    pDateTimeCtrl->GetTime( newDate );
    SetValue( newDate ); // 更新数据
    //CMFCPropertyGridProperty::OnUpdateValue();

    if ( oldDate != newDate )
    {
        m_pWndList->OnPropertyChanged( this );
    }

    return TRUE;
}

void DateTimeProperty::OnDrawDescription( CDC* pDC, CRect rect )
{
    CDrawingManager dm( *pDC );
    dm.FillGradient2( rect, RGB( 102, 200, 238 ), RGB( 0, 129, 185 ), 45 );

    CFont* pOldFont = pDC->SelectObject( &afxGlobalData.fontBold );

    // 2次DrawText造成阴影效果
    CString str;
    str.Format( _T( "%s: %s" ), GetDescription(), FormatProperty() );

    pDC->SetTextColor( RGB( 0, 65, 117 ) );
    pDC->DrawText( str, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE );

    rect.OffsetRect( -2, -2 );

    pDC->SetTextColor( RGB( 155, 251, 255 ) );
    pDC->DrawText( str, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE );

    pDC->SelectObject( pOldFont );
}

DataObjectProperty::DataObjectProperty( const CString& name, const CString& value, const AcStringArray& strValues, LPCTSTR lpszDescr /*= NULL*/ )
    : CMFCPropertyGridProperty( name, ( COleVariant )value, lpszDescr )
{
    int len = strValues.length();
    for( int i = 0; i < len; i++ )
    {
        AddOption( strValues[i].kACharPtr() );
    }
}