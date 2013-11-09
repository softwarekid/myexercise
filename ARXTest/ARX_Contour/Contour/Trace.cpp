// �ο�
// http://hi.baidu.com/longchengjiang/blog/item/2cdde73fb897cac07d1e713f.html
// ���ڲ�����������_TIN_��׷�ٵ�ֵ���㷨���Ե�ֵ�߹⻬�㷨���о�

#include "Contour.h"
#include "GeoAttribs.h"
#include "GeoAlgo.h"

#include <cmath>
#include <algorithm>
#include <iterator>

#define SCALE_FACTOR 1e-3

// ��z����С����ƫ��
// zValue -- ��ǰҪ׷�ٵ�zֵ
// z - ĳһ�������zֵ
static double TrimZ( double z, double zValue )
{
    return ( z + SCALE_FACTOR * zValue );
}

class Contour
{
public:
    Contour( const PointArray& pts, const EdgeArray& ea, const TriangleArray& ta );
    ~Contour();

    // �����߽�
    void searchBoundary( PointArray& bpts, IntArray& bpos );

    // ׷�ٵ�ֵ��
    void traceContour( double zValue, PointArray& cnpts, IntArray& cnpos, IntArray& dirs );

private:
    // �ֱ���ұ߽������κ��ڲ�������, ���ҵ�ֵ��׷�����
    bool findStartTrianlgeTwice( double z, int& t0, int& e0, DT_Point& p );
    // ���ҵ�1����������Ϊ��ֵ��׷�����
    bool findStartTrianlge( double z, int& t0, int& e0, DT_Point& p, bool useBoundary );
    // �ӵ�i�������εĵ�n���߽���
    // ������һ���������Լ���Ӧ��
    bool findNextTriangle( double z, int t1, int e1, int& t2, int& e2, DT_Point& p );
    // ��i�������Ƿ��е�ֵ��
    bool hasContourPoint( double z, int i );

    // ���õ�i���ߵ�����
    void setEdgePoint( int i, const DT_Point& p1, const DT_Point& p2 );
    // ���ص�i����ʼ������
    void getEdgePoint( int i, DT_Point& p1, DT_Point& p2 ) const;
    // ���ص�i���߶���ָ��
    EdgeAttrib* getEdgeObject( int i ) const;
    // ���ص�i�������ζ���ָ��
    TriangleAttrib* getTriangleObject( int i ) const;
    // ���ص�i�������εĵ�n����
    int getTriangleEdge( int i, int n ) const;
    // ���ص�i���ߵĵ�n��������
    int getEdgeTriangle( int i, int n ) const;
    // ��ǰ��eiλ��������ti�����رߵ���һ��������
    int getNextTriangle( int ei, int ti ) const;
    // ��ǰ��eiλ��������ti�����ظ������ε���һ����
    int getNextEdge( int ti, int ei ) const;
    // �жϵ�i�������εĵ�n�����ϵ���һ���������Ƿ��ѱ�ʹ��
    bool isNextTriangleUsed( int ti, int ei ) const;
    // �жϵ�i�������εĵ�n�����Ƿ����
    bool canEdgeUse( int i, int n, double z, bool useBoundary );
    // ���������в��ҿ��ñ�
    int findEdgeCanUse( int i, double z, bool useBoundary );
    // �ж�������ti��tj�Ƿ񹲱�ei
    bool isCoedge( int ti, int tj, int ei );
    // ��ȡ�����ߵĹ���
    int getCoNode( int e1, int e2 );
    // �Ա�ei�������Բ�ֵ
    void linearInterpolate( double z, int ei, DT_Point& p );
private:
    PointArray pa;
    EdgeAttribArray eaa;
    TriangleAttribArray taa;
};

Contour::Contour( const PointArray& pts, const EdgeArray& ea, const TriangleArray& ta )
{
    // �������ݸ���һ��
    std::copy( pts.begin(), pts.end(), std::back_inserter( pa ) );
    // ����ߺ���������������
    BuildAttribs( ea, ta, eaa, taa );
}

Contour::~Contour()
{
    pa.clear();
    for( int i = 0; i < ( int )eaa.size(); i++ )
    {
        delete eaa[i];
    }
    eaa.clear();
    for( int i = 0; i < ( int )taa.size(); i++ )
    {
        delete taa[i];
    }
    taa.clear();
}

