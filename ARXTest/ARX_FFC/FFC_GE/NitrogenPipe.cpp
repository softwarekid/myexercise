#include "StdAfx.h"
#include "NitrogenPipe.h"

Adesk::UInt32 NitrogenPipe::kCurrentVersionNumber = 1 ;

ACRX_DXF_DEFINE_MEMBERS (
    NitrogenPipe, PointGE,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    ע����, DEFGEAPP
)

NitrogenPipe::NitrogenPipe () : PointGE ()
{
}

NitrogenPipe::NitrogenPipe( const AcGePoint3d& pt ) : PointGE( pt )
{
}

NitrogenPipe::~NitrogenPipe ()
{
}
