/*-------------------------------------------------------------------------
作者：Racky Ye
该类封装了在AuToCAD上添加菜单及子菜单的方法，使用COM接口实现
大家可以任意修改，添加新功能，希望大家能多交流
Email: yeguixi@126.com
--------------------------------------------------------------*/

#include "stdafx.h"
#include "resource.h"
#include "acadi_i.c"

#include "AppMenu.h"

#import "acax18ENU.tlb" no_implementation raw_interfaces_only named_guids

static CStringArray stcMyTopMenuNames;//存储新添加的CAD顶级菜单项名称

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AppMenu::AppMenu()
{
    m_menuCount = 0;
    m_TopMenuNames.RemoveAll();
}

AppMenu::~AppMenu()
{
    int num = m_menus.GetSize();
    for ( int i = 0; i < num; i++ )
    {
        if ( m_menus.GetAt( i ) != NULL )
            delete m_menus[i];
    }
}

int AppMenu::CCADMenuArray::AddMenu( CADMenu& menu )
{
    CADMenu* pMenu = new CADMenu;
    _stprintf( pMenu->name, menu.name );
    _stprintf( pMenu->order, menu.order );
    return Add( pMenu );
}

int AppMenu::CCADMenuArray::AddMenu( const TCHAR* menuName, const TCHAR* command )
{
    if ( sizeof( menuName ) > 63 )
    {
#ifdef _DEBUG
        acutPrintf( _T( "menuName'%s'长度超出范围 " ), menuName );
#endif
        return -99;
    }
    if ( sizeof( command ) > 63 )
    {
#ifdef _DEBUG
        acutPrintf( _T( "command'%s'长度超出范围 " ), command );
#endif
        return -99;
    }

    CADMenu* pMenu = new CADMenu;
    _stprintf( pMenu->name, menuName );
    if ( command != NULL )
        _stprintf( pMenu->order, command );
    return Add( pMenu );
}

AppMenu::CCADMenuArray::~CCADMenuArray()
{
    for ( int i = 0; i < m_nSize; i++ )
    {
        if ( GetAt( i ) != NULL )
        {
            delete GetAt( i );
        }
    }
}

void AppMenu::AddSubMenu( const TCHAR* menuName,
                          const TCHAR* command )
{
    assert( menuName != NULL );
    if ( m_menuCount == 0 )
    {
#ifdef _DEBUG
        acutPrintf( _T( "添加子菜单%s前没有调用AddMenu添加一个菜单条目！无法添加子菜单 " ), menuName );
#endif
        return;
    }
    m_menus[m_menuCount - 1]->AddMenu( menuName, command );
}

void AppMenu::AddSeparator()
{
    m_menus[m_menuCount - 1]->AddMenu( _T( "--" ), _T( "" ) );
}

void AppMenu::AddSubSeparator()
{
    m_menus[m_menuCount - 1]->AddMenu( _T( "$--" ), _T( "" ) );
}

void AppMenu::AddMenu( const TCHAR* menuName )
{
    m_TopMenuNames.Add( menuName );
    CCADMenuArray* pArr = new CCADMenuArray;
    m_menuCount = m_menus.Add( pArr ) + 1;
}

bool AppMenu::GetAcadApplication( LPDISPATCH* pVal )
{
    LPDISPATCH pDispatch = acedGetAcadWinApp()->GetIDispatch( TRUE );
    if ( pDispatch == NULL )
        return false;
    *pVal = pDispatch;
    return true;
}

bool AppMenu::GetAcadApplication( IAcadApplication*& pAcadApp )
{
    LPDISPATCH  pDisp = NULL;

    if ( !GetAcadApplication( &pDisp ) )
        return false;

    HRESULT hr = S_OK;
    hr = pDisp->QueryInterface( IID_IAcadApplication, ( LPVOID* )&pAcadApp );
    if ( FAILED( hr ) )
        return false;

    return true;
}

