#include "stdafx.h"

#include <ctime>
#include "../ArxHelper/HelperClass.h"

// insert函数的bug???
// http://forums.autodesk.com/t5/Autodesk-ObjectARX/Problem-with-inserting-blocks-from-other-DWG/m-p/2846814#M27764
// http://bbs.mjtd.com/thread-27378-1-1.html
// http://www.mouldsky.com/archiver/tid-66211.html
// http://hi.baidu.com/slyrabbit/blog/item/447b2aff0b69623b5c600883.html
// 1) 它把原来的块复制了一份，尽管指定了新的块名，但新的块定义中根本就没有数据!!!
// 2) 每执行一次，它都会将所有的块复制1份???


// 没有采用上面的办法，用了一个蠢办法，只执行一次insert

// 从dwg文件中读取所有的块
static void ReadAllBlocks( AcDbDatabase* pDB, AcStringArray& allBlocks )
{
    AcDbBlockTable* pBlkTbl;
    if( Acad::eOk != pDB->getBlockTable( pBlkTbl, AcDb::kForRead ) ) return;

    AcDbBlockTableIterator* pBlockTableIterator;
    pBlkTbl->newIterator( pBlockTableIterator );
    for ( ; !pBlockTableIterator->done(); pBlockTableIterator->step() )
    {
        AcDbBlockTableRecord* pRecord;
        if( Acad::eOk == pBlockTableIterator->getRecord( pRecord, AcDb::kForRead ) )
        {
            AcString str;
            if( Acad::eOk == pRecord->getName( str ) )
            {
                // 排除所有*开头的块定义
                //    1) 系统自带的3个默认块(模型空间、图纸空间)
                //    2) 使用标注的时候，也会生成带有*号的块
                if( str.mid( 0, 1 ) != _T( "*" ) )
                {
                    allBlocks.append( str );
                }
            }
            pRecord->close();
        }
    }
    delete pBlockTableIterator;
    pBlkTbl->close();
}

static void GetBlcokIds( AcDbDatabase* pDB, const AcStringArray& allBlocks, AcDbObjectIdArray& blockIds )
{
    int n = allBlocks.length();
    for( int i = 0; i < n; i++ )
    {
        blockIds.append( AcDbObjectId::kNull );
    }

    AcDbBlockTable* pBlkTbl;
    if( Acad::eOk != pDB->getBlockTable( pBlkTbl, AcDb::kForRead ) ) return;

    for( int i = 0; i < n; i++ )
    {
        AcDbBlockTableRecord* pRecord;
        if( Acad::eOk == pBlkTbl->getAt( allBlocks[i], pRecord, AcDb::kForRead ) )
        {
            AcString blkName;
            pRecord->getName( blkName );

            int pos = allBlocks.find( blkName );
            if( pos != -1 )
            {
                blockIds[i] = pRecord->objectId();
            }

            pRecord->close();
        }
    }

    pBlkTbl->close();
}

static void InsertBlock( AcDbDatabase* pSourceDB, AcDbDatabase* pDestDB )
{
    CString blockName;
    blockName.Format( _T( "临时_%ld" ), ( long )time( NULL ) );
    acutPrintf( _T( "\n临时块名:%s" ), blockName );

    AcDbObjectId blockId;
    if( Acad::eOk == pDestDB->insert( blockId, blockName, pSourceDB ) )
    {
        acutPrintf( _T( "\n执行insert操作成功!" ) );
        ArxEntityHelper::EraseObject2( blockId, Adesk::kTrue );
    }
    else
    {
        acutPrintf( _T( "\n执行insert操作失败!" ) );
    }
}

static void InsertBlock( AcDbDatabase* pSourceDB, AcDbDatabase* pDestDB,
                         const AcStringArray& allBlocks, const CString& suffix,
                         AcDbObjectIdArray& newBlockIds )
{
    int n = allBlocks.length();
    for( int i = 0; i < 1; i++ )
    {
        CString blkName = allBlocks[i].kACharPtr();

        // 新的临时块名
        CString newBlkName;
        newBlkName.Format( _T( "%s%s" ), blkName, suffix );

        AcDbObjectId blockId;
        if( Acad::eOk == pDestDB->insert( blockId, blkName, newBlkName, pSourceDB ) )
        {
            acutPrintf( _T( "\ninsert块:%s 成功!" ), newBlkName );
        }
        else
        {
            acutPrintf( _T( "\ninsert块:%s 失败!" ), newBlkName );
        }
        newBlockIds.append( blockId );
    }
}

static void InsertBlock2( AcDbDatabase* pSourceDB, AcDbDatabase* pDestDB,
                          const CString& blkName, const CString& suffix,
                          AcDbObjectId& newBlockId )
{
    // 新的临时块名
    CString newBlkName;
    newBlkName.Format( _T( "%s%s" ), blkName, suffix );

    AcDbObjectId blockId;
    if( Acad::eOk == pDestDB->insert( newBlockId, blkName, newBlkName, pSourceDB ) )
    {
        acutPrintf( _T( "\ninsert块:%s 成功!" ), newBlkName );
    }
    else
    {
        acutPrintf( _T( "\ninsert块:%s 失败!" ), newBlkName );
    }
}

