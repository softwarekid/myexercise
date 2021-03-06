#include "StdAfx.h"
#include "resource.h"

#include "ReactorHelper.h"

#ifndef PROPDATA_SERVICE_NAME
#define PROPDATA_SERVICE_NAME _T("PROPDATA_SERVICE_NAME")
#endif

class CPropDataApp : public AcRxArxApp
{

public:
    CPropDataApp () : AcRxArxApp () {}

    virtual AcRx::AppRetCode On_kInitAppMsg ( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg ( pkt ) ;

        acrxRegisterService( PROPDATA_SERVICE_NAME );

        // 创建文档相关的reactor
        ReactorHelper::CreateDocumentReactorMap();

        acutPrintf( _T( "\nPropData::On_kLoadAppMsg\n" ) );

        return ( retCode ) ;
    }

    virtual AcRx::AppRetCode On_kUnloadAppMsg ( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg ( pkt ) ;

        // 删除文档reactor
        ReactorHelper::RemoveDocumentReactorMap();

        delete acrxServiceDictionary->remove( PROPDATA_SERVICE_NAME );

        acutPrintf( _T( "\nPropData::On_kLoadAppMsg\n" ) );

        return ( retCode ) ;
    }

    virtual AcRx::AppRetCode On_kLoadDwgMsg( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxDbxApp::On_kLoadDwgMsg ( pkt ) ;

        // 关联文档
        ReactorHelper::AddDocumentReactor( curDoc() );

        acutPrintf( _T( "\nPropData::On_kLoadDwgMsg\n" ) );

        return retCode;
    }

    virtual AcRx::AppRetCode On_kUnloadDwgMsg( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxDbxApp::On_kUnloadDwgMsg ( pkt ) ;

        // 解除文档关联
        ReactorHelper::RemoveDocumentReactor( curDoc() );

        acutPrintf( _T( "\nPropData::On_kUnloadDwgMsg\n" ) );

        return retCode;
    }

    virtual void RegisterServerComponents ()
    {
    }

} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT( CPropDataApp )