bool Contour::hasContourPoint( double z, int i )
{
    DT_Point p1, p2;
    getEdgePoint( i, p1, p2 );
    if( IsDoubleEqual( p1.z, z ) )
    {
        p1.z = TrimZ( z, p1.z );
    }
    if( IsDoubleEqual( p2.z, z ) )
    {
        p2.z = TrimZ( z, p2.z );
    }
    return ( InRange( p1.z, p2.z, z ) == 1 );
}

void Contour::setEdgePoint( int i, const DT_Point& p1, const DT_Point& p2 )
{
    DT_Edge e = getEdgeObject( i )->e;
    pa[e.s] = p1;
    pa[e.t] = p2;
}

void Contour::getEdgePoint( int i, DT_Point& p1, DT_Point& p2 ) const
{
    DT_Edge e = getEdgeObject( i )->e;
    p1 = pa[e.s];
    p2 = pa[e.t];
}

EdgeAttrib* Contour::getEdgeObject( int i ) const
{
    return eaa[i];
}

TriangleAttrib* Contour::getTriangleObject( int i ) const
{
    return taa[i];
}

int Contour::getTriangleEdge( int i, int n ) const
{
    return getTriangleObject( i )->getEdge( n );
}

int Contour::getEdgeTriangle( int i, int n ) const
{
    return getEdgeObject( i )->getTriangle( n );
}

int Contour::getNextTriangle( int ei, int ti ) const
{
    return getEdgeObject( ei )->getNextTriangle( ti );
}

int Contour::getNextEdge( int ti, int ei ) const
{
    return getTriangleObject( ti )->getNextEdge( ei );
}

bool Contour::isCoedge( int ti, int tj, int ei )
{
    // �ų��߽�
    if( ti == -1 || tj == -1 ) return false;

    // �ų�������������ͬ�����
    if( ti == tj ) return false;

    int t0 = getEdgeTriangle( ei, 0 );
    int t1 = getEdgeTriangle( ei, 1 );

    return ( ( t0 == ti ) && ( t1 == tj ) ) ||
           ( ( t0 == tj ) && ( t1 == t0 ) );
}

int Contour::getCoNode( int e1, int e2 )
{
    return getEdgeObject( e1 )->getCoNode( getEdgeObject( e2 ) );
}

void Contour::linearInterpolate( double z, int ei, DT_Point& p )
{
    // ��ȡ��ni���ߵĶ˵�����
    DT_Point p1, p2;
    getEdgePoint( ei, p1, p2 );

    // ���Բ�ֵ
    p.z = z;
    LinearInterpolate( p1, p2, p );
}

bool Contour::canEdgeUse( int i, int n, double z, bool useBoundary )
{
    int ei = getTriangleEdge( i, n );

    bool b1 = useBoundary;
    bool b2 = getEdgeObject( ei )->isBoundary();
    if( b1 && !b2 ) return false;
    if( !b1 && b2 ) return false;

    return hasContourPoint( z, ei );
}

int Contour::findEdgeCanUse( int i, double z, bool useBoundary )
{
    if( canEdgeUse( i, 0, z, useBoundary ) ) return 0;
    if( canEdgeUse( i, 1, z, useBoundary ) ) return 1;
    if( canEdgeUse( i, 2, z, useBoundary ) ) return 2;

    return -1;
}

bool Contour::findStartTrianlge( double z, int& t0, int& e0, DT_Point& p, bool useBoundary )
{
    bool ret = false;
    for( int i = 0; i < ( int )taa.size(); i++ )
    {
        TriangleAttrib* pTriangle = taa[i];
        if( pTriangle->used ) continue;

        int n = findEdgeCanUse( i, z, useBoundary );
        if( n != -1 )
        {
            t0 = i;
            e0 = pTriangle->getEdge( n );
            linearInterpolate( z, e0, p );
            ret = true;
            break;
        }
    }
    return ret;
}

bool Contour::isNextTriangleUsed( int ti, int ei ) const
{
    //if(getTriangleObject(ti)->used) return false;

    ti = getNextTriangle( ei, ti );
    if( ti == -1 )
        return false;
    else
        return ( getTriangleObject( ti )->used );
}

