#include "GeoAttribs.h"

bool operator==( const DT_Edge& e1, const DT_Edge& e2 )
{
    return ( ( e1.s == e2.s ) && ( e1.t == e2.t ) ) ||
           ( ( e1.s == e2.t ) && ( e1.t == e2.s ) );
}

static int FindEdge( const EdgeAttribArray& eaa, const DT_Edge& e )
{
    int pos = -1;
    for( int i = 0; i < ( int )eaa.size(); i++ )
    {
        if( eaa[i]->e == e )
        {
            pos = i;
            break;
        }
    }
    return pos;
}

// ���õ�i�������εĵ�n����
// �Լ��ñ���������������
static void SetAttrib( EdgeAttribArray& eaa, TriangleAttribArray& taa, int i, int n )
{
    int pos = FindEdge( eaa, taa[i]->getEdgeFromTriangle( n ) );
    if( pos != -1 )
    {
        eaa[pos]->setTriangle( i );
        taa[i]->setEdge( n, pos );
    }
}

void BuildAttribs(
    const EdgeArray& ea,
    const TriangleArray& ta,
    EdgeAttribArray& eaa,
    TriangleAttribArray& taa )
{
    // ��ʼ����֧��������
    for( int i = 0; i < ( int )ea.size(); i++ )
    {
        EdgeAttrib* pEdge = new EdgeAttrib( ea[i] );
        eaa.push_back( pEdge );
    }
    // ��ʼ�������ζ�������
    for( int i = 0; i < ( int )ta.size(); i++ )
    {
        TriangleAttrib* pTriangle = new TriangleAttrib( ta[i] );
        taa.push_back( pTriangle );
    }

    for( int i = 0; i < ( int )ta.size(); i++ )
    {
        // �����������Լ�3���ߵ�����
        SetAttrib( eaa, taa, i, 0 );
        SetAttrib( eaa, taa, i, 1 );
        SetAttrib( eaa, taa, i, 2 );
    }
}