bool AppMenu::GetLastMenuFromMenubar( CString& sMenu )
{
    IAcadApplication* pAcad = NULL;
    if( !GetAcadApplication( pAcad ) )
        return false;

    IAcadMenuBar* pMenuBar = NULL;
    IAcadPopupMenu*  pPopUpMenu = NULL;

    pAcad->get_MenuBar( &pMenuBar );
    pAcad->Release();
    long numberOfMenus;
    pMenuBar->get_Count( &numberOfMenus );

    //得到菜单栏上面的最后一个菜单
    BSTR name;
    VARIANT vIndex;
    vIndex.vt = VT_I4;
    vIndex.lVal = numberOfMenus - 1;

    pMenuBar->Item( vIndex, &pPopUpMenu );
    pMenuBar->Release();
    pPopUpMenu->get_NameNoMnemonic( &name );
    pPopUpMenu->Release();

    sMenu.Format( _T( "%s" ), name ); //debug 大写，用于转化LPWSTR类型字符串
    SysFreeString( name );

    return true;
}

bool AppMenu::GetAcadMenuGroup( IAcadMenuGroup**  pVal, LPCTSTR menuname )
{
    IAcadApplication* acadApp = NULL;
    HRESULT hr = S_OK;

    if( !GetAcadApplication( acadApp ) )
        return false;

    LPDISPATCH  pTempDisp = NULL;
    IAcadMenuGroups* mnuGrps = NULL;
    long cnt = 0, cntmnu = 0;

    //得到菜单组集
    hr = acadApp->get_MenuGroups( &mnuGrps );
    if ( FAILED( hr ) )
    {
        acadApp->Release();
        return false;
    }
    acadApp->Release();

    mnuGrps->get_Count( &cnt );

    //得到菜单组
    IAcadMenuGroup* mnuGrp = NULL;
    IAcadPopupMenus*  mnus = NULL;
    IAcadPopupMenu* pPopUpMenu = NULL;

    VARIANT  vtName, vtMenu;
    vtName.vt = VT_I4;
    vtMenu.vt = VT_I4;

    BSTR  mnuName;
    bool found = false ;
    for ( long i = 0; i < cnt; i++ )
    {
        vtName.lVal = i;
        hr = mnuGrps->Item( vtName, &mnuGrp );
        if ( FAILED( hr ) )
        {
            return false;
        }

        mnuGrp->get_Menus( &mnus );

        mnus->get_Count( &cntmnu );

        for( long j = 0; j < cntmnu; j++ )
        {
            vtMenu.lVal = j;
            hr = mnus->Item( vtMenu, &pPopUpMenu );
            if( FAILED( hr ) )
                continue;

            pPopUpMenu->get_Name( &mnuName );
            pPopUpMenu->Release();

            CString sMenuName( mnuName );
            SysFreeString( mnuName );

            sMenuName.Replace( _T( "&" ), _T( "" ) ); //debug
            if( sMenuName.CompareNoCase( menuname ) == 0 )
            {
                found = true;
                *pVal = mnuGrp;
                break;
            }
        }

        mnus->Release();

        if( found )
        {
            break;
        }
        else
        {
            mnuGrp->Release();
        }
    }

    mnuGrps->Release();

    return found;
}

bool AppMenu::GetAcadMenuGroup( IAcadMenuGroup**  pVal )
{
    IAcadApplication* acadApp = NULL;
    HRESULT hr = S_OK;

    if( !GetAcadApplication( acadApp ) )
        return false;

    LPDISPATCH  pTempDisp = NULL;
    IAcadMenuGroups* mnuGrps = NULL;
    long cnt = 0;

    //得到菜单组集
    hr = acadApp->get_MenuGroups( &mnuGrps );
    if ( FAILED( hr ) )
    {
        acadApp->Release();
        return false;
    }
    acadApp->Release();

    mnuGrps->get_Count( &cnt );

    //得到菜单组
    IAcadMenuGroup* mnuGrp = NULL;

    VARIANT  vtName;
    vtName.vt = VT_I4;
    BSTR  grpName;
    bool found = false ;
    for ( long i = 0; i < cnt; i++ )
    {
        vtName.lVal = i;
        hr = mnuGrps->Item( vtName, &mnuGrp );
        if ( FAILED( hr ) )
            continue;

        hr  = mnuGrp->get_Name( &grpName );
        CString cgrpName( grpName );
        SysFreeString( grpName );

        if ( cgrpName.CompareNoCase( _T( "Acad" ) ) == 0 )
        {
            found = true;
            *pVal = mnuGrp;
            break;
        }
        else
        {
            mnuGrp->Release();
        }
    }

    mnuGrps->Release();

    return found;
}

