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
    acutPrintf( _T( "\n\n�ҵ�%d��������·!\n" ), cl.length() );
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
        cl[i]->printQMsg( _T( "����������յ������" ) );
    }
}

static void PrintEdgeInfo( Digraph& dg, NetworkEdgeDataMap& datas )
{
    acutPrintf( _T( "\n��ӡ��֧��Ϣ..." ) );

    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        EdgeDataLink* pEdgeData = datas[e];
        acutPrintf( _T( "\n��֧:e%d  ����:%.3f, ����:%.3f ��������:%.3f" ), dg.id( e ), pEdgeData->r, pEdgeData->q, pEdgeData->c );
        if( pEdgeData->isValidFanEdge() )
        {
            FanDataLink* pFanData = pEdgeData->PFanData;
            acutPrintf( _T( "\n\t�������: a0:%.3f, a1:%.3f, a2:%.3f, a3:%.3f, a4:%.3f, a5:%.3f" ), pFanData->a0, pFanData->a1, pFanData->a2, pFanData->a3, pFanData->a4, pFanData->a5 );
        }
    }
    acutPrintf( _T( "\n" ) );
}

// ���Ҿ��������֮ǰ����������֧
static bool FindVirtualEdgesBeforeTTunnels( Digraph& dg, ObjectIdMap& om, const AcDbObjectIdArray& tObjIds, EdgeArray& es )
{
    AcDbObjectIdArray tObjIds2; // ��¼���������
    EdgeArray es2;            // ��¼���������֮ǰ���ӵ������֧
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        AcDbObjectId objId = om[e];
        if( objId.isNull() ) continue;
        if( !tObjIds.contains( objId ) ) continue;

        tObjIds2.append( objId );
        for( Digraph::InArcIt ie( dg, dg.source( e ) ); ie != INVALID; ++ie )
        {
            // idΪ�յķ�֧��Ϊ���ӵ������֧
            if( om[ie].isNull() )
            {
                es2.append( ie );
                break;
            }
        }
    }

    if( tObjIds2.length() != tObjIds.length() ) return false;
    if( tObjIds2.length() != es2.length() ) return false;

    // ����˳��
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
        datas[e] = 0; // ��ֹҰָ��
    }
    dg.clear();       // ɾ��ͼ
}

static void SetEdgeType( const AcDbObjectId& objId, EdgeDataLink* pEdgeData )
{
    if( pEdgeData->fq > 0 )
    {
        //acutPrintf(_T("\n�̶�������֧..."));
        pEdgeData->q = pEdgeData->fq;        // ����=�̶�����
        pEdgeData->et = VNC_FIXTED;          // �̶�������֧
    }
    else
    {
        //acutPrintf(_T("\nһ���֧..."));
        pEdgeData->q = 0;                    // ����ȡԭ�еķ���!!!
        pEdgeData->et = VNC_COMMON;          // һ���֧
    }
}

static EdgeDataLink* CreatEdgeData( const AcDbObjectId& objId )
{
    EdgeDataLink* pEdgeData = new EdgeDataLink();
    pEdgeData->setDataSource( objId );
    if( !pEdgeData->updateData( false ) )
    {
        acutPrintf( _T( "\n��ȡ�������ʧ��..." ) );
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
        acutPrintf( _T( "\n��ȡ�������ʧ��..." ) );
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
        acutPrintf( _T( "\n��ȡ��Ͳ����ʧ��..." ) );
        delete pChimneyData;
        pChimneyData = 0;
    }
    return pChimneyData;
}

