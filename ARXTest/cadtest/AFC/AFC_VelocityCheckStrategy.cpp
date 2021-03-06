#include "StdAfx.h"
#include "AFC_VelocityCheckStrategy.h"

namespace PJ
{
    AFC_VelocityCheckStrategy::AFC_VelocityCheckStrategy( const CString& tableName ) : AFC_CheckStrategy( tableName )
    {
        V_min = NORMAL_MIN_VELOCITY; // 0.25m/s
        V_max = NORMAL_MAX_VELOCITY; // 4m/s
    }

    void AFC_VelocityCheckStrategy::setArea( double area )
    {
        this->area = area;
    }

    void AFC_VelocityCheckStrategy::setMinMaxVelocity( MIN_VELOCITY V_min, MAX_VELOCITY V_max )
    {
        this->V_min = V_min;
        this->V_max = V_max;
    }

    void AFC_VelocityCheckStrategy::setVelocityCheckType( VelocityCheckType vct )
    {
        this->vct = vct;
    }

    double AFC_VelocityCheckStrategy::getMinVelocity() const
    {
        double v = 0.25;
        switch( V_min )
        {
        case SMALL_MIN_VELOCITY:
            v = 0.15;
            break;
        case NORMAL_MIN_VELOCITY:
            v = 0.25;
            break;
        case LARGE_MIN_VELOCITY:
            v = 0.5;
            break;
        case MORE_MIN_VELOCITY:
            v = 1.0;
            break;
        default:
            v = 0.25;
            break;
        }
        return v;
    }

    double AFC_VelocityCheckStrategy::getMaxVelocity() const
    {
        double v = 4;
        switch( V_max )
        {
        case NORMAL_MAX_VELOCITY:
            v = 4;
            break;
        case LARGE_MAX_VELOCITY:
            v = 5;
            break;
        case NO_LIMIT_MAX_VELOCITY:
            v = 100;
            break;
        default:
            v = 4;
            break;
        }
        return v;
    }

    double AFC_VelocityCheckStrategy::minQ()
    {
        return getMinVelocity() * area * 60;
    }

    double AFC_VelocityCheckStrategy::maxQ()
    {
        if( getMaxVelocity() == 100 ) return AFC_CheckStrategy::maxQ();

        return getMaxVelocity() * area * 60;
    }

    void AFC_VelocityCheckStrategy::createTableImpl( TableCreator* pTableCreator )
    {
        AFC_CheckStrategy::createTableImpl( pTableCreator );

        BEGIN_DEFINE_FIELD( TableCreator, pTableCreator )
        DEFINE_REAL_FIELD( area )
        DEFINE_INT_FIELD( vct )
        END_DEFINE_FIELD
    }

    void AFC_VelocityCheckStrategy::writeToTableImpl( DataWriter* pDataWriter )
    {
        AFC_CheckStrategy::writeToTableImpl( pDataWriter );

        BEGIN_WRITE_DATA( DataWriter, pDataWriter )
        WRITE_SIMPLE_REAL_DATA( area )
        WRITE_SIMPLE_INT_DATA( vct )
        END_WRITE_DATA
    }
}