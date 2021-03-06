#include "StdAfx.h"
#include "VentNetworkCalc.h"

#include "config.h"
#include "Tool.h"
#include "IndependentCycle.h"

#include <cmath>
#include <vector>
#include <iterator>

#include <lemon/adaptors.h>
#include <lemon/kruskal.h>
#include <lemon/dfs.h>

#include "../MineGE/DrawHelper.h"

typedef AcArray<double> DValueArray;

static void PrintCycle( CycleList& cl, Digraph& dg, NetworkEdgeDataMap& datas )
{
    acutPrintf( _T( "\n\n找到%d个独立回路!\n" ), cl.length() );
    int n = cl.length();
    for( int i = 0; i < n; i++ )
    {
        cl[i]->printMsg();
    }
    acutPrintf( _T( "\n" ) );
}

static void PrintCycle2( CycleList& cl )
{
    acutPrintf( _T( "\n\n\n" ) );
    int n = cl.length();
    for( int i = 0; i < n; i++ )
    {
        cl[i]->printQMsg( _T( "网络解算最终迭代结果" ) );
    }
}

static void PrintEdgeInfo( Digraph& dg, NetworkEdgeDataMap& datas )
{
    acutPrintf( _T( "\n打印分支信息..." ) );

    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        EdgeDataLink* pEdgeData = datas[e];
        acutPrintf( _T( "\n分支:e%d  风阻:%.3f, 风量:%.3f 迭代变量:%.3f" ), dg.id( e ), pEdgeData->r, pEdgeData->q, pEdgeData->c );
        if( pEdgeData->isValidFanEdge() )
        {
            FanDataLink* pFanData = pEdgeData->PFanData;
            acutPrintf( _T( "\n\t风机数据: a0:%.3f, a1:%.3f, a2:%.3f, a3:%.3f, a4:%.3f, a5:%.3f" ), pFanData->a0, pFanData->a1, pFanData->a2, pFanData->a3, pFanData->a4, pFanData->a5 );
        }
    }
    acutPrintf( _T( "\n" ) );
}

// 查找掘进工作面之前的入边虚拟分支
static bool FindVirtualEdgesBeforeTTunnels( Digraph& dg, ObjectIdMap& om, const AcDbObjectIdArray& tObjIds, EdgeArray& es )
{
    AcDbObjectIdArray tObjIds2; // 记录掘进工作面
    EdgeArray es2;            // 记录掘进工作面之前增加的虚拟分支
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        AcDbObjectId objId = om[e];
        if( objId.isNull() ) continue;
        if( !tObjIds.contains( objId ) ) continue;

        tObjIds2.append( objId );
        for( Digraph::InArcIt ie( dg, dg.source( e ) ); ie != INVALID; ++ie )
        {
            // id为空的分支即为增加的虚拟分支
            if( om[ie].isNull() )
            {
                es2.append( ie );
                break;
            }
        }
    }

    if( tObjIds2.length() != tObjIds.length() ) return false;
    if( tObjIds2.length() != es2.length() ) return false;

    // 调整顺序
    int len = tObjIds.length();
    for( int i = 0; i < len; i++ )
    {
        int pos = tObjIds2.find( tObjIds[i] );
        es.append( es2[pos] );
    }

    return true;
}

static void ClearGraph( Digraph& dg, NetworkEdgeDataMap& datas )
{
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        delete datas[e];
        datas[e] = 0; // 防止野指针
    }
    dg.clear();       // 删除图
}

static void SetEdgeType( const AcDbObjectId& objId, EdgeDataLink* pEdgeData )
{
    if( pEdgeData->fq > 0 )
    {
        //acutPrintf(_T("\n固定风量分支..."));
        pEdgeData->q = pEdgeData->fq;        // 风量=固定风量
        pEdgeData->et = VNC_FIXTED;          // 固定风量分支
    }
    else
    {
        //acutPrintf(_T("\n一般分支..."));
        pEdgeData->q = 0;                    // 不读取原有的风量!!!
        pEdgeData->et = VNC_COMMON;          // 一般分支
    }
}

