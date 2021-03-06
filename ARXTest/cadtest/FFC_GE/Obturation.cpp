#include "StdAfx.h"
#include "Obturation.h"

Adesk::UInt32 Obturation::kCurrentVersionNumber = 1 ;

ACRX_DXF_DEFINE_MEMBERS (
    Obturation, DirectionTagGE,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    �ܱ�, DEFGEAPP
)

Obturation::Obturation () : DirectionTagGE ()
{
}

Obturation::Obturation( const AcGePoint3d& pt, double angle ) : DirectionTagGE( pt, angle )
{
}

Obturation::~Obturation ()
{
}
