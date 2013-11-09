#include "StdAfx.h"
#include "DaoTest.h"

#include "../DaoHelper/TableCreator.h"
#include "../DaoHelper/DataWriter.h"

void DaoTest::dao_test()
{
    CString dbName = _T( "test.mdb" );
    CString tableName = _T( "cacl_result" );
    CDaoDatabase db;
    db.Create( dbName );
    // 创建表
    TableCreator* pTableCreator = new TableCreator( &db );
    if( !pTableCreator->isTableExist( tableName ) )
    {
        pTableCreator->createTable( tableName );
        pTableCreator->addPrimaryKeyField( "id" );
        pTableCreator->addIntField( "afc_id" );
        pTableCreator->addStringField( "afc_type" );
    }
    pTableCreator->closeTable();
    delete pTableCreator;

    // 写入数据
    DataWriter* pDataWriter = new DataWriter( &db );
    long id;
    CString msg;

    pDataWriter->beginWriteData( tableName );
    pDataWriter->writeIntData( _T( "afc_id" ), 1 );
    pDataWriter->writeStringData( _T( "afc_type" ), _T( "ws" ) );
    id = pDataWriter->endWriteData();
    /*msg.Format("主键编号：%ld", id);
    AfxMessageBox(msg);*/

    pDataWriter->beginWriteData( tableName );
    pDataWriter->writeIntData( _T( "afc_id" ), 11 );
    pDataWriter->writeStringData( _T( "afc_type" ), _T( "tws" ) );
    id = pDataWriter->endWriteData();
    /*msg.Format("主键编号：%ld", id);
    AfxMessageBox(msg);*/

    delete pDataWriter;
    db.Close();
}