void AppMenu::CreateMenus()
{
    CMDIFrameWnd* pMainFrame = acedGetAcadFrame();
    pMainFrame->LockWindowUpdate();

    //得到菜单栏上面的最后一个菜单
    CString sName;
    if( !GetLastMenuFromMenubar( sName ) )
        return;

    sName.Replace( _T( "&" ), _T( "" ) ); //debug

    IAcadMenuGroup* mnuGrp = NULL;
    if ( !GetAcadMenuGroup( &mnuGrp, sName ) )
        return ;

    //得到所有菜单组
    IAcadPopupMenus*   mnus = NULL;
    IAcadPopupMenu*   mnu = NULL;
    IAcadPopupMenuItem*  mnuitem = NULL;
    HRESULT hr = S_OK;
    hr = mnuGrp->get_Menus( &mnus );
    long cnt = 0l;
    hr = mnus->get_Count( &cnt );
    int i = 0;
    int j = 0;
    int k = 0;
    VARIANT index;
    VariantInit( &index );
    V_VT( &index ) = VT_I4;
    CArray<IAcadPopupMenu*, IAcadPopupMenu*> MnuAdrs;

    long lIndex = 0;
    TCHAR MenuName[256];
    for ( i = 0; i < m_menuCount; i++ )
    {
        CString tmpMenu = m_TopMenuNames[i];
        tmpMenu.Replace( _T( "&" ), _T( "" ) );
        if( IsMenuExist( mnu, lIndex, tmpMenu, mnus ) ) //处理二次加载
        {
            V_I4( &index ) = lIndex;
        }
        else
        {
            //			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPTSTR)(LPCTSTR)m_TopMenuNames[i], -1, MenuName, 256);
            _tcscpy( MenuName, m_TopMenuNames[i] );
            mnus->Add( MenuName, &mnu );
            V_I4( &index ) = cnt++;
        }

        MnuAdrs.Add( mnu );
        mnu->InsertInMenuBar( index );
    }

    IAcadPopupMenu* MenuItem = NULL;
    int subMenuNum = 0;
    CCADMenuArray* Menus;
    TCHAR szSubMenu[256];
    TCHAR szSubMenuMacro[256];
    CString MenusName;
    CString Menusorder;
    for ( j = 0; j < m_menuCount; j++ )
    {
        Menus = m_menus.GetAt( j );
        subMenuNum = Menus->GetSize();
        for ( k = 0; k < subMenuNum; k++ )
        {
            mnu = MnuAdrs.GetAt( j );
            if ( !_tcscmp( _T( "--" ), Menus->GetAt( k )->name ) )
            {
                V_I4( &index ) = k;
                mnu->AddSeparator( index, &mnuitem );
            }
            else
            {
                V_I4( &index ) = k;
                MenusName = Menus->GetAt( k )->name;
                Menusorder = Menus->GetAt( k )->order;
                _tcscpy( szSubMenu, MenusName );
                _tcscpy( szSubMenuMacro, Menusorder );
                //MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPTSTR)(LPCTSTR)MenusName, -1, szSubMenu, 256);
                //MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPTSTR)(LPCTSTR)Menusorder, -1, szSubMenuMacro, 256);
                if ( _tcscspn( Menus->GetAt( k )->name, _T( "*" ) ) < ( size_t )MenusName.GetLength() )
                {
                    MenusName.TrimLeft( _T( '*' ) );
                    //					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPTSTR)(LPCTSTR)MenusName, -1, szSubMenu, 256);
                    _tcscpy( szSubMenu, MenusName );
                    hr = mnu->AddSubMenu( index, szSubMenu, &MenuItem );
                }
                else if ( _tcscspn( Menus->GetAt( k )->name, _T( "$" ) ) < ( size_t )MenusName.GetLength() )
                {
                    MenusName.TrimLeft( _T( '$' ) );
                    if( MenusName.Compare( _T( "--" ) ) == 0 ) //添加子菜单的分隔符
                    {
                        hr = MenuItem->AddSeparator( index, &mnuitem );
                    }
                    else
                    {
                        //MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPTSTR)(LPCTSTR)MenusName, -1, szSubMenu, 256);
                        _tcscpy( szSubMenu, MenusName );
                        hr = MenuItem->AddMenuItem( index, szSubMenu, szSubMenuMacro, &mnuitem );
                    }
                }
                else
                {
                    hr = mnu->AddMenuItem( index, szSubMenu, szSubMenuMacro, &mnuitem );
                }
            }
        }
    }

    for ( i = 0; i < MnuAdrs.GetSize(); i++ )
    {
        MnuAdrs[i]->Release();
    }
    mnuitem->Release();
    mnus->Release();
    mnuGrp->Release();
    pMainFrame->UnlockWindowUpdate();

    stcMyTopMenuNames.Copy( m_TopMenuNames ); //将顶级菜单名存入静态数组中，卸载时使用

    //return ;
}

