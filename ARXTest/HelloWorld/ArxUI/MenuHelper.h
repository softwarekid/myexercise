/*
使用方法：
MenuHelper menu;
menu.AddMenu("我的菜单");
menu.AddSubMenu("弹出对话框", "_DLG ");
menu.AddSubMenu("绘制线条", "_DRAWL ");
menu.AddSubMenu("*Line");// 加"*"号表示是二级子菜单
menu.AddSubMenu("$CIRCLE", "_CIRCLE ");//加"$"号表示二级子菜单下的菜单条
menu.AddSubSeparator();
menu.AddSubMenu("$PLINE", "_PLINE ");//加"$"号表示二级子菜单下的菜单条
menu.AddSeparator();
menu.AddSubMenu("选择文件夹", "_SELFOLDER ");
menu.AddSubMenu("*TEXT");// 加"*"号表示是二级子菜单
menu.AddSubMenu("$MTEXT", "_TEXT ");//加"$"号表示二级子菜单下的菜单条
menu.CreateMenus();
*/

#pragma once

#include "dlimexp.h"

class AppMenu;

class ARXUI_DLLIMPEXP MenuHelper
{
public:
	MenuHelper(void);
	~MenuHelper(void);

	// 添加菜单
	void AddMenu(const TCHAR* menuName);
	// 添加子菜单，menuName-菜单名"--"为分隔符,command要加空格
	void AddSubMenu(const TCHAR* menuName, const TCHAR* command=NULL);
	// 添加分隔符
	void AddSeparator();
	// 添加二级子菜单分隔符
	void AddSubSeparator();

	// 创建菜单
	void CreateMenus();

	// 删除菜单
	//创建了Menu一定要调用这个函数
	static void CleanUpMenus();

private:
	AppMenu* m_pMenuImpl;
};