bool Contour::findNextTriangle( double z, int t1, int e1, int& t2, int& e2, DT_Point& p )
{
    // �Ѵﵽ�߽�
    if( t1 == -1 ) return false;

    // �Ե�i�������ε�����2���߽��м�鲢��ֵ
    e2 = getNextEdge( t1, e1 );
    if( isNextTriangleUsed( t1, e2 ) || !hasContourPoint( z, e2 ) )
    {
        e2 = getNextEdge( t1, e2 );
        if( isNextTriangleUsed( t1, e2 ) || !hasContourPoint( z, e2 ) )
        {
            return false;
        }
    }

    // ����ǰ�������εı��Ϊ��ʹ��
    getTriangleObject( t1 )->used = true;

    // �������εı�Ž���ת��
    t2 = getNextTriangle( e2, t1 );

    // �Ա�e2���Բ�ֵ
    linearInterpolate( z, e2, p );

    return true;
}

bool Contour::findStartTrianlgeTwice( double z, int& t0, int& e0, DT_Point& p )
{
    // ��1�������߽�������
    bool ret = findStartTrianlge( z, t0, e0, p, true );
    // ���û�б߽������ο��ã��������ڲ�������
    if( !ret )
    {
        ret = findStartTrianlge( z, t0, e0, p, false );
    }
    return ret;
}

void Contour::traceContour( double zValue, PointArray& cnpts, IntArray& cnpos, IntArray& dirs )
{
    // ��ʼ��Ĭ�������ζ�û�б�ʹ��
    for( int i = 0; i < ( int )taa.size(); i++ )
    {
        taa[i]->used = false;
    }

    // ��¼��1��������(�������)
    int t0 = -1, e0 = -1;
    DT_Point p;

    while( findStartTrianlgeTwice( zValue, t0, e0, p ) )
    {
        // ����������ʼ��
        PointArray z_cnpts;
        z_cnpts.push_back( p );

        int t1 = t0, e1 = e0, t2, e2;
        while( findNextTriangle( zValue, t1, e1, t2, e2, p ) )
        {
            t1 = t2;
            e1 = e2;
            z_cnpts.push_back( p );
        }

        // ���ֻ��һ���㣬��ʾ׷�ٵ�ֵ��ʧ��
        if( z_cnpts.size() == 1 )
        {
            break;
        }

        // ��ʾ�պϵ�ֵ�߷���(1��-1), ���ŵ�ֵ��Ϊ0
        int dir = 0;

        // ��ֵ���Ƿ�պ�(��ʼ�����������һ����������)
        if( isCoedge( t0, t1, e0 ) )
        {
            getTriangleObject( t1 )->used = true;
            z_cnpts.push_back( z_cnpts.front() );

            // ��ֵ�߱պ�,������һ����������,�����߹���
            dir = ( ( zValue > pa[getCoNode( e0, e1 )].z ) ? 1 : -1 );
        }

        // ���Ƽ���õ��ĵ�ֵ��
        std::copy( z_cnpts.begin(), z_cnpts.end(), std::back_inserter( cnpts ) );
        // ��¼��ֵ�߸���
        cnpos.push_back( z_cnpts.size() );
        // ��¼��ֵ�ߵķ���
        dirs.push_back( dir );
    }
}

// �߽��֧
struct BoundaryEdge
{
    int ei;
    bool used;
};
typedef std::vector<BoundaryEdge> BoundaryEdgeArray;

void Contour::searchBoundary( PointArray& bpts, IntArray& bpos )
{
    // ��¼���б߽��֧
    BoundaryEdgeArray bea;
    for( int i = 0; i < ( int )eaa.size(); i++ )
    {
        if( eaa[i]->isBoundary() )
        {
            BoundaryEdge be = {i, false};
            bea.push_back( be );
        }
    }
    if( bea.empty() ) return;

    // ��¼�����õ��ĵ���
    IntArray pia;

    // ǰ�᣺�߽��Ȼ�Ǳպϵ�
    // ���ܴ��ڶ���߽�(����߽�)
    while( true )
    {
        // �������õı߽�
        int bi = -1;
        for( int i = 0; i < ( int )bea.size(); i++ )
        {
            BoundaryEdge be = bea[i];
            if( !be.used )
            {
                bi = i;
                break;
            }
        }
        if( bi == -1 ) break;

        // ��¼֮ǰ���������ı߽�����
        int boundPts_num = pia.size();

        DT_Edge e = getEdgeObject( bea[bi].ei )->e;
        bea[bi].used = true;  // ���Ϊʹ��

        pia.push_back( e.s );
        pia.push_back( e.t );

        int nt = e.t;
        while( true )
        {
            int nbi = -1;
            for( int i = 0; i < ( int )bea.size(); i++ )
            {
                if( bea[i].used ) continue;

                DT_Edge ne = getEdgeObject( bea[i].ei )->e;
                if( nt == ne.s )
                {
                    nt = ne.t;
                    nbi = i;
                    break;
                }
                else if( nt == ne.t )
                {
                    nt = ne.s;
                    nbi = i;
                    break;
                }
            }
            if( nbi == -1 ) break;

            bea[nbi].used = true;
            pia.push_back( nt );
        }

        int n = pia.size() - boundPts_num;
        if( n >= 3 )
        {
            // ������3����
            bpos.push_back( n );
        }
    }

    // ������ת����ʵ�ʵĵ�����
    for( int i = 0; i < ( int )pia.size(); i++ )
    {
        bpts.push_back( pa[pia[i]] );
    }
}

