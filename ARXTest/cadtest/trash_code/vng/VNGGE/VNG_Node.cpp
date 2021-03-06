#include "StdAfx.h"
#include "VNG_Node.h"

Adesk::UInt32 VNG_Node::kCurrentVersionNumber = 1 ;

ACRX_DXF_DEFINE_MEMBERS (
    VNG_Node, VNG_GE,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    网络图节点, VNGGENAPP
)

VNG_Node::VNG_Node () : VNG_GE ()
{
}

VNG_Node::~VNG_Node ()
{
}

Acad::ErrorStatus VNG_Node::dwgOutFields ( AcDbDwgFiler* pFiler ) const
{
    assertReadEnabled () ;
    //----- Save parent class information first.
    Acad::ErrorStatus es = VNG_GE::dwgOutFields ( pFiler ) ;
    if ( es != Acad::eOk )
        return ( es ) ;
    //----- Object version number needs to be saved first
    if ( ( es = pFiler->writeUInt32 ( VNG_Node::kCurrentVersionNumber ) ) != Acad::eOk )
        return ( es ) ;

    // 保存节点数据
    pFiler->writePoint3d( m_pt );
    pFiler->writeDouble( m_width );
    pFiler->writeDouble( m_height );
    pFiler->writeDouble( m_textHeight );
    pFiler->writeInt32( m_id );

    return ( pFiler->filerStatus () ) ;
}

Acad::ErrorStatus VNG_Node::dwgInFields ( AcDbDwgFiler* pFiler )
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
    if ( version > VNG_Node::kCurrentVersionNumber )
        return ( Acad::eMakeMeProxy ) ;

    // 读取节点数据
    pFiler->readPoint3d( &m_pt );
    pFiler->readDouble( &m_width );
    pFiler->readDouble( &m_height );
    pFiler->readDouble( &m_textHeight );
    long L;
    pFiler->readInt32( &L );
    m_id = ( int )L;

    return ( pFiler->filerStatus () ) ;
}

void VNG_Node::pushKeyParamToWriter( DrawParamWriter& writer ) const
{
    writer.writePoint( m_pt );
    writer.writeDouble( m_width );
    writer.writeDouble( m_height );
    writer.writeDouble( m_textHeight );
    writer.writeInt( m_id );
}

void VNG_Node::pullKeyParamFromReader( DrawParamReader& reader )
{
    reader.readPoint( m_pt );
    reader.readDouble( m_width );
    reader.readDouble( m_height );
    reader.readDouble( m_textHeight );
    reader.readInt( m_id );
}