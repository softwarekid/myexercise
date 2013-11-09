#include "StdAfx.h"

// �ڵ�
#define VERTICES_NUM_VAR _T("vertices_num")
#define VERTICES_ARRAY_VAR _T("vertices")

// ��֧
#define EDGES_NUM_VAR _T("edges_num")
#define EDGES_ARRAY_VAR _T("edges")

// ��
#define FACES_NUM_VAR _T("total_edges_faces")
#define FACES_INFO_NUM_VAR _T("faces_num")
#define FACES_ARRAY_VAR _T("edges_faces")
#define FACES_INFO_ARRAY_VAR _T("num_edges_faces")

// ��ǽ
#define SPLIT_FACES_NUM_VAR _T("total_split_faces")
#define SPLIT_FACE_EDGES_NUM_VAR _T("total_edges_split_face")
#define SPLIT_FACES_ARRAY_VAR _T("id_faces_split")
#define SPLIT_FACES_INFO_ARRAY_VAR _T("length_edges_faces_split")
#define SPLIT_FACE_EDGES_ARRAY_VAR _T("id_edges_faces_split")

// ͨ��߽�
#define INLET_BOUNDARY_NUM_VAR _T("num_edges_ventilation")
#define INLET_BOUNDARY_ARRAY_VAR _T("edges_ventilation")

// ��˹�߽�
#define GAS_BOUNDARY_NUM_VAR _T("num_edges_gas")
#define GAS_BOUNDARY_ARRAY_VAR _T("edges_gas")

// ��������ɿ��������ѹ����
#define PRESS_FACILITY_NUM_VAR _T("num_droppressure_between_wg")
#define PRESS_FACILITY_ARRAY_VAR _T("droppressure_between_wg")

// ��ǽ��ѹ
#define WALL_FACILITY_NUM_VAR _T("num_droppressure_in_goaf")
#define WALL_FACILITY_ARRAY_VAR _T("droppressure_in_goaf")

// �ɿ�����ս���
#define GOAF_NUM_VAR _T("num_goaf")
#define GOAF_ARRAY_VAR _T("goafs")
#define GOAF_AREA_ARRAY_VAR _T("goafs_area")

extern double FacePolygonArea( const AcGePoint3dArray& vertices, const AcDbIntArray& edges,
                               const AcDbIntArray& faces, const AcDbIntArray& faces_info,
                               int k );

static CString DeclareComment( const CString& comment )
{
    CString str;
    str.Format( _T( "/%s\n" ), comment );
    return str;
}

static CString DeclareVar( const CString& name, int value )
{
    CString str;
    str.Format( _T( "$%s = %d\n" ), name, value );
    return str;
}

static CString DeclareArray( const CString& name, const CString& num_var, int c = 1 )
{
    CString str;
    if( c == 1 )
    {
        str.Format( _T( "declare $%s[1:($%s)]\n" ), name, num_var );
    }
    else
    {
        str.Format( _T( "declare $%s[1:(%d*$%s)]\n" ), name, c, num_var );
    }

    return str;
}

static void WriteVertices( CStdioFile& outfile, const AcGePoint3dArray& vertices )
{
    // ����ڵ��������
    outfile.WriteString( DeclareVar( VERTICES_NUM_VAR, vertices.length() ) );

    if( !vertices.isEmpty() )
    {
        // ����ڵ�����
        outfile.WriteString( DeclareArray( VERTICES_ARRAY_VAR, VERTICES_NUM_VAR, 3 ) );
    }

    // �ڵ����鸳ֵ
    CString str;
    for( int i = 0; i < vertices.length(); i++ )
    {
        str.Format( _T( "$%s[%d] = %.3f\n" ), VERTICES_ARRAY_VAR, 3 * i + 1, vertices[i].x );
        str.AppendFormat( _T( "$%s[%d] = %.3f\n" ), VERTICES_ARRAY_VAR, 3 * i + 2, vertices[i].y );
        str.AppendFormat( _T( "$%s[%d] = %.3f\n" ), VERTICES_ARRAY_VAR, 3 * i + 3, vertices[i].z );
        outfile.WriteString( str );
    }
}

