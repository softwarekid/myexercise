#include "StdAfx.h"
#include "ReactorHelper.h"

#include "SelectedGE_EditorReactor.h"
SelectedGE_EditorReactor* pSelectedGE_EditorReactor = 0;

void ReactorHelper::CreateSelectedGE_EditorReactor()
{
    if( pSelectedGE_EditorReactor == 0 )
    {
        pSelectedGE_EditorReactor = new SelectedGE_EditorReactor();
    }
}

void ReactorHelper::RemoveSelectedGE_EditorReactor()
{
    delete pSelectedGE_EditorReactor;
    pSelectedGE_EditorReactor = 0;
}

void ReactorHelper::SwitchMonitorPickFirst()
{
    if( pSelectedGE_EditorReactor )
    {
        bool bMonitorPickFirst = pSelectedGE_EditorReactor->m_bMonitorPickFirst;
        pSelectedGE_EditorReactor->m_bMonitorPickFirst = !( bMonitorPickFirst );
        acutPrintf( _T( "\n%s快速输入" ), bMonitorPickFirst ? _T( "关闭" ) : _T( "开启" ) );
    }
}