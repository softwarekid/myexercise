#include "StdAfx.h"
#include "resource.h"

#include "UIHelper.h"

#ifndef ARX_FFC_UI_SERVICE_NAME
#define ARX_FFC_UI_SERVICE_NAME _T("ARX_FFC_UI_SERVICE_NAME")
#endif

class CARX_FFC_UIApp : public AcRxArxApp
{
public:
    CARX_FFC_UIApp () : AcRxArxApp () {}

    virtual AcRx::AppRetCode On_kInitAppMsg ( void* pkt )
    {

        AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg ( pkt ) ;

        // 注册服务
        acrxRegisterService( ARX_FFC_UI_SERVICE_NAME );

        /*
        		// 1) 使用com代码添加菜单和工具栏
        		// 添加菜单
        		UIHelper::RegAppMenu();
        		// 添加工具栏
        		UIHelper::RegAppToolBar();
        */
        // 2) 加载CUI文件
        UIHelper::LoadCui();

        // 3) 使用arx接口添加工作面右键菜单
        UIHelper::RegObjectContextMenu( pkt );


        return ( retCode ) ;
    }

    virtual AcRx::AppRetCode On_kUnloadAppMsg ( void* pkt )
    {

        AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg ( pkt );

        /*
        		// 1) 使用com代码卸载菜单和工具栏
        		// 卸载菜单
        		UIHelper::UnRegAppMenu();
        		// 卸载工具栏
        		UIHelper::UnRegAppToolBar();
        */

        // 2) 卸载CUI文件
        if( !acDocManager->isApplicationContext() )
        {
            UIHelper::UnloadCui();
        }

        // 3) 卸载arx接口工作面右键菜单
        UIHelper::UnRegObjectContextMenu();

        // 删除服务
        delete acrxServiceDictionary->remove( ARX_FFC_UI_SERVICE_NAME );

        return ( retCode ) ;
    }

    virtual void RegisterServerComponents ()
    {
    }

} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT( CARX_FFC_UIApp )

