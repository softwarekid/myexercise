#include "StdAfx.h"
#include "JointHelper.h"

#include "../DefGE/Joint.h"
#include "../MineGE/LinkedGE.h"
#include "../MineGE/DrawHelper.h"

static void FindEdgeGEs( AcDbObjectIdArray& objIds )
{
    DrawHelper::FindMineGEs( _T( "LinkedGE" ), objIds );
}

static void FindJoints( AcDbObjectIdArray& objIds )
{
    DrawHelper::FindMineGEs( _T( "Joint" ), objIds );
}

static void GetJointPts( const AcDbObjectIdArray& objIds, AcGePoint3dArray& pts )
{
    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    if( pTrans == 0 ) return;

    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        AcDbObject* pObj;
        if( Acad::eOk != pTrans->getObject( pObj, objIds[i], AcDb::kForRead ) ) continue;

        Joint* pJoint = Joint::cast( pObj );
        if( pJoint == 0 ) continue;

        AcGePoint3d pt = pJoint->getInsertPoint();
        if( !pts.contains( pt ) ) pts.append( pt );
    }
    actrTransactionManager->endTransaction();
}

static void GetNoJointEdges( const AcDbObjectIdArray& objIds, const AcGePoint3dArray& pts, AcDbObjectIdArray& objIds2 )
{
    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    if( pTrans == 0 ) return;

    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        AcDbObject* pObj;
        if( Acad::eOk != pTrans->getObject( pObj, objIds[i], AcDb::kForRead ) ) continue;

        LinkedGE* pEdge = LinkedGE::cast( pObj );
        if( pEdge == 0 ) continue;

        AcGePoint3d startPt, endPt;
        pEdge->getSEPoint( startPt, endPt );

        if( !pts.contains( startPt ) || !pts.contains( endPt ) )
        {
            objIds2.append( objIds[i] );
        }
    }
    actrTransactionManager->endTransaction();
}

static void GetNodePts( const AcDbObjectIdArray& objIds, AcGePoint3dArray& pts )
{
    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        AcDbObject* pObj;
        if( Acad::eOk != pTrans->getObject( pObj, objIds[i], AcDb::kForRead ) ) continue;
        LinkedGE* pEdge = LinkedGE::cast( pObj );

        AcGePoint3d startPt, endPt;
        pEdge->getSEPoint( startPt, endPt );
        if( !pts.contains( startPt ) ) pts.append( startPt );
        if( !pts.contains( endPt ) ) pts.append( endPt );
    }
    actrTransactionManager->endTransaction();
}

void JointHelper::FindNoJointEdges( AcDbObjectIdArray& objIds )
{
    // 查找所有的巷道
    AcDbObjectIdArray edgeObjIds;
    FindEdgeGEs( edgeObjIds );
    if( edgeObjIds.isEmpty() ) return;

    // 查找所有的节点
    AcDbObjectIdArray jointObjIds;
    FindJoints( jointObjIds );
    if( jointObjIds.isEmpty() )
    {
        objIds.append( edgeObjIds );
        return;
    }

    // 获取所有节点的点坐标
    AcGePoint3dArray pts;
    GetJointPts( jointObjIds, pts );

    // 获取没有设置节点的巷道
    GetNoJointEdges( edgeObjIds, pts, objIds );
}