#include "stdafx.h"

#include "VNG_ParamHelper.h"
#include "config.h"
#include "MVSSData.h"
typedef AcArray<MVSSData*> MVSSDataArray;

#include "../VNGGen/VNGGen.h"
typedef Digraph::NodeMap<int> NodeIdMap;
typedef Digraph::ArcMap<int> ArcIdMap;

#include "../VNG_GE/VNG_Node.h"
#include "../VNG_GE/VNG_Edge.h"
#include "../VNG_GE/VNG_Gate.h"
#include "../VNG_GE/VNG_Fan.h"

#include "../ArxHelper/HelperClass.h"

#define PI 3.1415926535897932384626433832795

// ��¼��ǰͼ��
static CString CurLayerName;

static void GetDatas( CDaoDatabase* pDB, MVSS_DATA_TYPE type, MVSSDataArray& mda )
{
    CDaoRecordset rs( pDB );
    CString sql;
    sql.Format( _T( "select * from %s" ), GetTableName( type ) );
    rs.Open( dbOpenDynaset, sql, 0 );

    rs.MoveFirst () ;
    while( !rs.IsEOF() )
    {
        CString data;
        if( ReadComment( rs, data ) )
        {
            MVSSData* pData = CreateMVSSData( type );
            pData->readData( data );
            if( !pData->isValid() )
            {
                delete pData;
                pData = 0;
            }
            else
            {
                mda.append( pData );
            }
        }

        rs.MoveNext ();
    }

    rs.Close();
}

static void ClearDatas( MVSSDataArray& da )
{
    int n = da.length();
    for( int i = 0; i < n; i++ )
    {
        delete da[i];
        da[i] = 0;
    }
}

static void GetAllDatas( CDaoDatabase* pDB, MVSSDataArray& jd, MVSSDataArray& td, MVSSDataArray& gd, MVSSDataArray& fd )
{
    GetDatas( pDB, MDT_NODE, jd );
    GetDatas( pDB, MDT_EDGE, td );
    GetDatas( pDB, MDT_GATE, gd );
    GetDatas( pDB, MDT_FAN, fd );
}

static void ClearAllDatas( MVSSDataArray& jd, MVSSDataArray& td, MVSSDataArray& gd, MVSSDataArray& fd )
{
    ClearDatas( jd );
    ClearDatas( td );
    ClearDatas( gd );
    ClearDatas( fd );
}

static void ReadDataFromMVSSDB( const CString& dbPath, const CString& pwd, MVSSDataArray& jd, MVSSDataArray& td, MVSSDataArray& gd, MVSSDataArray& fd )
{
    CDaoDatabase db;
    // ���������ַ���
    CString cnt;
    if( pwd.GetLength() != 0 )
    {
        cnt.Format( _T( ";PWD=%s" ), pwd ); // ����
    }

    try
    {
        db.Open( dbPath, 0, 0, cnt );

        GetAllDatas( &db, jd, td, gd, fd );

        db.Close();
    }
    catch ( CDaoException* e )
    {
        e->ReportError();
        e->Delete();

        if( db.IsOpen() ) db.Close();
    }
}

