#include "StdAfx.h"
#include "RTC_AFCStrategy.h"

namespace PJ
{
    RTC_AFCStrategy::RTC_AFCStrategy( const CString& tableName ) : AFCStrategy( tableName )
    {
    }

    void RTC_AFCStrategy::setRTCNum( int num )
    {
        this->num = num;
    }

    void RTC_AFCStrategy::setRTCPower( double power )
    {
        this->power = power;
    }

    double RTC_AFCStrategy::getK() const
    {
        if( num == 1 ) return 1.0;
        if( num == 2 ) return 0.75;
        return 0.5;
    }

    double RTC_AFCStrategy::caculate()
    {
        return 4 * num * power * getK() * 1.36;
    }

    void RTC_AFCStrategy::createTableImpl( TableCreator* pTableCreator )
    {
        AFCStrategy::createTableImpl( pTableCreator );

        BEGIN_DEFINE_FIELD( TableCreator, pTableCreator )
        DEFINE_INT_FIELD( num )
        DEFINE_REAL_FIELD( power )
        DEFINE_REAL_FIELD( k ) // ���ϵ��
        END_DEFINE_FIELD
    }

    void RTC_AFCStrategy::writeToTableImpl( DataWriter* pDataWriter )
    {
        AFCStrategy::writeToTableImpl( pDataWriter );

        BEGIN_WRITE_DATA( DataWriter, pDataWriter )
        WRITE_SIMPLE_INT_DATA( num )
        WRITE_SIMPLE_REAL_DATA( power )
        WRITE_REAL_DATA( k, getK() )
        END_WRITE_DATA
    }
}