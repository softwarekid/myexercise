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
    AcDbObjectId objId = ArxUtilHelper::SelectObject( _T( "��ѡ��һ��ͼԪ" ) );
    if( objId.isNull() ) return;
    if( !ArxUtilHelper::IsEqualType( _T( "LinkedGE" ), objId ) ) return;

    double st = Timer::rdtscSeconds();

    FIPHelper fip;
    AcDbObjectIdArray p1, p2;
    bool ret = fip.doFIP( objId, p1, p2 );
    if( ret )
    {
        acutPrintf( _T( "\n��������·�߳ɹ�!" ) );
        AcDbObjectIdArray p;
        p.append( p1 );
        p.append( p2 );

        AcArray<Adesk::UInt16> colors;
        int len = p.length();
        for( int i = 0; i < len; i++ ) colors.append( 5 ); // ��ɫ

        if( !fip.isIncludeAirEdge( p1 ) && !fip.isIncludeAirEdge( p2 ) )
        {
            acutPrintf( _T( "\n�޴�����" ) );
        }
        else
        {
            acutPrintf( _T( "\n�д�����" ) );
            AcDbObjectIdArray se;
            fip.findSerialEdges( p, se );
            /*int */
            len = p.length();
            for( int i = 0; i < len; i++ )
            {
                if( se.contains( p[i] ) ) colors[i] = 2; // ��ɫ
            }
        }

        ArxUtilHelper::ShowEntitiesWithColor2( p, colors );
    }
    else
    {
        acutPrintf( _T( "\n��������·��ʧ��!" ) );
    }

    double et = Timer::rdtscSeconds();
    acutPrintf( _T( "\n���Ҷ���ͨ��·��FIP�ķ�ʱ�䣺%.5f" ), et - st );
}

void VNAlgoTest::SWDTest()
{
    double st = Timer::rdtscSeconds();

    if( VNAlgoHelper::DoSWD() )
    {
        acutPrintf( _T( "\n��/�ط绮�ֳɹ�!" ) );
    }
    else
    {
        acutPrintf( _T( "\n��/�ط绮��ʧ��!" ) );
    }

    double et = Timer::rdtscSeconds();
    acutPrintf( _T( "\n���ط绮��SWD�ķ�ʱ�䣺%.5f" ), et - st );
}

void VNAlgoTest::ILTTest2()
{
    double st = Timer::rdtscSeconds();

    ILTHelper ilt;
    AcDbObjectIdArray objIds;
    if( ilt.doILT2( objIds ) )
    {
        acutPrintf( _T( "\n����������ɹ�!" ) );
        acutPrintf( _T( "\n���������:%d" ), objIds.length() );
        ArxEntityHelper::SetEntitiesColor( objIds, 2 );
    }
    else
    {
        acutPrintf( _T( "\n����������ʧ��!" ) );
    }

    double et = Timer::rdtscSeconds();
    acutPrintf( _T( "\n����������(�㷨2)ILT�ķ�ʱ�䣺%.5f" ), et - st );
}

void VNAlgoTest::ILTTest3()
{
    double st = Timer::rdtscSeconds();

    ILTHelper ilt;
    AcDbObjectIdArray objIds;
    if( ilt.doILT3( objIds ) )
    {
        acutPrintf( _T( "\n����������ɹ�!" ) );
        acutPrintf( _T( "\n���������:%d" ), objIds.length() );
        ArxEntityHelper::SetEntitiesColor( objIds, 2 );
    }
    else
    {
        acutPrintf( _T( "\n����������ʧ��!" ) );
    }

    double et = Timer::rdtscSeconds();
    acutPrintf( _T( "\n����������(�㷨3)ILT�ķ�ʱ�䣺%.5f" ), et - st );
}

void VNAlgoTest::IntFlowTest()
{
    double st = Timer::rdtscSeconds();

    IntFlow iflow;
    iflow.doCacl();

    double et = Timer::rdtscSeconds();
    acutPrintf( _T( "\n������IFLOW�ķ�ʱ�䣺%.5f" ), et - st );
}