// !!!��δ����������
static void BuildNetwork( const MVSSDataArray& jd, const MVSSDataArray& td,
                          Digraph& dg, NodeIdMap& nid, ArcIdMap& eid )
{
    // �����¼���еĽڵ�
    typedef AcArray<Digraph::Node> NodeArray;
    NodeArray na;

    // �������飬���ڲ�ѯ
    AcDbIntArray ids;

    int n = jd.length();
    for( int i = 0; i < n; i++ )
    {
        // ���ӽڵ�
        Digraph::Node u = dg.addNode();
        // ��¼�ڵ�ʵ�ʱ��
        nid[u] = jd[i]->id;

        na.append( u );
        ids.append( jd[i]->id );
    }

    /*int */n = td.length();
    for( int i = 0; i < n; i++ )
    {
        TunnelData* pData = ( TunnelData* )td[i];

        // ȷ����֧��ʼĩ�ڵ�
        Digraph::Node u = na[ids.find( pData->spt )];
        Digraph::Node v = na[ids.find( pData->ept )];

        // ���ӷ�֧
        Digraph::Arc e = dg.addArc( u, v );
        // ��¼��֧��ʵ�ʱ��
        eid[e] = pData->id;
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

    if( d < 0.1 ) // ���Ƶ���0
    {
        v.normalize();
        v.rotateBy( PI / 2, AcGeVector3d::kZAxis );

        pt += v * 1.0; // ƫ��1����λ
    }
    else
    {
        AcGeVector3d v2 = cnpt[di] - line.closestPointTo( cnpt[di] );
        pt += v2;
    }
    return pt;
}

static void CaclEdgeData( NodeInfoArray& nis, EdgeInfo* ei, AcGePoint3d& spt, AcGePoint3d& ept, AcGePoint3d& pt )
{
    spt = GetNodePt( nis, ei->sn );
    ept = GetNodePt( nis, ei->tn );

    AcGePoint3dArray cnpt;
    int n = ei->cnx.size();
    for( int i = 0; i < n; i++ )
    {
        AcGePoint3d pt;
        pt.x = ei->cnx[i];
        pt.y = ei->cny[i];
        cnpt.append( pt );
    }

    pt = CaclThirdPt( spt, ept, cnpt );
}

// ������ֱ�ע����Բ����֧��3�����غϣ��ʵ�����ƫ��
static void OffsetPoint( const AcGePoint3d& spt, const AcGePoint3d& ept, const AcGePoint3d& pt, AcGePoint3d& tpt )
{
    if( tpt == spt || tpt == ept || tpt == pt )
    {
        AcGeCircArc3d arc( spt, pt, ept );
        AcGePoint3d cnt = arc.center();

        AcGeVector3d v = pt - cnt;
        v.normalize();

        double d = 5;
        tpt += v * d;
    }
}

static void DrawEdges( /* �ڵ�ͷ�֧�ļ�����Ϣ */
    NodeInfoArray& nis,
    EdgeInfoArray& eis,
    /* ��ͼ���� */
    const AcGePoint3d& basePt,
    double m_arrowWidth,
    double m_arrowLength,
    double m_textHeight,
    bool m_needEdge )
{
    // ����ϵ��׼ƫ������
    AcGeVector3d baseVector = basePt.asVector();

    int n = eis.size();
    for( int i = 0; i < n; i++ )
    {
        EdgeInfo* ei = eis[i];

        // ���Ʒ�֧
        VNG_Edge* pEdge = new VNG_Edge();

        // ���
        pEdge->m_id = ei->id;

        // �����֧3�㻡����
        AcGePoint3d spt, ept, pt;
        CaclEdgeData( nis, ei, spt, ept, pt );
        pEdge->m_spt = spt + baseVector;
        pEdge->m_ept = ept + baseVector;
        pEdge->m_pt = pt + baseVector;

        // ��ȡ��ͷ����
        pEdge->m_arrowWidth = m_arrowWidth;
        pEdge->m_arrowLength = m_arrowLength;

        // ��ȡ���ֱ�ע��Ϣ
        AcGePoint3d tpt;
        tpt.x = ei->tx;
        tpt.y = ei->ty;

        pEdge->m_tpt = tpt + baseVector;
        // ���⴦�����������ֱ�ע���Բ����֧��3���غ�
        OffsetPoint( pEdge->m_spt, pEdge->m_ept, pEdge->m_pt, pEdge->m_tpt );

        pEdge->m_textHeight = m_textHeight;
        pEdge->m_needEdge = m_needEdge;

        // �ύ�����ݿ�
        ArxUtilHelper::PostToModelSpace( pEdge );
    }
}

static void DrawNodes( /* �ڵ�ͷ�֧�ļ�����Ϣ */
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

        // ���ƽڵ�
        VNG_Node* pNode = new VNG_Node();

        // ���
        pNode->m_id = ni->id;
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

        // �ύ�����ݿ�
        ArxUtilHelper::PostToModelSpace( pNode );
    }
}

