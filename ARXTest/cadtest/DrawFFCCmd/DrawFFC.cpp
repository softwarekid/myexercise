#include "stdafx.h"
#include "DrawCmd.h"

#include "../MineGE/HelperClass.h"
#include "../ArxHelper/HelperClass.h"

// ͨ��ͼԪ
#include "../FFC_GE/Goaf.h"
#include "../FFC_GE/GasPipe.h"
#include "../FFC_GE/AirLeak.h"
#include "../FFC_GE/SandWall.h"
#include "../FFC_GE/Obturation.h"
#include "../FFC_GE/NitrogenPipe.h"

#include "PolyLineJig.h"

/* ȫ�ֺ���(ʵ����PromptTool.cpp) */
extern bool PromptSEPt( const CString& name, AcGePoint3d& startPt, AcGePoint3d& endPt, double& angle );
extern bool GetClosePtAndAngle( const AcDbObjectId& objId, AcGePoint3d& pt, double& angle );
extern bool PromptInsertPt( const AcDbObjectId& objId, AcGePoint3d& pt );

static void CreateObturation( const AcDbObjectId& host, const AcGePoint3d& pt, double angle )
{
    Obturation* pObturation = new Obturation( pt, angle );
    pObturation->setRelatedGE( host ); // ����ͼԪ

    // ��ʼ�����ύ�����ݿ�
    if( !ArxUtilHelper::PostToModelSpace( pObturation ) ) delete pObturation;
}

static void CreateSandWall( const AcGePoint3d& pt, double angle, double length )
{
    SandWall* pWall = new SandWall( pt, angle, length );

    // ��ʼ�����ύ�����ݿ�
    if( !ArxUtilHelper::PostToModelSpace( pWall ) ) delete pWall;
}

static void CreateGoaf( const AcGePoint3dArray& polygon )
{
    Goaf* pGoaf = new Goaf();
    for( int i = 0; i < polygon.length(); i++ )
    {
        pGoaf->addPoint( polygon[i] );
    }
    // ��ʼ�����ύ�����ݿ�
    if( !ArxUtilHelper::PostToModelSpace( pGoaf ) ) delete pGoaf;
}

static void CreateNitrogenPipe( const AcGePoint3d& pt )
{
    NitrogenPipe* pNP = new NitrogenPipe( pt );
    if( !ArxUtilHelper::PostToModelSpace( pNP ) ) delete pNP;
}

static void CreateAirLeak( const AcGePoint3d& pt, bool st )
{
    AirLeak* pAirLeak = 0;
    if( st )
    {
        pAirLeak = new SourceAirLeak( pt );
    }
    else
    {
        pAirLeak = new SinkAirLeak( pt );
    }

    if( !ArxUtilHelper::PostToModelSpace( pAirLeak ) ) delete pAirLeak;
}

//void DrawCmd::DrawSandWall()
//{
//	AcDbObjectId objId = ArxUtilHelper::SelectObject(_T("��ѡ��һ�����:"));
//	if(objId.isNull()) return;
//	if(!ArxUtilHelper::IsEqualType(_T("LinkedGE"), objId)) return;
//
//	AcGePoint3d pt;
//	if(!PromptInsertPt(objId, pt)) return;
//
//	double angle;
//	if(!GetClosePtAndAngle(objId, pt, angle)) return;
//
//	ads_real length;
//	if(RTNORM != acedGetDist(asDblArray(pt), _T("\n��ָ����ǽ�ĳ���:"), &length)) return;
//
//	CreateSandWall(objId, pt, angle, length);
//}
//
void DrawCmd::DrawSandWall()
{
    AcGePoint3d pt;
    if( !ArxUtilHelper::PromptPt( _T( "\n��ָ�����������:" ), pt ) ) return;

    //double angle;
    //if(RTNORM != acedGetAngle(asDblArray(pt), _T("\n��ָ������Ƕ�: "), &angle)) return;

    double length;
    if( RTNORM != acedGetDist( asDblArray( pt ), _T( "\n�������ǽһ�볤��[�����ָ��]:" ), &length ) ) return;

    //CreateSandWall(pt, angle, length);
    CreateSandWall( pt, 0, length );
}

void DrawCmd::DrawObturation()
{
    AcDbObjectId objId = ArxUtilHelper::SelectObject( _T( "��ѡ��һ�������" ) );
    if( objId.isNull() ) return;
    if( !ArxUtilHelper::IsEqualType( _T( "LinkedGE" ), objId ) ) return;

    AcGePoint3d pt;
    if( !PromptInsertPt( objId, pt ) ) return;

    double angle;
    if( !GetClosePtAndAngle( objId, pt, angle ) ) return;

    CreateObturation( objId, pt, angle );
}

static bool GetPolygonByJig( AcGePoint3dArray& polygon )
{
    //PolyLineJig jig;
    PolyLineJig jig;
    return jig.doJig( polygon );
}

void DrawCmd::DrawGoaf()
{
    acutPrintf( _T( "\n��ָ���ɿ����������: " ) );

    // jig��ȡ��ն����
    AcGePoint3dArray polygon;
    if( !GetPolygonByJig( polygon ) ) return;

    if( polygon.length() < 3 )
    {
        acutPrintf( _T( "\n���Ʋɿ���������Ҫָ��3������..." ) );
    }
    else
    {
        // �����ɿ���
        CreateGoaf( polygon );
    }
}

