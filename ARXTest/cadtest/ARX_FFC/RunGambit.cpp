#include "StdAfx.h"

#include "FFC_Def.h"
#include "SysTool.h"
#include "FluentTool.h"
#include "ThreadData.h"

static bool IsGambitExit( GambitThreadData* pData )
{
    DWORD exitCode;
    GetExitCodeProcess( pData->hProcess, &exitCode );

    return ( exitCode != STILL_ACTIVE );

    //return !IsProcessActive(_T("gambit.exe"));
}

static bool IsRunError( GambitThreadData* pData )
{
    return IsFileExist( pData->errorFile );
}

static bool IsRunOK( GambitThreadData* pData )
{
    return IsFileExist( pData->mshFile );
}

static bool NeedExitGambit( GambitThreadData* pData )
{
    return ( IsRunError( pData ) || IsRunOK( pData ) );
}

DWORD MyGambitThread( LPVOID lpParam )
{
    GambitThreadData* pData = ( GambitThreadData* )lpParam;

    while( true )
    {
        Sleep( THREAD_CHECK_TIME_INTERVAL );

        // 根据以下2种情况关闭Gambit进程
        //	1) 发现了error文件，则表示不能生成msh文件
        //	2) 发现msh文件，则表示生成msh文件成功
        // 或者用户发出消息要求退出
        if( IsUserAskForThreadEnd() || NeedExitGambit( pData ) )
        {
            // 关闭gambit startup窗口
            CloseGambitStartupWindow();
            CloseHandle( pData->hThread );
            CloseHandle( pData->hProcess );

            // 试图退出线程(没有成功???)
            //DWORD exitCode;
            //GetExitCodeProcess(pData->hProcess, &exitCode);
            //ExitProcess(exitCode);

            break;
        }
    }
    return 0;
}

RUN_ERROR_CODE RunGambit( const CString& appDir, HANDLE& hThread )
{
    CString gambitExePath;
    if( !ReadGambitBinPath( gambitExePath ) )
    {
        return REC_NOT_INSTALL;
    }

    if( !IsFileExist( appDir + GAMBIT_LOAD_JOU_FIEL ) )
    {
        return REC_FILE_NOT_EXIST;
    }

    // 启动gambit
    GambitThreadData data =
    {
        NULL,
        NULL,
        appDir + GAMBIT_MESH_FILE,
        appDir + GAMBIT_ERROR_FILE
    };

    if( !RunGambit(
                gambitExePath,
                appDir,
                GAMBIT_LOAD_JOU_FIEL,
                data.hProcess,
                data.hThread,
                true ) )
    {
        return REC_PROCESS_ERROR;
    }

    // 传递给全局变量(避免一些未知的线程错误)
    global_gambit_thread_data = data;

    // 创建监视线程
    DWORD dwThreadId;
    /*HANDLE*/
    hThread = CreateThread(
                  NULL,              // default security attributes
                  0,                 // use default stack size
                  ( LPTHREAD_START_ROUTINE )MyGambitThread,        // thread function
                  &global_gambit_thread_data,             // argument to thread function
                  0,                 // use default creation flags
                  &dwThreadId );  // returns the thread identifier

    return REC_NO_EEOR;
}

bool IsRunGambitOk( const CString& appDir )
{
    if( IsFileExist( appDir + GAMBIT_ERROR_FILE ) ) return false;
    if( IsFileExist( appDir + GAMBIT_MESH_FILE ) ) return true;

    return false;
}