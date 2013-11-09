#include "StdAfx.h"
#include "resource.h"

#include "ARX_VNC.h"

// ����ע���������
#ifndef ARX_VNC_SERVICE_NAME
#define ARX_VNC_SERVICE_NAME _T("ARX_VNC_SERVICE_NAME")
#endif

class CARX_VNCApp : public AcRxArxApp
{

public:
    CARX_VNCApp () : AcRxArxApp () {}

    virtual AcRx::AppRetCode On_kInitAppMsg ( void* pkt )
    {

        AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg ( pkt ) ;

        // ע�����
        acrxRegisterService( ARX_VNC_SERVICE_NAME );

        acutPrintf( _T( "\nARX_VNC::On_kLoadAppMsg\n" ) );

        return ( retCode ) ;
    }

    virtual AcRx::AppRetCode On_kUnloadAppMsg ( void* pkt )
    {

        AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg ( pkt ) ;

        // ɾ������
        delete acrxServiceDictionary->remove( ARX_VNC_SERVICE_NAME );

        acutPrintf( _T( "\nARX_VNC::On_kUnloadAppMsg\n" ) );

        return ( retCode ) ;
    }

    virtual AcRx::AppRetCode On_kLoadDwgMsg( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxDbxApp::On_kLoadDwgMsg ( pkt );

        acutPrintf( _T( "\nARX_VNC::On_kLoadDwgMsg\n" ) );

        return retCode;
    }

    virtual AcRx::AppRetCode On_kUnloadDwgMsg( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxDbxApp::On_kUnloadDwgMsg( pkt ) ;

        acutPrintf( _T( "\nARX_VNC::On_kUnloadDwgMsg\n" ) );

        return retCode;
    }

    virtual void RegisterServerComponents ()
    {
    }

    static void VNCTest_RandomAssignParam( void )
    {
        /* ȫ�ֺ���(ʵ����RandomAssignR.cpp��) */
        extern void RandomAssignParam();

        acutPrintf( _T( "\n���������衢����Ȳ�������..." ) );
        RandomAssignParam();
    }

    static void VNCTest_ARXVNC( void )
    {
        acutPrintf( _T( "\n����VNC.dll����..." ) );
        ARX_VNC();
    }
} ;

IMPLEMENT_ARX_ENTRYPOINT( CARX_VNCApp )
ACED_ARXCOMMAND_ENTRY_AUTO( CARX_VNCApp, VNCTest, _RandomAssignParam, RandomAssignParam, ACRX_CMD_TRANSPARENT, NULL )
ACED_ARXCOMMAND_ENTRY_AUTO( CARX_VNCApp, VNCTest, _ARXVNC, ARRXVNC, ACRX_CMD_TRANSPARENT, NULL )