static bool InitMainFanEdges( Digraph& dg, ObjectIdMap& om, NetworkEdgeDataMap& datas )
{
    // �������е���Ҫͨ���
    AcDbObjectIdArray fanObjIds;
    FindMainFans( fanObjIds );
    if( fanObjIds.isEmpty() ) return true;

    // ������Ҫͨ��������������
    AcDbObjectIdArray hosts;
    GetHosts( fanObjIds, hosts );

    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        int pos = hosts.find( om[e] );
        if( pos == -1 ) continue;

        FanDataLink* pFanData = CreateFanData( fanObjIds[pos] );
        if( pFanData == 0 ) return false; // ���·������ʧ��!!!

        EdgeDataLink* pEdgeData = datas[e];
        pEdgeData->PFanData = pFanData;   // ��������ݹ�������֧��
    }
    return true;
}

// ��ʼ���ֲ�ͨ����������ľ���������뵼�����֮��������֧�Ĳ���
// ��Ҫ��ȡ�ֲ�ͨ����ͷ�Ͳ������
static bool InitLocalFanEdges( Digraph& dg, ObjectIdMap& om, NetworkEdgeDataMap& datas )
{
    // �������еľ��������
    AcDbObjectIdArray tObjIds;
    FindTTunnels( tObjIds );
    if( tObjIds.isEmpty() ) return true;

    // �������еľֲ�ͨ���
    // �������FindLocalFanstrue��
    // ������(tObjIds.length() == fanObjIds.length()) ==> true
    AcDbObjectIdArray fanObjIds;
    if( !FindLocalFans( tObjIds, fanObjIds ) )
    {
        acutPrintf( _T( "\n���־��������û�й������ֲ�ͨ�����" ) );
        return false;
    }

    // �������еķ�Ͳ
    // �������FindChimneystrue��
    // ������(tObjIds.length() == cObjIds.length()) ==> true
    AcDbObjectIdArray cObjIds;
    if( !FindChimneys( tObjIds, cObjIds ) )
    {
        acutPrintf( _T( "\n���־��������û�й�������Ͳ��" ) );
        return false;
    }

    // �����ھ��������֮ǰ���ӵ������֧
    EdgeArray es;
    if( !FindVirtualEdgesBeforeTTunnels( dg, om, tObjIds, es ) ) return false;

    int len = es.length();
    for( int i = 0; i < len; i++ )
    {
        Digraph::Arc e = es[i];

        FanDataLink* pFanData = CreateFanData( fanObjIds[i] );
        if( pFanData == 0 ) return false; // ��ȡ�������ʧ��!!!

        ChimneyDataLink* pChimneyData = CreateChimneyData( cObjIds[i] );
        if( pChimneyData == 0 ) return false; // ��ȡ��Ͳ����ʧ��

        EdgeDataLink* pEdgeData = datas[e];
        pEdgeData->r = pChimneyData->r;   // ����Ͳ�ķ��踳�������֧
        pEdgeData->PFanData = pFanData;   // ��������ݹ�������֧��

        delete pChimneyData;       // �����ڴ�(��Ͳ����ֻ����ʱ��)
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
             * ֻҪ�÷�֧������Ϲ����
             * �÷�֧��������ͨ��֧���ҷ��������
             * ͬʱ��շ�֧����������
             */
            pEdgeData->et = VNC_COMMON;
            pEdgeData->r = 1.0 * INFINITE_GREATE_R; // ������ϵķ���Ϊ�����
            pEdgeData->q = 0;
            pEdgeData->ar = 0;
            pEdgeData->fq = 0;

            // ɾ���������
            if( pEdgeData->PFanData != 0 )
            {
                delete pEdgeData->PFanData;
                pEdgeData->PFanData = 0;
            }
        }
    }
}

// ��ʼ��id��null�ķ�֧
static bool InitEdgeDatas( Digraph& dg, ObjectIdMap& om, NetworkEdgeDataMap& datas )
{
    bool ret = true;
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        if( om[e].isNull() ) continue;

        EdgeDataLink* pEdgeData = CreatEdgeData( om[e] );
        SetEdgeType( om[e], pEdgeData ); // �����������(һ���֧��̶���֧)
        datas[e] = pEdgeData;
    }
    return ret;
}

