#include "StdAfx.h"
#include "UIHelper.h"

#include "Resource.h"

#include "../ArxUI/AppMenu.h"
#include "../ArxUI/AppToolBar.h"
#include "../ArxUI/ObjectMenu.h"

#include "../ArxCUI/CUIHelper.h"

// CUI�ļ�����
#define CUI_FILE_NAME _T("��ú���˿�ɿ��������˹���ģ��.cuix")
// CUI����
#define CUI_GROUP_NAME _T("��ú���˿�ɿ��������˹���ģ��")

// ffc�����ͼ�������ļ���
#define FFC_APP_BMP_DIR _T("ͼ��")

// ffc����˵�����
AppMenu* global_ffc_app_menu = 0;
// ffc���򹤾�������
AppToolBar* global_ffc_app_toolbar = 0;
// �������Ҽ��˵�
ObjectMenu* global_ffc_app_ws_context_menu = 0;

static void CreateFFCAppMenu( AppMenu& menu )
{
    // ��"*"�ű�ʾ�Ƕ����Ӳ˵�
    //��"$"�ű�ʾ�����Ӳ˵��µĲ˵���

    menu.AddMenu( _T( "��ú���˿�ɿ��������˹���ģ��" ) );

    menu.AddSubMenu( _T( "*�ɿ�����ģ" ) );
    menu.AddSubMenu( _T( "$���" ), _T( "JL.DrawTunnel" ) );
    menu.AddSubMenu( _T( "$������" ), _T( "JL.DrawWorkSurface" ) );
    menu.AddSubMenu( _T( "$��������" ), _T( "JL.DrawDirection" ) );
    menu.AddSubMenu( _T( "$��˹���" ), _T( "JL.DrawGasPipe" ) );
    menu.AddSubMenu( _T( "$ע����" ), _T( "JL.DrawNitrogenPipe" ) );
    menu.AddSubMenu( _T( "$�ܱ�" ), _T( "JL.DrawObturation" ) );
    menu.AddSubMenu( _T( "$��ǽ" ), _T( "JL.DrawSandWall" ) );
    menu.AddSubMenu( _T( "$©��Դ" ), _T( "JL.DrawSourceAirLeak" ) );
    menu.AddSubMenu( _T( "$©���" ), _T( "JL.DrawSinkAirLeak" ) );

    menu.AddSubMenu( _T( "�ɿ�����˹ӿ��" ), _T( "JL.ShowGoafGasDlg" ) );
    menu.AddSubMenu( _T( "ģ��ɿ�����͸��ɢ����" ), _T( "JL.ShowFFCDlg" ) );

    menu.AddSubMenu( _T( "*��������" ) );
    menu.AddSubMenu( _T( "$�Զ���ע��������" ), _T( "JL.AutoDirection" ) );
    menu.AddSubMenu( _T( "$����������" ), _T( "JL.ReverseDirection" ) );
    menu.AddSubMenu( _T( "$�л���������" ), _T( "JL.SwitchMonitorPickFirst" ) );

    menu.CreateMenus();
}

static CString GetAppPathDir()
{
    TCHAR szMoudlePath[_MAX_PATH];
    GetModuleFileName( _hdllInstance, szMoudlePath, _MAX_PATH );

    TCHAR drive[_MAX_DRIVE];
    TCHAR dir[_MAX_DIR];
    _tsplitpath( szMoudlePath, drive, dir, NULL, NULL );

    TCHAR path[_MAX_PATH];
    _tmakepath( path, drive, dir, NULL, NULL );

    return CString( path );
}

// ����ͼ���ļ���·��
// ������ڵ�ǰarxģ��·���µ�"ͼ��"�ļ�����
static CString BuildBmpDirPath( const CString& bmpDir )
{
    CString str;
    str.Format( _T( "%s%s\\" ), GetAppPathDir(), bmpDir );
    return str;
}

