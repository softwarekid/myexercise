#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

typedef vector<string> StringArray;

#define GAMBIT_VAR_JOU_FIEL_NAME "GambitVar.jou"
#define GAMBIT_STARTUP_WINDOWS_NAME "Gambit Startup"
#define GAMBIT_SESSION_ID "ffc"

//static
void SplitString( const string& content, const string& token, StringArray& cc )
{
    const char* delimeter =  token.c_str();
    char* pch = strtok( ( char* )content.c_str(), delimeter );
    while( pch != NULL )
    {
        cc.push_back( pch );
        pch = strtok( NULL, delimeter );
    }
}

string GetAppPathDir()
{
    char szMoudlePath[_MAX_PATH];
    //GetModuleFileName(GetModuleHandle("VNGGen.dll"), szMoudlePath, _MAX_PATH);
    GetModuleFileName( NULL, szMoudlePath, _MAX_PATH );

    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    _splitpath( szMoudlePath, drive, dir, NULL, NULL );

    char path[_MAX_PATH];
    _makepath( path, drive, dir, NULL, NULL );

    return string( path );
}

string BuildPath( const string& dir, const string& fileName )
{
    stringstream ss;
    //ss<<dir<<"\\"<<fileName;
    ss << dir << fileName;
    return ss.str();
}

void RemoveFile( const string& filePath )
{
    // 在<stdio.h>中提供
    remove( filePath.c_str() );
}

// 读取gambit安装路径
static bool ReadGambitInstallPath( string& path )
{
    long lRet = 0;
    string strRegGV = "Software\\Fluent Inc Products\\Gambit"; // graphviz
    HKEY hResult;
    lRet = ::RegOpenKeyEx( HKEY_CURRENT_USER, strRegGV.c_str(), 0, KEY_QUERY_VALUE, &hResult );
    if( lRet == ERROR_SUCCESS )
    {
        DWORD      WordType =  REG_SZ;
        DWORD      WordData =  100;
        char     KeyByte[1024];
        lRet = ::RegQueryValueEx( hResult, NULL, NULL, &WordType, ( LPBYTE )KeyByte, &WordData );
        if( lRet == ERROR_SUCCESS )
        {
            path = KeyByte;

            // 合并路径
            stringstream ss;
            ss << path << "ntbin\\ntx86\\gambit.exe";
            path = ss.str();
        }
        RegCloseKey( hResult );
    }
    return ( lRet == ERROR_SUCCESS );
}

static void CloseGambit()
{
    HWND hwnd = FindWindow( NULL, GAMBIT_STARTUP_WINDOWS_NAME );
    if( hwnd != NULL )
    {
        cout << "查找gambit startup窗口句柄成功" << endl;

        // 查找close按钮
        HWND bhwnd = FindWindowEx( hwnd, NULL, "Button", "Close" );

        if( bhwnd != NULL )
        {
            cout << "查找close按钮成功" << endl;
            ::PostMessage( hwnd, WM_SYSCOMMAND, SC_CLOSE, 0 );
            //::PostMessage(hwnd, WM_CLOSE, 0, 0);
        }
        else
        {
            cout << "查找close按钮失败" << endl;
        }
    }
    else
    {
        cout << "查找gambit startup窗口句柄失败" << endl;
    }
}

static bool RunGambit( const string& gambitExePath, const string& cwdPath, const string& jouFileName )
{
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    memset( &si, 0, sizeof( si ) );
    si.cb = sizeof( si );
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW;

    stringstream ss;
    ss << "-r2.2.30 -id " << GAMBIT_SESSION_ID << " -inputfile " << jouFileName;
    string cmdLine = ss.str();

    cout << "gambit路径:" << gambitExePath << endl;
    cout << "参数: " << cmdLine << endl;

    BOOL ret = CreateProcess( ( char* )gambitExePath.c_str(), ( char* )cmdLine.c_str(), NULL, FALSE, NULL, NULL, NULL, cwdPath.c_str(), &si, &pi );
    if( ret )
    {
        //WaitForSingleObject(pi.hProcess, INFINITE);
        WaitForSingleObject( pi.hProcess, 60000 );
        CloseHandle( pi.hThread );
        CloseHandle( pi.hProcess );
    }
    return ret;
}

int main()
{
    string gambitExePath;
    if( !ReadGambitInstallPath( gambitExePath ) )
    {
        cout << "请安装Gambit/Fluent软件" << endl;
        return 0;
    }

    string appDir = GetAppPathDir();
    cout << "当前目录:" << appDir << endl;

    cout << "开始运行gambit..." << endl;
    RunGambit( gambitExePath, appDir, GAMBIT_VAR_JOU_FIEL_NAME );
    cout << "gambit运行结束..." << endl;

    CloseGambit();

    cout << "test run gambit" << endl;
    return 0;
}
