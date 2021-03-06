#include "StdAfx.h"
#include "TVE_AFCStrategy.h"

namespace PJ
{
    TVE_AFCStrategy::TVE_AFCStrategy( const CString& tableName ) : AFCStrategy( tableName )
    {
    }

    TVE_AFCStrategy::~TVE_AFCStrategy( void )
    {
    }

    void TVE_AFCStrategy::setReasonalVelocity( double v )
    {
        this->reasonalVelocity = v;
    }

    void TVE_AFCStrategy::setArea( double area )
    {
        this->area = area;
    }

    double TVE_AFCStrategy::caculate()
    {
        return 60 * reasonalVelocity * area;
    }

    void TVE_AFCStrategy::createTableImpl( TableCreator* pTableCreator )
    {
        AFCStrategy::createTableImpl( pTableCreator );

        BEGIN_DEFINE_FIELD( TableCreator, pTableCreator )
        DEFINE_REAL_FIELD( reasonalVelocity )
        DEFINE_REAL_FIELD( area )
        END_DEFINE_FIELD
    }

    void TVE_AFCStrategy::writeToTableImpl( DataWriter* pDataWriter )
    {
        AFCStrategy::writeToTableImpl( pDataWriter );

        BEGIN_WRITE_DATA( DataWriter, pDataWriter )
        WRITE_SIMPLE_REAL_DATA( reasonalVelocity )
        WRITE_SIMPLE_REAL_DATA( area )
        END_WRITE_DATA
    }
}