#include "StdAfx.h"
#include "DrawHelper.h"

#include "LinkedGE.h"
#include "TagGE.h"
#include "ModelGE.h"

#include "CurDrawTool.h"

#include "../ArxHelper/HelperClass.h"
#include "../MineGEDraw/MineGEDrawSystem.h"

/* ȫ�ֺ���(ʵ����EdgeJunctionClosure.cpp) */
extern void LinkedGEJunctionClosure_Helper( const AcGePoint3d& junctionPt );

static MineGE* CreateGEByType( const CString& geType )
{
    AcRxObject* pClass = acrxClassDictionary->at( geType );
    if( pClass == 0 ) return 0;

    AcRxObject* pObj = AcRxClass::cast( pClass )->create();
    MineGE* pGE = MineGE::cast( pObj );
    if( pGE == 0 )
    {
        delete pObj;
        return 0;
    }
    return pGE;
}

void DrawHelper::LinkedGEJunctionClosure( const AcGePoint3d& junctionPt )
{
    LinkedGEJunctionClosure_Helper( junctionPt );
}

static void UpdateLinkedGE( const AcDbObjectIdArray& objIds )
{
    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        DrawHelper::LinkedGEJunctionClosure2( objIds[i] );
    }
}

void DrawHelper::SwitchDraw( const CString& geType, const CString& drawName )
{
    // ���õ�ǰ���ӻ�Ч��
    if( !SetCurDraw( geType, drawName ) ) return;

    // ��������ָ�����͵�ͼԪ
    AcDbObjectIdArray objIds;
    ArxDataTool::GetEntsByType( geType, objIds, false );
    if( objIds.isEmpty() ) return; // dwgͼ����û���κ�MineGE����ͼԪ

    // ������geType����ͼԪ�Ļ���Ч������
    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    if( pTrans == 0 ) return;

    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        AcDbObject* pObj;
        if( Acad::eOk != pTrans->getObject( pObj, objIds.at( i ), AcDb::kForWrite ) ) continue;

        MineGE* pGE = MineGE::cast( pObj );
        if( pGE == 0 ) continue;

        pGE->updateDraw();             // ��ϸ���ӻ�Ч��ָ��
        //pGE->recordGraphicsModified(true);  // ����ͼ��
    }

    actrTransactionManager->endTransaction();

    AcRxClass* pClass = AcRxClass::cast( acrxClassDictionary->at( geType ) );
    if( pClass != 0 && pClass->isDerivedFrom( LinkedGE::desc() ) )
    {
        UpdateLinkedGE( objIds );
    }
}

void DrawHelper::ConfigDraw( const CString& geType, const CString& drawName )
{
    MineGEDraw* pDraw = MineGEDrawSystem::GetInstance()->getGEDraw( geType, drawName );
    if( pDraw == 0 ) return; // �û���Ч��������

    pDraw->configExtraParams(); // ���ø��Ӳ���

    AcDbObjectIdArray objIds;
    ArxDataTool::GetEntsByType( geType, objIds, false );
    if( objIds.isEmpty() ) return; // dwgͼ����û���κ�geType����ͼԪ

    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    if( pTrans == 0 ) return;

    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        AcDbObject* pObj;
        if( Acad::eOk != pTrans->getObject( pObj, objIds.at( i ), AcDb::kForWrite ) ) continue;

        MineGE* pGE = MineGE::cast( pObj );
        if( pGE == 0 ) continue;

        pGE->configDraw( pDraw->isA()->name() ); // ����extra param
        pGE->recordGraphicsModified( true );   // ����ͼ��
    }
    actrTransactionManager->endTransaction();

    AcRxClass* pClass = AcRxClass::cast( acrxClassDictionary->at( geType ) );
    if( pClass != 0 && pClass->isDerivedFrom( LinkedGE::desc() ) )
    {
        UpdateLinkedGE( objIds );
    }
}

MineGE* DrawHelper::DrawByJig( const CString& geType )
{
    MineGE* pGE = CreateGEByType( geType );
    if( pGE == 0 ) return 0;

    if( !pGE->drawByJig() ) // jig���̻���ʧ��
    {
        delete pGE;
        return 0;
    }

    return pGE;
}

void DrawHelper::LinkedGEJunctionClosure2( const AcDbObjectId& objId )
{
    AcDbEntity* pEnt;
    if( Acad::eOk != acdbOpenAcDbEntity( pEnt, objId, AcDb::kForRead ) ) return;

    LinkedGE* pEdge = LinkedGE::cast( pEnt );
    if( pEdge == 0 )
    {
        pEdge->close();
        return;
    }
    pEdge->close();

    AcGePoint3d startPt, endPt;
    pEdge->getSEPoint( startPt, endPt );

    LinkedGEJunctionClosure( startPt );
    LinkedGEJunctionClosure( endPt );
}

