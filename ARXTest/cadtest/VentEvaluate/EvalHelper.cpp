#include "stdafx.h"

#include "EvalHelper.h"

// ��ȡoffice2003�İ�װĿ¼
bool GetWordExePathFromRegistry( CString& path )
{
    long lRet = 0;
    CString strRegOffice = _T( "Software\\Microsoft\\Office\\11.0\\Word\\InstallRoot" ); // office03�汾
    HKEY   hResult;
    lRet = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, strRegOffice, 0, KEY_QUERY_VALUE, &hResult );
    if( lRet == ERROR_SUCCESS )
    {
        DWORD      WordType =  REG_SZ;
        DWORD      WordData =  100;
        LPTSTR     KeyByte[1024];
        lRet = ::RegQueryValueEx( hResult, ( _T( "Path" ) ), NULL, &WordType, ( LPBYTE )KeyByte, &WordData );
        if( lRet == ERROR_SUCCESS )
        {
            path = ( LPTSTR )KeyByte;
            path.Append( _T( "word.exe" ) ); // �ϲ��õ�word.exe·��
        }
        RegCloseKey( hResult );
    }
    return ( lRet == ERROR_SUCCESS );
}

// ʹ��ShellExecute���ĵ�
void OpenWordDocument( const CString& filePath )
{
    //CString filePath = _T("C:\\Users\\anheihb03dlj\\Desktop\\��ú��ȫ��̡�103����������㷽��.doc");
    ShellExecute( NULL, _T( "open" ), filePath, NULL, NULL, SW_SHOWNORMAL );
}

CString String2Time( const CString& v )
{
    // ����ַ���Ϊ�գ���ʹ�õ�ǰʱ��
    COleDateTime dt;
    if( v.GetLength() == 0 )
    {
        dt = COleDateTime::GetCurrentTime();
    }
    else
    {
        dt.ParseDateTime( v );
    }
    CString dtStr;
    dtStr.Format( _T( "%d��%d��%d��" ), dt.GetYear(), dt.GetMonth(), dt.GetDay() );
    return dtStr;
}

// ��access��: -1��ʾ�棬0��ʾ��
// ��Ҫת����bool����
int Access2Bool( int value )
{
    return -1 * value;
}