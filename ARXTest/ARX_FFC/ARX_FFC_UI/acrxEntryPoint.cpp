#include "StdAfx.h"
#include "resource.h"

#include "UIHelper.h"

#ifndef ARX_FFC_UI_SERVICE_NAME
#define ARX_FFC_UI_SERVICE_NAME _T("ARX_FFC_UI_SERVICE_NAME")
#endif

class CARX_FFC_UIApp : public AcRxArxApp
{
public:
    CARX_FFC_UIApp () : AcRxArxApp () {}

    virtual AcRx::AppRetCode On_kInitAppMsg ( void* pkt )
    {

        AcRx::AppRetCode retCode = AcRxArxApp::On_kInitAppMsg ( pkt ) ;

        // ע�����
        acrxRegisterService( ARX_FFC_UI_SERVICE_NAME );

        /*
        		// 1) ʹ��com�������Ӳ˵��͹�����
        		// ���Ӳ˵�
        		UIHelper::RegAppMenu();
        		// ���ӹ�����
        		UIHelper::RegAppToolBar();
        */
        // 2) ����CUI�ļ�
        UIHelper::LoadCui();

        // 3) ʹ��arx�ӿ����ӹ������Ҽ��˵�
        UIHelper::RegObjectContextMenu( pkt );


        return ( retCode ) ;
    }

    virtual AcRx::AppRetCode On_kUnloadAppMsg ( void* pkt )
    {

        AcRx::AppRetCode retCode = AcRxArxApp::On_kUnloadAppMsg ( pkt );

        /*
        		// 1) ʹ��com����ж�ز˵��͹�����
        		// ж�ز˵�
        		UIHelper::UnRegAppMenu();
        		// ж�ع�����
        		UIHelper::UnRegAppToolBar();
        */

        // 2) ж��CUI�ļ�
        if( !acDocManager->isApplicationContext() )
        {
            UIHelper::UnloadCui();
        }

        // 3) ж��arx�ӿڹ������Ҽ��˵�
        UIHelper::UnRegObjectContextMenu();

        // ɾ������
        delete acrxServiceDictionary->remove( ARX_FFC_UI_SERVICE_NAME );

        return ( retCode ) ;
    }

    virtual void RegisterServerComponents ()
    {
    }

} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT( CARX_FFC_UIApp )
