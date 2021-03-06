#include "StdAfx.h"
#include "DaoData.h"

#include "DaoDataNames.h"

DaoData::DaoData() : current_id( 0 )
{
}

DaoData::DaoData( const CString& tableName ) : current_id( 0 ), m_tableName( tableName )
{

}

CString DaoData::getTableName() const
{
    return m_tableName;
}

void DaoData::setTableName( const CString& tableName )
{
    m_tableName = tableName;
}

long DaoData::getCurrentRecordID() const
{
    return current_id;
}

void DaoData::doCreateTable( TableCreator* pTableCreator )
{
    if( !pTableCreator->isTableExist( getTableName() ) )
    {
        pTableCreator->createTable( getTableName() );
        pTableCreator->addPrimaryKeyField( ID ); // 强制添加一个主键: id
        createTableImpl( pTableCreator );
        pTableCreator->closeTable();
    }
}

void DaoData::doWriteToTable( DataWriter* pDataWriter )
{
    pDataWriter->beginWriteData( getTableName() );
    writeToTableImpl( pDataWriter );
    current_id = pDataWriter->endWriteData();
}

void DaoData::createTable( TableCreator* pTableCreator )
{
    doCreateTable( pTableCreator );
}

void DaoData::writeToTable( DataWriter* pDataWriter )
{
    doWriteToTable( pDataWriter );
}
