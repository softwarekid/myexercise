#include "StdAfx.h"
#include "ContourInfo.h"

Adesk::UInt32 ContourInfo::kCurrentVersionNumber = 1 ;

ACRX_DXF_DEFINE_MEMBERS (
    ContourInfo, ContourGE,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    �����ɫ����,	CONTOURAPP )

ContourInfo::ContourInfo () : ContourGE (), m_bFill( false )
{
}

ContourInfo::~ContourInfo ()
{
}

Acad::ErrorStatus ContourInfo::dwgOutFields ( AcDbDwgFiler* pFiler ) const
{
    assertReadEnabled () ;
    //----- Save parent class information first.
    Acad::ErrorStatus es = ContourGE::dwgOutFields ( pFiler ) ;
    if ( es != Acad::eOk )
        return ( es ) ;
    //----- Object version number needs to be saved first
    if ( ( es = pFiler->writeUInt32 ( ContourInfo::kCurrentVersionNumber ) ) != Acad::eOk )
        return ( es ) ;

    int n = m_datas.length();
    pFiler->writeInt32( n );

    for( int i = 0; i < n; i++ )
    {
        pFiler->writePoint3d( m_datas[i] );
    }

    n = m_zValues.length();
    pFiler->writeInt32( n );

    for( int i = 0; i < n; i++ )
    {
        pFiler->writeDouble( m_zValues[i] );
    }

    n = m_colors.length();
    pFiler->writeInt32( n );

    for( int i = 0; i < n; i++ )
    {
        pFiler->writeUInt32( m_colors[i] );
    }

    pFiler->writeBool( m_bFill );

    return ( pFiler->filerStatus () ) ;
}

Acad::ErrorStatus ContourInfo::dwgInFields ( AcDbDwgFiler* pFiler )
{
    assertWriteEnabled () ;
    //----- Read parent class information first.
    Acad::ErrorStatus es = ContourGE::dwgInFields ( pFiler ) ;
    if ( es != Acad::eOk )
        return ( es ) ;
    //----- Object version number needs to be read first
    Adesk::UInt32 version = 0 ;
    if ( ( es = pFiler->readUInt32 ( &version ) ) != Acad::eOk )
        return ( es ) ;
    if ( version > ContourInfo::kCurrentVersionNumber )
        return ( Acad::eMakeMeProxy ) ;

    long n = 0;
    pFiler->readInt32( &n );

    m_datas.removeAll();
    for( int i = 0; i < ( int )n; i++ )
    {
        AcGePoint3d pt;
        pFiler->readPoint3d( &pt );
        m_datas.append( pt );
    }

    n = 0;
    pFiler->readInt32( &n );

    m_zValues.removeAll();
    for( int i = 0; i < ( int )n; i++ )
    {
        double d = 0;
        pFiler->readDouble( &d );
        m_zValues.append( d );
    }

    n = 0;
    pFiler->readInt32( &n );

    m_colors.removeAll();
    for( int i = 0; i < ( int )n; i++ )
    {
        unsigned long rgb;
        pFiler->readUInt32( &rgb );
        m_colors.append( rgb );
    }

    pFiler->readBool( &m_bFill );

    return ( pFiler->filerStatus() ) ;
}

void ContourInfo::getZValues( AcGeDoubleArray& zValues )
{
    assertReadEnabled();
    zValues.append( m_zValues );
}

void ContourInfo::getColors( AcArray<COLORREF>& colors )
{
    assertReadEnabled();
    colors.append( m_colors );
}

void ContourInfo::setZValues( const AcGeDoubleArray& zValues )
{
    assertWriteEnabled();
    m_zValues.removeAll();
    m_zValues.append( zValues );
}

void ContourInfo::setColors( const AcArray<COLORREF>& colors )
{
    assertWriteEnabled();
    m_colors.removeAll();
    m_colors.append( colors );
}

void ContourInfo::setDatas( const AcGePoint3dArray& datas )
{
    assertWriteEnabled();
    m_datas.removeAll();
    m_datas.append( datas );
}

void ContourInfo::getDatas( AcGePoint3dArray& datas )
{
    assertReadEnabled();
    datas.append( m_datas );
}

bool ContourInfo::fill() const
{
    assertReadEnabled();
    return m_bFill;
}

void ContourInfo::needFill( bool bFill )
{
    assertWriteEnabled();
    m_bFill = bFill;
}