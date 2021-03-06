#include "StdAfx.h"
#include "FindWindStation.h"

#include <vector>
#include <algorithm>

#include <lemon/adaptors.h>
#include <lemon/dfs.h>
typedef Path<Digraph> DPath;

#include "../MineGE/DrawHelper.h"
#include "../ArxHelper/ArxUtilHelper.h"
#include "../ArxHelper/ArxEntityHelper.h"

#include "../GraphTool/Tool.h"

static void ClearGraph( Digraph& dg, FWS_EdgeDataMap& datas )
{
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        delete datas[e];
        datas[e] = 0; // 防止野指针
    }
    dg.clear();       // 删除图
}

// 用测风地点：采掘工作面、硐室
// 还应该包括主要进回大巷，采区进回大巷(但目前还没有办法识别出来)
static void SetEdgeType( const AcDbObjectId& objId, EdgeData* pEdgeData )
{
    if( objId.isNull() )
    {
        pEdgeData->et = ET_VIRTUAL; // 虚拟分支
    }
    else if( ArxUtilHelper::IsEqualType( _T( "WorkSurface" ), objId ) )
    {
        pEdgeData->et = ET_USEAIR;  // 工作面
    }
    else if( ArxUtilHelper::IsEqualType( _T( "StorageGE" ), objId ) )
    {
        pEdgeData->et = ET_USEAIR;  // 硐室
    }
    else if( ArxUtilHelper::IsEqualType( _T( "TTunnel" ), objId ) )
    {
        pEdgeData->et = ET_USEAIR;  // 掘进工作面
    }
    /*else if(IsNeedAir(objId))
    {
    	pEdgeData->et = ET_USEAIR;
    }*/
}

static void InitEdgeDatas( Digraph& dg, ObjectIdMap& om, FWS_EdgeDataMap& datas )
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

FindWindStation::FindWindStation( void ) : datas( dg ), ef( dg, true ), bInitNetwokState( true )
{
    // 初始化网络以及添加处理源汇
    if( !initNetwok() )
    {
        ClearGraph( dg, datas );
        setInitNetwokState( false );
    }
    else
    {
        initAirEdges();   // 记录用风地点分支
    }
}

FindWindStation::~FindWindStation( void )
{
    if( bInitNetwokState )
    {
        // 不需要更新数据
//		UpdateData(dg, datas);
        ClearGraph( dg, datas );
    }
}

Digraph::Arc FindWindStation::getEdgeById( const AcDbObjectId& objId )
{
    if( objId.isNull() ) return INVALID;

    Digraph::Arc ee = INVALID;
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        if( datas[e]->objId == objId )
        {
            ee = e;
            break;
        }
    }
    return ee;
}

AcDbObjectId FindWindStation::getIdByEdge( Digraph::Arc e )
{
    if( e == INVALID ) return AcDbObjectId::kNull;
    // 应该判断分支是否包含在图中
    // 目前就假设e肯定是图的分支
    return datas[e]->objId;
}

struct NodeDist
{
    Digraph::Node u;
    int dist;
    bool operator==( const NodeDist& nd )
    {
        return u == nd.u;
    }
};

struct SortNodeDist
{
    bool operator()( const NodeDist& nd1, const NodeDist& nd2 ) const
    {
        return nd1.dist < nd2.dist;
    }
};

typedef std::vector<NodeDist> NDArray;

static void DFS_Helper2( Digraph& dg, Digraph::Node u, NDArray& nds )
{
    NodeArray ns;
    Dfs<Digraph> aDfs( dg );
    aDfs.run( u );
    for( Digraph::NodeIt n( dg ); n != INVALID; ++n )
    {
        if( aDfs.reached( n ) )
        {
            NodeDist nd;
            nd.u = n;
            nd.dist = aDfs.dist( n );
            nds.push_back( nd );
        }
    }
}

static void DFS_Helper3( Digraph& dg, Digraph::Node u, NDArray& nds )
{
    typedef ReverseDigraph<Digraph> RDigraph;
    RDigraph rdg( dg );

    Dfs<RDigraph> aDfs( rdg );
    aDfs.run( u );
    for( Digraph::NodeIt n( dg ); n != INVALID; ++n )
    {
        if( aDfs.reached( n ) )
        {
            NodeDist nd;
            nd.u = n;
            nd.dist = aDfs.dist( n );
            nds.push_back( nd );
        }
    }
}