void DrawCmd::DrawNitrogenPipe()
{
    AcGePoint3d pt;
    if( !ArxUtilHelper::PromptPt( _T( "\n��ָ�����������:" ), pt ) ) return;

    CreateNitrogenPipe( pt );
}

void DrawCmd::DrawGasPipe()
{
    AcGePoint3d startPt, endPt;
    double angle;
    if( !PromptSEPt( _T( "��˹���" ), startPt, endPt, angle ) ) return;

    // 3������ͼԪ
    GasPipe* pGasPipe = new GasPipe( startPt, endPt );  // ��˹���

    // 4����ʼ�����ύ�����ݿ�
    if( !ArxUtilHelper::PostToModelSpace( pGasPipe ) ) delete pGasPipe;
}

void DrawCmd::DrawAirLeak( bool st )
{
    AcGePoint3d pt;
    if( !ArxUtilHelper::PromptPt( _T( "\n��ָ�����������:" ), pt ) ) return;

    CreateAirLeak( pt, st );
}

static void GetGoafPolygon( const AcDbObjectId& objId, AcGePoint3dArray& polygon )
{
    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    if( pTrans == 0 ) return;

    AcDbObject* pObj;
    if( Acad::eOk == pTrans->getObject( pObj, objId, AcDb::kForRead ) )
    {
        Goaf* pGoaf = Goaf::cast( pObj );
        if( pGoaf != 0 )
        {
            pGoaf->getPolygon( polygon );
        }
    }

    actrTransactionManager->endTransaction();
}

static double DistOfPointToLine( const AcGePoint3d& spt, const AcGePoint3d& ept, const AcGePoint3d& pt )
{
    AcGeLine3d L( spt, ept );
    return L.distanceTo( pt );
}

static int GetClosePolygonEdge( const AcGePoint3dArray& polygon, const AcGePoint3d& pt )
{
    int pos = 0;
    double minDist = DistOfPointToLine( polygon[0], polygon[1], pt );
    int n = polygon.length();
    for( int i = 1; i < n; i++ )
    {
        double dist = DistOfPointToLine( polygon[i], polygon[( i + 1 ) % n], pt );
        if( minDist > dist )
        {
            minDist = dist;
            pos = i;
        }
    }
    return pos;
}

static void SplitGoafLine( const AcDbObjectId& objId, int pos, const AcGePoint3d& pt )
{
    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    if( pTrans == 0 ) return;

    AcDbObject* pObj;
    if( Acad::eOk == pTrans->getObject( pObj, objId, AcDb::kForWrite ) )
    {
        Goaf* pGoaf = Goaf::cast( pObj );
        if( pGoaf != 0 )
        {
            pGoaf->splitLine( pos, pt );
        }
    }

    actrTransactionManager->endTransaction();
}

void DrawCmd::SplitGoaf()
{
    AcDbObjectId objId = ArxUtilHelper::SelectObject( _T( "��ѡ��һ���ɿ���:" ) );
    if( !ArxUtilHelper::IsEqualType( _T( "Goaf" ), objId ) ) return;

    // �ɿ��������
    AcGePoint3dArray polygon;
    GetGoafPolygon( objId, polygon );
    if( polygon.length() < 3 ) return;


    AcGePoint3d pt;
    ArxUtilHelper::PromptPt( _T( "��ָ���ָ������: [����(D)]" ), pt );

    // ���Ҿ�������ı�
    int pos = GetClosePolygonEdge( polygon, pt );
    //acutPrintf(_T("\n�����λ��:%d"), pos);
    SplitGoafLine( objId, pos, pt );
}

static int FindBoundaryPoint( const AcGePoint3dArray& polyon, const AcGePoint3d& pt )
{
    int pos = -1;

    int n = polyon.length();
    for( int i = 0; i < n; i++ )
    {
        if( pt == polyon[i] )
        {
            pos = i;
            break;
        }
    }
    return pos;
}

static void RemoveGoafBoundaryPoint( const AcDbObjectId& objId, int pos )
{
    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    if( pTrans == 0 ) return;

    AcDbObject* pObj;
    if( Acad::eOk == pTrans->getObject( pObj, objId, AcDb::kForWrite ) )
    {
        Goaf* pGoaf = Goaf::cast( pObj );
        if( pGoaf != 0 )
        {
            pGoaf->removePoint( pos );
        }
    }

    actrTransactionManager->endTransaction();
}

void DrawCmd::RemoveGoafPoint()
{
    AcDbObjectId objId = ArxUtilHelper::SelectObject( _T( "��ѡ��һ���ɿ���:" ) );
    if( !ArxUtilHelper::IsEqualType( _T( "Goaf" ), objId ) ) return;

    // �ɿ��������
    AcGePoint3dArray polygon;
    GetGoafPolygon( objId, polygon );
    if( polygon.length() < 3 ) return;

    AcGePoint3d pt;
    ArxUtilHelper::PromptPt( _T( "��ָ��Ҫɾ���Ĳɿ����߽��: " ), pt );

    int pos = FindBoundaryPoint( polygon, pt );
    if( pos != -1 )
    {
        RemoveGoafBoundaryPoint( objId, pos );
    }
}