static void CreateFFCAppToolBar( AppToolBar& toolbar )
{
    toolbar.AddToolBar( _T( "�ɿ��������˹���ģ��" ) );
    toolbar.AddToolButton( _T( "���" ), _T( "JL.DrawTunnel" ), _T( "�������" ), _T( "���.bmp" ) );
    toolbar.AddToolButton( _T( "������" ), _T( "JL.DrawWorkSurface" ), _T( "���ƹ�����" ), _T( "������.bmp" ) );
    toolbar.AddToolButton( _T( "��������" ), _T( "JL.DrawDirection" ), _T( "���Ʒ�������" ), _T( "��������.bmp" ) );
    toolbar.AddToolButton( _T( "��˹���" ), _T( "JL.DrawGasPipe" ), _T( "������˹���" ), _T( "��˹���.bmp" ) );
    toolbar.AddToolButton( _T( "ע����" ), _T( "JL.DrawNitrogenPipe" ), _T( "����ע����" ), _T( "ע����.bmp" ) );
    toolbar.AddToolButton( _T( "�ܱ�" ), _T( "JL.DrawObturation" ), _T( "�����ܱ�" ), _T( "�ܱ�.bmp" ) );
    toolbar.AddToolButton( _T( "��ǽ" ), _T( "JL.DrawSandWall" ), _T( "���Ʊ�ǽ" ), _T( "��ǽ.bmp" ) );
    toolbar.AddSeparator();
    toolbar.AddToolButton( _T( "©��Դ" ), _T( "JL.DrawSourceAirLeak" ), _T( "����©��Դ" ), _T( "©��Դ.bmp" ) );
    toolbar.AddToolButton( _T( "©���" ), _T( "JL.DrawSinkAirLeak" ), _T( "����©���" ), _T( "©���.bmp" ) );
    toolbar.AddSeparator();
    toolbar.AddToolButton( _T( "�Զ���ע��������" ), _T( "JL.AutoDirection" ), _T( "�Զ���ע��������" ), _T( "�Զ���ע��������.bmp" ) );
    toolbar.AddToolButton( _T( "����������" ), _T( "JL.ReverseDirection" ), _T( "����������" ), _T( "����������.bmp" ) );
    toolbar.AddToolButton( _T( "�л���������" ), _T( "JL.SwitchMonitorPickFirst" ), _T( "�л���������" ), _T( "�л���������.bmp" ) );
    toolbar.AddToolButton( _T( "����Gambit���ν�ģ����������" ), _T( "JL.ShowGambitParamDlg " ), _T( "����Gambit���ν�ģ����������" ), _T( "����Gambit���ν�ģ����������.bmp" ) );

    // ����ͼ���ļ���·��
    CString bmpDir = BuildBmpDirPath( FFC_APP_BMP_DIR );
    //acutPrintf(_T("\nͼ���ļ���·��: %s"), bmpDir);

    toolbar.CreateToolBars( bmpDir );
}

static ObjectMenu* CreateWsObjectContextMenu( const void* appId )
{
    // �����˵���ԴID
    ObjectMenu* pObjMenu = new ObjectMenu( IDR_MENU_WS_OBJECT );
    // ע�������ID
    pObjMenu->RegMenuCmd( ID_MENU_ADVANCE_WS, _T( "JL.AdvanceWS" ) );
    // ���ö�������
    pObjMenu->SetObjectType( _T( "WorkSurface" ) );

    // ��������˵�
    pObjMenu->CreateObjectMenu( appId );

    return pObjMenu;
}

void UIHelper::RegAppMenu()
{
    if( global_ffc_app_menu == 0 )
    {
        global_ffc_app_menu = new AppMenu();
        CreateFFCAppMenu( *global_ffc_app_menu );
    }
}

void UIHelper::UnRegAppMenu()
{
    if( global_ffc_app_menu != 0 )
    {
        // ɾ�������Զ�ж�ز˵�
        delete global_ffc_app_menu;
        global_ffc_app_menu = 0;
    }
}

void UIHelper::RegAppToolBar()
{
    if( global_ffc_app_toolbar == 0 )
    {
        global_ffc_app_toolbar = new AppToolBar();
        CreateFFCAppToolBar( *global_ffc_app_toolbar );
    }
}

void UIHelper::UnRegAppToolBar()
{
    if( global_ffc_app_toolbar != 0 )
    {
        delete global_ffc_app_toolbar;
        global_ffc_app_toolbar = 0;
    }
}

void UIHelper::RegObjectContextMenu( const void* appId )
{
    if( global_ffc_app_ws_context_menu == 0 )
    {
        global_ffc_app_ws_context_menu = CreateWsObjectContextMenu( appId );
    }
}

void UIHelper::UnRegObjectContextMenu()
{
    if( global_ffc_app_ws_context_menu != 0 )
    {
        delete global_ffc_app_ws_context_menu;
        global_ffc_app_ws_context_menu = 0;
    }
}

void UIHelper::LoadCui()
{
    CUIHelper::UnLoadPartialMenuUseCom( CUI_GROUP_NAME );
    CUIHelper::LoadPartialMenuUseCom( CUI_FILE_NAME, CUI_GROUP_NAME );
}

void UIHelper::UnloadCui()
{
    CUIHelper::UnLoadPartialMenuUseCom( CUI_GROUP_NAME );
}