static EdgeDataLink* CreatEdgeData( const AcDbObjectId& objId )
{
    EdgeDataLink* pEdgeData = new EdgeDataLink();
    pEdgeData->setDataSource( objId );
    if( !pEdgeData->updateData( false ) )
    {
        acutPrintf( _T( "\n提取巷道数据失败..." ) );
        delete pEdgeData;
        pEdgeData = 0;
    }
    return pEdgeData;
}

static FanDataLink* CreateFanData( const AcDbObjectId& objId )
{
    FanDataLink* pFanData = new FanDataLink();
    pFanData->setDataSource( objId );
    if( !pFanData->updateData( false ) )
    {
        acutPrintf( _T( "\n提取风机数据失败..." ) );
        delete pFanData;
        pFanData = 0;
    }
    return pFanData;
}

static ChimneyDataLink* CreateChimneyData( const AcDbObjectId& objId )
{
    ChimneyDataLink* pChimneyData = new ChimneyDataLink();
    pChimneyData->setDataSource( objId );
    if( !pChimneyData->updateData( false ) )
    {
        acutPrintf( _T( "\n提取风筒数据失败..." ) );
        delete pChimneyData;
        pChimneyData = 0;
    }
    return pChimneyData;
}

static bool InitMainFanEdges( Digraph& dg, ObjectIdMap& om, NetworkEdgeDataMap& datas )
{
    // 查找所有的主要通风机
    AcDbObjectIdArray fanObjIds;
    FindMainFans( fanObjIds );
    if( fanObjIds.isEmpty() ) return true;

    // 查找主要通风机所关联的巷道
    AcDbObjectIdArray hosts;
    GetHosts( fanObjIds, hosts );

    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        int pos = hosts.find( om[e] );
        if( pos == -1 ) continue;

        FanDataLink* pFanData = CreateFanData( fanObjIds[pos] );
        if( pFanData == 0 ) return false; // 更新风机数据失败!!!

        EdgeDataLink* pEdgeData = datas[e];
        pEdgeData->PFanData = pFanData;   // 将风机数据关联到分支上
    }
    return true;
}

// 初始化局部通风机所关联的掘进工作面与导风巷道之间的虚拟分支的参数
// 需要读取局部通风机和风筒的数据
static bool InitLocalFanEdges( Digraph& dg, ObjectIdMap& om, NetworkEdgeDataMap& datas )
{
    // 查找所有的掘进工作面
    AcDbObjectIdArray tObjIds;
    FindTTunnels( tObjIds );
    if( tObjIds.isEmpty() ) return true;

    // 查找所有的局部通风机
    // 如果返回FindLocalFanstrue，
    // 则隐含(tObjIds.length() == fanObjIds.length()) ==> true
    AcDbObjectIdArray fanObjIds;
    if( !FindLocalFans( tObjIds, fanObjIds ) )
    {
        acutPrintf( _T( "\n部分掘进工作面没有关联【局部通风机】" ) );
        return false;
    }

    // 查找所有的风筒
    // 如果返回FindChimneystrue，
    // 则隐含(tObjIds.length() == cObjIds.length()) ==> true
    AcDbObjectIdArray cObjIds;
    if( !FindChimneys( tObjIds, cObjIds ) )
    {
        acutPrintf( _T( "\n部分掘进工作面没有关联【风筒】" ) );
        return false;
    }

    // 查找在掘进工作面之前添加的虚拟分支
    EdgeArray es;
    if( !FindVirtualEdgesBeforeTTunnels( dg, om, tObjIds, es ) ) return false;

    int len = es.length();
    for( int i = 0; i < len; i++ )
    {
        Digraph::Arc e = es[i];

        FanDataLink* pFanData = CreateFanData( fanObjIds[i] );
        if( pFanData == 0 ) return false; // 提取风机数据失败!!!

        ChimneyDataLink* pChimneyData = CreateChimneyData( cObjIds[i] );
        if( pChimneyData == 0 ) return false; // 提取风筒数据失败

        EdgeDataLink* pEdgeData = datas[e];
        pEdgeData->r = pChimneyData->r;   // 将风筒的风阻赋给虚拟分支
        pEdgeData->PFanData = pFanData;   // 将风机数据关联到分支上

        delete pChimneyData;       // 销毁内存(风筒数据只作临时用)
    }
    return true;
}

