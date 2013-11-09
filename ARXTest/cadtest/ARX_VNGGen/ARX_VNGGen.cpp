#include "StdAfx.h"

#include "VNG_Node.h"
#include "VNG_Edge.h"
#include "VNG_ParamHelper.h"

#include "../MineGE/HelperClass.h"
#include "../ArxHelper/HelperClass.h"

#include "../GraphTool/BuildNetwork.h"
#include "../VNGGen/VNGGen.h"

/* ͨ������ͼ���ڵ�ͼ��*/
#define VNG_LAYER _T("ͨ������ͼ")

#define PI 3.1415926535897932384626433832795

static void MapEdgeInfoWithObjId( Digraph& dg, ObjectIdMap& om, EdgeInfoArray& eis, AcDbObjectIdArray& objIds )
{
    for( EdgeInfoArray::iterator itr = eis.begin(); itr != eis.end(); itr++ )
    {
        EdgeInfo* ei = *itr;
        for( Digraph::ArcIt e( dg ); e != INVALID; ++e )
        {
            if( dg.id( e ) == ei->id )
            {
                objIds.append( om[e] );
                break;
            }
        }
    }
}

static void GetDataObjects( const AcDbObjectIdArray& objIds, AcDbObjectIdArray& dObjIds )
{
    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        AcDbObjectId dObjId;
        DataHelper::GetDataObject( objIds[i], dObjId );
        dObjIds.append( dObjId );
    }
}

static AcGePoint3d GetNodePt( const NodeInfoArray& nis, int id )
{
    AcGePoint3d pt;
    for( NodeInfoArray::const_iterator itr = nis.begin(); itr != nis.end(); ++itr )
    {
        NodeInfo* ni = *itr;
        if( ni->id == id )
        {
            pt.x = ni->x;
            pt.y = ni->y;
            break;
        }
    }
    return pt;
}

static AcGePoint3d CaclThirdPt( const AcGePoint3d& spt, const AcGePoint3d& ept, const AcGePoint3dArray& cnpt )
{
    AcGeLine3d line( spt, ept );

    // ����������
    double d = line.distanceTo( cnpt[0] );
    int di = 0;
    int n = cnpt.length();
    for( int i = 1; i < n; i++ )
    {
        double dd = line.distanceTo( cnpt[i] );
        if( dd > d )
        {
            di = i;
            d = dd;
        }
    }

    AcGeVector3d v = ept - spt;
    AcGePoint3d pt = spt + v * 0.5;

    acutPrintf( _T( "\nminD: %.5f" ), d );

    if( d < 0.1 ) // ���Ƶ���0
    {
        //acutPrintf(_T("\nƫ��һ����λ..."));
        v.normalize();
        v.rotateBy( PI / 2, AcGeVector3d::kZAxis );

        pt += v * 1.0; // ƫ��1����λ
    }
    else
    {
        //acutPrintf(_T("\nȡ������..."));
        AcGeVector3d v2 = cnpt[di] - line.closestPointTo( cnpt[di] );
        pt += v2;
    }
    return pt;
}

