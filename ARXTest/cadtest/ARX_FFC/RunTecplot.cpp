#include "StdAfx.h"

#include "FFC_Def.h"
#include "SysTool.h"
#include "FluentTool.h"

static void WritePlotTypeFile( const CString& filepath, int type )
{
    CStdioFile outfile;
    outfile.Open( filepath, CFile::modeCreate | CFile::modeWrite );

    outfile.WriteString( GetTecplotTypeName( ( TECPLOT_SHOW_TYPE )type ) );

    outfile.Close();
}

RUN_ERROR_CODE RunTecplot( const CString& appDir, int type )
{
    CString fluentExePath;
    if( !ReadFluentBinPath( fluentExePath ) )
    {
        return REC_NOT_INSTALL;
    }

    if( !IsFileExist( appDir + FLUENT_DAT_FILE ) )
    {
        // ffc.dat文件不存在
        return REC_FILE_NOT_EXIST;
    }

    // 写入plot type文件
    WritePlotTypeFile( appDir + TECPLOT_PLOT_TYPE_FILE, type );

    // 启动fluent
    HANDLE hThread;
    HANDLE hProcess;
    if( !RunFluent(
                fluentExePath,
                appDir,
                TECPLOT_LOAD_SCM_FILE,
                hProcess,
                hThread,
                false ) )
    {
        return REC_PROCESS_ERROR;
    }

    return REC_NO_EEOR;
}