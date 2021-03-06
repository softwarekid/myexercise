#include "StdAfx.h"
#include "resource.h"

// 定义注册服务名称
#ifndef DRAW_FFC_CMD_SERVICE_NAME
#define DRAW_FFC_CMD_SERVICE_NAME _T("DRAWFFCCMD_SERVICE_NAME")
#endif

#include "DrawCmd.h"

class CDrawFFCCmdApp : public AcRxArxApp
{

public:
    CDrawFFCCmdApp () : AcRxArxApp () {}

    virtual AcRx::AppRetCode On_kInitAppMsg ( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg ( pkt ) ;

        // 注册服务
        acrxRegisterService( DRAW_FFC_CMD_SERVICE_NAME );

        acutPrintf( _T( "\nDrawFFCCmd::On_kLoadAppMsg\n" ) );

        return ( retCode ) ;
    }

    virtual AcRx::AppRetCode On_kUnloadAppMsg ( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg ( pkt ) ;

        // 删除服务
        delete acrxServiceDictionary->remove( DRAW_FFC_CMD_SERVICE_NAME );

        acutPrintf( _T( "\nDrawFFCCmd::On_kUnloadAppMsg\n" ) );

        return ( retCode ) ;
    }

    virtual AcRx::AppRetCode On_kLoadDwgMsg( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxDbxApp::On_kLoadDwgMsg ( pkt ) ;

        acutPrintf( _T( "\nDrawFFCCmd::On_kLoadDwgMsg\n" ) );

        return retCode;
    }

    virtual AcRx::AppRetCode On_kUnloadDwgMsg( void* pkt )
    {
        AcRx::AppRetCode retCode = AcRxDbxApp::On_kUnloadDwgMsg ( pkt ) ;

        acutPrintf( _T( "\nDrawFFCCmd::On_kUnloadDwgMsg\n" ) );

        return retCode;
    }

    virtual void RegisterServerComponents ()
    {
    }

    // 绘制永久挡风墙
    static void JL_DrawSandWall()
    {
        DrawCmd::DrawSandWall();
    }

    static void JL_DrawObturation()
    {
        DrawCmd::DrawObturation();
    }

    static void JL_DrawGoaf()
    {
        DrawCmd::DrawGoaf();
    }

    static void JL_DrawNitrogenPipe()
    {
        DrawCmd::DrawNitrogenPipe();
    }

    static void JL_DrawGasPipe()
    {
        DrawCmd::DrawGasPipe();
    }

    static void JL_DrawSourceAirLeak()
    {
        DrawCmd::DrawAirLeak( true );
    }

    static void JL_DrawSinkAirLeak()
    {
        DrawCmd::DrawAirLeak( false );
    }

    static void JL_SplitGoaf( void )
    {
        DrawCmd::SplitGoaf();
    }

    static void JL_RemoveGoafPoint( void )
    {
        DrawCmd::RemoveGoafPoint();
    }

    static void JL_AdvanceWS( void )
    {
        //	DrawCmd::AdvanceWS();
        DrawCmd::AdvanceWS2();
    }
} ;

IMPLEMENT_ARX_ENTRYPOINT( CDrawFFCCmdApp )

ACED_ARXCOMMAND_ENTRY_AUTO( CDrawFFCCmdApp, JL, _DrawSandWall, DrawSandWall, ACRX_CMD_TRANSPARENT, NULL )
ACED_ARXCOMMAND_ENTRY_AUTO( CDrawFFCCmdApp, JL, _DrawObturation, DrawObturation, ACRX_CMD_TRANSPARENT, NULL )
ACED_ARXCOMMAND_ENTRY_AUTO( CDrawFFCCmdApp, JL, _DrawGoaf, DrawGoaf, ACRX_CMD_TRANSPARENT, NULL )
ACED_ARXCOMMAND_ENTRY_AUTO( CDrawFFCCmdApp, JL, _DrawNitrogenPipe, DrawNitrogenPipe, ACRX_CMD_TRANSPARENT, NULL )
ACED_ARXCOMMAND_ENTRY_AUTO( CDrawFFCCmdApp, JL, _DrawGasPipe, DrawGasPipe, ACRX_CMD_TRANSPARENT, NULL )
ACED_ARXCOMMAND_ENTRY_AUTO( CDrawFFCCmdApp, JL, _DrawSourceAirLeak, DrawSourceAirLeak, ACRX_CMD_TRANSPARENT, NULL )
ACED_ARXCOMMAND_ENTRY_AUTO( CDrawFFCCmdApp, JL, _DrawSinkAirLeak, DrawSinkAirLeak, ACRX_CMD_TRANSPARENT, NULL )
ACED_ARXCOMMAND_ENTRY_AUTO( CDrawFFCCmdApp, JL, _SplitGoaf, SplitGoaf, ACRX_CMD_TRANSPARENT, NULL )
ACED_ARXCOMMAND_ENTRY_AUTO( CDrawFFCCmdApp, JL, _RemoveGoafPoint, RemoveGoafPoint, ACRX_CMD_TRANSPARENT, NULL )
//ACED_ARXCOMMAND_ENTRY_AUTO(CDrawFFCCmdApp, JL, _AdvanceWS, AdvanceWS, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO( CDrawFFCCmdApp, JL, _AdvanceWS, AdvanceWS, ACRX_CMD_TRANSPARENT | ACRX_CMD_USEPICKSET | ACRX_CMD_REDRAW, NULL )