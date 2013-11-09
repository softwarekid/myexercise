#include "StdAfx.h"

#include "GeoDef.h"
#include "DoubleLine.h"

/* ȫ�ֺ���(Tool.cpp) */
extern void FindAllLines( AcDbObjectIdArray& objIds );
extern void BuildWSRelation( const AcDbVoidPtrArray& lines, AcDbIntArray& ws_relation );
extern void BuildGoafRelation( const AcDbVoidPtrArray& lines, AcDbIntArray& goaf_relation );
extern void RemoveUnrelatedLines( AcDbObjectIdArray& objIds,
                                  AcDbVoidPtrArray& lines,
                                  const AcDbIntArray& ws_relation,
                                  const AcDbIntArray& goaf_relation );
extern void FindPointSourceOnGoaf( AcDbObjectIdArray& objIds, AcDbVoidPtrArray& lines,
                                   const AcDbIntArray& ws_relation, const AcDbIntArray& goaf_relation,
                                   AcDbObjectIdArray& ob_objIds, AcGePoint3dArray& ob_pts, AcDbIntArray& ob_types );

extern void TruncateLines( const AcDbVoidPtrArray& lines, double trlen,
                           const AcDbIntArray& ws_relation,
                           const AcDbIntArray& goaf_relation );

/* ȫ�ֺ���(CC.cpp) */
extern int CC( const AcDbObjectIdArray& objIds, AcDbIntArray& ccIds );

/* ȫ�ֺ���(BuildLineArray.cpp) */
extern void BuildLineArray( const AcDbObjectIdArray& objIds, AcDbVoidPtrArray& lines );

/* ȫ�ֺ���(EdgeJunctionClosure.cpp) */
extern void EdgeJunctionClourse( const AcDbVoidPtrArray& lines );

/* ȫ�ֺ���(GeoTool.cpp) */
extern int ClockWise( const AcGePoint3dArray& polygon );
extern double LinePerpendicularAngle( const AcGePoint3d& spt, const AcGePoint3d& ept, bool isClockWise );

static void PrintLine( DoubleLine* pLine )
{
    AcGePoint3d ls, le, rs, re;
    pLine->getStartPoints( ls, rs );
    pLine->getEndPoints( le, re );

    acutPrintf( _T( "���: (%.3f, %.3f) -> (%.3f, %.3f)\n" ), ls.x, ls.y, le.x, le.y );
    acutPrintf( _T( "�Ҳ�: (%.3f, %.3f) -> (%.3f, %.3f)\n" ), rs.x, rs.y, re.x, re.y );
}

void PrintLines( const AcDbVoidPtrArray& lines )
{
    acutPrintf( _T( "ֱ�߸���:%d\n" ), lines.length() );

    for( int i = 0; i < lines.length(); i++ )
    {
        DoubleLine* pLine = ( DoubleLine* )lines[i];
        acutPrintf( _T( "��%d��ֱ��:\n" ), i );
        PrintLine( pLine );
        acutPrintf( _T( "\n" ) );
    }
}

void PrintObPoints( const AcGePoint3dArray& ob_pts )
{
    acutPrintf( _T( "\n�ܱյ�Դ����:%d" ), ob_pts.length() );
    int n = ob_pts.length();
    for( int i = 0; i < n; i++ )
    {
        acutPrintf( _T( "\n(%.3f, %.3f)" ), ob_pts[i].x, ob_pts[i].y );
    }
}

void GetNodePoints( const AcDbVoidPtrArray& lines, AcGePoint3dArray& pts )
{
    for( int i = 0; i < lines.length(); i++ )
    {
        DoubleLine* pLine = ( DoubleLine* )lines[i];
        AcGePoint3d startPt, endPt;
        pLine->getSEPoint( startPt, endPt );
        if( !pts.contains( startPt ) )
        {
            pts.append( startPt );
        }
        if( !pts.contains( endPt ) )
        {
            pts.append( endPt );
        }
    }
}

