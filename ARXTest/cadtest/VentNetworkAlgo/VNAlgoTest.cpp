#include "StdAfx.h"
#include "VNAlgoTest.h"

#include "VNAlgoHelper.h"
#include "FindIndependentPath.h"
#include "IntFlow.h"
#include "DfsNetWork.h"
#include "IdentifyDiagonalStructure.h"

#include "../ArxHelper/ArxUtilHelper.h"
#include "../ArxHelper/ArxEntityHelper.h"

//#include "../Tool/MyRandom.h"
#include "../Tool/Timer.h"

void VNAlgoTest::FIPTest()
{
    AcDbObjectId objId = ArxUtilHelper::SelectObject( _T( "请选择一个图元" ) );
    if( objId.isNull() ) return;
    if( !ArxUtilHelper::IsEqualType( _T( "LinkedGE" ), objId ) ) return;

    double st = Timer::rdtscSeconds();

    FIPHelper fip;
    AcDbObjectIdArray p1, p2;
    bool ret = fip.doFIP( objId, p1, p2 );
    if( ret )
    {
        acutPrintf( _T( "\n搜索独立路线成功!" ) );
        AcDbObjectIdArray p;
        p.append( p1 );
        p.append( p2 );

        AcArray<Adesk::UInt16> colors;
        int len = p.length();
        for( int i = 0; i < len; i++ ) colors.append( 5 ); // 蓝色

        if( !fip.isIncludeAirEdge( p1 ) && !fip.isIncludeAirEdge( p2 ) )
        {
            acutPrintf( _T( "\n无串联风" ) );
        }
        else
        {
            acutPrintf( _T( "\n有串联风" ) );
            AcDbObjectIdArray se;
            fip.findSerialEdges( p, se );
            /*int */
            len = p.length();
            for( int i = 0; i < len; i++ )
            {
                if( se.contains( p[i] ) ) colors[i] = 2; // 黄色
            }
        }

        ArxUtilHelper::ShowEntitiesWithColor2( p, colors );
    }
    else
    {
        acutPrintf( _T( "\n搜索独立路线失败!" ) );
    }

    double et = Timer::rdtscSeconds();
    acutPrintf( _T( "\n查找独立通风路线FIP耗费时间：%.5f" ), et - st );
}

void VNAlgoTest::SWDTest()
{
    double st = Timer::rdtscSeconds();

    if( VNAlgoHelper::DoSWD() )
    {
        acutPrintf( _T( "\n进/回风划分成功!" ) );
    }
    else
    {
        acutPrintf( _T( "\n进/回风划分失败!" ) );
    }

    double et = Timer::rdtscSeconds();
    acutPrintf( _T( "\n进回风划分SWD耗费时间：%.5f" ), et - st );
}

void VNAlgoTest::ILTTest2()
{
    double st = Timer::rdtscSeconds();

    ILTHelper ilt;
    AcDbObjectIdArray objIds;
    if( ilt.doILT2( objIds ) )
    {
        acutPrintf( _T( "\n查找联络巷成功!" ) );
        acutPrintf( _T( "\n联络巷个数:%d" ), objIds.length() );
        ArxEntityHelper::SetEntitiesColor( objIds, 2 );
    }
    else
    {
        acutPrintf( _T( "\n查找联络巷失败!" ) );
    }

    double et = Timer::rdtscSeconds();
    acutPrintf( _T( "\n查找联络巷(算法2)ILT耗费时间：%.5f" ), et - st );
}

void VNAlgoTest::ILTTest3()
{
    double st = Timer::rdtscSeconds();

    ILTHelper ilt;
    AcDbObjectIdArray objIds;
    if( ilt.doILT3( objIds ) )
    {
        acutPrintf( _T( "\n查找联络巷成功!" ) );
        acutPrintf( _T( "\n联络巷个数:%d" ), objIds.length() );
        ArxEntityHelper::SetEntitiesColor( objIds, 2 );
    }
    else
    {
        acutPrintf( _T( "\n查找联络巷失败!" ) );
    }

    double et = Timer::rdtscSeconds();
    acutPrintf( _T( "\n查找联络巷(算法3)ILT耗费时间：%.5f" ), et - st );
}

void VNAlgoTest::IntFlowTest()
{
    double st = Timer::rdtscSeconds();

    IntFlow iflow;
    iflow.doCacl();

    double et = Timer::rdtscSeconds();
    acutPrintf( _T( "\n整数流IFLOW耗费时间：%.5f" ), et - st );
}