static void InitBlockEdges( Digraph& dg, ObjectIdMap& om, ArcFilter& ef, NetworkEdgeDataMap& datas )
{
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        if( !ef[e] )
        {
            EdgeDataLink* pEdgeData = datas[e];
            /*
             * 只要该分支包含阻断构筑物，
             * 该分支都看作普通分支，且风阻无穷大
             * 同时清空分支的其它数据
             */
            pEdgeData->et = VNC_COMMON;
            pEdgeData->r = 1.0 * INFINITE_GREATE_R; // 设置阻断的风阻为无穷大
            pEdgeData->q = 0;
            pEdgeData->ar = 0;
            pEdgeData->fq = 0;

            // 删除风机数据
            if( pEdgeData->PFanData != 0 )
            {
                delete pEdgeData->PFanData;
                pEdgeData->PFanData = 0;
            }
        }
    }
}

// 初始化id非null的分支
static bool InitEdgeDatas( Digraph& dg, ObjectIdMap& om, NetworkEdgeDataMap& datas )
{
    bool ret = true;
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        if( om[e].isNull() ) continue;

        EdgeDataLink* pEdgeData = CreatEdgeData( om[e] );
        SetEdgeType( om[e], pEdgeData ); // 设置巷道类型(一般分支或固定分支)
        datas[e] = pEdgeData;
    }
    return ret;
}

// 初始化虚拟分支数据
static void InitNullEdgeDatas( Digraph& dg, ObjectIdMap& om, NetworkEdgeDataMap& datas )
{
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        if( om[e].isNull() )
        {
            // 默认为一般分支，风阻r=0，风量q=0，固定风量fq=0，调节风阻ar=0
            EdgeDataLink* pEdgeData = new EdgeDataLink();
            datas[e] = pEdgeData;
        }
    }
}

static void UpdateData( Digraph& dg, NetworkEdgeDataMap& datas )
{
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        EdgeDataLink* pEdgeData = datas[e];

        // 更新风机数据
        if( pEdgeData->PFanData != 0 )
        {
            pEdgeData->PFanData->updateData( true );
        }

        // 如果是负风量，需要调整图元的方向
        if( pEdgeData->q < 0 )
        {
            // 反向图元
            DrawHelper::ReverseDirection( pEdgeData->getDataSource() );
            pEdgeData->q = -1.0 * ( pEdgeData->q );
        }

        // 计算固定风量的调节风阻
        // 待定...

        // 将数据更新到图元
        pEdgeData->updateData( true );
    }
}

static double MaxAbsValue( DValueArray& dvalues )
{
    // 求绝对值最大的元素
    double maxD = abs( dvalues[0] );

    int n = dvalues.length();
    for( int i = 0; i < n; i++ )
    {
        double d = abs( dvalues[i] );
        if( ( maxD - d ) < 0 ) maxD = d;
    }
    return maxD;
}

static double AbsSquare( DValueArray& dvalues )
{
    // 所有数据的绝对值的均方差
    int n = dvalues.length();

    // 1、平均值
    double a = 0;
    for( int i = 0; i < n; i++ )
    {
        a += abs( dvalues[i] );
    }
    a = a / n;

    // 2、均方差
    double s = 0;
    for( int i = 0; i < n; i++ )
    {
        double v = abs( dvalues[i] );
        s += ( a - v ) * ( a - v );
    }
    return sqrt( s / n );
}

// 也可以参考"数理统计"中的一些误差计算方法
static bool IsMeetErrorRequirement( DValueArray& dvalues, double precise )
{
    if( dvalues.isEmpty() ) return true;

    double maxD = MaxAbsValue( dvalues );

    bool ret = ( maxD - precise ) <= 0;

    acutPrintf( _T( "\n最大偏差值:%.5f, %s精度要求:%.5f\n" ), maxD, ( ret ? _T( "符合" ) : _T( "不符合" ) ), precise );

    // 比较"最大的数据的绝对值"与"精度"的大小
    return ret;
}