static void DrawGates( MVSSDataArray& gd )
{
    // �������еķ�֧
    AcDbObjectIdArray objIds;
    ArxDataTool::GetEntsByType( _T( "VNG_Edge" ), objIds );
    if( objIds.isEmpty() ) return;

    // ����һ���������飬���ڲ�ѯ
    AcDbIntArray ids;
    for( int i = 0; i < gd.length(); i++ )
    {
        GateData* pData = ( GateData* )gd[i];
        ids.append( pData->tObjId );
    }

    AcTransaction* pTrans = actrTransactionManager->startTransaction();

    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        AcDbObject* pObj;
        if( Acad::eOk != pTrans->getObject( pObj, objIds[i], AcDb::kForRead ) ) continue;

        VNG_Edge* pEdge = VNG_Edge::cast( pObj );
        int pos = ids.find( pEdge->m_id );
        if( pos < 0 ) continue;

        // ���ƹ�����
        VNG_Gate* pGate = new VNG_Gate();

        // ���
        pGate->m_id = gd[pos]->id;

        // ���㷽��Ƕ�
        pGate->setData( pEdge->m_spt, pEdge->m_pt, pEdge->m_ept );

        // �ύ�����ݿ�
        ArxUtilHelper::PostToModelSpace( pGate );
    }
    actrTransactionManager->endTransaction();
}

static void DrawFans( const MVSSDataArray& fd )
{
    // �������еķ�֧
    AcDbObjectIdArray objIds;
    ArxDataTool::GetEntsByType( _T( "VNG_Edge" ), objIds );
    if( objIds.isEmpty() ) return;

    // ����һ���������飬���ڲ�ѯ
    AcDbIntArray ids;
    for( int i = 0; i < fd.length(); i++ )
    {
        FanData* pData = ( FanData* )fd[i];
        ids.append( pData->tObjId );
    }

    AcTransaction* pTrans = actrTransactionManager->startTransaction();

    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        AcDbObject* pObj;
        if( Acad::eOk != pTrans->getObject( pObj, objIds[i], AcDb::kForRead ) ) continue;

        VNG_Edge* pEdge = VNG_Edge::cast( pObj );
        int pos = ids.find( pEdge->m_id );
        if( pos < 0 ) continue;

        // ���ƹ�����
        VNG_Fan* pFan = new VNG_Fan();

        // ���
        pFan->m_id = fd[pos]->id;

        // ���㷽��Ƕ�
        pFan->setData( pEdge->m_spt, pEdge->m_pt, pEdge->m_ept );

        // �ύ�����ݿ�
        ArxUtilHelper::PostToModelSpace( pFan );
    }
    actrTransactionManager->endTransaction();
}

static void BeginDrawVentNetworkGraph()
{
    // ��ȡ��ǰͼ������
    CurLayerName = LayerHelper::GetCurrentLayerName();
    //acutPrintf(_T("\n��ǰͼ��: %s"), CurLayerName);

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
}

static void EndDrawVentNetworkGraph()
{
    // �л���ԭ����ͼ��
    LayerHelper::SetCurrentLayer( CurLayerName );
}

static int GetRealNodeNum( Digraph& dg, NodeIdMap& nid, int id )
{
    return nid[dg.nodeFromId( id )];
}

static int GetRealEdgeNum( Digraph& dg, ArcIdMap& eid, int id )
{
    return eid[dg.arcFromId( id )];
}

// �ָ�ʵ�ʵı��
// nis��eis�еı��������ͼdg�����ı��
static void RestoreRealNum( Digraph& dg,
                            NodeIdMap& nid,
                            ArcIdMap& eid,
                            /* �ڵ�ͷ�֧�ļ�����Ϣ */
                            NodeInfoArray& nis,
                            EdgeInfoArray& eis )
{
    // �ָ��ڵ���
    int n = nis.size();
    for( int i = 0; i < n; i++ )
    {
        NodeInfo* ni = nis[i];
        ni->id = GetRealNodeNum( dg, nid, ni->id );
    }

    // �ָ���֧���
    n = eis.size();
    for( int i = 0; i < n; i++ )
    {
        EdgeInfo* ei = eis[i];
        ei->id = GetRealEdgeNum( dg, eid, ei->id );
        ei->sn = GetRealNodeNum( dg, nid, ei->sn );
        ei->tn = GetRealNodeNum( dg, nid, ei->tn );
    }
}

