#include "StdAfx.h"
#include "resource.h"

#include "UIHelper.h"

#include "MyDoubleClick.h"
#include "MyDoubleClick2.h"

#include "DblClick.h"

DblClick* pClick3 = 0;

MyDoubleClick* pClick = 0;
MyDoubleClick2* pClick2 = 0;

#pragma comment(lib, "acapp.lib")

class CUILoaderApp : public AcRxArxApp
{

public:
    CUILoaderApp () : AcRxArxApp () {}

    virtual AcRx::AppRetCode On_kInitAppMsg ( void* pkt )
    {

        AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg ( pkt ) ;

        //UIHelper::RegisterAppMenu();
        UIHelper::RegisterAppToolBar();
        //UIHelper::RegisterObjectContextMenu(pkt);

        //pClick = new MyDoubleClick();
        //pClick2 = new MyDoubleClick2();

        //AcDbDoubleClickEdit::rxInit();
        //acrxBuildClassHierarchy();

        //pClick3 = new DblClick();
        //AcDbLine::desc()->addX(AcDbDoubleClickEdit::desc(), pClick3);

        return ( retCode ) ;
    }

    virtual AcRx::AppRetCode On_kUnloadAppMsg ( void* pkt )
    {

        AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg( pkt ) ;

        //delete pClick; pClick = 0;
        //delete pClick2; pClick2 = 0;

        //AcDbLine::desc()->delX(AcDbDoubleClickEdit::desc());
        ////UIHelper::UnRegisterAppMenu();
        //UIHelper::UnRegisterAppToolBar();
        //UIHelper::UnRegisterObjectContextMenu();

        return ( retCode ) ;
    }

    virtual void RegisterServerComponents ()
    {
    }

    static void LoadMenuApp_test()
    {
        //CAcModuleResourceOverride myResources;
    }
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT( CUILoaderApp )
ACED_ARXCOMMAND_ENTRY_AUTO( CUILoaderApp, LoadMenuApp, _test, test, ACRX_CMD_TRANSPARENT, NULL )