void VNAlgoTest::FWSTest()
{
    double st = Timer::rdtscSeconds();

    AcDbObjectId objId = ArxUtilHelper::SelectObject( _T( "请选择一个图元" ) );
    if( objId.isNull() ) return;
    if( !ArxUtilHelper::IsEqualType( _T( "LinkedGE" ), objId ) ) return;

    FWSHelper fws;
    AcDbObjectId wsObjId = fws.doFWS( objId );
    if( !wsObjId.isNull() )
    {
        acutPrintf( _T( "\n查找测风站成功" ) );
        ArxUtilHelper::ShowEntityWithColor( wsObjId, 2 );
    }
    else
    {
        acutPrintf( _T( "\n查找测风站失败" ) );
    }

    double et = Timer::rdtscSeconds();
    acutPrintf( _T( "\n查找测风站FWS耗费时间：%.5f" ), et - st );
}

void VNAlgoTest::DFSTest()
{
    AcDbObjectId sObjId = ArxUtilHelper::SelectObject( _T( "\n请选择一条巷道作为搜索的【源点】:" ) );
    if( sObjId.isNull() ) return;
    if( !ArxUtilHelper::IsEqualType( _T( "LinkedGE" ), sObjId ) ) return;

    AcDbObjectId tObjId = ArxUtilHelper::SelectObject( _T( "\n请选择一条巷道作为搜索的【汇点】:" ) );
    if( tObjId.isNull() ) return;
    if( !ArxUtilHelper::IsEqualType( _T( "LinkedGE" ), tObjId ) ) return;

    DfsNetWork dn;
    EdgeArray p;
    dn.doDFS( dn.getEdgeById( sObjId ), dn.getEdgeById( tObjId ), p );
    if( p.isEmpty() )
    {
        acutPrintf( _T( "\n没有找到路径!" ) );
    }
    else
    {
        AcDbObjectIdArray objIds;
        int len = p.length();
        acutPrintf( _T( "\n路径长度:%d" ), len );
        for( int i = 0; i < len; i++ )
        {
            objIds.append( dn.getIdByEdge( p[i] ) );
        }
        ArxEntityHelper::SetEntitiesColor( objIds, 5 ); // 蓝色高亮显示路径
    }
}

void VNAlgoTest::DoIDSTest( bool findNST )
{
    AcDbObjectId objId = ArxUtilHelper::SelectObject( _T( "\n请选择一条巷道:" ) );
    if( objId.isNull() ) return;
    if( !ArxUtilHelper::IsEqualType( _T( "LinkedGE" ), objId ) ) return;

    AcDbObjectIdArray p1, p2;
    IdentifyDiagonalStructure ids;
    if( !ids.doIDS( objId, p1, p2, findNST ) )
    {
        AfxMessageBox( _T( "所选巷道不是角联分支!" ) );
    }
    else
    {
        acutPrintf( _T( "\n查找角联成功..." ) );

        ArxUtilHelper::ShowEntitiesWithColor( p1, 2 );
        ArxUtilHelper::ShowEntitiesWithColor( p2, 5 );

        AcDbObjectIdArray objIds;
        objIds.append( p1 );
        objIds.append( p2 );

        AcArray<Adesk::UInt16> colors;
        int len = p1.length();
        for( int i = 0; i < len; i++ ) colors.append( 2 ); // 黄色高亮显示左关联通路

        len = p2.length();
        for( int i = 0; i < len; i++ ) colors.append( 5 ); // 蓝色高亮显示右关联通路

        ArxUtilHelper::ShowEntitiesWithColor2( objIds, colors );
    }
}

void VNAlgoTest::IsIDSTest()
{
    AcDbObjectId objId = ArxUtilHelper::SelectObject( _T( "\n请选择一条巷道:" ) );
    if( objId.isNull() ) return;
    if( !ArxUtilHelper::IsEqualType( _T( "LinkedGE" ), objId ) ) return;

    IdentifyDiagonalStructure ids;
    if( !ids.isDS( objId ) )
    {
        AfxMessageBox( _T( "所选巷道不是角联分支!" ) );
    }
    else
    {
        AfxMessageBox( _T( "所选巷道是角联分支!" ) );
    }
}