// ��ʼ�������֧����
static void InitNullEdgeDatas( Digraph& dg, ObjectIdMap& om, NetworkEdgeDataMap& datas )
{
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        if( om[e].isNull() )
        {
            // Ĭ��Ϊһ���֧������r=0������q=0���̶�����fq=0�����ڷ���ar=0
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

        // ���·������
        if( pEdgeData->PFanData != 0 )
        {
            pEdgeData->PFanData->updateData( true );
        }

        // ����Ǹ���������Ҫ����ͼԪ�ķ���
        if( pEdgeData->q < 0 )
        {
            // ����ͼԪ
            DrawHelper::ReverseDirection( pEdgeData->getDataSource() );
            pEdgeData->q = -1.0 * ( pEdgeData->q );
        }

        // ����̶������ĵ��ڷ���
        // ����...

        // �����ݸ��µ�ͼԪ
        pEdgeData->updateData( true );
    }
}

static double MaxAbsValue( DValueArray& dvalues )
{
    // �����ֵ����Ԫ��
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
    // �������ݵľ���ֵ�ľ�����
    int n = dvalues.length();

    // 1��ƽ��ֵ
    double a = 0;
    for( int i = 0; i < n; i++ )
    {
        a += abs( dvalues[i] );
    }
    a = a / n;

    // 2��������
    double s = 0;
    for( int i = 0; i < n; i++ )
    {
        double v = abs( dvalues[i] );
        s += ( a - v ) * ( a - v );
    }
    return sqrt( s / n );
}

// Ҳ���Բο�"����ͳ��"�е�һЩ�����㷽��
static bool IsMeetErrorRequirement( DValueArray& dvalues, double precise )
{
    if( dvalues.isEmpty() ) return true;

    double maxD = MaxAbsValue( dvalues );

    bool ret = ( maxD - precise ) <= 0;

    acutPrintf( _T( "\n���ƫ��ֵ:%.5f, %s����Ҫ��:%.5f\n" ), maxD, ( ret ? _T( "����" ) : _T( "������" ) ), precise );

    // �Ƚ�"�������ݵľ���ֵ"��"����"�Ĵ�С
    return ret;
}

