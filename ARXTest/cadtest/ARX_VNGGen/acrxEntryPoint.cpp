#include "StdAfx.h"

#include "VNG_ParamHelper.h"
#include "VNG_ParamDlg.h"

#include "../MineGE/HelperClass.h"
#include "../ArxHelper/HelperClass.h"

// ����ע���������
#ifndef ARX_VNG_GEN_SERVICE_NAME
#define ARX_VNG_GEN_SERVICE_NAME _T("ARX_VNGGEN_SERVICE_NAME")
#endif

/* ȫ�ֺ���(ʵ����ARX_VNGGen.cpp) */
extern void ARX_VNGGen( const AcGePoint3d& basePt );

class CARX_VNGGenApp : public AcRxArxApp
{

public:
    CARX_VNGGenApp () : AcRxArxApp () {}

    virtual AcRx::AppRetCode On_kInitAppMsg ( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg ( pkt ) ;

        // ע�����
        acrxRegisterService( ARX_VNG_GEN_SERVICE_NAME );

        acutPrintf( _T( "\nARX_VNGGen::On_kInitAppMsg\n" ) );

        return ( retCode ) ;
    }

    virtual AcRx::AppRetCode On_kUnloadAppMsg ( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg ( pkt ) ;

        // ɾ������
        delete acrxServiceDictionary->remove( ARX_VNG_GEN_SERVICE_NAME );

        acutPrintf( _T( "\nARX_VNGGen::On_kUnloadAppMsg\n" ) );

        return ( retCode ) ;
    }

    virtual AcRx::AppRetCode On_kLoadDwgMsg( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxDbxApp::On_kLoadDwgMsg ( pkt ) ;

        // ��ʼ�����Ʋ���
        VNG_ParamHelper::InitParams();
        acutPrintf( _T( "\nARX_VNGGen::On_kLoadDwgMsg\n" ) );

        return retCode;
    }

    virtual AcRx::AppRetCode On_kUnloadDwgMsg( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxDbxApp::On_kUnloadDwgMsg( pkt ) ;

        acutPrintf( _T( "\nARX_VNGGen::On_kUnloadDwgMsg\n" ) );

        return retCode;
    }

    virtual void RegisterServerComponents ()
    {
    }

    static void JL_SetDrawParams( void )
    {
        // �л���Դ
        CAcModuleResourceOverride myResources;

        VNG_ParamDlg dlg;
        dlg.DoModal();
    }

    static void JL_DrawVentNetWorkGraph( void )
    {
        AcGePoint3d pt;
        if( !ArxUtilHelper::PromptPt( _T( "��ѡ��һ����׼��:" ), pt ) ) return;

        ARX_VNGGen( pt );
    }
} ;

IMPLEMENT_ARX_ENTRYPOINT( CARX_VNGGenApp )
ACED_ARXCOMMAND_ENTRY_AUTO( CARX_VNGGenApp, JL, _SetDrawParams, SetDrawParams, ACRX_CMD_TRANSPARENT, NULL )
ACED_ARXCOMMAND_ENTRY_AUTO( CARX_VNGGenApp, JL, _DrawVentNetWorkGraph, DrawVentNetWorkGraph, ACRX_CMD_TRANSPARENT, NULL )