bool AppMenu::IsMenuExist( IAcadPopupMenu*& pMnu, long& index, LPCTSTR mnuname, IAcadPopupMenus* mnus )
{
    ASSERT( mnus != NULL );

    long cnt = 0l;
    HRESULT hr = mnus->get_Count( &cnt );
    bool found = false;
    int curMenuIndex = -1;
    BSTR tempName;

#ifdef _DEBUG
    acutPrintf( _T( "IsMenuExist cnt=%ld " ), cnt );
#endif

    //
    IAcadPopupMenu* toolsMenu = NULL;
    for ( long i = 0; i < cnt; i++ )
    {
        VARIANT vtName;
        VariantInit( &vtName );
        V_VT( &vtName ) = VT_I4;
        V_I4( &vtName ) = i;
        hr = mnus->Item( vtName, &toolsMenu );
        if ( FAILED( hr ) )
            return false;
        hr = toolsMenu->get_NameNoMnemonic( &tempName );
        CString mnuName( tempName );

#ifdef _DEBUG
        acutPrintf( _T( " %d,%s" ), i, mnuName );
#endif
        if( mnuName.Compare( mnuname ) == 0 )
        {
            index = i;
            pMnu = toolsMenu;
            return true;
        }
    }
    pMnu = NULL;
    return false;
}

void AppMenu::CleanUpMenus()
{
    IAcadMenuGroup* mnuGrp = NULL;
    for( int i = 0; i < stcMyTopMenuNames.GetSize(); i++ )
    {
        CString sMenu = stcMyTopMenuNames.GetAt( i );
        sMenu.Replace( _T( "&" ), _T( "" ) );

        if ( !GetAcadMenuGroup( &mnuGrp, sMenu ) )
        {
#ifdef _DEBUG
            acutPrintf( _T( "没有找到 %s 所属的菜单组，无法卸载。 " ), sMenu );
#endif

            //   mnuGrp->Release();
            continue;
        }

        //得到所有菜单组
        IAcadPopupMenus*  mnus = NULL;
        HRESULT hr = S_OK;
        hr = mnuGrp->get_Menus( &mnus );

        ClearMenu( mnus, sMenu );

        mnus->Release();
        mnuGrp->Release();

    }

    stcMyTopMenuNames.RemoveAll();
}

void AppMenu::ClearMenu( IAcadPopupMenus* mnus, LPCTSTR menuname )
{
    long cnt = 0l;
    HRESULT hr = mnus->get_Count( &cnt );

    //now get Tools menu
    IAcadPopupMenu* toolsMenu = NULL;
    BSTR   tempName;
    VARIANT vtName;
    VariantInit( &vtName );

    bool found = false;
    for ( long i = 0; i < cnt; i++ )
    {
        vtName.vt = VT_I4;
        vtName.lVal = i;
        hr = mnus->Item( vtName, &toolsMenu );
        if ( FAILED( hr ) )
            return ;

        hr = toolsMenu->get_NameNoMnemonic( &tempName );
        CString mnuName( tempName );
        SysFreeString( tempName );
        if( mnuName.CompareNoCase( menuname ) == 0 )
        {
            found = true;
            break;
        }
    }

    if ( !found )
    {
        acutPrintf( _T( "没有找到菜单 : %s " ), menuname );
        return;
    }

    hr = toolsMenu->get_Count( &cnt );
    VARIANT vIndex;
    vIndex.vt = VT_I4;
    vIndex.lVal = cnt;
    IAcadPopupMenuItem* cmd1 = NULL;
    while( cnt-- )
    {
        vIndex.lVal = cnt;
        hr = toolsMenu->Item( vIndex, &cmd1 );
        hr = cmd1->Delete();
        cmd1->Release();
    }

    toolsMenu->RemoveFromMenuBar();
    toolsMenu->Release();

    mnus->RemoveMenuFromMenuBar( vtName );
}
