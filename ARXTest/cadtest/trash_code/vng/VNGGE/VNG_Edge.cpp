#include "StdAfx.h"
#include "VNG_Edge.h"

Adesk::UInt32 VNG_Edge::kCurrentVersionNumber = 1 ;

ACRX_DXF_DEFINE_MEMBERS (
    VNG_Edge, VNG_GE,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    ����ͼ��֧, ARX_VNG_APP
)

VNG_Edge::VNG_Edge () : VNG_GE ()
{
}

VNG_Edge::~VNG_Edge ()
{
}

Acad::ErrorStatus VNG_Edge::dwgOutFields ( AcDbDwgFiler* pFiler ) const
{
    assertReadEnabled () ;
    //----- Save parent class information first.
    Acad::ErrorStatus es = VNG_GE::dwgOutFields ( pFiler ) ;
    if ( es != Acad::eOk )
        return ( es ) ;
    //----- Object version number needs to be saved first
    if ( ( es = pFiler->writeUInt32 ( VNG_Edge::kCurrentVersionNumber ) ) != Acad::eOk )
        return ( es ) ;

    pFiler->writePoint3d( m_spt );
    pFiler->writePoint3d( m_ept );
    pFiler->writePoint3d( m_pt );

    pFiler->writeDouble( m_arrowWidth );
    pFiler->writeDouble( m_arrowLength );

    pFiler->writeBool( m_needEdge );
    pFiler->writePoint3d( m_tpt );
    pFiler->writeDouble( m_textHeight );
    pFiler->writeInt32( m_id );

    return ( pFiler->filerStatus () ) ;
}

Acad::ErrorStatus VNG_Edge::dwgInFields ( AcDbDwgFiler* pFiler )
{
    assertWriteEnabled () ;
    //----- Read parent class information first.
    Acad::ErrorStatus es = VNG_GE::dwgInFields ( pFiler ) ;
    if ( es != Acad::eOk )
        return ( es ) ;
    //----- Object version number needs to be read first
    Adesk::UInt32 version = 0 ;
    if ( ( es = pFiler->readUInt32 ( &version ) ) != Acad::eOk )
        return ( es ) ;
    if ( version > VNG_Edge::kCurrentVersionNumber )
        return ( Acad::eMakeMeProxy ) ;

    pFiler->readPoint3d( &m_spt );
    pFiler->readPoint3d( &m_ept );
    pFiler->readPoint3d( &m_pt );

    pFiler->readDouble( &m_arrowWidth );
    pFiler->readDouble( &m_arrowLength );

    pFiler->readBool( &m_needEdge );
    pFiler->readPoint3d( &m_tpt );
    pFiler->readDouble( &m_textHeight );
    long L;
    pFiler->readInt32( &L );
    m_id = ( int )L;

    return ( pFiler->filerStatus () ) ;
}

void VNG_Edge::pushKeyParamToWriter( DrawParamWriter& writer ) const
{
    writer.writePoint( m_spt );
    writer.writePoint( m_spt );
    writer.writePoint( m_pt );
    writer.writeDouble( m_arrowWidth );
    writer.writeDouble( m_arrowLength );
    writer.writeBoolean( m_needEdge );
    writer.writePoint( m_tpt );
    writer.writeDouble( m_textHeight );
    writer.writeInt( m_id );
}

void VNG_Edge::pullKeyParamFromReader( DrawParamReader& reader )
{
    reader.readPoint( m_spt );
    reader.readPoint( m_spt );
    reader.readPoint( m_pt );
    reader.readDouble( m_arrowWidth );
    reader.readDouble( m_arrowLength );
    reader.readBoolean( m_needEdge );
    reader.readPoint( m_tpt );
    reader.readDouble( m_textHeight );
    reader.readInt( m_id );
}