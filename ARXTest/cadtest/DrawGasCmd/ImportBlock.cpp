#include "stdafx.h"

#include <ctime>
#include "../ArxHelper/HelperClass.h"

// insert������bug???
// http://forums.autodesk.com/t5/Autodesk-ObjectARX/Problem-with-inserting-blocks-from-other-DWG/m-p/2846814#M27764
// http://bbs.mjtd.com/thread-27378-1-1.html
// http://www.mouldsky.com/archiver/tid-66211.html
// 1) ����ԭ���Ŀ鸴����һ�ݣ�����ָ�����µĿ��������µĿ鶨���и�����û������!!!
// 2) ÿִ��һ�Σ������Ὣ���еĿ鸴��1��???

// http://hi.baidu.com/slyrabbit/blog/item/447b2aff0b69623b5c600883.html (���!!!)

// ��dwg�ļ��ж�ȡ���еĿ�
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
                // �ų�����*��ͷ�Ŀ鶨��
                //    1) ϵͳ�Դ���3��Ĭ�Ͽ�(ģ�Ϳռ䡢ͼֽ�ռ�)
                //    2) ʹ�ñ�ע��ʱ��Ҳ�����ɴ���*�ŵĿ�
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

bool InsertDwg( const CString& dwgFilePath )
{
    // ��ȡģ��dwg�ļ���һ�������ݿ�
    AcDbDatabase db( false );
    if( Acad::eOk != db.readDwgFile( dwgFilePath ) ) return false;

    // ��ȡģ��dwg�ļ��е����в���*��ͷ�Ŀ鶨��
    AcStringArray allBlocks;
    ReadAllBlocks( &db, allBlocks );

    // ��ȡģ��dwg�ļ��еĿ鶨��id
    AcDbObjectIdArray oldBlockIds;
    GetBlcokIds( &db, allBlocks, oldBlockIds );

    // ��ǰ�򿪵����ݿ�
    AcDbDatabase* pCurDB = acdbHostApplicationServices()->workingDatabase();

    AcDbIdMapping idMap;
    idMap.setDestDb( pCurDB );
    return ( Acad::eOk ==
             pCurDB->wblockCloneObjects( oldBlockIds, pCurDB->blockTableId(), idMap, AcDb::kDrcReplace ) );
}

// ��ȡ��ǰģ���·��
static CString GetAppPathDir()
{
    TCHAR szMoudlePath[_MAX_PATH];
    GetModuleFileName( _hdllInstance, szMoudlePath, _MAX_PATH );

    TCHAR drive[_MAX_DRIVE];
    TCHAR dir[_MAX_DIR];
    _tsplitpath( szMoudlePath, drive, dir, NULL, NULL );

    TCHAR path[_MAX_PATH];
    _tmakepath( path, drive, dir, NULL, NULL );

    return CString( path );
}

// ����·��
static CString BuildPath( const CString& dir, const CString& fileName )
{
    CString path;
    path.Format( _T( "%s%s" ), dir, fileName );
    return path;
}

// ������˹����ͼ�鶨��
// �ӵ�ǰģ�����Ŀ¼�¼���
void UpdateDwgBlock()
{
    //CString dwgFilePath = _T("C:\\Users\\anheihb03dlj\\Desktop\\��˹����ͼ�鶨��.dwg");
    CString dwgFilePath = BuildPath( GetAppPathDir(), _T( "��˹����ͼ�鶨��.dwg" ) );
    if( InsertDwg( dwgFilePath ) )
    {
        acutPrintf( _T( "\n���¿�ɹ�!\n" ) );
        acedCommand( RTSTR, _T( "REGEN" ), 0 ); // ִ��regen����
    }
    else
    {
        acutPrintf( _T( "\n���¿�ʧ��!\n" ) );
    }
}