VentNetworkCalc::VentNetworkCalc() : datas( dg ), ef( dg, true ),
    bInitNetwokState( true ), airEdge( INVALID ),
    precise( DEF_PRECISE ), maxCount( DEF_MAX_COUNT )
{
    // 初始化网络
    if( !initNetwok() )
    {
        ClearGraph( dg, datas );
        setInitNetwokState( false );
    }
}

VentNetworkCalc::~VentNetworkCalc()
{
    if( bInitNetwokState )
    {
        UpdateData( dg, datas );
        ClearGraph( dg, datas );
    }
}

void VentNetworkCalc::setTotalQ( double q )
{
    if( q > 0 )
    {
        this->totalQ = q;
    }
}

void VentNetworkCalc::setPrecise( double precise )
{
    this->precise = precise;
    if( precise <= 0 || precise >= 1 )
    {
        this->precise = DEF_PRECISE;
    }
}

void VentNetworkCalc::setIterateCount( int maxCount )
{
    this->maxCount = maxCount;
    if( maxCount <= 0 )
    {
        this->maxCount = DEF_MAX_COUNT;
    }
}

bool VentNetworkCalc::iterate()
{
    // 初始化网络失败
    if( !isInitNetworkOk() ) return false;

    // 虚拟大气分支的风量=总风量Q
    datas[getAirEdge()]->q = totalQ;

    // 初始化迭代参数,涉及到分支的2个参数: 风量q和风阻r
    initIterateVar( false );
    bool ret = doIterate( min( maxCount, DEF_CONVERGENCE_COUNT ) );

    // 如果迭代不收敛，且迭代次数超过20次，采用加速迭代方法
    if( !ret && maxCount > DEF_CONVERGENCE_COUNT )
    {
        acutPrintf( _T( "\n ===============>采用加速迭代方法<===================" ) );
        initIterateVar( true );
        ret = doIterate( maxCount );
    }
    return ret;
}

bool VentNetworkCalc::initNetwok()
{
    ObjectIdMap om( dg );
    if( !BuildNetwork( dg, sn, tn, om, ef ) ) return false;

    //if(!GetNetworkGeoData(dg, om, ngm, egm)) return false;

    // 删除阻断分支(网络解算不考虑)
    //EraseBlockEdges(dg, ef);

    if( !InitEdgeDatas( dg, om, datas ) ) return false;	 // 初始化id不等于null的分支数据
    InitNullEdgeDatas( dg, om, datas );                  // 初始化增加的虚拟分支

    if( !InitMainFanEdges( dg, om, datas ) ) return false; // 初始化主要通风机所在巷道的数据
    if( !InitLocalFanEdges( dg, om, datas ) ) return false; // 初始化处理掘进工作面添加的虚拟分支数据
    InitBlockEdges( dg, om, ef, datas );                 // 初始化阻断分支数据(修改原始分支的风阻)

    addAirEdge();	                                     // 增加虚拟大气分支

    return true;
}

void VentNetworkCalc::addAirEdge()
{
    if( airEdge != INVALID ) return;

    // 增加一个大气分支(虚拟汇点t-->虚拟源点s)
    airEdge = dg.addArc( tn, sn );

    EdgeDataLink* pData = new EdgeDataLink();
    pData->et = VNC_AIR;
    datas[airEdge] = pData;
}

Digraph::Arc VentNetworkCalc::getAirEdge() const
{
    return airEdge;
}

bool VentNetworkCalc::isInitNetworkOk() const
{
    return bInitNetwokState;
}

void VentNetworkCalc::setInitNetwokState( bool bState )
{
    bInitNetwokState = bState;
}

