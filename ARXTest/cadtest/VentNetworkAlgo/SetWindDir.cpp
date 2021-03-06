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
        datas[e] = 0; // 防止野指针
    }
    dg.clear();       // 删除图
}

// 设置独立用风分支
// 独立用风地点：采掘工作面、采区变电所
static bool SetEdgeType( const AcDbObjectId& objId, EdgeData* pEdgeData )
{
    bool ret = true;
    if( objId.isNull() )
    {
        pEdgeData->et = ET_VIRTUAL;
    }
    else if( ArxUtilHelper::IsEqualType( _T( "WorkSurface" ), objId ) )
    {
        pEdgeData->et = ET_USEAIR;  // 工作面(用风地点)
    }
    else if( ArxUtilHelper::IsEqualType( _T( "StorageGE" ), objId ) )
    {
        if( !ArxUtilHelper::IsEqualType( _T( "MachineRoom" ), objId ) )
        {
            pEdgeData->et = ET_USEAIR;  // 火药库或充电室
        }
        else if( IsPowerSubstation( objId ) )
        {
            pEdgeData->et = ET_USEAIR;  // 机电硐室--变电所(用风地点)
        }
    }
    else if( ArxUtilHelper::IsEqualType( _T( "TTunnel" ), objId ) )
    {
        pEdgeData->et = ET_USEAIR;  // 掘进工作面
    }
    //else if(IsNeedAir(objId))
    //{
    //	pEdgeData->et = ET_USEAIR; // 需风巷道
    //}
    return ret;
}

static void InitEdgeDatas( Digraph& dg, ObjectIdMap& om, SWD_EdgeDataMap& datas )
{
    // 关联分支属性
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        EdgeData* pEdgeData = new EdgeData();
        pEdgeData->objId = om[e];
        SetEdgeType( om[e], pEdgeData );
        datas[e] = pEdgeData; // 关联一个Digraph::Arc属性
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
                DataHelper::SetPropertyData( dirs[i], _T( "通风类型" ), v );
                break;
            }
        }
    }
}

SetWindDir::SetWindDir( void ) : datas( dg ), ef( dg, true ), bInitNetwokState( true )
{
    // 初始化网络以及添加处理源汇
    if( !initNetwok() )
    {
        ClearGraph( dg, datas );
        setInitNetwokState( false );
    }
    else
    {
        // 初始化用风分支
        initAirEdges();
    }
}

SetWindDir::~SetWindDir( void )
{
    if( bInitNetwokState )
    {
        // 不需要更新数据
        UpdateData( dg, datas );
        ClearGraph( dg, datas );
    }
}

bool SetWindDir::initNetwok()
{
    // 构建网络
    ObjectIdMap om( dg );
    if( !BuildNetwork( dg, om ) ) return false;
    if( !AddVirtualSTNode( dg, om, sn, tn ) ) return false;
    FilterBlockEdges( dg, om, ef );

    // 关联分支属性数据
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

// 污风visitor
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
        //acutPrintf(_T("\n分支被设置为回风"));
    }

    SWD_EdgeDataMap& datas;
};

bool SetWindDir::doSWD()
{
    if( !isInitNetworkOk() ) return false;

    // 1、将所有分支风流方向类型设置为新风
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        datas[e]->vt = VT_IN;
    }

    // 过滤阻断分支后的图
    AFGraph afg( dg, ef );

    double st = Timer::rdtscSeconds();

    // 2、从所有用风地点的末点开始，依次执行一次dfs
    //    遍历到的所有分支均视作回风分支
    int len = airEdges.length();
    acutPrintf( _T( "\n用风地点个数:%d" ), len );
    for( int i = 0; i < len; i++ )
    {
        Digraph::Arc e = airEdges[i];
        PolluteAirVisitor visitor( datas );
        DfsVisit<AFGraph, PolluteAirVisitor> dv( afg, visitor );
        if( ArxUtilHelper::IsEqualType( _T( "TTunnel" ), datas[e]->objId ) )
        {
            dv.run( dg.source( e ) ); // 如果是掘进工作面，则从始节点开始dfs
        }
        else
        {
            dv.run( dg.target( e ) ); // 其它独立用风分支从末节点开始dfs
        }
    }

    double et = Timer::rdtscSeconds();
    acutPrintf( _T( "\ndfs耗费时间:%.5f" ), et - st );

    st = Timer::rdtscSeconds();

    // 遍历所有阻断分支
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        if( !ef[e] ) // false表示阻断分支
        {
            // 查找始节点的入边分支是否包含回风
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
    acutPrintf( _T( "\n修改阻断分支的通风类型耗费时间:%.5f" ), et - st );

    return true;
}