static void DrawEdges( /* ����ͼ */
    Digraph& dg,
    ObjectIdMap& om,
    /* �ڵ�ͷ�֧�ļ�����Ϣ */
    NodeInfoArray& nis,
    EdgeInfoArray& eis,
    /* ��ͼ���� */
    const AcGePoint3d& basePt,
    double m_arrowWidth,
    double m_arrowLength,
    double m_textHeight,
    bool m_needEdge )
{
    AcDbObjectIdArray objIds;
    MapEdgeInfoWithObjId( dg, om, eis, objIds );

    // ��ȡͼԪ�����ݶ���
    AcDbObjectIdArray dObjIds;
    GetDataObjects( objIds, dObjIds );

    // ����ϵ��׼ƫ������
    AcGeVector3d baseVector = basePt.asVector();

    //assert(dObjIds.length() == eis.size());
    int len = dObjIds.length();
    for( int i = 0; i < len; i++ )
    {
        EdgeInfo* ei = eis[i];

        VNG_Edge* pEdge = new VNG_Edge();

        // �������ݶ���
        // ��Ϊ�������֧��dObjIds[i]�����ǿյ�
        pEdge->setDataObject( dObjIds[i] );

        AcGePoint3d spt = GetNodePt( nis, ei->sn );
        AcGePoint3d ept = GetNodePt( nis, ei->tn );

        AcGePoint3dArray cnpt;
        int n = ei->cnx.size();
        for( int i = 0; i < n; i++ )
        {
            AcGePoint3d pt;
            pt.x = ei->cnx[i];
            pt.y = ei->cny[i];
            cnpt.append( pt );
        }

        pEdge->m_spt = spt + baseVector;
        pEdge->m_ept = ept + baseVector;
        pEdge->m_pt = CaclThirdPt( spt, ept, cnpt ) + baseVector;

        // ��ȡ��ͷ����
        pEdge->m_arrowWidth = m_arrowWidth;
        pEdge->m_arrowLength = m_arrowLength;

        // ��ȡ���ֱ�ע��Ϣ
        AcGePoint3d tpt;
        tpt.x = ei->tx;
        tpt.y = ei->ty;

        pEdge->m_tpt = tpt + baseVector;
        pEdge->m_textHeight = m_textHeight;
        pEdge->m_needEdge = m_needEdge;

        pEdge->m_id = ei->id;

        // �ύ�����ݿ�
        ArxUtilHelper::PostToModelSpace( pEdge );
    }
}

static void DrawNodes( Digraph& dg,
                       ObjectIdMap& om,
                       /* �ڵ�ͷ�֧�ļ�����Ϣ */
                       NodeInfoArray& nis,
                       /* ��ͼ���� */
                       const AcGePoint3d& basePt,
                       double m_textHeight )
{
    // ����ϵ��׼ƫ������
    AcGeVector3d baseVector = basePt.asVector();

    int count = nis.size();
    for( int i = 0; i < count; i++ )
    {
        NodeInfo* ni = nis[i];

        VNG_Node* pNode = new VNG_Node();

        // ���������
        AcGePoint3d pt;
        pt.x = ni->x;
        pt.y = ni->y;
        pNode->m_pt = ( pt + baseVector );
        // ��Բ���������
        pNode->m_width = ni->cx;
        pNode->m_height = ni->cy;
        // ���ָ߶�
        pNode->m_textHeight = m_textHeight;
        // ���ֱ�ע����
        pNode->m_id = ni->id;

        // �ύ�����ݿ�
        ArxUtilHelper::PostToModelSpace( pNode );
    }
}

static void DrawVentNetworkGraph( /* ����ͼ */
    Digraph& dg,
    ObjectIdMap& om,
    /* �ڵ�ͷ�֧�ļ�����Ϣ */
    GraphInfo& gi,
    NodeInfoArray& nis,
    EdgeInfoArray& eis,
    /* ��ͼ���� */
    const AcGePoint3d& basePt,
    double m_nodeTextHeight,
    double m_arrowWidth,
    double m_arrowLength,
    double m_edgeTextHeight,
    bool m_needEdge )
{
    acutPrintf( _T( "\n���ݽ����õ��ļ�����Ϣ��������ͼ..." ) );

    // ��ȡ��ǰͼ������
    CString curLayer = LayerHelper::GetCurrentLayerName();
    acutPrintf( _T( "\n��ǰͼ��:��%s��" ), curLayer );

    if( !LayerHelper::IsLayerExist( VNG_LAYER ) )
    {
        LayerHelper::AddLayer( VNG_LAYER ); // ��������򲻽���
    }
    else
    {
        // ɾ��ͼ���ϵ�����ͼ��ʵ��
        LayerHelper::DeleteAllEntitiesOnLayer( VNG_LAYER );
    }
    // �л���ͼ��VNG_LAYER
    LayerHelper::SetCurrentLayer( VNG_LAYER );

    // ���Ʒ�֧
    DrawEdges( dg, om, nis, eis, basePt, m_arrowWidth, m_arrowLength, m_edgeTextHeight, m_needEdge );

    // ���ƽڵ�
    DrawNodes( dg, om, nis, basePt, m_nodeTextHeight );

    // �л���ԭ����ͼ��
    LayerHelper::SetCurrentLayer( curLayer );
}

