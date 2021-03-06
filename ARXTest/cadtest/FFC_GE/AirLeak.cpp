#include "StdAfx.h"
#include "AirLeak.h"

Adesk::UInt32 SourceAirLeak::kCurrentVersionNumber = 1 ;
Adesk::UInt32 SinkAirLeak::kCurrentVersionNumber = 1 ;

ACRX_NO_CONS_DEFINE_MEMBERS( AirLeak, PointGE )

ACRX_DXF_DEFINE_MEMBERS (
    SourceAirLeak, AirLeak,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    ©��Դ, DEFGEAPP
)

ACRX_DXF_DEFINE_MEMBERS (
    SinkAirLeak, AirLeak,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    ©���, DEFGEAPP
)

AirLeak::AirLeak () : PointGE ()
{
}

AirLeak::AirLeak( const AcGePoint3d& pt ) : PointGE( pt )
{
}

SourceAirLeak::SourceAirLeak () : AirLeak ()
{
}

SourceAirLeak::SourceAirLeak( const AcGePoint3d& pt ) : AirLeak( pt )
{
}

SinkAirLeak::SinkAirLeak () : AirLeak ()
{
}

SinkAirLeak::SinkAirLeak( const AcGePoint3d& pt ) : AirLeak( pt )
{
}

