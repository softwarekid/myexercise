#include "StdAfx.h"
#include "DataObjectModifiedReactor.h"

#include "DataObject.h"
#include "../ArxHelper/HelperClass.h"

DataObjectModifiedReactor::DataObjectModifiedReactor ( AcDbDatabase* pDb ) : AcDbDatabaseReactor(), mpDatabase( pDb )
{
    if ( pDb )
    {
        //acutPrintf(_T("\nMineGEErase_DbReactor : %ld"), (long)pDb);
        pDb->addReactor ( this ) ;
    }
}

DataObjectModifiedReactor::~DataObjectModifiedReactor ()
{
    Detach () ;
}

void DataObjectModifiedReactor::Attach ( AcDbDatabase* pDb )
{
    Detach () ;
    if ( mpDatabase == NULL )
    {
        if ( ( mpDatabase = pDb ) != NULL )
            pDb->addReactor ( this ) ;
    }
}

void DataObjectModifiedReactor::Detach ()
{
    if ( mpDatabase )
    {
        mpDatabase->removeReactor ( this ) ;
        mpDatabase = NULL ;
    }
}

AcDbDatabase* DataObjectModifiedReactor::Subject () const
{
    return ( mpDatabase ) ;
}

bool DataObjectModifiedReactor::IsAttached () const
{
    return ( mpDatabase != NULL ) ;
}

// 在objectModified中无法启动事务
void DataObjectModifiedReactor::objectModified( const AcDbDatabase* dwg, const AcDbObject* dbObj )
{
    AcDbDatabaseReactor::objectModified ( dwg, dbObj );
    if( dbObj->isKindOf( DataObject::desc() ) )
    {
        DataObject* pDO = DataObject::cast( dbObj );
        ArxEntityHelper::UpdateEntity( pDO->getGE() ); // 强制更新显示效果
    }
}