#include "StdAfx.h"
#include "MineBoundary.h"

Adesk::UInt32 MineBoundary::kCurrentVersionNumber = 1 ;

ACRX_DXF_DEFINE_MEMBERS (
    MineBoundary, GeologyGE,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    ����߽�,	GASGEAPP )

MineBoundary::MineBoundary () : GeologyGE ()
{
}

MineBoundary::~MineBoundary ()
{
}

Acad::ErrorStatus MineBoundary::dwgOutFields ( AcDbDwgFiler* pFiler ) const
{
    assertReadEnabled () ;
    //----- Save parent class information first.
    Acad::ErrorStatus es = GeologyGE::dwgOutFields ( pFiler ) ;
    if ( es != Acad::eOk )
        return ( es ) ;
    //----- Object version number needs to be saved first
    if ( ( es = pFiler->writeUInt32 ( MineBoundary::kCurrentVersionNumber ) ) != Acad::eOk )
        return ( es ) ;

    // д���������
    int len = m_pts.length();
    pFiler->writeInt32( len );

    // д������
    for( int i = 0; i < len; i++ )
    {
        pFiler->writePoint3d( m_pts[i] );
    }

    return ( pFiler->filerStatus () ) ;
}

Acad::ErrorStatus MineBoundary::dwgInFields ( AcDbDwgFiler* pFiler )
{
    assertWriteEnabled () ;
    //----- Read parent class information first.
    Acad::ErrorStatus es = GeologyGE::dwgInFields ( pFiler ) ;
    if ( es != Acad::eOk )
        return ( es ) ;
    //----- Object version number needs to be read first
    Adesk::UInt32 version = 0 ;
    if ( ( es = pFiler->readUInt32 ( &version ) ) != Acad::eOk )
        return ( es ) ;
    if ( version > MineBoundary::kCurrentVersionNumber )
        return ( Acad::eMakeMeProxy ) ;

    // ��ȡ�������
    long len = 0;
    pFiler->readInt32( &len );

    // ��ȡ����
    m_pts.removeAll();
    for( int i = 0; i < ( int )len; i++ )
    {
        AcGePoint3d pt;
        pFiler->readPoint3d( &pt );
        m_pts.append( pt );
    }

    return ( pFiler->filerStatus() ) ;
}

void MineBoundary::writeKeyParam( DrawParamWriter& writer ) const
{
    int len = m_pts.length();
    writer.writeInt( len );
    for( int i = 0; i < len; i++ )
    {
        writer.writePoint( m_pts[i] );
    }
}

void MineBoundary::readKeyParam( DrawParamReader& reader )
{
    m_pts.removeAll();

    int len = 0;
    reader.readInt( len );
    for( int i = 0; i < len; i++ )
    {
        AcGePoint3d pt;
        reader.readPoint( pt );
        m_pts.append( pt );
    }
}

void MineBoundary::addControlPoint( const AcGePoint3d& pt )
{
    assertWriteEnabled();

    if( !m_pts.contains( pt ) ) m_pts.append( pt );
}

void MineBoundary::getControlPoints( AcGePoint3dArray& pts )
{
    assertReadEnabled();
    pts.append( m_pts );
}

static AcGePoint3d CacLineClosePt( const AcGePoint3d& spt, const AcGePoint3d& ept, const AcGePoint3d& pt )
{
    // ����һ�������߶�
    AcGeLineSeg3d line( spt, ept );
    return line.closestPointTo( pt ); // ���������������ĵ�
}

void MineBoundary::splitLine( int pos, const AcGePoint3d& pt )
{
    assertWriteEnabled();

    int n = m_pts.length();
    if( pos < 0 || pos >= n ) return;
    if( m_pts.contains( pt ) ) return;

    m_pts.insertAt( pos + 1, CacLineClosePt( m_pts[pos], m_pts[( pos + 1 ) % n], pt ) );
}

void MineBoundary::removePoint( int pos )
{
    assertWriteEnabled();

    int n = m_pts.length();
    if( n < 5 ) return; // ������һ���ı���
    if( pos < 0 || pos >= n ) return;

    m_pts.removeAt( pos );
}

void MineBoundary::setPoint( int pos, const AcGePoint3d& pt )
{
    assertWriteEnabled();

    int n = m_pts.length();
    if( pos < 0 || pos >= n ) return;
    if( m_pts.contains( pt ) ) return;

    m_pts[pos] = pt;
}

void MineBoundary::insertPoints( int pos, const AcGePoint3dArray& pts, bool bPrevOrNext )
{
    assertWriteEnabled();

    if( pts.isEmpty() ) return;
    int n = m_pts.length();
    if( pos < 0 || pos >= n ) return;

    // �ų��ظ���
    AcGePoint3dArray new_pts;
    for( int i = 0; i < pts.length(); i++ )
    {
        if( !m_pts.contains( pts[i] ) )
        {
            new_pts.append( pts[i] );
        }
    }

    int k = ( bPrevOrNext ? pos : pos + 1 );

    // ���Ч��
    AcGePoint3dArray left_pts, right_pts;
    for( int i = 0; i < k; i++ )
    {
        left_pts.append( m_pts[i] );
    }
    for( int i = k; i < n; i++ )
    {
        right_pts.append( m_pts[i] );
    }

    //acutPrintf(_T("\nn=%d, k=%d, p=%d, m=%d, left:%d, right:%d"), n, k, pts.length(), new_pts.length(), left_pts.length(), right_pts.length());
    // ���
    m_pts.removeAll();
    m_pts.append( left_pts );
    m_pts.append( new_pts );
    m_pts.append( right_pts );

    //acutPrintf(_T("\ntn = %d"), m_pts.length());
}