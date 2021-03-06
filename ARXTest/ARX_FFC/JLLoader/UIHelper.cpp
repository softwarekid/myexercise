#include "StdAfx.h"
#include "UIHelper.h"

#include "../ArxUI/AppMenu.h"

AppMenu* global_jl_loader_app_menu = 0;

static void CreateJLLoaderAppMenu( AppMenu& menu )
{
    // 加"*"号表示是二级子菜单
    //加"$"号表示二级子菜单下的菜单条
    menu.AddMenu( _T( "基蓝软件" ) );
    menu.AddSubMenu( _T( "启动程序" ), _T( "JL.LoadARXProgram " ) );
    menu.AddSubMenu( _T( "卸载程序" ), _T( "JL.UnLoadARXProgram " ) );
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