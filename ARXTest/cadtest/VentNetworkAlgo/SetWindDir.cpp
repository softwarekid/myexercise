#include "StdAfx.h"
#include "SetWindDir.h"

#include "../DefGE/WindDirection.h"

#include "../MineGE/DrawHelper.h"
#include "../MineGE/DataHelper.h"
#include "../MineGE/ConstData.h"

#include "../ArxHelper/ArxUtilHelper.h"
#include "../ArxHelper/ArxEntityHelper.h"

#include <lemon/adaptors.h>
#include <lemon/dfs.h>
typedef FilterArcs<Digraph, ArcFilter> AFGraph;

//#include "../Tool/MyRandom.h"
#include "../Tool/Timer.h"

#include "../GraphTool/Tool.h"

static void ClearGraph( Digraph& dg, SWD_EdgeDataMap& datas )
{
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        delete datas[e];
        datas[e] = 0; // ��ֹҰָ��
    }
    dg.clear();       // ɾ��ͼ
}

// ���ö����÷��֧
// �����÷�ص㣺�ɾ����桢���������
static bool SetEdgeType( const AcDbObjectId& objId, EdgeData* pEdgeData )
{
    bool ret = true;
    if( objId.isNull() )
    {
        pEdgeData->et = ET_VIRTUAL;
    }
    else if( ArxUtilHelper::IsEqualType( _T( "WorkSurface" ), objId ) )
    {
        pEdgeData->et = ET_USEAIR;  // ������(�÷�ص�)
    }
    else if( ArxUtilHelper::IsEqualType( _T( "StorageGE" ), objId ) )
    {
        if( !ArxUtilHelper::IsEqualType( _T( "MachineRoom" ), objId ) )
        {
            pEdgeData->et = ET_USEAIR;  // ��ҩ�������
        }
        else if( IsPowerSubstation( objId ) )
        {
            pEdgeData->et = ET_USEAIR;  // ��������--�����(�÷�ص�)
        }
    }
    else if( ArxUtilHelper::IsEqualType( _T( "TTunnel" ), objId ) )
    {
        pEdgeData->et = ET_USEAIR;  // ���������
    }
    //else if(IsNeedAir(objId))
    //{
    //	pEdgeData->et = ET_USEAIR; // ������
    //}
    return ret;
}

static void InitEdgeDatas( Digraph& dg, ObjectIdMap& om, SWD_EdgeDataMap& datas )
{
    // ������֧����
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        EdgeData* pEdgeData = new EdgeData();
        pEdgeData->objId = om[e];
        SetEdgeType( om[e], pEdgeData );
        datas[e] = pEdgeData; // ����һ��Digraph::Arc����
    }
}

static void UpdateData( Digraph& dg, SWD_EdgeDataMap& datas )
{
    AcDbObjectIdArray dirs;
    DrawHelper::FindMineGEs( _T( "WindDirection" ), dirs );
    if( dirs.isEmpty() ) return;

    AcDbObjectIdArray hosts;
    GetHosts( dirs, hosts );

    int len = dirs.length();
    for( int i = 0; i < len; i++ )
    {
        for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
        {
            if( hosts[i] == datas[e]->objId )
            {
                CString v;
                v.Format( _T( "%d" ), datas[e]->vt );
                DataHelper::SetPropertyData( dirs[i], _T( "ͨ������" ), v );
                break;
            }
        }
    }
}

SetWindDir::SetWindDir( void ) : datas( dg ), ef( dg, true ), bInitNetwokState( true )
{
    // ��ʼ�������Լ����Ӵ���Դ��
    if( !initNetwok() )
    {
        ClearGraph( dg, datas );
        setInitNetwokState( false );
    }
    else
    {
        // ��ʼ���÷��֧
        initAirEdges();
    }
}

SetWindDir::~SetWindDir( void )
{
    if( bInitNetwokState )
    {
        // ����Ҫ��������
        UpdateData( dg, datas );
        ClearGraph( dg, datas );
    }
}

bool SetWindDir::initNetwok()
{
    // ��������
    ObjectIdMap om( dg );
    if( !BuildNetwork( dg, om ) ) return false;
    if( !AddVirtualSTNode( dg, om, sn, tn ) ) return false;
    FilterBlockEdges( dg, om, ef );

    // ������֧��������
    InitEdgeDatas( dg, om, datas );

    return true;
}

bool SetWindDir::isInitNetworkOk() const
{
    return bInitNetwokState;
}

void SetWindDir::setInitNetwokState( bool bState )
{
    bInitNetwokState = bState;
}

void SetWindDir::initAirEdges()
{
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        if( datas[e]->et == ET_USEAIR )
        {
            airEdges.append( e );
        }
    }
}

// �۷�visitor
class PolluteAirVisitor : public DfsVisitor<AFGraph>
{
public:
    PolluteAirVisitor( SWD_EdgeDataMap& datas_ ) : datas( datas_ ) {}
    void discover( const Digraph::Arc& e )
    {
        setPolluteAirFlag( e );
    }
    void examine( const Digraph::Arc& e )
    {
        setPolluteAirFlag( e );
    }

private:
    void setPolluteAirFlag( const Digraph::Arc& e )
    {
        datas[e]->vt = VT_OUT;
        //acutPrintf(_T("\n��֧������Ϊ�ط�"));
    }

    SWD_EdgeDataMap& datas;
};

bool SetWindDir::doSWD()
{
    if( !isInitNetworkOk() ) return false;

    // 1�������з�֧����������������Ϊ�·�
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        datas[e]->vt = VT_IN;
    }

    // ������Ϸ�֧���ͼ
    AFGraph afg( dg, ef );

    double st = Timer::rdtscSeconds();

    // 2���������÷�ص��ĩ�㿪ʼ������ִ��һ��dfs
    //    �����������з�֧�������ط��֧
    int len = airEdges.length();
    acutPrintf( _T( "\n�÷�ص����:%d" ), len );
    for( int i = 0; i < len; i++ )
    {
        Digraph::Arc e = airEdges[i];
        PolluteAirVisitor visitor( datas );
        DfsVisit<AFGraph, PolluteAirVisitor> dv( afg, visitor );
        if( ArxUtilHelper::IsEqualType( _T( "TTunnel" ), datas[e]->objId ) )
        {
            dv.run( dg.source( e ) ); // ����Ǿ�������棬���ʼ�ڵ㿪ʼdfs
        }
        else
        {
            dv.run( dg.target( e ) ); // ���������÷��֧��ĩ�ڵ㿪ʼdfs
        }
    }

    double et = Timer::rdtscSeconds();
    acutPrintf( _T( "\ndfs�ķ�ʱ��:%.5f" ), et - st );

    st = Timer::rdtscSeconds();

    // ����������Ϸ�֧
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        if( !ef[e] ) // false��ʾ��Ϸ�֧
        {
            // ����ʼ�ڵ����߷�֧�Ƿ�����ط�
            Digraph::Node u = dg.source( e );
            for( Digraph::InArcIt ie( dg, u ); ie != INVALID; ++ie )
            {
                if( datas[ie]->vt == VT_OUT )
                {
                    datas[e]->vt = VT_OUT;
                    break;
                }
            }
        }
    }

    et = Timer::rdtscSeconds();
    acutPrintf( _T( "\n�޸���Ϸ�֧��ͨ�����ͺķ�ʱ��:%.5f" ), et - st );

    return true;
}