VentNetworkCalc::VentNetworkCalc() : datas( dg ), ef( dg, true ),
    bInitNetwokState( true ), airEdge( INVALID ),
    precise( DEF_PRECISE ), maxCount( DEF_MAX_COUNT )
{
    // ��ʼ������
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
    // ��ʼ������ʧ��
    if( !isInitNetworkOk() ) return false;

    // ���������֧�ķ���=�ܷ���Q
    datas[getAirEdge()]->q = totalQ;

    // ��ʼ����������,�漰����֧��2������: ����q�ͷ���r
    initIterateVar( false );
    bool ret = doIterate( min( maxCount, DEF_CONVERGENCE_COUNT ) );

    // ����������������ҵ�����������20�Σ����ü��ٵ�������
    if( !ret && maxCount > DEF_CONVERGENCE_COUNT )
    {
        acutPrintf( _T( "\n ===============>���ü��ٵ�������<===================" ) );
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

    // ɾ����Ϸ�֧(������㲻����)
    //EraseBlockEdges(dg, ef);

    if( !InitEdgeDatas( dg, om, datas ) ) return false;	 // ��ʼ��id������null�ķ�֧����
    InitNullEdgeDatas( dg, om, datas );                  // ��ʼ�����ӵ������֧

    if( !InitMainFanEdges( dg, om, datas ) ) return false; // ��ʼ����Ҫͨ����������������
    if( !InitLocalFanEdges( dg, om, datas ) ) return false; // ��ʼ������������������ӵ������֧����
    InitBlockEdges( dg, om, ef, datas );                 // ��ʼ����Ϸ�֧����(�޸�ԭʼ��֧�ķ���)

    addAirEdge();	                                     // �������������֧

    return true;
}

void VentNetworkCalc::addAirEdge()
{
    if( airEdge != INVALID ) return;

    // ����һ��������֧(������t-->����Դ��s)
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
    acutPrintf( _T( "\n������С������..." ) );

    // ���˴�����֧
    ArcFilter af( dg, true );
    af[getAirEdge()] = false;

    // ���˴�����֧���ͼ
    typedef FilterArcs<Digraph, ArcFilter> AFGraph;
    AFGraph afg( dg, af );

    // ��ʼ��Ȩ��
    typedef AFGraph::ArcMap<double> Weight;
    Weight w( afg );
    for( AFGraph::ArcIt e( afg ); e != INVALID; ++e )
    {
        w[e] = datas[e]->c; // ��һ���ϴ��ϵ��(1000)
    }

    /*
     * ʹ��kruskal�㷨����С������
     *
     * ˼·�����̶�������֧�������֧�Լ��ķ������һ���ϴ��ϵ��
     *        ʹ֮Զ����һ���֧�ķ���, ����С���������㷨����ѡ��
     *        �̶�������֧�������֧��Ϊ��֦
     */
    typedef std::vector<Digraph::Arc> TreeArray;
    TreeArray ta;
    kruskal( afg, w, std::back_inserter( ta ) );

    for( TreeArray::iterator itr = ta.begin(); itr != ta.end(); itr++ )
    {
        tree.append( *itr );
    }
}

// Tree Filter == > tfĬ��ȫ��Ϊfalse
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
    // ������ͼ���й���, ���˵����е���֦��Ȼ��ת��������ͼ
    typedef FilterArcs<Digraph, ArcFilter> AFGraph;
    typedef Undirector<AFGraph> Graph;
    typedef Path<Graph> UPath;

    AFGraph afg( dg, tf );
    Graph g( afg );

    // �����������(u->v)
    Dfs<Graph> aDfs( g );
    if( !aDfs.run( u, v ) ) return false;

    // ��ȡ·��
    UPath up = aDfs.path( v );
    for( UPath::ArcIt ue( up ); ue != INVALID; ++ue )
    {
        UPath::Arc ua = ue;
        Digraph::Arc a = ua;     // ������ͼ��(Arc->Edge)->����ͼ��Arc
        p.append( a );
    }

    return true;
}

void VentNetworkCalc::findCycleByTree( EdgeArray& tree, CycleList& cl )
{
    // ������֦(��֦-true, ����֦-false)
    ArcFilter tf( dg, false );
    BuildTreeFilter( tree, tf );

    // ������֦
    EdgeArray leftTree;
    FindLeftTree( dg, tf, leftTree );

    // ��ÿ����֦������·
    int n = leftTree.length();
    for( int i = 0; i < n; i++ )
    {
        Digraph::Arc e = leftTree[i];          // ��·�Ļ�׼��֧(��֦)
        Digraph::Node u = dg.source( e );      // ��׼��֧��ʼ��
        Digraph::Node v = dg.target( e );      // ��׼��֧��ĩ��

        //  ����v->u��һ��·��
        EdgeArray p;
        if( !DFS_Helper( dg, tf, v, u, p ) ) continue;

        // ���������·����
        IndependentCycle* pCycle = new IndependentCycle( dg, datas, e );

        // ��·����֧���ӵ���·����
        int len = p.length();
        for( int j = 0; j < len; j++ )
        {
            pCycle->addEdge( p[j] );
        }

        // ��ʼ������ϵ��
        pCycle->init();

        cl.append( pCycle );
    }
}

void VentNetworkCalc::findCycle( CycleList& cl )
{
    // ������С������
    EdgeArray tree;
    findMST( tree );
    if( tree.isEmpty() ) return;

    // ������С���������Ҷ�����·
    findCycleByTree( tree, cl );
}