static void DrawVentNetworkGraph( /* �������� */
    const AcGePoint3d& basePt,
    /* �ڵ�ͷ�֧�ļ�����Ϣ */
    GraphInfo& gi,
    NodeInfoArray& nis,
    EdgeInfoArray& eis,
    /* ��ͼ���� */
    const GraphParam& gp,
    const NodeParam& np,
    const EdgeParam& ep,
    /* ��������*/
    MVSSDataArray& jd,
    MVSSDataArray& td,
    MVSSDataArray& gd,
    MVSSDataArray& fd )
{
    //acutPrintf(_T("\n���ݽ����õ��ļ�����Ϣ��������ͼ..."));

    BeginDrawVentNetworkGraph();

    // a) ���ƽڵ�
    DrawNodes( nis, basePt, np.m_nodeTextHeight );
    // b) ���Ʒ�֧
    DrawEdges( nis, eis, basePt, ep.m_arrowWidth, ep.m_arrowLength, ep.m_edgeTextHeight, ep.m_needEdge );
    // c) ���Ʒ���
    DrawGates( gd );
    // d) ���Ʒ��
    DrawFans( fd );

    EndDrawVentNetworkGraph();
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

static void MakeVNGParam( const GraphParam& gp, const NodeParam& np, const EdgeParam& ep, VNGParam& param )
{
    param.m_nodeSep = gp.m_nodeSep;
    param.m_rankSep = gp.m_rankSep;
    param.m_graphRatio = gp.m_graphRatio;
    param.m_graphWidth = gp.m_graphWidth;
    param.m_graphHeight = gp.m_graphHeight;
    param.m_useDefWH = gp.m_useDefWH;

    param.m_nodeWidth = np.m_nodeWidth;
    param.m_nodeHeight = np.m_nodeHeight;
    param.m_nodeTextHeight = np.m_nodeTextHeight;

    param.m_edgeTextHeight = ep.m_edgeTextHeight;
}

bool DoVNGGen( const CString& dbPath, const CString& pwd, const AcGePoint3d& basePt )
{
    MVSSDataArray jd, td, gd, fd;
    ReadDataFromMVSSDB( dbPath, pwd, jd, td, gd, fd );
    if( jd.isEmpty() || td.isEmpty() ) return false;

    // 1) ��������
    //acutPrintf(_T("\n1) ��������"));
    Digraph dg;
    NodeIdMap nid( dg ); // ��¼ʵ�ʵĽڵ���
    ArcIdMap eid( dg ); // ��¼ʵ�ʵķ�֧���
    BuildNetwork( jd, td, dg, nid, eid );

    // 2) ��ȡ����ͼ���Ʋ���(�����ڴʵ�VNG_PARAM_DICT��)
    //acutPrintf(_T("\n2) ��ȡ����"));
    GraphParam gp;
    NodeParam np;
    EdgeParam ep;
    VNG_ParamHelper::ReadGraphParams( gp );
    VNG_ParamHelper::ReadNodeParams( np );
    VNG_ParamHelper::ReadEdgeParams( ep );

    // 3) ����VNGGen.dll��������ͼ�����㼸����Ϣ
    //acutPrintf(_T("\n3) ����VNGGen.dll��������ͼ\n"));
    VNGParam param;
    MakeVNGParam( gp, np, ep, param ); // ������Ʋ�������

    GraphInfo gi;                     // ͼ�ļ�����Ϣ
    NodeInfoArray nis;                // �ڵ㼸����Ϣ
    EdgeInfoArray eis;                // ��֧������Ϣ

    bool ret = VNGGen( dg, param, gi, nis, eis ); // ��������ͼ(���㼸����Ϣ)
    if( ret )
    {
        //acutPrintf(_T("\n��������ͼ�ɹ�!"));
        //PrintGraphInfo(gi);
        //PrintNodeInfo(nis);
        //PrintEdgeInfo(eis);

        // �ָ��ڵ�ͷ�֧��ʵ�ʱ��
        RestoreRealNum( dg, nid, eid, nis, eis );

        //��������ͼ
        DrawVentNetworkGraph(
            basePt,
            gi, nis, eis,
            gp, np, ep,
            jd, td, gd, fd );

        // ɾ���ڴ�
        ClearNodeInfoArray( nis );
        ClearEdgeInfoArray( eis );
    }

    // ɾ���������ݵ��ڴ�
    ClearAllDatas( jd, td, gd, fd );

    return ret;
}