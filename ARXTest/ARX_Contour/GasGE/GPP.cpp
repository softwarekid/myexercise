#include "StdAfx.h"
#include "GPP.h"

Adesk::UInt32 GPP::kCurrentVersionNumber = 1 ;

ACRX_DXF_DEFINE_MEMBERS (
    GPP, GasGeologyGE,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    ��˹ѹ����, GASGEAPP
)

GPP::GPP() : GasGeologyGE()
{

}

GPP::GPP( const AcGePoint3d& insertPt ) : GasGeologyGE( insertPt )
{

}
