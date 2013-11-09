#include "BuildNetwork.h"

#include <map>
#include <cmath>
#include <vector>
#include <fstream>
//#include <string>

typedef std::vector<int>             IntArray;
typedef std::vector<double>          DoubleArray;
typedef std::map<int, Digraph::Node> IntNodeMap;
typedef std::map<int, Digraph::Arc>  IntArcMap;
//typedef vector<string>          StringArray;

//static void SplitString(const string& content, const string& token, StringArray& cc)
//{
//	const char* delimeter =  token.c_str();
//	char * pch = strtok((char*)content.c_str(), delimeter);
//	while(pch != NULL)
//	{
//		cc.push_back(pch);
//		pch = strtok(NULL, delimeter);
//	}
//}

static void EraseZeroDegreeNodes( Digraph& dg )
{
    // ���Ҳ�ɾ����(����+���)����0�Ľڵ�
    NodeArray ns;

    for( Digraph::NodeIt n( dg ); n != INVALID; ++n )
    {
        if( ( countOutArcs( dg, n ) + countInArcs( dg, n ) ) == 0 )
        {
            ns.push_back( n );
        }
    }
    int len = ns.size();
    for( int i = 0; i < len; i++ )
    {
        dg.erase( ns[i] ); // ɾ���սڵ�(�ȵ���0)
    }
}

static void ChangeOutArcSourceNode( Digraph& dg, EdgeArray& es, Digraph::Node v )
{
    int len = es.size();
    for( int i = 0; i < len; i++ )
    {
        dg.changeSource( es[i], v );
    }
}

// ����֧es��ĩ����v
static void ChangeInArcTargetNode( Digraph& dg, EdgeArray& es, Digraph::Node v )
{
    int len = es.size();
    for( int i = 0; i < len; i++ )
    {
        dg.changeTarget( es[i], v );
    }
}

static Digraph::Node AddVirtualSource( Digraph& dg )
{
    NodeArray sNodes;  // Դ�㼯��

    // ��������з���,����Դ��
    for( Digraph::NodeIt n( dg ); n != INVALID; ++n )
    {
        // ��¼���=0��Դ��
        if( 0 == countInArcs( dg, n ) ) sNodes.push_back( n );
    }

    /*
     * !!!ǰ������!!!
     * ������һ��Դ���һ�����
     * Ŀǰ��ʱ������������"���������"
     * ���磺û��Դ���û�л�㣬����Դ��㶼û��
     */
    if( sNodes.empty() ) return INVALID;

    // ����һ������Դ��
    Digraph::Node sn = dg.addNode();

    // ����Դ��"������֧"(����Դ��-->Դ��)
    int len = sNodes.size();
    for( int i = 0; i < len; i++ )
    {
        Digraph::Arc e = dg.addArc( sn, sNodes[i] );
    }
    return sn;  // ��������������Դ��
}

static Digraph::Node AddVirtualTarget( Digraph& dg )
{
    NodeArray tNodes;  // Դ�㼯��

    // ��������з���,���һ��
    for( Digraph::NodeIt n( dg ); n != INVALID; ++n )
    {
        // ��¼����=0�Ļ��
        if( 0 == countOutArcs( dg, n ) ) tNodes.push_back( n );
    }

    /*
     * !!!ǰ������!!!
     * ������һ��Դ���һ�����
     * Ŀǰ��ʱ������������"���������"
     * ���磺û��Դ���û�л�㣬����Դ��㶼û��
     */

    if( tNodes.empty() ) return INVALID;

    // ����һ��������
    Digraph::Node tn = dg.addNode();

    // ���ӻ��"�����֧"(���-->������)
    int len = tNodes.size();
    for( int i = 0; i < len; i++ )
    {
        Digraph::Arc e = dg.addArc( tNodes[i], tn );
    }
    return tn; // ����������������
}

static bool ReadTopologyDatas( const char* filename, IntArray& E, IntArray& U, IntArray& V, DoubleArray& R )
{
    /* ��ȡ�������˺ͳ�ʼ��֧���� */

    std::ifstream inFile( filename );
    if( !inFile ) return false;

    while( !inFile.eof() )
    {
        int e, u, v;
        double r;
        inFile >> e >> u >> v >> r;

        if( inFile.fail() ) break;

        E.push_back( e );
        U.push_back( u );
        V.push_back( v );
        R.push_back( r );
    }

    inFile.close();

    return true;
}

static void ReadNetworkData( std::ifstream& inFile, double& Q )
{
    /* ��ȡ�ܷ��� */
    inFile >> Q;
}

static void ReadVNR_EdgeDatas_Q( std::ifstream& inFile, IntArray& E )
{
    // 1) ��ȡ�Ѳ������֧����
    int n = 0;
    inFile >> n;

    // 2) ��ȡ�Ѳ��֧
    for( int i = 0; i < n; i++ )
    {
        int e;
        inFile >> e;
        E.push_back( e );
    }
}

