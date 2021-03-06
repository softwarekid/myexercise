#include "StdAfx.h"

#include "VNG_Node.h"
#include "DrawTool.h"

Adesk::UInt32 VNG_Node::kCurrentVersionNumber = 1 ;

ACRX_DXF_DEFINE_MEMBERS (
    VNG_Node, ModelGE,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    网络图节点, VNGGENAPP
)

VNG_Node::VNG_Node () : ModelGE ()
{
}

VNG_Node::~VNG_Node ()
{
}

Acad::ErrorStatus VNG_Node::dwgOutFields ( AcDbDwgFiler* pFiler ) const
{
    assertReadEnabled () ;
    //----- Save parent class information first.
    Acad::ErrorStatus es = ModelGE::dwgOutFields ( pFiler ) ;
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
    Acad::ErrorStatus es = ModelGE::dwgInFields ( pFiler ) ;
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

// DWORD <==> Adesk::UInt32 <==> unsigned long
// Adesk::UInt8 <==> unsigned char
static void LONG2RGB( Adesk::UInt32 rgbColor, Adesk::UInt8& r, Adesk::UInt8& g, Adesk::UInt8& b )
{
    // 下面2段代码是等价的

    /*r = ( rgbColor & 0xffL );
    g = ( rgbColor & 0xff00L ) >> 8;
    b = rgbColor >> 16;*/

    r = GetRValue( rgbColor );
    g = GetGValue( rgbColor );
    b = GetBValue( rgbColor );
}

// 获取cad绘图窗口背景色
static bool GetBackgroundColor( Adesk::UInt8& r, Adesk::UInt8& g, Adesk::UInt8& b )
{
    // 获取cad当前的所有颜色设置
    // 右键"选项"->"显示"->"颜色"
    AcColorSettings cs;
    if( !acedGetCurrentColors( &cs ) ) return false;

    // 读取背景颜色
    DWORD rgbColor = cs.dwGfxModelBkColor;
    LONG2RGB( rgbColor, r, g, b );
    return true;
}

static void DrawNode( AcGiWorldDraw* mode, const AcGePoint3d& pt, double width, double height )
{
    // 获取cad绘图窗口背景色
    Adesk::UInt8 r, g, b;
    GetBackgroundColor( r, g, b );

    AcGiSubEntityTraits& traits = mode->subEntityTraits();
    Adesk::UInt16 cl = traits.color();;

    AcCmEntityColor bgColor( r, g, b );
    traits.setTrueColor( bgColor );

    DrawEllipse( mode, pt, width, height, true );

    traits.setColor( cl );
}

Adesk::Boolean VNG_Node::subWorldDraw ( AcGiWorldDraw* mode )
{
    assertReadEnabled();

    DrawEllipse( mode, m_pt, m_width, m_height, false );
    //DrawNode(mode, m_pt, m_width, m_height);

    CString idStr;
    idStr.Format( _T( "%d" ), m_id );

    CString str;
    str.Format( _T( "V%s" ), MakeLowerText( idStr ) );
    DrawMText( mode, m_pt, 0, str, m_textHeight );

    return Adesk::kTrue;
    //return (ModelGE::subWorldDraw (mode));
}

Acad::ErrorStatus VNG_Node::subTransformBy( const AcGeMatrix3d& xform )
{
    // 构造一个椭圆，进行变换
    AcGeVector3d v( AcGeVector3d::kXAxis );
    v *= m_width / 2;
    AcDbEllipse ellipse( m_pt, AcGeVector3d::kZAxis, v, m_height / m_width );
    ellipse.transformBy( xform );

    // 构造一个字体对象进行变换
    AcDbMText mt;
    mt.setContents( _T( "xx" ) );
    mt.setTextHeight( m_textHeight );
    mt.setRotation( 0 );
    mt.setLocation( m_pt );
    mt.transformBy( xform );

    // 更新参数
    m_pt = ellipse.center();
    m_width = ellipse.majorAxis().length() * 2;
    m_height = ellipse.minorAxis().length() * 2;

    // 更新参数
    m_textHeight = mt.textHeight();

    return Acad::eOk;
}

Acad::ErrorStatus VNG_Node::subGetOsnapPoints (
    AcDb::OsnapMode osnapMode,
    int gsSelectionMark,
    const AcGePoint3d& pickPoint,
    const AcGePoint3d& lastPoint,
    const AcGeMatrix3d& viewXform,
    AcGePoint3dArray& snapPoints,
    AcDbIntArray& geomIds ) const
{
    assertReadEnabled () ;

    if( osnapMode == AcDb::kOsModeCen )
    {
        snapPoints.append( m_pt );
    }
    return Acad::eOk;
}

Acad::ErrorStatus VNG_Node::subGetGripPoints(
    AcGePoint3dArray& gripPoints,
    AcDbIntArray& osnapModes,
    AcDbIntArray& geomIds ) const
{
    assertReadEnabled () ;

    gripPoints.append( m_pt );

    return Acad::eOk;
}

Acad::ErrorStatus VNG_Node::subMoveGripPointsAt ( const AcDbIntArray& indices, const AcGeVector3d& offset )
{
    assertWriteEnabled () ;

    for( int i = 0; i < indices.length(); i++ )
    {
        int idx = indices.at( i );

        if ( idx == 0 )
        {
            m_pt += offset;
        }
    }
    return Acad::eOk;
}