void FindLinesByPoint( const AcDbVoidPtrArray& lines, const AcGePoint3d& junctionPt, AcDbIntArray& linePos )
{
    int n = lines.length();
    for( int i = 0; i < n; i++ )
    {
        DoubleLine* pEdge = ( DoubleLine* )lines[i];
        AcGePoint3d startPt, endPt;
        pEdge->getSEPoint( startPt, endPt );
        if( startPt == junctionPt || endPt == junctionPt )
        {
            linePos.append( i );
        }
    }
}

static void RemoveNullLines( AcDbObjectIdArray& objIds, AcDbVoidPtrArray& lines )
{
    AcDbObjectIdArray all_objIds;
    all_objIds.append( objIds );
    objIds.removeAll();

    AcDbVoidPtrArray all_lines;
    all_lines.append( lines );
    lines.removeAll();

    int n = all_lines.length();
    for( int i = 0; i < n; i++ )
    {
        if( all_lines[i] != 0 && !all_objIds[i].isNull() )
        {
            lines.append( all_lines[i] );
            objIds.append( all_objIds[i] );
        }
    }
}

// ������,��ӡ��Ч��ֱ�߸���
static int CountValidLines( const AcDbVoidPtrArray& lines )
{
    int count = 0;
    int n = lines.length();
    for( int i = 0; i < n; i++ )
    {
        if( lines[i] != 0 ) count++;
    }
    return count;
}

void CreateLineArray( AcDbObjectIdArray& objIds, AcDbIntArray& ccIds, AcDbVoidPtrArray& lines, double truncLen,
                      AcDbObjectIdArray& ob_objIds, AcGePoint3dArray& ob_pts, AcDbIntArray& ob_types )
{
    // 1) �������е�ֱ��ͼԪ(�����������)
    FindAllLines( objIds );

    // 2) ����ֱ������(DoubleLineArray)
    BuildLineArray( objIds, lines );
    acutPrintf( _T( "\nֱ���������:%d" ), lines.length() );

    PrintLines( lines );

    // 3) ����ֱ���빤����Ĺ�ϵ
    // ����ֱ���빤����Ĺ�ϵ
    //	0) �빤���治����
    //	1) һ�����빤�������
    //		+1/-1�ֱ��ʾʼ���ĩ���빤�������
    //	2) ���㶼�ڹ�������
    AcDbIntArray ws_relation;
    BuildWSRelation( lines, ws_relation );

    // 4) ����ֱ����ɿ����Ĺ�ϵ
    // ֱ����ɿ����Ĺ�ϵ
    //	0) �����˵㲻�ڲɿ�����
    //	1) һ���˵��ڲɿ����ϣ�����һ���ڲɿ�����
    //     ע��+1/-1�ֱ�ʼ��spt��ĩ��ept�ڲɿ�����
    //	2) �����˵��ڲɿ�����һ������
    //     ע��+2/-2�ֱ��ʾ�����˵���һ�����ϻ�����������
    AcDbIntArray goaf_relation;
    BuildGoafRelation( lines, goaf_relation );

    assert( ws_relation.length() == lines.length() );
    assert( ws_relation.length() == goaf_relation.length() );

    // 3) �ų��빤����Ͳɿ���û�й��������
    RemoveUnrelatedLines( objIds, lines, ws_relation, goaf_relation );
    acutPrintf( _T( "\n�ų��޹������ĸ���:%d" ), CountValidLines( lines ) );

    // 4) ���ҵ�Դ
    FindPointSourceOnGoaf( objIds, lines, ws_relation, goaf_relation, ob_objIds, ob_pts, ob_types );
    acutPrintf( _T( "\n���ҵ�Դ����������:%d, ��Դ����:%d" ), CountValidLines( lines ), ob_pts.length() );

    // 5) �ضϹ�����֮ǰ�����
    TruncateLines( lines, truncLen, ws_relation, goaf_relation );
    acutPrintf( _T( "\n�ض������ĸ���: %d" ), CountValidLines( lines ) );

    // 6) ���ָ��Ϊnull��DoubleLineָ��
    RemoveNullLines( objIds, lines );
    acutPrintf( _T( "\n���nullָ���ĸ���:%d" ), lines.length() );

    // 7) �����պ�
    EdgeJunctionClourse( lines );

    // 8) ������ͨ��
    CC( objIds, ccIds );
    acutPrintf( _T( "\n��ͨ������ĸ���:%d" ), ccIds.length() );
}

