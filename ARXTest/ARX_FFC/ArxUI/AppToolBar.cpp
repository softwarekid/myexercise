#include "StdAfx.h"
#include "AppToolBar.h"

#include "ArxToolBar.h"

AppToolBar::AppToolBar( void )
{
    m_pToolBar = new ArxToolBar();
}

AppToolBar::~AppToolBar( void )
{
    CleanUpToolBars();
    delete m_pToolBar;
    m_pToolBar = 0;
}

void AppToolBar::AddToolBar( const TCHAR* toolBarName )
{
    m_pToolBar->AddToolBar( toolBarName );
}

void AppToolBar::AddToolButton( const TCHAR* name, const TCHAR* command, const TCHAR* help /*= NULL*/, const TCHAR* bmp/*= NULL*/ )
{
    // 对命令串进行处理
    // 前面添加2个esc，末尾添加一个空格
    if( command != NULL )
    {
        CString cmd;
        cmd.Format( _T( "%s" ), command );
        cmd.Trim();

        TCHAR szCmd[256];
        _stprintf( szCmd, _T( "\003\003%s " ), cmd );
        m_pToolBar->AddToolButton( name, szCmd, help, bmp );
    }
    else
    {
        m_pToolBar->AddToolButton( name, NULL, help, bmp );
    }
}

void AppToolBar::AddSeparator()
{
    m_pToolBar->AddSeparator();
}

void AppToolBar::CreateToolBars( const TCHAR* bmpDir )
{
    m_pToolBar->CreateToolBars( bmpDir );
}

void AppToolBar::CleanUpToolBars()
{
    m_pToolBar->CleanUpToolBars();
}