static bool DFS_Helper( Digraph& dg, Digraph::Node u, Digraph::Node v, DPath& p )
{
    // 执行dfs
    Dfs<Digraph> aDfs( dg );
    bool ret = aDfs.run( u, v );
    if( ret )
    {
        p = aDfs.path( v ); // u->v
    }
    return ret;
}

static void FindStationFromNode( Digraph& dg, Digraph::Node u, EdgeArray& es, bool reverse = false )
{
    NDArray nds;
    if( reverse )
    {
        DFS_Helper3( dg, u, nds );
    }
    else
    {
        DFS_Helper2( dg, u, nds );
    }

    std::sort( nds.begin(), nds.end(), SortNodeDist() );

    for( int i = 0; i < nds.size(); i++ )
    {
        if( i < MAX_PATH_LENGTH )
        {
            bool ret = true;
            DPath p;
            if( reverse )
            {
                ret = DFS_Helper( dg, nds[i].u, u, p );
            }
            else
            {
                ret = DFS_Helper( dg, u, nds[i].u, p );
            }
            if( !ret ) continue;

            for( DPath::ArcIt e( p ); e != INVALID; ++e )
            {
                if( !es.contains( e ) ) es.append( e );
            }
        }
    }
}

static void FindStation_Helper( Digraph& dg, Digraph::Arc e, EdgeArray& es, bool searchStart = true, bool searchEnd = true )
{
    if( searchStart ) // 是否需要从分支的始节点搜索
    {
        FindStationFromNode( dg, dg.source( e ), es, true );
    }
    if( searchEnd ) // 是否需要从分支的末节点搜索
    {
        FindStationFromNode( dg, dg.target( e ), es, false );
    }
}

static bool HasStation( const AcDbObjectId& objId )
{
    AcDbObjectIdArray stations;
    DrawHelper::GetTagGEById2( objId, _T( "WindStation" ), stations );
    return !( stations.isEmpty() );
}

static Digraph::Arc FindStationEdge( FWS_EdgeDataMap& datas, EdgeArray& es )
{
    Digraph::Arc wsEdge = INVALID;
    int len = es.length();
    for( int i = 0; i < len; i++ )
    {
        Digraph::Arc e = es[i];
        if( datas[e]->et != ET_VIRTUAL && HasStation( datas[e]->objId ) )
        {
            wsEdge = e;
            break;
        }
    }
    return wsEdge;
}

Digraph::Arc FindWindStation::doFWS( Digraph::Arc e )
{
    if( !isInitNetworkOk() ) return INVALID;
    if( e == INVALID ) return INVALID;
    if( datas[e]->et == ET_VIRTUAL ) return INVALID;

    // 本身就有测风站
    if( HasStation( datas[e]->objId ) )
    {
        //acutPrintf(_T("\n本身就有测风站"));
        return e;
    }

    bool searchStart = true, searchEnd = true;
    // 如果是掘进工作面
    if( ArxUtilHelper::IsEqualType( _T( "TTunnel" ), datas[e]->objId ) ) // 掘进工作面只查找回风侧
    {
        searchStart = false;
        searchEnd = true;
    }
    //else if(ArxUtilHelper::IsEqualType(_T("WorkSurface"), datas[e]->objId)) // 回采工作面进回风侧都查找
    //{
    //	searchStart = true;
    //	searchEnd = true;
    //}
    //else if(ArxUtilHelper::IsEqualType(_T("StorageGE"), datas[e]->objId)) // 硐室只查找进风侧
    //{
    //	searchStart = true;
    //	searchEnd = false;
    //}
    // 其它巷道，例如主进回、采区进回进回风都要查找

    EdgeArray es;
    FindStation_Helper( dg, e, es, searchStart, searchEnd );
    return FindStationEdge( datas, es );
}

bool FindWindStation::initNetwok()
{
    ObjectIdMap om( dg );
    if( !BuildNetwork( dg, om ) ) return false;
    if( !AddVirtualSTNode( dg, om, sn, tn ) ) return false;
    FilterBlockEdges( dg, om, ef );

    // 关联分支属性数据
    InitEdgeDatas( dg, om, datas );

    return true;
}

bool FindWindStation::isInitNetworkOk() const
{
    return bInitNetwokState;
}

void FindWindStation::setInitNetwokState( bool bState )
{
    bInitNetwokState = bState;
}

void FindWindStation::initAirEdges()
{
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        if( datas[e]->et == ET_USEAIR )
        {
            airEdges.append( e );
        }
    }
}
