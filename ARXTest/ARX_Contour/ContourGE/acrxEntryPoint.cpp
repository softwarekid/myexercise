#include "StdAfx.h"
#include "resource.h"

#ifndef CONTOURGE_SERVICE_NAME
#define CONTOURGE_SERVICE_NAME _T("CONTOURGE_SERVICE_NAME")
#endif

class CContourGEApp : public AcRxArxApp
{

public:
    CContourGEApp () : AcRxArxApp () {}

    virtual AcRx::AppRetCode On_kInitAppMsg ( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg ( pkt ) ;

        acrxRegisterService( CONTOURGE_SERVICE_NAME );
        acutPrintf( _T( "\nContourGE::On_kLoadAppMsg\n" ) );

        return ( retCode ) ;
    }

    virtual AcRx::AppRetCode On_kUnloadAppMsg ( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg ( pkt ) ;

        delete acrxServiceDictionary->remove( CONTOURGE_SERVICE_NAME );
        acutPrintf( _T( "\nContourGE::On_kUnloadAppMsg\n" ) );

        return ( retCode ) ;
    }

    virtual void RegisterServerComponents ()
    {
    }

} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT( CContourGEApp )

