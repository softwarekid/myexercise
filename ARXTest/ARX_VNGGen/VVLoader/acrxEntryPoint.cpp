#include "StdAfx.h"
#include "resource.h"

static CString BuildArxFileName( const CString& arxModuleName )
{
    CString str;
    str.Format( _T( "%s.arx" ), arxModuleName );
    return str;
}

static CString BuildServiceName( const CString& arxModuleName )
{
    CString str;
    str.Format( _T( "%s_SERVICE_NAME" ), arxModuleName );
    return str.MakeUpper();
}

class CVVLoaderApp : public AcRxArxApp
{

public:
    CVVLoaderApp () : AcRxArxApp () {}

    virtual AcRx::AppRetCode On_kInitAppMsg ( void* pkt )
    {

        AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg ( pkt ) ;

        if( !loadArxModule( _T( "VNG_GE" ) ) ) return AcRx::kRetError;
        if( !loadArxModule( _T( "ARX_VNGGen" ) ) ) return AcRx::kRetError;

        //acutPrintf(_T("\nVVLoader::On_kInitAppMsg\n"));

        return ( retCode ) ;
    }

    virtual AcRx::AppRetCode On_kUnloadAppMsg ( void* pkt )
    {

        AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg ( pkt ) ;

        unloadArxModule( _T( "ARX_VNGGen" ) );
        unloadArxModule( _T( "VNG_GE" ) );

        //acutPrintf(_T("\nVVLoader::On_kUnloadAppMsg\n"));

        return ( retCode ) ;
    }

    virtual void RegisterServerComponents ()
    {
    }

    bool loadArxModule( const CString& arxModuleName )
    {
        CString arxName = BuildArxFileName( arxModuleName );
        CString serviceName = BuildServiceName( arxModuleName );

        AcRxObject* pSvc = acrxServiceDictionary->at ( serviceName );
        if( pSvc == NULL )
        {
            if( !acrxDynamicLinker->loadModule( arxName, false ) )
            {
                acutPrintf ( _T( "\n加载%s模块失败...\n" ), arxName ) ;
                return false;
            }
            acutPrintf ( _T( "\n成功加载%s模块...\n" ), arxName ) ;
        }
        else
        {
            acutPrintf ( _T( "\n%s模块已经加载过了...\n" ), arxName ) ;
        }
        return true;
    }

    void unloadArxModule( const CString& arxModuleName )
    {
        acrxDynamicLinker->unloadModule( BuildArxFileName( arxModuleName ), 0 );
        acutPrintf( _T( "\n卸载模块:%s\n" ), arxModuleName );
    }

} ;

IMPLEMENT_ARX_ENTRYPOINT( CVVLoaderApp )