static void ReadVNR_EdgeDatas_R( std::ifstream& inFile, IntArray& E )
{
    // 1) ��ȡ�Ѳ�����֧����
    int n = 0;
    inFile >> n;

    // 2) ��ȡ�Ѳ��֧
    for( int i = 0; i < n; i++ )
    {
        int e;
        inFile >> e;
        E.push_back( e );
    }
}

static void ReadVNR_NodeDatas( std::ifstream& inFile, IntArray& U )
{
    // 1) ��ȡ�ڵ����ݸ���
    int n = 0;
    inFile >> n;

    // 2) ��ȡ�ڵ�����
    for( int i = 0; i < n; i++ )
    {
        int u;
        inFile >> u;

        U.push_back( u );
    }
}

static void BuildNodesMap( const IntArray& nodes, Digraph& dg, IntNodeMap& nodes_map )
{
    for( unsigned int i = 0; i < nodes.size(); i++ )
    {
        if( nodes_map.find( nodes[i] ) == nodes_map.end() )
        {
            nodes_map.insert( IntNodeMap::value_type( nodes[i], dg.addNode() ) );
        }
    }
}

static void BuildEdgesMap( const IntArray& E, const IntArray& U, const IntArray& V,
                           Digraph& dg, IntNodeMap& nodes_map, IntArcMap& edges_map )
{
    for( unsigned int i = 0; i < E.size(); i++ )
    {
        if( edges_map.find( E[i] ) == edges_map.end() )
        {
            Digraph::Node u = nodes_map[U[i]];
            Digraph::Node v = nodes_map[V[i]];
            Digraph::Arc e = dg.addArc( u, v );
            edges_map.insert( IntArcMap::value_type( E[i], e ) );
        }
    }
}

static bool ReadVNRDatas( const char* filename,
                          double& Q,
                          IntArray& TE, IntArray& TER, IntArray& TU )
{
    // ���ļ��ж�ȡ��������
    std::ifstream inFile( filename );
    if( !inFile ) return false;

    ReadNetworkData( inFile, Q );
    ReadVNR_EdgeDatas_Q( inFile, TE );
    ReadVNR_EdgeDatas_R( inFile, TER );
    ReadVNR_NodeDatas( inFile, TU );

    inFile.close();

    return true;
}

static void BuildNetworkMap( const IntArray& E, const IntArray& U, const IntArray& V,
                             Digraph& dg, IntNodeMap& nodes_map, IntArcMap& edges_map )
{
    // �����ڵ���-->�ڵ�ӳ��
    BuildNodesMap( U, dg, nodes_map );
    BuildNodesMap( V, dg, nodes_map );

    // ������֧���-->��֧ӳ��
    BuildEdgesMap( E, U, V, dg, nodes_map, edges_map );

    // ɾ����Ϊ0�Ľڵ�
    EraseZeroDegreeNodes( dg );
}

static void InitVNR_EdgeData( Digraph& dg, IntArcMap& edges_map, VNR_EdgeMap& vnr_edge_map )
{
    // ��ʼ�����з�֧����
    for( IntArcMap::iterator itr = edges_map.begin(); itr != edges_map.end(); ++itr )
    {
        int id = itr->first;
        Digraph::Arc e = itr->second;

        VNR_Edge_Data data = {id, 0, 0, 0, 0, false};
        vnr_edge_map[e] = data;
    }
}

static bool InitVNR_EdgeData_R( const IntArray& E,
                                const DoubleArray& R,
                                Digraph& dg,
                                IntArcMap& edges_map,
                                VNR_EdgeMap& vnr_edge_map )
{
    int count = 0;
    int n = E.size();
    for( int i = 0; i < n; i++ )
    {
        IntArcMap::iterator itr = edges_map.find( E[i] );
        if( itr != edges_map.end() )
        {
            Digraph::Arc e = itr->second;
            vnr_edge_map[e].r = fabs( R[i] );

            count++;
        }
    }
    // ��֤ÿ����֧�������˷���
    return ( count == n );
}

static bool InitVNR_EdgeData_TQ( const IntArray& TE,
                                 Digraph& dg,
                                 IntArcMap& edges_map,
                                 VNR_EdgeMap& vnr_edge_map )
{
    int count = 0;
    int n = TE.size();
    for( int i = 0; i < n; i++ )
    {
        IntArcMap::iterator itr = edges_map.find( TE[i] );
        if( itr != edges_map.end() )
        {
            Digraph::Arc e = itr->second;
            vnr_edge_map[e].q = vnr_edge_map[e].cq;
            vnr_edge_map[e].bInitQ = true;

            count++;
        }
    }
    return ( count != 0 );
}

