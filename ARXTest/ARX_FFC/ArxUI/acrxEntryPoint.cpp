#include "StdAfx.h"
#include "resource.h"

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CArxUIApp : public AcRxArxApp
{

public:
    CArxUIApp () : AcRxArxApp () {}

    virtual AcRx::AppRetCode On_kInitAppMsg ( void* pkt )
    {
        // TODO: Load dependencies here

        // You *must* call On_kInitAppMsg here
        AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg ( pkt ) ;

        // TODO: Add your initialization code here

        return ( retCode ) ;
    }

    virtual AcRx::AppRetCode On_kUnloadAppMsg ( void* pkt )
    {
        // TODO: Add your code here

        // You *must* call On_kUnloadAppMsg here
        AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg ( pkt ) ;

        // TODO: Unload dependencies here

        return ( retCode ) ;
    }

    virtual void RegisterServerComponents ()
    {
    }

} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT( CArxUIApp )

