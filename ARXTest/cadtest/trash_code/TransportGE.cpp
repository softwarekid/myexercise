#include "StdAfx.h"
#include "TransportGE.h"

Adesk::UInt32 TransportGE::kCurrentVersionNumber = 1 ;
Adesk::UInt32 Diesel::kCurrentVersionNumber = 1 ;
Adesk::UInt32 ElectricWireCar::kCurrentVersionNumber = 1 ;

ACRX_NO_CONS_DEFINE_MEMBERS( TransportGE, DirectionTagGE )

ACRX_DXF_DEFINE_MEMBERS (
    Diesel, TransportGE,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    DIESEL, DEFGEAPP
)

ACRX_DXF_DEFINE_MEMBERS (
    ElectricWireCar, TransportGE,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    ELECTRICWIRECAR, DEFGEAPP
)

TransportGE::TransportGE( void ) : DirectionTagGE ()
{
}

TransportGE::TransportGE( const AcGePoint3d& insertPt, double angle )
    : DirectionTagGE( insertPt, angle )
{

}

TransportGE::~TransportGE( void )
{
}

Diesel::Diesel( void ) : TransportGE ()
{
}

Diesel::Diesel( const AcGePoint3d& insertPt, double angle )
    : TransportGE( insertPt, angle )
{

}

Diesel::~Diesel( void )
{
}

ElectricWireCar::ElectricWireCar( void ) : TransportGE ()
{
}

ElectricWireCar::ElectricWireCar( const AcGePoint3d& insertPt, double angle )
    : TransportGE( insertPt, angle )
{

}

ElectricWireCar::~ElectricWireCar( void )
{
}