static void DeleteBlocks( const AcStringArray& allBlocks )
{
    AcDbBlockTable* pBlkTbl;
    if( Acad::eOk != acdbHostApplicationServices()->workingDatabase()->getBlockTable( pBlkTbl, AcDb::kForRead ) ) return;

    int n = allBlocks.length();
    for( int i = 0; i < n; i++ )
    {
        AcDbBlockTableRecord* pRecord;
        if( Acad::eOk == pBlkTbl->getAt( allBlocks[i], pRecord, AcDb::kForWrite ) )
        {
            pRecord->erase( true );
            pRecord->close();
        }
    }

    pBlkTbl->close();
}

static void RenameBlock( AcDbObjectIdArray& blockIds, const AcStringArray& blockNames, const CString& suffix )
{
    int n = blockIds.length();
    for( int i = 0; i < n; i++ )
    {
        AcDbObject* pObj;
        if( Acad::eOk != acdbOpenObject( pObj, blockIds[i], AcDb::kForWrite ) ) continue;

        AcDbBlockTableRecord* pBlkRecord = AcDbBlockTableRecord::cast( pObj );

        CString blkName;
        blkName.Format( _T( "%s%s" ), blockNames[i].kACharPtr(), suffix );
        pBlkRecord->setName( blkName );

        pObj->close();
    }
}

static void UpdateAllBlockReference( const AcDbObjectIdArray& oldBlockIds, const AcDbObjectIdArray& newBlockIds )
{
    AcDbBlockTable* pBlkTbl;
    if( Acad::eOk != acdbHostApplicationServices()->workingDatabase()->getBlockTable( pBlkTbl, AcDb::kForRead ) ) return;

    AcDbBlockTableRecord* pBlkRecord;
    if( Acad::eOk == pBlkTbl->getAt( ACDB_MODEL_SPACE, pBlkRecord, AcDb::kForRead ) )
    {
        AcDbBlockTableRecordIterator* pIterator;
        pBlkRecord->newIterator( pIterator );
        for( ; !pIterator->done(); pIterator->step() )
        {
            AcDbEntity* pEnt;
            if( Acad::eOk != pIterator->getEntity( pEnt, AcDb::kForWrite ) ) continue;

            AcDbBlockReference* pRef = AcDbBlockReference::cast( pEnt );
            if( pRef != 0 )
            {
                int pos = oldBlockIds.find( pRef->blockTableRecord() );
                if( pos != -1 )
                {
                    AcDbObjectId newBlockId = newBlockIds[pos];
                    if( !newBlockId.isNull() )
                    {
                        pRef->setBlockTableRecord( newBlockId ); // 修改块引用指向的块定义
                    }
                }
            }
            pEnt->close();
        }
        delete pIterator;
        pBlkRecord->close();
    }
    pBlkTbl->close();
}

void InsertDwg2( const CString& dwgFileName )
{
    // 读取模板dwg文件到一个空数据库
    AcDbDatabase db( false );
    if( Acad::eOk != db.readDwgFile( dwgFileName ) ) return;

    // 读取模板dwg文件中的所有不是*开头的块定义
    AcStringArray allBlocks;
    ReadAllBlocks( &db, allBlocks );

    // 读取模板dwg文件中的块定义id
    AcDbObjectIdArray oldBlockIds;
    GetBlcokIds( &db, allBlocks, oldBlockIds );

    // 当前打开的数据库
    AcDbDatabase* pCurDB = acdbHostApplicationServices()->workingDatabase();

    AcDbIdMapping idMap;
    idMap.setDestDb( pCurDB );
    pCurDB->wblockCloneObjects( oldBlockIds, pCurDB->blockTableId(), idMap, AcDb::kDrcReplace );
}

// 执行insert命令
void InsertDwg( const CString& dwgFileName )
{
    // 读取模板dwg文件到一个空数据库
    AcDbDatabase db( false );
    if( Acad::eOk != db.readDwgFile( dwgFileName ) ) return;

    // 读取模板dwg文件中的所有不是*开头的块定义
    AcStringArray allBlocks;
    ReadAllBlocks( &db, allBlocks );

    // 当前打开的数据库
    AcDbDatabase* pCurDB = acdbHostApplicationServices()->workingDatabase();

    // 获取所有的块定义id
    AcDbObjectIdArray oldBlockIds;
    GetBlcokIds( pCurDB, allBlocks, oldBlockIds );

    // 当前时间
    long t = ( long )time( NULL );
    CString suffix1, suffix2; // 后缀
    suffix1.Format( _T( "_%ld_1" ), t );
    suffix2.Format( _T( "_%ld_2" ), t );

    // 重命名块定义
    RenameBlock( oldBlockIds, allBlocks, suffix1 );

    AcDbObjectId newBlockId;
    InsertBlock2( &db, acdbHostApplicationServices()->workingDatabase(), allBlocks[0].kACharPtr(), suffix2, newBlockId );

    // 删除插入的新块定义
    ArxEntityHelper::EraseObject2( newBlockId, Adesk::kTrue );

    AcDbObjectIdArray newBlockIds;
    GetBlcokIds( acdbHostApplicationServices()->workingDatabase(), allBlocks, newBlockIds );

    // 更新块引用
    UpdateAllBlockReference( oldBlockIds, newBlockIds );

    // 删除原有的块定义
    ArxEntityHelper::EraseObjects2( oldBlockIds, Adesk::kTrue );
}