void DeleteLineArray( const AcDbVoidPtrArray& lines )
{
    for( int i = 0; i < lines.length(); i++ )
    {
        DoubleLine* pLine = ( DoubleLine* )lines[i];
        delete pLine;
    }
}

int FindLineByPoints( const AcDbVoidPtrArray& lines, const AcGePoint3d& spt, const AcGePoint3d& ept )
{
    int pos = -1;
    for( int i = 0; i < lines.length(); i++ )
    {
        // ֱ�ߵ�����
        DoubleLine* pLine = ( DoubleLine* )lines[i];
        AcGePoint3d startPt, endPt;
        pLine->getSEPoint( startPt, endPt );

        // ����arx����ֱ��
        //AcGeLine3d ge_line(startPt, endPt);
        //if(ge_line.isOn(spt) && ge_line.isOn(ept))
        if( ( spt == startPt && ept == endPt ) ||
                ( spt == endPt && ept == startPt ) )
        {
            pos = i;
            break;
        }
    }
    return pos;
}

// ����˵����
//		1) һ��˫��(DoubleLine)
//		2) �����polygon�ϵ�һ����(spt --> ept)
//		3) ����εķ���isClockWise(��ʱ��/˳ʱ��)
//		4) ��չ����isInner(�������ڲ����ⲿ����)
// ������:
//		1) ����˫������ı�(left/right),�޸�spt��ept
static void ExtendLine( DoubleLine* pLine, AcGePoint3d& spt, AcGePoint3d& ept, bool isClockWise, bool isInner )
{
    AcGePoint3d startPt, endPt;
    pLine->getSEPoint( startPt, endPt );

    AcGePoint3d ls, rs, le, re;
    pLine->getStartPoints( ls, rs );
    pLine->getEndPoints( le, re );

    AcGeVector3d v1 = endPt - startPt;
    AcGeVector3d v2 = ept - spt;

    // spt->ept��start->endPt�Ƿ�����ͬ
    if( v1.dotProduct( v2 ) > 0 )
    {
        if( ( !isClockWise && isInner ) || ( isClockWise && !isInner ) )
        {
            spt = rs;
            ept = re;
        }
        else
        {
            spt = ls;
            ept = le;
        }
    }
    else
    {
        if( ( isClockWise && isInner ) || ( !isClockWise && !isInner ) )
        {
            spt = re;
            ept = rs;
        }
        else
        {
            spt = le;
            ept = ls;
        }
    }
}

static void ExpandLinesOnPolygon_Helper(
    const AcDbVoidPtrArray& lines,
    const AcGePoint3dArray& polygon,
    AcGePoint3dArray& spts,
    AcGePoint3dArray& epts,
    AcGeDoubleArray& dirs,
    AcDbIntArray& modifyPos,
    bool isInner )
{
    // �ж϶���η���
    // ClockWise()����-1��ʾ��ʱ��
    bool isClockWise = ( ClockWise( polygon ) == -1 );

    // ���Ҳɿ���������еĻزɹ�����ֱ��
    // �����ض�����еı�
    int n = polygon.length();
    for( int i = 0; i < n; i++ )
    {
        AcGePoint3d spt = polygon[i], ept = polygon[( i + 1 ) % n];
        int pos = FindLineByPoints( lines, spt, ept );
        if( pos > -1 )
        {
            ExtendLine( ( DoubleLine* )lines[pos], spt, ept, isClockWise, isInner );
            spts.append( spt );
            epts.append( ept );
            dirs.append( LinePerpendicularAngle( spt, ept, isClockWise ) );
            modifyPos.append( i ); // ��¼����������߱��޸���???
        }
    }
}