void VentNetworkCalc::findMST( EdgeArray& tree )
{
    acutPrintf( _T( "\n查找最小生成树..." ) );

    // 过滤大气分支
    ArcFilter af( dg, true );
    af[getAirEdge()] = false;

    // 过滤大气分支后的图
    typedef FilterArcs<Digraph, ArcFilter> AFGraph;
    AFGraph afg( dg, af );

    // 初始化权重
    typedef AFGraph::ArcMap<double> Weight;
    Weight w( afg );
    for( AFGraph::ArcIt e( afg ); e != INVALID; ++e )
    {
        w[e] = datas[e]->c; // 乘一个较大的系数(1000)
    }

    /*
     * 使用kruskal算法求最小生成树
     *
     * 思路：将固定风量分支、风机分支以及的风阻乘以一个较大的系数
     *        使之远大于一般分支的风阻, 则最小生成树的算法不会选择
     *        固定风量分支、风机分支作为树枝
     */
    typedef std::vector<Digraph::Arc> TreeArray;
    TreeArray ta;
    kruskal( afg, w, std::back_inserter( ta ) );

    for( TreeArray::iterator itr = ta.begin(); itr != ta.end(); itr++ )
    {
        tree.append( *itr );
    }
}

// Tree Filter == > tf默认全部为false
static void BuildTreeFilter( EdgeArray& tree, ArcFilter& tf )
{
    int n = tree.length();
    for( int i = 0; i < n; i++ )
    {
        tf[tree[i]] = true;
    }
}

static void FindLeftTree( Digraph& dg, ArcFilter& tf, EdgeArray& leftTree )
{
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        if( !tf[e] )
        {
            leftTree.append( e );
        }
    }
}

static bool DFS_Helper( Digraph& dg, ArcFilter& tf, Digraph::Node u, Digraph::Node v, EdgeArray& p )
{
    // 对有向图进行过滤, 过滤掉所有的余枝，然后转换成无向图
    typedef FilterArcs<Digraph, ArcFilter> AFGraph;
    typedef Undirector<AFGraph> Graph;
    typedef Path<Graph> UPath;

    AFGraph afg( dg, tf );
    Graph g( afg );

    // 深度优先搜索(u->v)
    Dfs<Graph> aDfs( g );
    if( !aDfs.run( u, v ) ) return false;

    // 获取路径
    UPath up = aDfs.path( v );
    for( UPath::ArcIt ue( up ); ue != INVALID; ++ue )
    {
        UPath::Arc ua = ue;
        Digraph::Arc a = ua;     // 从无向图的(Arc->Edge)->有向图的Arc
        p.append( a );
    }

    return true;
}

void VentNetworkCalc::findCycleByTree( EdgeArray& tree, CycleList& cl )
{
    // 过滤余枝(树枝-true, 非树枝-false)
    ArcFilter tf( dg, false );
    BuildTreeFilter( tree, tf );

    // 查找余枝
    EdgeArray leftTree;
    FindLeftTree( dg, tf, leftTree );

    // 对每个余枝搜索回路
    int n = leftTree.length();
    for( int i = 0; i < n; i++ )
    {
        Digraph::Arc e = leftTree[i];          // 回路的基准分支(余枝)
        Digraph::Node u = dg.source( e );      // 基准分支的始点
        Digraph::Node v = dg.target( e );      // 基准分支的末点

        //  查找v->u的一条路径
        EdgeArray p;
        if( !DFS_Helper( dg, tf, v, u, p ) ) continue;

        // 构造独立回路对象
        IndependentCycle* pCycle = new IndependentCycle( dg, datas, e );

        // 将路径分支添加到回路对象
        int len = p.length();
        for( int j = 0; j < len; j++ )
        {
            pCycle->addEdge( p[j] );
        }

        // 初始化方向系数
        pCycle->init();

        cl.append( pCycle );
    }
}

void VentNetworkCalc::findCycle( CycleList& cl )
{
    // 查找最小生成树
    EdgeArray tree;
    findMST( tree );
    if( tree.isEmpty() ) return;

    // 根据最小生成树查找独立回路
    findCycleByTree( tree, cl );
}

