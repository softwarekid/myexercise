#include "StdAfx.h"
#include "DrawHelper.h"

#include "LinkedGE.h"
#include "TagGE.h"

#include "CurDrawTool.h"

#include "../ArxHelper/HelperClass.h"
#include "../MineGEDraw/MineGEDrawSystem.h"

/* 全局函数(实现在EdgeJunctionClosure.cpp) */
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
    // 设置当前可视化效果
    if( !SetCurDraw( geType, drawName ) ) return;

    // 更新所有指定类型的图元
    AcDbObjectIdArray objIds;
    ArxDataTool::GetEntsByType( geType, objIds, false );
    if( objIds.isEmpty() ) return; // dwg图形中没有任何MineGE类型图元

    // 将所有geType类型图元的可视化效果更新
    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    if( pTrans == 0 ) return;

    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        AcDbObject* pObj;
        if( Acad::eOk != pTrans->getObject( pObj, objIds.at( i ), AcDb::kForWrite ) ) continue;

        MineGE* pGE = MineGE::cast( pObj );
        if( pGE == 0 ) continue;

        pGE->updateDraw();             // 更细可视化效果指针
        //pGE->recordGraphicsModified(true);  // 更新图形
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
    if( pDraw == 0 ) return; // 该可视化效果不存在

    pDraw->configExtraParams(); // 配置附加参数

    AcDbObjectIdArray objIds;
    ArxDataTool::GetEntsByType( geType, objIds, false );
    if( objIds.isEmpty() ) return; // dwg图形中没有任何geType类型图元

    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    if( pTrans == 0 ) return;

    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        AcDbObject* pObj;
        if( Acad::eOk != pTrans->getObject( pObj, objIds.at( i ), AcDb::kForWrite ) ) continue;

        MineGE* pGE = MineGE::cast( pObj );
        if( pGE == 0 ) continue;

        pGE->configDraw( pDraw->isA()->name() ); // 更新extra param
        pGE->recordGraphicsModified( true );   // 更新图形
    }
    actrTransactionManager->endTransaction();

    AcRxClass* pClass = AcRxClass::cast( acrxClassDictionary->at( geType ) );
    if( pClass != 0 && pClass->isDerivedFrom( LinkedGE::desc() ) )
    {
        UpdateLinkedGE( objIds );
    }
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
    if( pClass == 0 ) return; // 该类型图元尚未在CAD中注册

    // 查找所有geType及派生类的图元
    ArxDataTool::GetEntsByType( geType, objIds, true );
}

static void GetAllTagGEById_Helper( const AcDbObjectId& objId, const AcDbObjectIdArray& tagObjIds, AcDbObjectIdArray& objIds )
{
    if( tagObjIds.isEmpty() ) return;

    //acutPrintf(_T("\n找到%d个图元，进行比较判断..."), tagObjIds.length());
    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    if( pTrans == 0 ) return;

    int len = tagObjIds.length();
    for( int i = 0; i < len; i++ )
    {
        AcDbObject* pObj;
        if( Acad::eOk != pTrans->getObject( pObj, tagObjIds[i], AcDb::kForRead ) ) continue;

        TagGE* pTag = TagGE::cast( pObj );
        if( pTag == 0 ) continue;

        if( pTag->getHost() == objId )
        {
            objIds.append( tagObjIds[i] );
        }
    }
    actrTransactionManager->endTransaction();
}

void DrawHelper::GetAllTagGEById( const AcDbObjectId& objId, AcDbObjectIdArray& objIds )
{
    if( objId.isNull() ) return;

    //acutPrintf(_T("\n查找图元..."));
    AcDbObjectIdArray tagObjIds;
    ArxDataTool::GetEntsByType( _T( "TagGE" ), tagObjIds, true );

    // 筛选过滤出与ObjId关联的tag
    GetAllTagGEById_Helper( objId, tagObjIds, objIds );

    //acutPrintf(_T("\n剩余%d个图元..."), objIds.length());
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
    // 查找图元ObjId关联的所有标签
    AcDbObjectIdArray allObjIds;
    DrawHelper::GetAllTagGEById( objId, allObjIds );
    if( allObjIds.isEmpty() ) return;

    // 筛选出类型为geType的tag
    GetTagGEById2_Helper( geType, allObjIds, objIds );
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

    pEdge->reverse();  // 交换始末点坐标

    double angle = pEdge->getAngle(); // 反向后的角度

    // 调整所有方向类图元方向，包括风流方向
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

    host = pTag->getHost();
    actrTransactionManager->endTransaction();

    return true;
}

static void PrintMsg( int n )
{
    CString msg;
    msg.Format( _T( "该图元包含%d个标签图元" ), n );
    AfxMessageBox( msg );
}

void DrawHelper::ShowAllTagGE( const AcDbObjectId& objId, unsigned short colorIndex )
{
    AcDbObjectIdArray objIds;
    DrawHelper::GetAllTagGEById( objId, objIds );

    PrintMsg( objIds.length() );

    if( objIds.isEmpty() ) return;

    // 记录原来的颜色
    AcArray<Adesk::UInt16> colors;
    if( !ArxEntityHelper::GetEntitiesColor( objIds, colors ) ) return;

    // 用黄色高亮显示标签图元
    ArxEntityHelper::SetEntitiesColor( objIds, colorIndex );

    // 中断
    ArxUtilHelper::Pause();

    // 恢复原有颜色
    ArxEntityHelper::SetEntitiesColor2( objIds, colors );
}

void DrawHelper::ShowHostGE( const AcDbObjectId& objId, unsigned short colorIndex )
{
    // 获取标签图元关联的宿主
    AcDbObjectId host;
    if( !GetHostGE( objId, host ) ) return;

    AcDbObjectIdArray objIds;
    objIds.append( host );

    // 记录宿主图元的原颜色
    AcArray<Adesk::UInt16> colors;
    if( !ArxEntityHelper::GetEntitiesColor( objIds, colors ) ) return;

    // 用黄色高亮显示宿主图元
    ArxEntityHelper::SetEntitiesColor( objIds, colorIndex );

    // 中断
    ArxUtilHelper::Pause();

    // 恢复宿主的原有颜色
    ArxEntityHelper::SetEntitiesColor2( objIds, colors );
}

bool DrawHelper::GetCurrentDraw( const CString& type, CString& draw )
{
    return GetCurDraw( type, draw );
}