double VentNetworkCalc::sumQ( CycleList& cl, Digraph::Arc e )
{
    double sq = 0;

    int n = cl.length();
    for( int i = 0; i < n; i++ )
    {
        IndependentCycle* pCycle = cl[i];
        double baseQ = datas[pCycle->getBaseEdge()]->q; // ��·��׼��֧�ķ���(��֦����)
        short c = pCycle->getCycleCoeff( e );           // ��֧e�ڻ�·�����е�ϵ��
        sq += c * baseQ;
    }

    return sq;
}

void VentNetworkCalc::initQ( CycleList& cl, double Q )
{
    // ���������֧�ķ���=�ܷ���Q
    datas[getAirEdge()]->q = Q;

    // ��ʼ����֦�ķ���
    initLeftTreeQ( cl, Q );

    // ������֦�ķ���(ʵ����Ҳ�ظ��ļ�������֦����)
    initTreeQ( cl );
}

void VentNetworkCalc::initTreeQ( CycleList& cl )
{
    /*
     * ������֦�ķ���(ʵ����Ҳ�ظ��ļ�������֦����)
     * ����Q = CT*Qy
     * ����CT -- ��·����ķ�ת�任
     *      Qy -- ��֦�ķ�������
     * �ȼ���q = c1*q1 + c2*q2 + ... + ck*qk (����k=n-m+1)
     */
    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        datas[e]->q = sumQ( cl, e );
    }
}

void VentNetworkCalc::initLeftTreeQ( CycleList& cl, double Q )
{
    // ��ȡ��֦��Ҳ����·�Ļ�׼��֧
    EdgeArray leftTree;

    int n = cl.length();
    for( int i = 0; i < n; i++ )
    {
        Digraph::Arc e = cl[i]->getBaseEdge();
        leftTree.append( e );
    }

    /*
     * �����2��������û�п���"�����֧"�ĳ�ʼ����
     * Ŀǰ������һ���֧��ͬ�Ĵ�����ʽ
     * �����϶�˵��ָ��һ��"λ�ڸ�Ч������״̬�ĳ�ʼ����"
     * ��ô����Ҫ�ڷ��ͼԪ������һ��"��ʼ����"�ֶ�
     * �����ٿ���???
     */
    initLeftTreeQ1( leftTree, Q );
}

void VentNetworkCalc::initLeftTreeQ1( EdgeArray& leftTree, double Q )
{
    /*
     * ������֧����֦����
     * ��������֦������������������
     * �������ƽ������ķ���
     */
    int n = leftTree.length();
    for( int i = 0; i < n; i++ )
    {
        Digraph::Arc e = leftTree[i];
        if( datas[e]->isFixedEdge() ) continue;    // �ų��̶�������֧
        if( datas[e]->isVirtualAirEdge() ) continue; // �ų�������֧

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
    // ���Ҷ�����·
    CycleList cl;
    findCycle( cl );
    if( cl.isEmpty() ) return true;

    // ��ʼ������
    initQ( cl, totalQ );

    // ��ӡ��֧��Ϣ
    PrintEdgeInfo( dg, datas );

    // ��ӡ������·��Ϣ
    PrintCycle( cl, dg, datas );

    // ��������
    bool ret = false;
    int k = 0;  // ��������
    while( !ret && k <= count )
    {
        acutPrintf( _T( "\n\n========== * ================ * ��%d�ε��� * =============== * ===========" ), k );

        DValueArray dq, df;                    // ��¼��·�����õ������������Լ�������

        int n = cl.length();
        for( int i = 0; i < n; i++ )
        {
            double delta_q = 0, delta_f = 0;     // ��·����������������

            cl[i]->iterate( delta_q, delta_f );  // ÿ����·���е���

            dq.append( delta_q );
            df.append( delta_f );
        }

        //PrintEdgeInfo(dg, datas);

        if( IsMeetErrorRequirement( dq, precise ) &&
                IsMeetErrorRequirement( df, precise ) )
        {
            acutPrintf( _T( "\n=====�ﵽ����Ҫ��===" ) );
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