double VentNetworkCalc::sumQ( CycleList& cl, Digraph::Arc e )
{
    double sq = 0;

    int n = cl.length();
    for( int i = 0; i < n; i++ )
    {
        IndependentCycle* pCycle = cl[i];
        double baseQ = datas[pCycle->getBaseEdge()]->q; // 回路基准分支的风量(余枝风量)
        short c = pCycle->getCycleCoeff( e );           // 分支e在回路矩阵中的系数
        sq += c * baseQ;
    }

    return sq;
}

void VentNetworkCalc::initQ( CycleList& cl, double Q )
{
    // 虚拟大气分支的风量=总风量Q
    datas[getAirEdge()]->q = Q;

    // 初始化余枝的风量
    initLeftTreeQ( cl, Q );

    // 计算树枝的风量(实质上也重复的计算了余枝风量)
    initTreeQ( cl );
}

void VentNetworkCalc::initTreeQ( CycleList& cl )
{
    /*
     * 计算树枝的风量(实质上也重复的计算了余枝风量)
     * 采用Q = CT*Qy
     * 其中CT -- 回路矩阵的翻转变换
     *      Qy -- 余枝的风量向量
     * 等价于q = c1*q1 + c2*q2 + ... + ck*qk (其中k=n-m+1)
     */
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        datas[e]->q = sumQ( cl, e );
    }
}

void VentNetworkCalc::initLeftTreeQ( CycleList& cl, double Q )
{
    // 获取余枝，也即回路的基准分支
    EdgeArray leftTree;

    int n = cl.length();
    for( int i = 0; i < n; i++ )
    {
        Digraph::Arc e = cl[i]->getBaseEdge();
        leftTree.append( e );
    }

    /*
     * 下面的2个方法都没有考虑"风机分支"的初始风量
     * 目前采用与一般分支相同的处理方式
     * 资料上都说是指定一个"位于高效工况点状态的初始风量"
     * 那么就需要在风机图元中增加一个"初始风量"字段
     * 后续再考虑???
     */
    initLeftTreeQ1( leftTree, Q );
}

void VentNetworkCalc::initLeftTreeQ1( EdgeArray& leftTree, double Q )
{
    /*
     * 大气分支是余枝变量
     * 其它的余枝变量可以任意分配风量
     * 这里采用平均分配的方法
     */
    int n = leftTree.length();
    for( int i = 0; i < n; i++ )
    {
        Digraph::Arc e = leftTree[i];
        if( datas[e]->isFixedEdge() ) continue;    // 排除固定风量分支
        if( datas[e]->isVirtualAirEdge() ) continue; // 排除大气分支

        datas[e]->q = Q / n;
    }
}

void VentNetworkCalc::initIterateVar( bool speedUp )
{
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        datas[e]->initIterateVar( speedUp );
    }
}

bool VentNetworkCalc::doIterate( int count )
{
    // 查找独立回路
    CycleList cl;
    findCycle( cl );
    if( cl.isEmpty() ) return true;

    // 初始化风量
    initQ( cl, totalQ );

    // 打印分支信息
    PrintEdgeInfo( dg, datas );

    // 打印独立回路信息
    PrintCycle( cl, dg, datas );

    // 迭代计算
    bool ret = false;
    int k = 0;  // 迭代次数
    while( !ret && k <= count )
    {
        acutPrintf( _T( "\n\n========== * ================ * 第%d次迭代 * =============== * ===========" ), k );

        DValueArray dq, df;                    // 记录回路迭代得到的修正风量以及阻力差

        int n = cl.length();
        for( int i = 0; i < n; i++ )
        {
            double delta_q = 0, delta_f = 0;     // 回路修正风量和阻力差

            cl[i]->iterate( delta_q, delta_f );  // 每个回路进行迭代

            dq.append( delta_q );
            df.append( delta_f );
        }

        //PrintEdgeInfo(dg, datas);

        if( IsMeetErrorRequirement( dq, precise ) &&
                IsMeetErrorRequirement( df, precise ) )
        {
            acutPrintf( _T( "\n=====达到精度要求===" ) );
            ret = true;
            break;
        }
        else
        {
            //dq.removeAll();
            //df.removeAll();
            k++;
        }
    }

    PrintEdgeInfo( dg, datas );

    PrintCycle2( cl );

    return ret;
}
