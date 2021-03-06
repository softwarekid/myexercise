#include "StdAfx.h"
#include "resource.h"

#include "EvalTest.h"
#include "AFCTest.h"

// 定义注册服务名称
#ifndef ALLTESTS_SERVICE_NAME
#define ALLTESTS_SERVICE_NAME _T("ALLTESTS_SERVICE_NAME")
#endif

class CAllTestsApp : public AcRxArxApp
{
public:
    CAllTestsApp () : AcRxArxApp () {}

    virtual AcRx::AppRetCode On_kInitAppMsg ( void* pkt )
    {

        AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg ( pkt ) ;

        // 注册服务
        acrxRegisterService( ALLTESTS_SERVICE_NAME );

        acutPrintf( _T( "\nAllTests::On_kInitAppMsg\n" ) );

        return ( retCode ) ;
    }

    virtual AcRx::AppRetCode On_kUnloadAppMsg ( void* pkt )
    {

        AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg ( pkt ) ;

        // 删除服务
        delete acrxServiceDictionary->remove( ALLTESTS_SERVICE_NAME );

        acutPrintf( _T( "\nAllTests::On_kUnloadAppMsg\n" ) );

        return ( retCode ) ;
    }

    virtual void RegisterServerComponents ()
    {
    }

    // - VentEvaluate._evalTest command (do not rename)
    static void VentEvaluate_evalTest( void )
    {
        acutPrintf( _T( "\n评价测试...\n" ) );
        EvalTest et;
        et.eval_test();
    }

    // - VentEvaluate._afcTest command (do not rename)
    static void VentEvaluate_afcTest( void )
    {
        acutPrintf( _T( "\n风量计算测试...\n" ) );
        AFCTest afc_test;
        afc_test.ws_afc_test();
        afc_test.tws_afc_test();
        afc_test.ot_afc_test();
        afc_test.dhc_afc_test();
    }

    static void VentEvaluate_afcTest2( void )
    {
        acutPrintf( _T( "\n风量计算测试...\n" ) );
        AFCTest afc_test;
        afc_test.all_afc_test();
    }
} ;

IMPLEMENT_ARX_ENTRYPOINT( CAllTestsApp )

ACED_ARXCOMMAND_ENTRY_AUTO( CAllTestsApp, VentEvaluate, _evalTest, evalTest, ACRX_CMD_TRANSPARENT, NULL )
ACED_ARXCOMMAND_ENTRY_AUTO( CAllTestsApp, VentEvaluate, _afcTest, afcTest, ACRX_CMD_TRANSPARENT, NULL )
ACED_ARXCOMMAND_ENTRY_AUTO( CAllTestsApp, VentEvaluate, _afcTest2, afcTest2, ACRX_CMD_TRANSPARENT, NULL )

