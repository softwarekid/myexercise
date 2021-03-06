#include "StdAfx.h"
#include "LocalFan_AFCStrategy.h"

namespace PJ
{
    LocalFan_AFCStrategy::LocalFan_AFCStrategy( const CString& tableName ) : AFCStrategy( tableName ), isRock( true )
    {
    }

    void LocalFan_AFCStrategy::setArea( double area )
    {
        this->area = area;
    }

    void LocalFan_AFCStrategy::setFanNum( int num )
    {
        this->fanNum = num;
    }

    void LocalFan_AFCStrategy::setQ_FAN( double q )
    {
        this->q_fan = q;
    }

    void LocalFan_AFCStrategy::changeTunnelType( bool isRock )
    {
        this->isRock = isRock;
    }

    double LocalFan_AFCStrategy::caculate()
    {
        double v = ( isRock ? 0.15 : 0.25 );
        return q_fan * fanNum + 60 * v * area;
    }

    void LocalFan_AFCStrategy::createTableImpl( TableCreator* pTableCreator )
    {
        AFCStrategy::createTableImpl( pTableCreator );

        BEGIN_DEFINE_FIELD( TableCreator, pTableCreator )
        DEFINE_REAL_FIELD( q_fan )
        DEFINE_INT_FIELD( fanNum )
        DEFINE_BOOL_FIELD( isRock )
        DEFINE_REAL_FIELD( area )
        END_DEFINE_FIELD
    }

    void LocalFan_AFCStrategy::writeToTableImpl( DataWriter* pDataWriter )
    {
        AFCStrategy::writeToTableImpl( pDataWriter );

        BEGIN_WRITE_DATA( DataWriter, pDataWriter )
        WRITE_SIMPLE_REAL_DATA( q_fan )
        WRITE_SIMPLE_INT_DATA( fanNum )
        WRITE_SIMPLE_BOOL_DATA( isRock )
        WRITE_SIMPLE_REAL_DATA( area )
        END_WRITE_DATA
    }
}
