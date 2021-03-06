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

// 记录当前图层
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
    // 构造连接字符串
    CString cnt;
    if( pwd.GetLength() != 0 )
    {
        cnt.Format( _T( ";PWD=%s" ), pwd ); // 密码
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

// !!!尚未检查冗余情况
static void BuildNetwork( const MVSSDataArray& jd, const MVSSDataArray& td,
                          Digraph& dg, NodeIdMap& nid, ArcIdMap& eid )
{
    // 数组记录所有的节点
    typedef AcArray<Digraph::Node> NodeArray;
    NodeArray na;

    // 索引数组，用于查询
    AcDbIntArray ids;

    int n = jd.length();
    for( int i = 0; i < n; i++ )
    {
        // 添加节点
        Digraph::Node u = dg.addNode();
        // 记录节点实际编号
        nid[u] = jd[i]->id;

        na.append( u );
        ids.append( jd[i]->id );
    }

    /*int */n = td.length();
    for( int i = 0; i < n; i++ )
    {
        TunnelData* pData = ( TunnelData* )td[i];

        // 确定分支的始末节点
        Digraph::Node u = na[ids.find( pData->spt )];
        Digraph::Node v = na[ids.find( pData->ept )];

        // 添加分支
        Digraph::Arc e = dg.addArc( u, v );
        // 记录分支的实际编号
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

    // 计算最大距离
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

    if( d < 0.1 ) // 近似等于0
    {
        v.normalize();
        v.rotateBy( PI / 2, AcGeVector3d::kZAxis );

        pt += v * 1.0; // 偏移1个单位
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

// 如果文字标注点与圆弧分支的3点有重合，适当进行偏移
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

static void DrawEdges( /* 节点和分支的几何信息 */
    NodeInfoArray& nis,
    EdgeInfoArray& eis,
    /* 绘图参数 */
    const AcGePoint3d& basePt,
    double m_arrowWidth,
    double m_arrowLength,
    double m_textHeight,
    bool m_needEdge )
{
    // 坐标系基准偏移向量
    AcGeVector3d baseVector = basePt.asVector();

    int n = eis.size();
    for( int i = 0; i < n; i++ )
    {
        EdgeInfo* ei = eis[i];

        // 绘制分支
        VNG_Edge* pEdge = new VNG_Edge();

        // 编号
        pEdge->m_id = ei->id;

        // 计算分支3点弧参数
        AcGePoint3d spt, ept, pt;
        CaclEdgeData( nis, ei, spt, ept, pt );
        pEdge->m_spt = spt + baseVector;
        pEdge->m_ept = ept + baseVector;
        pEdge->m_pt = pt + baseVector;

        // 读取箭头参数
        pEdge->m_arrowWidth = m_arrowWidth;
        pEdge->m_arrowLength = m_arrowLength;

        // 读取文字标注信息
        AcGePoint3d tpt;
        tpt.x = ei->tx;
        tpt.y = ei->ty;

        pEdge->m_tpt = tpt + baseVector;
        // 额外处理，避免文字标注点和圆弧分支的3点重合
        OffsetPoint( pEdge->m_spt, pEdge->m_ept, pEdge->m_pt, pEdge->m_tpt );

        pEdge->m_textHeight = m_textHeight;
        pEdge->m_needEdge = m_needEdge;

        // 提交到数据库
        ArxUtilHelper::PostToModelSpace( pEdge );
    }
}

static void DrawNodes( /* 节点和分支的几何信息 */
    NodeInfoArray& nis,
    /* 绘图参数 */
    const AcGePoint3d& basePt,
    double m_textHeight )
{
    // 坐标系基准偏移向量
    AcGeVector3d baseVector = basePt.asVector();

    int count = nis.size();
    for( int i = 0; i < count; i++ )
    {
        NodeInfo* ni = nis[i];

        // 绘制节点
        VNG_Node* pNode = new VNG_Node();

        // 编号
        pNode->m_id = ni->id;
        // 插入点坐标
        AcGePoint3d pt;
        pt.x = ni->x;
        pt.y = ni->y;
        pNode->m_pt = ( pt + baseVector );
        // 椭圆横轴和纵轴
        pNode->m_width = ni->cx;
        pNode->m_height = ni->cy;
        // 文字高度
        pNode->m_textHeight = m_textHeight;

        // 提交到数据库
        ArxUtilHelper::PostToModelSpace( pNode );
    }
}

static void DrawGates( MVSSDataArray& gd )
{
    // 查找所有的分支
    AcDbObjectIdArray objIds;
    ArxDataTool::GetEntsByType( _T( "VNG_Edge" ), objIds );
    if( objIds.isEmpty() ) return;

    // 构造一个索引数组，用于查询
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

        // 绘制构筑物
        VNG_Gate* pGate = new VNG_Gate();

        // 编号
        pGate->m_id = gd[pos]->id;

        // 计算方向角度
        pGate->setData( pEdge->m_spt, pEdge->m_pt, pEdge->m_ept );

        // 提交到数据库
        ArxUtilHelper::PostToModelSpace( pGate );
    }
    actrTransactionManager->endTransaction();
}

static void DrawFans( const MVSSDataArray& fd )
{
    // 查找所有的分支
    AcDbObjectIdArray objIds;
    ArxDataTool::GetEntsByType( _T( "VNG_Edge" ), objIds );
    if( objIds.isEmpty() ) return;

    // 构造一个索引数组，用于查询
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

        // 绘制构筑物
        VNG_Fan* pFan = new VNG_Fan();

        // 编号
        pFan->m_id = fd[pos]->id;

        // 计算方向角度
        pFan->setData( pEdge->m_spt, pEdge->m_pt, pEdge->m_ept );

        // 提交到数据库
        ArxUtilHelper::PostToModelSpace( pFan );
    }
    actrTransactionManager->endTransaction();
}

static void BeginDrawVentNetworkGraph()
{
    // 获取当前图层名称
    CurLayerName = LayerHelper::GetCurrentLayerName();
    //acutPrintf(_T("\n当前图层: %s"), CurLayerName);

    if( !LayerHelper::IsLayerExist( VNG_LAYER ) )
    {
        LayerHelper::AddLayer( VNG_LAYER ); // 如果存在则不建立
    }
    else
    {
        // 删除图层上的所有图形实体
        LayerHelper::DeleteAllEntitiesOnLayer( VNG_LAYER );
    }
    // 切换到图层VNG_LAYER
    LayerHelper::SetCurrentLayer( VNG_LAYER );
}

static void EndDrawVentNetworkGraph()
{
    // 切换回原来的图层
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

// 恢复实际的编号
// nis和eis中的编号是网络图dg产生的编号
static void RestoreRealNum( Digraph& dg,
                            NodeIdMap& nid,
                            ArcIdMap& eid,
                            /* 节点和分支的几何信息 */
                            NodeInfoArray& nis,
                            EdgeInfoArray& eis )
{
    // 恢复节点编号
    int n = nis.size();
    for( int i = 0; i < n; i++ )
    {
        NodeInfo* ni = nis[i];
        ni->id = GetRealNodeNum( dg, nid, ni->id );
    }

    // 恢复分支编号
    n = eis.size();
    for( int i = 0; i < n; i++ )
    {
        EdgeInfo* ei = eis[i];
        ei->id = GetRealEdgeNum( dg, eid, ei->id );
        ei->sn = GetRealNodeNum( dg, nid, ei->sn );
        ei->tn = GetRealNodeNum( dg, nid, ei->tn );
    }
}

static void DrawVentNetworkGraph( /* 基点坐标 */
    const AcGePoint3d& basePt,
    /* 节点和分支的几何信息 */
    GraphInfo& gi,
    NodeInfoArray& nis,
    EdgeInfoArray& eis,
    /* 绘图参数 */
    const GraphParam& gp,
    const NodeParam& np,
    const EdgeParam& ep,
    /* 拓扑数据*/
    MVSSDataArray& jd,
    MVSSDataArray& td,
    MVSSDataArray& gd,
    MVSSDataArray& fd )
{
    //acutPrintf(_T("\n根据解析得到的几何信息绘制网络图..."));

    BeginDrawVentNetworkGraph();

    // a) 绘制节点
    DrawNodes( nis, basePt, np.m_nodeTextHeight );
    // b) 绘制分支
    DrawEdges( nis, eis, basePt, ep.m_arrowWidth, ep.m_arrowLength, ep.m_edgeTextHeight, ep.m_needEdge );
    // c) 绘制风门
    DrawGates( gd );
    // d) 绘制风机
    DrawFans( fd );

    EndDrawVentNetworkGraph();
}

/* 打印图几何信息(测试用) */
static void PrintGraphInfo( GraphInfo& gi )
{
    acutPrintf( _T( "\n缩放因子ratio = %.3f\t图的宽度width = %.3f\t图的高度height = %.3f\n" ), gi.ratio, gi.width, gi.height );
}

static void PrintOneNodeInfo( NodeInfo* ni )
{
    acutPrintf( _T( "\n节点名称name = v%d\t坐标(x=%.3f, y=%.3f)\t宽度width=%.3f\t高度=%.3f\n" ), ni->id, ni->x, ni->y, ni->cx, ni->cy );
}

static void PrintOneEdgeInfo( EdgeInfo* ei )
{
    acutPrintf( _T( "\n分支名称name = e%d\t始节点:%d\t末节点:%d\n文字标注位置(x=%.3f, y=%.3f)\n" ), ei->id, ei->sn, ei->tn, ei->tx, ei->ty );
    int n = ei->cnx.size();
    for( int i = 0; i < n; i++ )
    {
        acutPrintf( _T( "\t控制点:%d\t (x=%.3f, y=%.3f)\n" ), i + 1, ei->cnx[i], ei->cny[i] );
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

    // 1) 构建网络
    //acutPrintf(_T("\n1) 创建网络"));
    Digraph dg;
    NodeIdMap nid( dg ); // 记录实际的节点编号
    ArcIdMap eid( dg ); // 记录实际的分支编号
    BuildNetwork( jd, td, dg, nid, eid );

    // 2) 读取网络图绘制参数(保存在词典VNG_PARAM_DICT中)
    //acutPrintf(_T("\n2) 读取参数"));
    GraphParam gp;
    NodeParam np;
    EdgeParam ep;
    VNG_ParamHelper::ReadGraphParams( gp );
    VNG_ParamHelper::ReadNodeParams( np );
    VNG_ParamHelper::ReadEdgeParams( ep );

    // 3) 调用VNGGen.dll生成网络图，计算几何信息
    //acutPrintf(_T("\n3) 调用VNGGen.dll生成网络图\n"));
    VNGParam param;
    MakeVNGParam( gp, np, ep, param ); // 构造绘制参数对象

    GraphInfo gi;                     // 图的几何信息
    NodeInfoArray nis;                // 节点几何信息
    EdgeInfoArray eis;                // 分支几何信息

    bool ret = VNGGen( dg, param, gi, nis, eis ); // 生成网络图(计算几何信息)
    if( ret )
    {
        //acutPrintf(_T("\n生成网络图成功!"));
        //PrintGraphInfo(gi);
        //PrintNodeInfo(nis);
        //PrintEdgeInfo(eis);

        // 恢复节点和分支的实际编号
        RestoreRealNum( dg, nid, eid, nis, eis );

        //绘制网络图
        DrawVentNetworkGraph(
            basePt,
            gi, nis, eis,
            gp, np, ep,
            jd, td, gd, fd );

        // 删除内存
        ClearNodeInfoArray( nis );
        ClearEdgeInfoArray( eis );
    }

    // 删除所有数据的内存
    ClearAllDatas( jd, td, gd, fd );

    return ret;
}