void ExpandLinesOnPolygon( const AcDbVoidPtrArray& lines,
                           const AcGePoint3dArray& polygon,
                           AcGePoint3dArray& spts,
                           AcGePoint3dArray& epts,
                           AcGeDoubleArray& dirs,
                           bool isInner )
{
    AcDbIntArray modifyPos;
    ExpandLinesOnPolygon_Helper( lines, polygon, spts, epts, dirs, modifyPos, isInner );
}

// ���������,�޸ĵ�����
void AdjustPointPolygon( const AcDbVoidPtrArray& lines, const AcGePoint3dArray& polygon, AcGePoint3dArray& ext_polygon, bool isInner )
{
    // ���Ҷ�����е����п�����Ҫ�޸ĵı�(�����)
    AcGePoint3dArray spts, epts;
    AcGeDoubleArray dirs;
    AcDbIntArray modifyPos;
    ExpandLinesOnPolygon_Helper( lines, polygon, spts, epts, dirs, modifyPos, isInner ); // ��������

    // ��������ת���ɱ߶����
    int m = polygon.length();
    ext_polygon.append( polygon );

    // �޸Ķ����
    int n = modifyPos.length();
    for( int i = 0; i < n; i++ )
    {
        int pos = modifyPos[i];
        acutPrintf( _T( "\n�ɿ�����%d���߱��޸�" ), pos );
        ext_polygon[pos] = spts[i];
        ext_polygon[( pos + 1 ) % m] = epts[i];
    }
}

// ���������,�޸ĵ�����
void AdjustLinePolygon( const AcDbVoidPtrArray& lines, const AcGePoint3dArray& polygon, AcGePoint3dArray& line_polygon, bool isInner )
{
    // ���Ҷ�����е����п�����Ҫ�޸ĵı�(�����)
    AcGePoint3dArray spts, epts;
    AcGeDoubleArray dirs;
    AcDbIntArray modifyPos;
    ExpandLinesOnPolygon_Helper( lines, polygon, spts, epts, dirs, modifyPos, isInner ); // ��������

    // ��������ת���ɱ߶����
    int m = polygon.length();
    for( int i = 0; i < m; i++ )
    {
        line_polygon.append( polygon[i] );
        line_polygon.append( polygon[( i + 1 ) % m] );
    }

    // �޸Ķ����
    int n = modifyPos.length();
    for( int i = 0; i < n; i++ )
    {
        int pos = modifyPos[i];
        line_polygon[2 * pos] = spts[i];
        line_polygon[2 * pos + 1] = epts[i];
    }
}

void FilterLines( const AcDbVoidPtrArray& all_lines, AcDbVoidPtrArray& lines, bool findWS )
{
    for( int i = 0; i < all_lines.length(); i++ )
    {
        DoubleLine* pLine = ( DoubleLine* )all_lines[i];
        bool isWS = pLine->isWorkSurface();
        if( ( isWS && findWS ) || ( !isWS && !findWS ) )
        {
            lines.append( all_lines[i] );
        }
    }
}

// �������˫���ϵ�ͶӰ
// ���ߵ㡢���㡢�Ҳ��
void ProjectPointOnLine( const AcGePoint3d& pt, DoubleLine* pLine,
                         AcGePoint3d& cpt, AcGePoint3d& lpt, AcGePoint3d& rpt )
{
    AcGePoint3d spt, ept;
    pLine->getSEPoint( spt, ept );

    AcGePoint3d ls, le, rs, re;
    pLine->getStartPoints( ls, rs );
    pLine->getEndPoints( le, re );

    // ��ȡ���������
    AcGeLineSeg3d line( spt, ept );
    cpt = line.closestPointTo( pt );

    // ��ȡ"���"�����
    line.set( ls, le );
    lpt = line.closestPointTo( pt );

    // ��ȡ"�Ҳ�"�����
    line.set( rs, re );
    rpt = line.closestPointTo( pt );
}