static void WriteEdges( CStdioFile& outfile, const AcDbIntArray& edges )
{
    // �����֧��������(2��)
    outfile.WriteString( DeclareVar( EDGES_NUM_VAR, edges.length() / 2 ) );

    if( !edges.isEmpty() )
    {
        // �����֧����
        outfile.WriteString( DeclareArray( EDGES_ARRAY_VAR, EDGES_NUM_VAR, 2 ) );
    }

    // ��֧���鸳ֵ
    CString str;
    for( int i = 0; i < edges.length() / 2; i++ )
    {
        str.Format( _T( "$%s[%d] = %d\n" ), EDGES_ARRAY_VAR, 2 * i + 1, edges[2 * i] );
        str.AppendFormat( _T( "$%s[%d] = %d\n" ), EDGES_ARRAY_VAR, 2 * i + 2, edges[2 * i + 1] );
        outfile.WriteString( str );
    }
}

static void WriteFaces( CStdioFile& outfile, const AcDbIntArray& faces, const AcDbIntArray& faces_info )
{
    // �������������
    outfile.WriteString( DeclareVar( FACES_NUM_VAR, faces.length() ) );

    if( !faces.isEmpty() )
    {
        // �������֧����
        outfile.WriteString( DeclareArray( FACES_ARRAY_VAR, FACES_NUM_VAR, 1 ) );
    }

    // ��������Ϣ����
    outfile.WriteString( DeclareVar( FACES_INFO_NUM_VAR, faces_info.length() ) );

    if( !faces_info.isEmpty() )
    {
        // ��������Ϣ����
        outfile.WriteString( DeclareArray( FACES_INFO_ARRAY_VAR, FACES_INFO_NUM_VAR, 1 ) );
    }

    // �����鸳ֵ
    CString str;
    for( int i = 0; i < faces.length(); i++ )
    {
        str.Format( _T( "$%s[%d] = %d\n" ), FACES_ARRAY_VAR, i + 1, faces[i] );
        outfile.WriteString( str );
    }

    // ����Ϣ���鸳ֵ
    for( int i = 0; i < faces_info.length(); i++ )
    {
        str.Format( _T( "$%s[%d] = %d\n" ), FACES_INFO_ARRAY_VAR, i + 1, faces_info[i] );
        outfile.WriteString( str );
    }
}

static void WriteSplitFaces( CStdioFile& outfile, const AcDbIntArray& split_faces, const AcDbIntArray& split_faces_info, const AcDbIntArray& split_face_edges )
{
    //assert(split_faces_info.length() == split_faces.length());

    // ����ָ������
    outfile.WriteString( DeclareVar( SPLIT_FACES_NUM_VAR, split_faces.length() ) );

    // ����ָ����е����б߸���
    outfile.WriteString( DeclareVar( SPLIT_FACE_EDGES_NUM_VAR, split_face_edges.length() ) );

    if( !split_faces.isEmpty() )
    {
        // ����ָ�������
        outfile.WriteString( DeclareArray( SPLIT_FACES_ARRAY_VAR, SPLIT_FACES_NUM_VAR, 1 ) );
        // ����ָ����е����б�����
        outfile.WriteString( DeclareArray( SPLIT_FACES_INFO_ARRAY_VAR, SPLIT_FACES_NUM_VAR, 1 ) );
    }

    if( !split_face_edges.isEmpty() )
    {
        // ����ָ����еķ�֧��������
        outfile.WriteString( DeclareArray( SPLIT_FACE_EDGES_ARRAY_VAR, SPLIT_FACE_EDGES_NUM_VAR, 1 ) );
    }

    // �ָ������鸴��
    CString str;
    for( int i = 0; i < split_faces.length(); i++ )
    {
        str.Format( _T( "$%s[%d] = %d\n" ), SPLIT_FACES_ARRAY_VAR, i + 1, split_faces[i] );
        outfile.WriteString( str );
    }

    // �ָ���������鸴��
    for( int i = 0; i < split_faces_info.length(); i++ )
    {
        str.Format( _T( "$%s[%d] = %d\n" ), SPLIT_FACES_INFO_ARRAY_VAR, i + 1, split_faces_info[i] );
        outfile.WriteString( str );
    }

    // �ָ�������鸴��
    for( int i = 0; i < split_face_edges.length(); i++ )
    {
        str.Format( _T( "$%s[%d] = %d\n" ), SPLIT_FACE_EDGES_ARRAY_VAR, i + 1, split_face_edges[i] );
        outfile.WriteString( str );
    }
}

