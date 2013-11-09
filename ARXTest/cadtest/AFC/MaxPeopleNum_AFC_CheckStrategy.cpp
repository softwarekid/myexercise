#include "StdAfx.h"
#include "MaxPeopleNum_AFC_CheckStrategy.h"

namespace PJ
{
    MaxPeopleNum_AFC_CheckStrategy::MaxPeopleNum_AFC_CheckStrategy( const CString& tableName ) : AFC_CheckStrategy( tableName )
    {
    }

    void MaxPeopleNum_AFC_CheckStrategy::setMaxPeopleNum( int num )
    {
        this->maxPeopleNum = num;
    }

    double MaxPeopleNum_AFC_CheckStrategy::minQ()
    {
        return 4 * maxPeopleNum;
    }

    void MaxPeopleNum_AFC_CheckStrategy::createTableImpl( TableCreator* pTableCreator )
    {
        AFC_CheckStrategy::createTableImpl( pTableCreator );

        BEGIN_DEFINE_FIELD( TableCreator, pTableCreator )
        DEFINE_INT_FIELD( maxPeopleNum )
        END_DEFINE_FIELD
    }

    void MaxPeopleNum_AFC_CheckStrategy::writeToTableImpl( DataWriter* pDataWriter )
    {
        AFC_CheckStrategy::writeToTableImpl( pDataWriter );

        BEGIN_WRITE_DATA( DataWriter, pDataWriter )
        WRITE_SIMPLE_INT_DATA( maxPeopleNum )
        END_WRITE_DATA
    }
}