void DrawHelper::GetAllRegGETypesForDraw( AcStringArray& allGETypes )
{
    ArxClassHelper::GetArxClassTypes( _T( "MineGE" ), allGETypes, false );
}

//static void printId(const CString& msg, const AcDbObjectIdArray& objIds)
//{
//	acutPrintf(_T("%s\n"), msg);
//	int len=objIds.length();
//	for(int i=0;i<len;i++)
//	{
//		acutPrintf(_T("\t--%d\n"), objIds[i]);
//	}
//	acutPrintf(_T("\n"));
//}

void DrawHelper::FindMineGEs( const CString& geType, AcDbObjectIdArray& objIds )
{
    AcRxClass* pClass = AcRxClass::cast( acrxClassDictionary->at( geType ) );
    if( pClass == 0 ) return; // ������ͼԪ��δ��CAD��ע��

    // ��������geType���������ͼԪ
    ArxDataTool::GetEntsByType( geType, objIds, true );
}

static void GetAllTagGEById_Helper( const AcDbObjectId& objId, const AcDbObjectIdArray& tagObjIds, AcDbObjectIdArray& objIds )
{
    if( tagObjIds.isEmpty() ) return;

    //acutPrintf(_T("\n�ҵ�%d��ͼԪ�����бȽ��ж�..."), tagObjIds.length());
    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    if( pTrans == 0 ) return;

    int len = tagObjIds.length();
    for( int i = 0; i < len; i++ )
    {
        AcDbObject* pObj;
        if( Acad::eOk != pTrans->getObject( pObj, tagObjIds[i], AcDb::kForRead ) ) continue;

        TagGE* pTag = TagGE::cast( pObj );
        if( pTag == 0 ) continue;

        if( pTag->getRelatedGE() == objId )
        {
            objIds.append( tagObjIds[i] );
        }
    }
    actrTransactionManager->endTransaction();
}

void DrawHelper::GetAllTagGEById( const AcDbObjectId& objId, AcDbObjectIdArray& objIds )
{
    if( objId.isNull() ) return;

    //acutPrintf(_T("\n����ͼԪ..."));
    AcDbObjectIdArray tagObjIds;
    ArxDataTool::GetEntsByType( _T( "TagGE" ), tagObjIds, true );

    // ɸѡ���˳���ObjId������tag
    GetAllTagGEById_Helper( objId, tagObjIds, objIds );

    //acutPrintf(_T("\nʣ��%d��ͼԪ..."), objIds.length());
}

static void GetTagGEById2_Helper( const CString& geType, const AcDbObjectIdArray& allObjIds, AcDbObjectIdArray& objIds )
{
    AcRxClass* pClass = AcRxClass::cast( acrxClassDictionary->at( geType ) );
    if( pClass == 0 ) return;

    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    if( pTrans == 0 ) return;

    int len = allObjIds.length();
    for( int i = 0; i < len; i++ )
    {
        AcDbObject* pObj;
        if( Acad::eOk != pTrans->getObject( pObj, allObjIds[i], AcDb::kForRead ) ) continue;

        if( pObj->isKindOf( pClass ) )
        {
            objIds.append( allObjIds[i] );
        }
    }
    actrTransactionManager->endTransaction();
}

void DrawHelper::GetTagGEById2( const AcDbObjectId& objId, const CString& geType, AcDbObjectIdArray& objIds )
{
    // ����ͼԪObjId���������б�ǩ
    AcDbObjectIdArray allObjIds;
    DrawHelper::GetAllTagGEById( objId, allObjIds );
    if( allObjIds.isEmpty() ) return;

    // ɸѡ������ΪgeType��tag
    GetTagGEById2_Helper( geType, allObjIds, objIds );
}

static void GetModelGEById_Helper( const AcDbObjectId& objId, const AcDbObjectIdArray& modelObjIds, AcDbObjectIdArray& objIds )
{
    if( modelObjIds.isEmpty() ) return;

    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    if( pTrans == 0 ) return;

    int len = modelObjIds.length();
    for( int i = 0; i < len; i++ )
    {
        AcDbObject* pObj;
        if( Acad::eOk != pTrans->getObject( pObj, modelObjIds[i], AcDb::kForRead ) ) continue;

        ModelGE* pModel = ModelGE::cast( pObj );
        if( pModel == 0 ) continue;

        if( pModel->getDataObject() == objId )
        {
            objIds.append( modelObjIds[i] );
        }
    }
    actrTransactionManager->endTransaction();
}