/* ��ӡͼ������Ϣ(������) */
static void PrintGraphInfo( GraphInfo& gi )
{
    acutPrintf( _T( "\n��������ratio = %.3f\tͼ�Ŀ���width = %.3f\tͼ�ĸ߶�height = %.3f\n" ), gi.ratio, gi.width, gi.height );
}

static void PrintOneNodeInfo( NodeInfo* ni )
{
    acutPrintf( _T( "\n�ڵ�����name = v%d\t����(x=%.3f, y=%.3f)\t����width=%.3f\t�߶�=%.3f\n" ), ni->id, ni->x, ni->y, ni->cx, ni->cy );
}

static void PrintOneEdgeInfo( EdgeInfo* ei )
{
    acutPrintf( _T( "\n��֧����name = e%d\tʼ�ڵ�:%d\tĩ�ڵ�:%d\n���ֱ�עλ��(x=%.3f, y=%.3f)\n" ), ei->id, ei->sn, ei->tn, ei->tx, ei->ty );
    int n = ei->cnx.size();
    for( int i = 0; i < n; i++ )
    {
        acutPrintf( _T( "\t���Ƶ�:%d\t (x=%.3f, y=%.3f)\n" ), i + 1, ei->cnx[i], ei->cny[i] );
    }
    acutPrintf( _T( "\n" ) );
}

static void PrintNodeInfo( NodeInfoArray& nis )
{
    NodeInfoArray::iterator itr;
    for( itr = nis.begin(); itr != nis.end(); itr++ )
    {
        PrintOneNodeInfo( *itr );
    }
}

static void PrintEdgeInfo( EdgeInfoArray& eis )
{
    EdgeInfoArray::iterator itr;
    for( itr = eis.begin(); itr != eis.end(); itr++ )
    {
        PrintOneEdgeInfo( *itr );
    }
}

void ARX_VNGGen( const AcGePoint3d& basePt )
{
    // 1) ��������
    acutPrintf( _T( "\n1) ��������" ) );
    Digraph dg;
    ObjectIdMap om( dg );
    if( !BuildNetwork( dg, om ) ) return /*false*/;

    // 2) ��ȡ����ͼ���Ʋ���(�����ڴʵ�VNG_PARAM_DICT��)
    acutPrintf( _T( "\n2) ��ȡ����" ) );
    // ͼ�β���
    double m_nodeSep;
    double m_rankSep;
    double m_graphRatio;
    double m_graphWidth;
    double m_graphHeight;
    bool m_useDefWH;

    // �ڵ����
    double m_nodeWidth;
    double m_nodeHeight;
    double m_nodeTextHeight;

    // ��֧����
    double m_arrowWidth;
    double m_arrowLength;
    double m_edgeTextHeight;
    bool m_needEdge;

    VNG_ParamHelper::ReadParams( m_nodeSep, m_rankSep,
                                 m_graphRatio, m_graphWidth, m_graphHeight, m_useDefWH,
                                 m_nodeWidth, m_nodeHeight, m_nodeTextHeight,
                                 m_arrowWidth, m_arrowLength, m_edgeTextHeight, m_needEdge );

    // 3) ����VNGGen.dll��������ͼ�����㼸����Ϣ
    acutPrintf( _T( "3) ����VNGGen.dll��������ͼ\n" ) );
    GraphInfo gi;                     // ͼ�ļ�����Ϣ
    NodeInfoArray nis;                // �ڵ㼸����Ϣ
    EdgeInfoArray eis;                // ��֧������Ϣ
    if( VNGGen( dg, m_nodeSep, m_rankSep,
                m_graphRatio, m_graphWidth, m_graphHeight, m_useDefWH,
                m_nodeWidth, m_nodeHeight, m_nodeTextHeight, m_edgeTextHeight,
                gi, nis, eis ) )
    {
        acutPrintf( _T( "\n��������ͼ�ɹ�!" ) );
        PrintGraphInfo( gi );
        PrintNodeInfo( nis );
        PrintEdgeInfo( eis );

        // ��������ͼ
        DrawVentNetworkGraph( dg, om, gi, nis, eis, basePt, m_nodeTextHeight, m_arrowWidth, m_arrowLength, m_edgeTextHeight, m_needEdge );

        // ɾ���ڴ�
        ClearNodeInfoArray( nis );
        ClearEdgeInfoArray( eis );
    }
}