void VNAlgoTest::FWSTest()
{
    double st = Timer::rdtscSeconds();

    AcDbObjectId objId = ArxUtilHelper::SelectObject( _T( "��ѡ��һ��ͼԪ" ) );
    if( objId.isNull() ) return;
    if( !ArxUtilHelper::IsEqualType( _T( "LinkedGE" ), objId ) ) return;

    FWSHelper fws;
    AcDbObjectId wsObjId = fws.doFWS( objId );
    if( !wsObjId.isNull() )
    {
        acutPrintf( _T( "\n���Ҳ��վ�ɹ�" ) );
        ArxUtilHelper::ShowEntityWithColor( wsObjId, 2 );
    }
    else
    {
        acutPrintf( _T( "\n���Ҳ��վʧ��" ) );
    }

    double et = Timer::rdtscSeconds();
    acutPrintf( _T( "\n���Ҳ��վFWS�ķ�ʱ�䣺%.5f" ), et - st );
}

void VNAlgoTest::DFSTest()
{
    AcDbObjectId sObjId = ArxUtilHelper::SelectObject( _T( "\n��ѡ��һ�������Ϊ�����ġ�Դ�㡿:" ) );
    if( sObjId.isNull() ) return;
    if( !ArxUtilHelper::IsEqualType( _T( "LinkedGE" ), sObjId ) ) return;

    AcDbObjectId tObjId = ArxUtilHelper::SelectObject( _T( "\n��ѡ��һ�������Ϊ�����ġ���㡿:" ) );
    if( tObjId.isNull() ) return;
    if( !ArxUtilHelper::IsEqualType( _T( "LinkedGE" ), tObjId ) ) return;

    DfsNetWork dn;
    EdgeArray p;
    dn.doDFS( dn.getEdgeById( sObjId ), dn.getEdgeById( tObjId ), p );
    if( p.isEmpty() )
    {
        acutPrintf( _T( "\nû���ҵ�·��!" ) );
    }
    else
    {
        AcDbObjectIdArray objIds;
        int len = p.length();
        acutPrintf( _T( "\n·������:%d" ), len );
        for( int i = 0; i < len; i++ )
        {
            objIds.append( dn.getIdByEdge( p[i] ) );
        }
        ArxEntityHelper::SetEntitiesColor( objIds, 5 ); // ��ɫ������ʾ·��
    }
}

void VNAlgoTest::DoIDSTest( bool findNST )
{
    AcDbObjectId objId = ArxUtilHelper::SelectObject( _T( "\n��ѡ��һ�����:" ) );
    if( objId.isNull() ) return;
    if( !ArxUtilHelper::IsEqualType( _T( "LinkedGE" ), objId ) ) return;

    AcDbObjectIdArray p1, p2;
    IdentifyDiagonalStructure ids;
    if( !ids.doIDS( objId, p1, p2, findNST ) )
    {
        AfxMessageBox( _T( "��ѡ������ǽ�����֧!" ) );
    }
    else
    {
        acutPrintf( _T( "\n���ҽ����ɹ�..." ) );

        ArxUtilHelper::ShowEntitiesWithColor( p1, 2 );
        ArxUtilHelper::ShowEntitiesWithColor( p2, 5 );

        AcDbObjectIdArray objIds;
        objIds.append( p1 );
        objIds.append( p2 );

        AcArray<Adesk::UInt16> colors;
        int len = p1.length();
        for( int i = 0; i < len; i++ ) colors.append( 2 ); // ��ɫ������ʾ�����ͨ·

        len = p2.length();
        for( int i = 0; i < len; i++ ) colors.append( 5 ); // ��ɫ������ʾ�ҹ���ͨ·

        ArxUtilHelper::ShowEntitiesWithColor2( objIds, colors );
    }
}

void VNAlgoTest::IsIDSTest()
{
    AcDbObjectId objId = ArxUtilHelper::SelectObject( _T( "\n��ѡ��һ�����:" ) );
    if( objId.isNull() ) return;
    if( !ArxUtilHelper::IsEqualType( _T( "LinkedGE" ), objId ) ) return;

    IdentifyDiagonalStructure ids;
    if( !ids.isDS( objId ) )
    {
        AfxMessageBox( _T( "��ѡ������ǽ�����֧!" ) );
    }
    else
    {
        AfxMessageBox( _T( "��ѡ����ǽ�����֧!" ) );
    }
}