static void WriteInletBoundary( CStdioFile& outfile, const AcDbIntArray& inlet_edges )
{
    // ����ͨ��߽��������
    outfile.WriteString( DeclareVar( INLET_BOUNDARY_NUM_VAR, inlet_edges.length() ) );

    if( !inlet_edges.isEmpty() )
    {
        // ����ͨ��߽��֧����
        outfile.WriteString( DeclareArray( INLET_BOUNDARY_ARRAY_VAR, INLET_BOUNDARY_NUM_VAR, 1 ) );
    }

    // ͨ��߽��֧���鸳ֵ
    CString str;
    for( int i = 0; i < inlet_edges.length(); i++ )
    {
        str.Format( _T( "$%s[%d] = %d\n" ), INLET_BOUNDARY_ARRAY_VAR, i + 1, inlet_edges[i] );
        outfile.WriteString( str );
    }
}

static void WriteGasBoundary( CStdioFile& outfile, const AcDbIntArray& gas_edges )
{
    // ������˹�߽��������
    outfile.WriteString( DeclareVar( GAS_BOUNDARY_NUM_VAR, gas_edges.length() ) );

    if( !gas_edges.isEmpty() )
    {
        // ������˹�߽��֧����
        outfile.WriteString( DeclareArray( GAS_BOUNDARY_ARRAY_VAR, GAS_BOUNDARY_NUM_VAR, 1 ) );
    }

    // ͨ����˹��֧���鸳ֵ
    CString str;
    for( int i = 0; i < gas_edges.length(); i++ )
    {
        str.Format( _T( "$%s[%d] = %d\n" ), GAS_BOUNDARY_ARRAY_VAR, i + 1, gas_edges[i] );
        outfile.WriteString( str );
    }
}

static void WritePressFacility( CStdioFile& outfile, const AcDbIntArray& press_edges )
{
    // ���幤������ɿ��������ѹ���ø�������
    outfile.WriteString( DeclareVar( PRESS_FACILITY_NUM_VAR, press_edges.length() ) );

    if( !press_edges.isEmpty() )
    {
        // ���幤������ɿ��������ѹ���÷�֧����
        outfile.WriteString( DeclareArray( PRESS_FACILITY_ARRAY_VAR, PRESS_FACILITY_NUM_VAR, 1 ) );
    }

    // ��������ɿ��������ѹ���÷�֧���鸳ֵ
    CString str;
    for( int i = 0; i < press_edges.length(); i++ )
    {
        str.Format( _T( "$%s[%d] = %d\n" ), PRESS_FACILITY_ARRAY_VAR, i + 1, press_edges[i] );
        outfile.WriteString( str );
    }
}

static void WriteWallFacility( CStdioFile& outfile,
                               const AcDbIntArray& split_faces,
                               const AcDbIntArray& split_faces_info,
                               const AcDbIntArray& split_face_edges )
{
    // �����ǽ��ѹ���ø�������
    outfile.WriteString( DeclareVar( WALL_FACILITY_NUM_VAR, split_face_edges.length() ) );

    if( !split_face_edges.isEmpty() )
    {
        // �����ǽ��ѹ���÷�֧����
        outfile.WriteString( DeclareArray( WALL_FACILITY_ARRAY_VAR, WALL_FACILITY_NUM_VAR, 1 ) );
    }

    // ��ǽ��ѹ���÷�֧���鸳ֵ
    CString str;
    for( int i = 0; i < split_face_edges.length(); i++ )
    {
        str.Format( _T( "$%s[%d] = %d\n" ), WALL_FACILITY_ARRAY_VAR, i + 1, split_face_edges[i] );
        outfile.WriteString( str );
    }
}