void TraceContours(
    /* �� */
    const PointArray& pts,
    /* �� */
    const EdgeArray& ea,
    /* ������ */
    const TriangleArray& ta,
    /* Ҫ׷�ٵ�zֵ */
    const DoubleArray& zValues,
    /* ���еĵ�ֵ���ϵĵ㶼����һ������ */
    PointArray& cnpts,
    /* ��¼��ֵ����Ϣ */
    ContourArray& cna )
{
    Contour cnt( pts, ea, ta );
    for( int i = 0; i < ( int )zValues.size(); i++ )
    {
        PointArray z_cnpts;
        IntArray z_cnpos;
        IntArray dirs;
        cnt.traceContour( zValues[i], z_cnpts, z_cnpos, dirs );

        if( z_cnpos.empty() )
        {
            DT_Contour z_contour = {i, 0, 1, 0};
            cna.push_back( z_contour );
        }
        else
        {
            for( int j = 0; j < ( int )z_cnpos.size(); j++ )
            {
                DT_Contour z_contour = {i, z_cnpos[j], dirs[j], 0};
                cna.push_back( z_contour );
            }
            // ���Ƽ���õ��ĵ�ֵ������
            std::copy( z_cnpts.begin(), z_cnpts.end(), std::back_inserter( cnpts ) );
        }
    }
}

static int SearchMaxAreaBoundary( const PointArray& bpts, const IntArray& bpos )
{
    double maxArea = -1;
    int pos = -1;
    for( int i = 0; i < ( int )bpos.size(); i++ )
    {
        int s = 0;
        for( int j = 0; j < i; j++ )
        {
            s += bpos[j];
        }
        int t = s + bpos[i];

        PointArray polygon;
        std::copy( bpts.begin() + s, bpts.begin() + t, std::back_inserter( polygon ) );
        if( pos == -1 )
        {
            pos = i;
            maxArea = PolygonArea( polygon );
        }
        else
        {
            double area = PolygonArea( polygon );
            if( area > maxArea )
            {
                pos = i;
                maxArea = area;
            }
        }
    }
    return pos;
}

static void GetSEPos( const IntArray& bpos, int k, int& s, int& t )
{
    s = 0;
    for( int i = 0; i < k; i++ )
    {
        s += bpos[i];
    }
    t = s + bpos[k];
}

static void GetOutMostBoundary( const PointArray& all_bpts, const IntArray& all_bpos, PointArray& bpts )
{
    // �������߽�(����Χ�߽�)
    int k = SearchMaxAreaBoundary( all_bpts, all_bpos );
    if( k != -1 )
    {
        int ks = 0, kt = 0;
        GetSEPos( all_bpos, k, ks, kt );
        std::copy( all_bpts.begin() + ks, all_bpts.begin() + kt, std::back_inserter( bpts ) );
    }
}

void SearchBoundary(
    /* �� */
    const PointArray& pts,
    /* �� */
    const EdgeArray& ea,
    /* ������ */
    const TriangleArray& ta,
    /* ���еı߽�㶼����һ������ */
    PointArray& bpts )
{
    // �������еı߽�
    PointArray all_bpts;
    IntArray all_bpos;
    Contour cnt( pts, ea, ta );
    cnt.searchBoundary( all_bpts, all_bpos );

    // ���������Χ�߽�
    GetOutMostBoundary( all_bpts, all_bpos, bpts );
}