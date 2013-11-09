#include "StdAfx.h"
#include "UIHelper.h"

#include "../ArxUI/AppMenu.h"

AppMenu* global_jl_loader_app_menu = 0;

static void CreateJLLoaderAppMenu( AppMenu& menu )
{
    // ��"*"�ű�ʾ�Ƕ����Ӳ˵�
    //��"$"�ű�ʾ�����Ӳ˵��µĲ˵���
    menu.AddMenu( _T( "��������" ) );
    menu.AddSubMenu( _T( "��������" ), _T( "JL.LoadARXProgram " ) );
    menu.AddSubMenu( _T( "ж�س���" ), _T( "JL.UnLoadARXProgram " ) );
    menu.CreateMenus();
}

void UIHelper::RegAppMenu()
{
    if( global_jl_loader_app_menu == 0 )
    {
        global_jl_loader_app_menu = new AppMenu();
        CreateJLLoaderAppMenu( *global_jl_loader_app_menu );
    }
}

void UIHelper::UnRegAppMenu()
{
    if( global_jl_loader_app_menu != 0 )
    {
        delete global_jl_loader_app_menu;
        global_jl_loader_app_menu = 0;
    }
}