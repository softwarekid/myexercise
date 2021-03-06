#include "stdafx.h"

#include "../MineGE/LinkedGE.h"
#include "../DefGE/WindDirection.h"

#include "../MineGE/HelperClass.h"
#include "../ArxHelper/HelperClass.h"

static void GetSEPts( const AcDbObjectIdArray& objIds, AcGePoint3dArray& spts, AcGePoint3dArray& epts )
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

        AcGePoint3d spt, ept;
        pEdge->getSEPoint( spt, ept );

        spts.append( spt );
        epts.append( ept );
    }
    actrTransactionManager->endTransaction();
}

static void EraseAllDirections()
{
    AcDbObjectIdArray dirs;
    DrawHelper::FindMineGEs( _T( "WindDirection" ), dirs );
    ArxEntityHelper::EraseObjects( dirs, true );
}

static void AddDirection( const AcDbObjectId& objId, const AcGePoint3d& spt, const AcGePoint3d& ept )
{
    AcGeVector3d v = ept - spt;
    if( v.length() < 100 ) return;

    double angle = v.angleTo( AcGeVector3d::kXAxis, -AcGeVector3d::kZAxis );
    WindDirection* pDir = new WindDirection( spt + v * 0.5, angle ); // 巷道中心位置
    pDir->setRelatedGE( objId );
    ArxUtilHelper::PostToModelSpace( pDir );
}

void AutoDirection()
{
    AcDbObjectIdArray objIds;
    DrawHelper::FindMineGEs( _T( "LinkedGE" ), objIds );
    if( objIds.isEmpty() ) return;

    // 删除所有的风流方向
    EraseAllDirections();

    AcGePoint3dArray spts, epts;
    GetSEPts( objIds, spts, epts );

    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        // 添加风流方向
        AddDirection( objIds[i], spts[i], epts[i] );
    }
}