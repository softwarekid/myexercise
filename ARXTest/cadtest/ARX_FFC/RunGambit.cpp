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

        // ��������2������ر�Gambit����
        //	1) ������error�ļ������ʾ��������msh�ļ�
        //	2) ����msh�ļ������ʾ����msh�ļ��ɹ�
        // �����û�������ϢҪ���˳�
        if( IsUserAskForThreadEnd() || NeedExitGambit( pData ) )
        {
            // �ر�gambit startup����
            CloseGambitStartupWindow();
            CloseHandle( pData->hThread );
            CloseHandle( pData->hProcess );

            // ��ͼ�˳��߳�(û�гɹ�???)
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

    // ����gambit
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

    // ���ݸ�ȫ�ֱ���(����һЩδ֪���̴߳���)
    global_gambit_thread_data = data;

    // ���������߳�
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