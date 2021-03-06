#include "StdAfx.h"
#include "resource.h"

#include "ReactorHelper.h"

// 定义注册服务名称
#ifndef REACTOR_SERVICE_NAME
#define REACTOR_SERVICE_NAME _T("REACTOR_SERVICE_NAME")
#endif

class CReactorApp : public AcRxArxApp
{

public:
    CReactorApp () : AcRxArxApp () {}

    virtual AcRx::AppRetCode On_kInitAppMsg ( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg ( pkt ) ;

        // 注册服务
        acrxRegisterService( REACTOR_SERVICE_NAME );

        ReactorHelper::CreateLinkedGE_EditorReactor();
        ReactorHelper::CreateDocumentReactorMap();

        acutPrintf( _T( "\nReactor::On_kInitAppMsg\n" ) );

        return ( retCode ) ;
    }

    virtual AcRx::AppRetCode On_kUnloadAppMsg ( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg ( pkt ) ;

        // 删除AcEditorReactor
        ReactorHelper::RemoveLinkedGE_EditorReactor();
        ReactorHelper::RemoveDocumentReactorMap();

        // 删除服务
        delete acrxServiceDictionary->remove( REACTOR_SERVICE_NAME );

        acutPrintf( _T( "\nReactor::On_kUnloadAppMsg\n" ) );

        return ( retCode ) ;
    }

    static void PrintAddress()
    {
        AcDbDatabase* pDB = acdbHostApplicationServices()->workingDatabase();
        AcEditor* pEditor = acedEditor;
        AcApDocument* pDoc = curDoc();
        AcDbDatabase* pDocDB = pDoc->database();
        AcEdInputPointManager* pIPM = pDoc->inputPointManager();

        acutPrintf( _T( "\nworking database: %ld" ), ( long )pDB );
        acutPrintf( _T( "\neditor: %ld" ), ( long )pEditor );
        acutPrintf( _T( "\ncurrent doc: %ld" ), ( long )pDoc );
        acutPrintf( _T( "\ncurrent doc database: %ld" ), ( long )pDocDB );
        acutPrintf( _T( "\ncurrent doc InputPointManager: %ld" ), ( long )pIPM );
    }

    // 可以用来处理与文档对象AcDocument相关联的reactor
    //  例如AcEdInputPointMonitor等
    // 在On_kLoadDwgMsg和On_kUnloadDwgMsg消息中文档对象还是有效的；
    // 而在On_kInitAppMsg和On_kUnloadAppMsg
    // 文档对象已经失效; 数据库对象同样也是如此
    virtual AcRx::AppRetCode On_kLoadDwgMsg( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxDbxApp::On_kLoadDwgMsg ( pkt ) ;

        //PrintAddress();

        ReactorHelper::AddDocumentReactor( curDoc() );

        acutPrintf( _T( "\nReactor::On_kLoadDwgMsg\n" ) );

        return retCode;
    }

    virtual AcRx::AppRetCode On_kUnloadDwgMsg( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxDbxApp::On_kUnloadDwgMsg ( pkt ) ;

        ReactorHelper::RemoveDocumentReactor( curDoc() );

        acutPrintf( _T( "\nReactor::On_kUnloadDwgMsg\n" ) );

        return retCode;
    }

    virtual void RegisterServerComponents ()
    {
    }

} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT( CReactorApp )

