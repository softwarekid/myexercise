#include "StdAfx.h"

#include "DoubleLine.h"

/* ȫ�ֺ���(DoubleLineHelper.cpp) */
extern void GetNodePoints( const AcDbVoidPtrArray& lines, AcGePoint3dArray& pts );
extern void FindLinesByPoint( const AcDbVoidPtrArray& lines, const AcGePoint3d& junctionPt, AcDbIntArray& linePos );

/* ȫ�ֺ���(GeoTool.cpp) */
extern int FindEdgeByTwoPoint( const AcGePoint3dArray& vertices, const AcDbIntArray& edges, const AcGePoint3d& spt, const AcGePoint3d& ept );

// ��������ֻ����һ��ֱ�ߵĵ������Ӧ�ķ�֧�Լ�����
void FindInletBoundary( const AcDbObjectIdArray& objIds,
                        const AcDbVoidPtrArray& lines,
                        AcGePoint3dArray& inlet_spts,
                        AcGePoint3dArray& inlet_epts,
                        AcGeDoubleArray& inlet_dirs,
                        AcDbObjectIdArray& inlet_objIds )
{
    // �������еĵ�
    AcGePoint3dArray pts;
    GetNodePoints( lines, pts );
    if( pts.isEmpty() ) return;

    for( int i = 0; i < pts.length(); i++ )
    {
        // ���ҵ������˫��
        AcDbIntArray linePos;
        FindLinesByPoint( lines, pts[i], linePos );

        if( linePos.length() != 1 ) continue;

        // ��ȡ˫�ߵ�ʼĩ������
        int pos = linePos[0];
        DoubleLine* pLine = ( DoubleLine* )lines[pos];
        AcGePoint3d spt, ept;
        pLine->getSEPoint( spt, ept );

        // �жϵ�ǰ����ʼ�㻹��ĩ��
        AcGePoint3d line_spt, line_ept;
        if( spt == pts[i] )
        {
            pLine->getStartPoints( line_spt, line_ept );
        }
        else
        {
            pLine->getEndPoints( line_spt, line_ept );
        }

        inlet_spts.append( line_spt );
        inlet_epts.append( line_ept );
        inlet_dirs.append( pLine->getAngle() );
        inlet_objIds.append( objIds[pos] );
    }
}

void PrintInletBoundary( const AcGePoint3dArray& inlet_spts,
                         const AcGePoint3dArray& inlet_epts,
                         const AcGeDoubleArray& inlet_dirs )
{
    acutPrintf( _T( "��ӡͨ��߽�: \n" ) );
    int n = inlet_spts.length();
    for( int i = 0; i < n; i++ )
    {
        AcGePoint3d spt = inlet_spts[i], ept = inlet_epts[i];
        double dir = inlet_dirs[i];
        acutPrintf( _T( "  (%.3f, %.3f) --> (%.3f, %.3f) ����: %.3f\n" ), spt.x, spt.y, ept.x, ept.y, dir );
    }
    acutPrintf( _T( "\n" ) );
}