#include "StdAfx.h"
#include "GasPipe.h"

Adesk::UInt32 GasPipe::kCurrentVersionNumber = 1 ;

ACRX_DXF_DEFINE_MEMBERS (
    GasPipe, MineGE,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    瓦斯钻孔, DEFGEAPP
)

GasPipe::GasPipe () : MineGE ()
{
}

GasPipe::GasPipe( const AcGePoint3d& startPt, const AcGePoint3d& endPt )
    : MineGE(), m_startPt( startPt ), m_endPt( endPt )
{
}

GasPipe::~GasPipe ()
{
}

void GasPipe::setSEPoint( const AcGePoint3d& startPt, const AcGePoint3d& endPt )
{
    assertWriteEnabled();

    m_startPt = startPt;
    m_endPt = endPt;
}

void GasPipe::getSEPoint( AcGePoint3d& startPt, AcGePoint3d& endPt ) const
{
    assertReadEnabled();

    startPt = m_startPt;
    endPt = m_endPt;
}

void GasPipe::writeKeyParam( DrawParamWriter& writer ) const
{
    writer.writePoint( m_startPt );
    writer.writePoint( m_endPt );
}

void GasPipe::readKeyParam( DrawParamReader& reader )
{
    reader.readPoint( m_startPt );
    reader.readPoint( m_endPt );
}

Acad::ErrorStatus GasPipe::dwgOutFields ( AcDbDwgFiler* pFiler ) const
{
    assertReadEnabled () ;

    //acutPrintf(_T("\nLinkedGE::dwgOutFields"));
    Acad::ErrorStatus es = MineGE::dwgOutFields ( pFiler ) ;
    if ( es != Acad::eOk )
        return ( es ) ;

    if ( ( es = pFiler->writeUInt32 ( GasPipe::kCurrentVersionNumber ) ) != Acad::eOk )
        return ( es ) ;

    // 保存始末点坐标
    pFiler->writeItem( m_startPt );
    pFiler->writeItem( m_endPt );

    return ( pFiler->filerStatus () ) ;
}

Acad::ErrorStatus GasPipe::dwgInFields ( AcDbDwgFiler* pFiler )
{
    assertWriteEnabled () ;

    //acutPrintf(_T("\nLinkedGE::dwgInFields"));
    Acad::ErrorStatus es = MineGE::dwgInFields ( pFiler ) ;
    if ( es != Acad::eOk )
        return ( es ) ;

    Adesk::UInt32 version = 0 ;
    if ( ( es = pFiler->readUInt32 ( &version ) ) != Acad::eOk )
        return ( es ) ;
    if ( version > GasPipe::kCurrentVersionNumber )
        return ( Acad::eMakeMeProxy ) ;

    // 读取始末点坐标
    pFiler->readItem( &m_startPt );
    pFiler->readItem( &m_endPt );

    return ( pFiler->filerStatus () ) ;
}