static void WriteGoaf( CStdioFile& outfile, const AcDbIntArray& goafs, const AcGeDoubleArray& goaf_areas )
{
    // ����ɿ�����������
    outfile.WriteString( DeclareVar( GOAF_NUM_VAR, goafs.length() ) );

    if( !goafs.isEmpty() )
    {
        // ����ɿ�������
        outfile.WriteString( DeclareArray( GOAF_ARRAY_VAR, GOAF_NUM_VAR, 1 ) );
        // �ɿ����������
        outfile.WriteString( DeclareArray( GOAF_AREA_ARRAY_VAR, GOAF_NUM_VAR, 1 ) );
    }

    // �ɿ������鸳ֵ
    CString str;
    for( int i = 0; i < goafs.length(); i++ )
    {
        str.Format( _T( "$%s[%d] = %d\n" ), GOAF_ARRAY_VAR, i + 1, goafs[i] );
        outfile.WriteString( str );
    }

    // �ɿ���������鸳ֵ
    for( int i = 0; i < goafs.length(); i++ )
    {
        str.Format( _T( "$%s[%d] = %.4f\n" ), GOAF_AREA_ARRAY_VAR, i + 1, goaf_areas[i] );
        outfile.WriteString( str );
    }
}

static void NumPlusPlus( AcDbIntArray& ia )
{
    for( int i = 0; i < ia.length(); i++ )
    {
        ia[i] = ia[i] + 1;
    }
}

// ��ʱ�����δ�޸�
static void CaclGoafAreas( const AcGePoint3dArray& vertices, const AcDbIntArray& edges,
                           const AcDbIntArray& faces, const AcDbIntArray& faces_info,
                           const AcDbIntArray& goafs, AcGeDoubleArray& goaf_areas )
{
    for( int i = 0; i < goafs.length(); i++ )
    {
        goaf_areas.append( FacePolygonArea( vertices, edges, faces, faces_info, goafs[i] ) );
    }
}

void WriteJournalFile(
    const CString& filepath,
    /* �������˹�ϵ */
    const AcGePoint3dArray& vertices, const AcDbIntArray& edges,
    const AcDbIntArray& faces, const AcDbIntArray& faces_info,
    /* �ָ��� */
    const AcDbIntArray& split_faces,
    const AcDbIntArray& split_faces_info,
    const AcDbIntArray& split_face_edges,
    /* ͨ��߽� */
    const AcDbIntArray& inlet_edges,
    /* ��˹�߽� */
    const AcDbIntArray& gas_edges,
    /* ��ǽ��ѹ��ʩ */
    const AcDbIntArray& press_edges,
    /* �ɿ�����׽��� */
    const AcDbIntArray& goafs )
{
    // ����ɿ������
    AcGeDoubleArray goaf_areas;
    CaclGoafAreas( vertices, edges, faces, faces_info, goafs, goaf_areas );

    // ��ԭ�еı����ص����鸴��һ��
    // �����޸�ԭʼ����
    AcGeIntArray copy_edges, copy_faces;
    AcGeIntArray copy_split_faces, copy_split_face_edges;
    AcGeIntArray copy_inlet_edges;
    AcGeIntArray copy_gas_edges;
    AcGeIntArray copy_press_edges;
    AcGeIntArray copy_goafs;

    copy_edges.append( edges );
    copy_faces.append( faces );
    copy_split_faces.append( split_faces );
    copy_split_face_edges.append( split_face_edges );
    copy_inlet_edges.append( inlet_edges );
    copy_gas_edges.append( gas_edges );
    copy_press_edges.append( press_edges );
    copy_goafs.append( goafs );

    // ���б��+1
    NumPlusPlus( copy_edges );
    NumPlusPlus( copy_faces );
    NumPlusPlus( copy_split_faces );
    NumPlusPlus( copy_split_face_edges );
    NumPlusPlus( copy_inlet_edges );
    NumPlusPlus( copy_gas_edges );
    NumPlusPlus( copy_press_edges );
    NumPlusPlus( copy_goafs );

    CStdioFile outfile;
    outfile.Open( filepath, CFile::modeCreate | CFile::modeWrite );

    WriteVertices( outfile, vertices );
    WriteEdges( outfile, copy_edges );
    WriteFaces( outfile, copy_faces, faces_info );
    WriteSplitFaces( outfile, copy_split_faces, split_faces_info, copy_split_face_edges );
    WriteInletBoundary( outfile, copy_inlet_edges );
    WriteGasBoundary( outfile, copy_gas_edges );
    WritePressFacility( outfile, copy_press_edges );
    WriteWallFacility( outfile, copy_split_faces, split_faces_info, copy_split_face_edges );
    WriteGoaf( outfile, copy_goafs, goaf_areas );

    outfile.Close();
}