static bool InitVNR_EdgeData_TR( const IntArray& TR,
                                 Digraph& dg,
                                 IntArcMap& edges_map,
                                 VNR_EdgeMap& vnr_edge_map )
{
    int count = 0;
    int n = TR.size();
    for( int i = 0; i < n; i++ )
    {
        IntArcMap::iterator itr = edges_map.find( TR[i] );
        if( itr != edges_map.end() )
        {
            Digraph::Arc e = itr->second;
            vnr_edge_map[e].bInitR = true;

            count++;
        }
    }
    return ( count != 0 );
}

static void InitVNR_NodeData( Digraph& dg, IntNodeMap& nodes_map, VNR_NodeMap& vnr_node_map )
{
    for( IntNodeMap::iterator itr = nodes_map.begin(); itr != nodes_map.end(); ++itr )
    {
        int id = itr->first;
        Digraph::Node u = itr->second;

        VNR_Node_Data data = {id, 0, 0, false};
        vnr_node_map[u] = data;
    }
}

static bool InitVNR_NodeData_TP( const IntArray& TU,
                                 Digraph& dg, IntNodeMap& nodes_map,
                                 VNR_NodeMap& vnr_node_map )
{
    int count = 0;
    int n = TU.size();
    for( int i = 0; i < n; i++ )
    {
        IntNodeMap::iterator itr = nodes_map.find( TU[i] );
        if( itr != nodes_map.end() )
        {
            Digraph::Node u = itr->second;
            vnr_node_map[u].p = vnr_node_map[u].cp;
            vnr_node_map[u].bInitP = true;

            count++;
        }
    }
    return ( count != 0 );
}

static bool BuildNetworkFromFile( const char* topology_data_filename,
                                  const char* vnr_data_filename,
                                  Digraph& dg,
                                  VNR_EdgeMap& vnr_edge_map,
                                  VNR_NodeMap& vnr_node_map,
                                  VNR_Network_Data& vnr_network_data )
{
    // �������˺ͷ�֧����
    IntArray E, U, V;
    DoubleArray R;

    // ���ļ��ж�ȡ������������(�������ˡ���֧���ݡ��ڵ�����)
    if( !ReadTopologyDatas( topology_data_filename, E, U, V, R ) ) return false;

    // ���ļ��ж�ȡ����ϵ������(�ܷ���������������֧���ݡ��ڵ�����)
    // �Ѳ�����ķ�֧
    IntArray TE;
    // �Ѳ����ķ�֧
    IntArray TR;
    // �Ѳ�ڵ�
    IntArray TU;
    // �ܷ�����������
    double Q;
    if( !ReadVNRDatas( vnr_data_filename, Q, TE, TR, TU ) ) return false;
    vnr_network_data.Q = Q;

    // ������ŵ������֧�ͽڵ��ӳ���ϵ
    // ���ܴ��ڷ�֧�ͽڵ����ظ����������Ҫ�޳���
    IntNodeMap nodes_map;
    IntArcMap edges_map;
    BuildNetworkMap( E, U, V, dg, nodes_map, edges_map );

    // ��ʼ���Ѳ��֧����(���衢����)
    // ���ܻ���Ҫ���ǣ�
    //		1) Ҫ��ʼ�����������ٷ�֧����
    //		2) ��Щ��֧����Ҫ��ʼ��???
    //      3) ��Щ��֧�ķ�������֪��
    InitVNR_EdgeData( dg, edges_map, vnr_edge_map );
    if( !InitVNR_EdgeData_R( E, R, dg, edges_map, vnr_edge_map ) ) return false;
    if( !InitVNR_EdgeData_TQ( TE, dg, edges_map, vnr_edge_map ) ) return false;
    if( !InitVNR_EdgeData_TR( TR, dg, edges_map, vnr_edge_map ) ) return false;

    // ��ʼ���ڵ�ѹ������
    // �����ṩԴ��ͻ��ڵ��ѹ��
    InitVNR_NodeData( dg, nodes_map, vnr_node_map );
    if( !InitVNR_NodeData_TP( TU, dg, nodes_map, vnr_node_map ) ) return false;

    return true;
}

bool BuildNetwork(
    const char* topology_data_filename,
    const char* vnr_data_filename,
    Digraph& dg,
    VNR_EdgeMap& vnr_edge_map,
    VNR_NodeMap& vnr_node_map,
    VNR_Network_Data& vnr_network_data )
{
    return BuildNetworkFromFile( topology_data_filename, vnr_data_filename, dg, vnr_edge_map, vnr_node_map, vnr_network_data );
}

bool AddVirtualSTNode( Digraph& dg, Digraph::Node& sn, Digraph::Node& tn )
{
    // ����һ������Դ�㼰��֧
    /*Digraph::Node*/ sn = AddVirtualSource( dg );

    // ����һ�������㼰��֧
    /*Digraph::Node*/
    tn = AddVirtualTarget( dg );

    return ( sn != INVALID ) && ( tn != INVALID );
}