void DrawHelper::GetModelGEById( const AcDbObjectId& objId, AcDbObjectIdArray& objIds )
{
    if( objId.isNull() ) return;

    // ��������ģ��ͼԪModelGE
    AcDbObjectIdArray modelObjIds;
    ArxDataTool::GetEntsByType( _T( "ModelGE" ), modelObjIds, true );

    // ɸѡ��ͼԪ������ģ��ͼԪModelGE
    GetModelGEById_Helper( objId, modelObjIds, objIds );
}

bool DrawHelper::IsGETypeHasDrawJig( const CString& geType )
{
    MineGEDrawSystem* pInstance = MineGEDrawSystem::GetInstance();
    CString drawName = pInstance->getDefaultGEDrawName( geType );
    if( drawName.GetLength() == 0 ) return false; // ��ͼԪû��ע���ʵ���κο��ӻ�Ч��

    return ( pInstance->getGEDraw_Jig( geType, drawName ) != 0 );
}

void DrawHelper::ReverseDirection( const AcDbObjectId& objId )
{
    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    if( pTrans == 0 ) return;

    AcDbObject* pObj;
    if( Acad::eOk != pTrans->getObject( pObj, objId, AcDb::kForWrite ) )
    {
        actrTransactionManager->abortTransaction();
        return;
    }

    LinkedGE* pEdge = LinkedGE::cast( pObj );
    if( pEdge == 0 )
    {
        actrTransactionManager->abortTransaction();
        return;
    }

    pEdge->reverse();  // ����ʼĩ������

    double angle = pEdge->getAngle(); // �����ĽǶ�

    // �������з�����ͼԪ���򣬰�����������
    AcDbObjectIdArray objIds;
    DrawHelper::GetTagGEById2( objId, _T( "DirectionTagGE" ), objIds );
    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        if( Acad::eOk != pTrans->getObject( pObj, objIds[i], AcDb::kForWrite ) ) continue;

        DirectionTagGE* pDir = DirectionTagGE::cast( pObj );
        pDir->setDirectionAngle( angle );
    }

    actrTransactionManager->endTransaction();
}

static bool GetHostGE( const AcDbObjectId& objId, AcDbObjectId& host )
{
    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    if( pTrans == 0 ) return false;

    AcDbObject* pObj;
    if( Acad::eOk != pTrans->getObject( pObj, objId, AcDb::kForRead ) )
    {
        actrTransactionManager->abortTransaction();
        return false;
    }
    TagGE* pTag = TagGE::cast( pObj );
    if( pTag == 0 )
    {
        actrTransactionManager->abortTransaction();
        return false;
    }

    host = pTag->getRelatedGE();
    actrTransactionManager->endTransaction();

    return true;
}

static void PrintMsg( int n )
{
    CString msg;
    msg.Format( _T( "��ͼԪ����%d����ǩͼԪ" ), n );
    AfxMessageBox( msg );
}

void DrawHelper::ShowAllTagGE( const AcDbObjectId& objId, unsigned short colorIndex )
{
    AcDbObjectIdArray objIds;
    DrawHelper::GetAllTagGEById( objId, objIds );

    PrintMsg( objIds.length() );

    if( objIds.isEmpty() ) return;

    // ��¼ԭ������ɫ
    AcArray<Adesk::UInt16> colors;
    if( !ArxEntityHelper::GetEntitiesColor( objIds, colors ) ) return;

    // �û�ɫ������ʾ��ǩͼԪ
    ArxEntityHelper::SetEntitiesColor( objIds, colorIndex );

    // �ж�
    ArxUtilHelper::Pause();

    // �ָ�ԭ����ɫ
    ArxEntityHelper::SetEntitiesColor2( objIds, colors );
}

void DrawHelper::ShowHostGE( const AcDbObjectId& objId, unsigned short colorIndex )
{
    // ��ȡ��ǩͼԪ����������
    AcDbObjectId host;
    if( !GetHostGE( objId, host ) ) return;

    AcDbObjectIdArray objIds;
    objIds.append( host );

    // ��¼����ͼԪ��ԭ��ɫ
    AcArray<Adesk::UInt16> colors;
    if( !ArxEntityHelper::GetEntitiesColor( objIds, colors ) ) return;

    // �û�ɫ������ʾ����ͼԪ
    ArxEntityHelper::SetEntitiesColor( objIds, colorIndex );

    // �ж�
    ArxUtilHelper::Pause();

    // �ָ�������ԭ����ɫ
    ArxEntityHelper::SetEntitiesColor2( objIds, colors );
}

bool DrawHelper::GetCurrentDraw( const CString& type, CString& draw )
{
    return GetCurDraw( type, draw );
}