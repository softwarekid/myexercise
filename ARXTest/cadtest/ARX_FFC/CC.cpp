#include "StdAfx.h"
#include "BuildNetwork.h"

#include "../MineGE/HelperClass.h"

#include <lemon/adaptors.h>
#include <lemon/connectivity.h>

typedef Digraph::NodeMap<int> CompMap;

// ��ͨ��֧visitor(��������ͼ)
class CNNEdgeVisitor : public DfsVisitor<Digraph>
{
public:
    CNNEdgeVisitor( EdgeArray& es_ ) : es( es_ ) {}
    void discover( const Digraph::Arc& e )
    {
        addCNNEdge( e );
    }
    void examine( const Digraph::Arc& e )
    {
        addCNNEdge( e );
    }

private:
    void addCNNEdge( const Digraph::Arc& e )
    {
        es.append( e );
    }

    EdgeArray& es;
};

static void FindEdgeByCCId( Digraph& dg, CompMap& compMap, int cc, EdgeArray& es )
{
    //���ҵ�cc����ͨ��
    typedef Digraph::NodeMap<bool> NodeFilter;
    NodeFilter nf( dg, false );  // ���ڹ�����ͨ��ʶ������cc�Ľڵ�

    int count = 0;
    for( Digraph::NodeIt n( dg ); n != INVALID; ++n )
    {
        if( compMap[n] == cc )
        {
            count++;
            nf[n] = true;
        }
    }
    if( count < 2 ) return; // ���Ե���һ���ڵ���(ǿ)��ͨ���������

    // ������ͼ���й���, ���˵�������ͨ���ʶ��Ϊcc�Ľڵ�
    typedef FilterNodes<Digraph, NodeFilter> NFGraph;
    NFGraph nfg( dg, nf );

    // ִ��һ��dfs
    // ��¼��ͨ��ʶΪcc�Ľڵ�֮��ķ�֧����(����ͼ��֧)
    CNNEdgeVisitor visitor( es );
    DfsVisit<NFGraph, CNNEdgeVisitor> dv( nfg, visitor );
    dv.run();
}

// ���ҷ�֧ͼԪ�������������
//static void FindEdgeGEs(AcDbObjectIdArray& objIds)
//{
//	DrawHelper::FindMineGEs(_T("LinkedGE"), objIds);
//}

// ������ͨ��
// ʹ��1ά���鱣����ͨ����Ϣ
int CC( const AcDbObjectIdArray& objIds, AcDbIntArray& ccIds )
{
    // 1) �������еķ�֧ͼԪ
    //AcDbObjectIdArray allObjIds;
    //FindEdgeGEs(allObjIds);
    //acutPrintf(_T("\n���ҵ��ķ�֧����:%d"), allObjIds.length());
    //if(allObjIds.isEmpty()) return -1;

    // 2) ��������
    Digraph dg;
    ObjectIdMap om( dg );
    if( !BuildNetwork( objIds, dg, om ) ) return -1;

    // ����ڵ���ͨ��map
    CompMap compMap( dg );

    // ת��������ͼ
    typedef Undirector<Digraph> UGraph;
    UGraph ug( dg );

    // ������ͨ�����
    int c = lemon::connectedComponents( ug, compMap );
    //acutPrintf(_T("\n��ͨ�����:%d"), c);
    //if(c == 0) return true; // ����Ϊ��

    // ��ʼ��
    for( int i = 0; i < objIds.length(); i++ )
    {
        ccIds.append( 0 );
    }

    for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
    {
        int pos = objIds.find( om[e] );
        ccIds[pos] = compMap[dg.target( e )];
    }
    //for(int i=0;i<c;i++)
    //{
    //	EdgeArray es;
    //	FindEdgeByCCId(dg, compMap, i, es);
    //	if(es.isEmpty()) continue;

    //	int len = es.length();
    //	for(int j=0;j<len;j++)
    //	{
    //		Digraph::Arc e = es[j];
    //		objIds.append(om[e]);
    //	}
    //	ccIds.append(len);
    //}

    return c;
}