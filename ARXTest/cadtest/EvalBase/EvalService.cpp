#include "StdAfx.h"
#include "EvalService.h"
#include "EvalNames.h"
#include "EvalMethod.h"
#include "EvalMethodManager.h"
#include "EvalPluginManager.h"
#include "EvalResultGenerator.h"

namespace PJ
{
    static CDaoDatabase* createDB( const CString& dbName, bool openExist = false )
    {
        CDaoDatabase* pDB = new CDaoDatabase();
        CFileFind ff;
        if( ff.FindFile( dbName ) )
        {
            if( openExist )
            {
                pDB->Open( dbName );
            }
            else
            {
                CFile::Remove( dbName );
                pDB->Create( dbName );
            }
        }
        else
        {
            pDB->Create( dbName );
        }
        return pDB;
    }

    static EvalResultGenerator* CreateEvalResultGenerator( EvalMethod* pEvalMethod )
    {
        EvalResultGenerator* pERG = new EvalResultGenerator( pEvalMethod );
        pERG->setTableName( TOTAL_EVAL_RESULT_TABLE_NAME );
        return pERG;
    }

    EvalService* EvalService::m_pEvalService = 0;

    EvalService::EvalService( void )
    {
        m_pEvalMethodManager = new EvalMethodManager();
        m_pEvalPluginManager = new EvalPluginManager( m_pEvalMethodManager );
    }

    EvalService::~EvalService( void )
    {
        delete m_pEvalPluginManager;
        delete m_pEvalMethodManager;
    }

    EvalService* EvalService::CreateInstance()
    {
        if( m_pEvalService == 0 )
        {
            m_pEvalService = new EvalService();
        }
        return m_pEvalService;
    }

    EvalService* EvalService::GetInstance()
    {
        return m_pEvalService;
    }

    void EvalService::DestroyInstance()
    {
        delete m_pEvalService;
        // ����Ҫ������һ�д��룬����m_pEvalServiceָ��δ֪
        // ����ʱ��������
        m_pEvalService = 0;
    }

    void EvalService::setEvalResultDataBasePath( const CString& dbName )
    {
        m_evalResultDataBasePath = dbName;
    }

    void EvalService::loadPlugin( const CString& arxFilePath )
    {
        TCHAR szDrive[_MAX_DRIVE];
        TCHAR szDir[_MAX_DIR];
        TCHAR szFile[_MAX_FNAME];
        TCHAR szExt[_MAX_EXT];
        _tsplitpath( ( LPCTSTR )arxFilePath, szDrive, szDir, szFile, szExt ); // ���·��

        CString serviceName;
        serviceName.Format( _T( "%s_SERVICE_NAME" ), CString( szFile ).MakeUpper() );

        /* ���⣺���(cmsr2010.arx)���ع������޷��ҵ�����ʹ�õ�����dll
         * ��֣���������dllӦ�ôӵ�ǰĿ¼������������˵Ӧ�����ܹ��ҵ���???
         * ��ʱ���������
         *		1) ����������ļ���·�����ӵ�dll����·��
         *		2) ��dll��·�����ӵ�path��������
         *		3) ����ǰ���ز��(�ֶ�����VVLoader���Զ�����)
         */
        // ����arx�����ļ���·��
        TCHAR szPath[_MAX_PATH];
        _tmakepath( szPath, szDrive, szDir, NULL, NULL ); // �ϲ��̷�

        // ����·�����ӵ�dll����·����
        // APIʹ��ǰ�᣺xp sp1(>=0x0502),��stdafx.h���޸�
        SetDllDirectory( szPath );

        m_pEvalPluginManager->load( arxFilePath, serviceName );

        //TCHAR szDllPath[_MAX_PATH];
        //GetDllDirectory(_MAX_PATH, szDllPath);
        //AfxMessageBox(szDllPath);

        // ��ԭdll����·��
        SetDllDirectory( NULL );

        // ��ӡ��ʾ·��Ϊ��
        //     ע��191�汾�ܹ��������У�������ʾ·��Ҳ��Ϊ��(Ĭ��)
        //TCHAR szDllPath2[_MAX_PATH];
        //GetDllDirectory(_MAX_PATH, szDllPath2);
        //AfxMessageBox(szDllPath2);
    }

    void EvalService::unloadPlugin( const CString& arxFilePath )
    {
        m_pEvalPluginManager->unload( arxFilePath );
    }

    void EvalService::getPluginInfo( const CString& arxFilePath, EvalPluginInfo& info )
    {
        m_pEvalPluginManager->getPluginInfo( arxFilePath, info );
    }

    void EvalService::getAllLoadedPluginPaths( AcStringArray& paths )
    {
        m_pEvalPluginManager->getAllLoadedPluginPaths( paths );
    }

    bool EvalService::isRegulationEvalImpl( const CString& regulation ) const
    {
        EvalMethod* pEvalMethod = m_pEvalMethodManager->getEvalMethodByName( regulation );
        return ( pEvalMethod != 0 );
    }

    bool EvalService::isClauseEvalImpl( const CString& regulation, int clauseNum ) const
    {
        // �������û��ʵ�ֻ�����δע��
        EvalMethod* pEvalMethod = m_pEvalMethodManager->getEvalMethodByName( regulation );
        if( pEvalMethod == 0 ) return false;

        EvalFactory* pFactory = pEvalMethod->getEvalFactoryByClauseNum( clauseNum );
        return ( pFactory != 0 );
    }

    bool EvalService::doRegEval( const CString& regulation, const AcDbIntArray& clauses )
    {
        // �������û��ʵ�ֻ�����δע��
        EvalMethod* pEvalMethod = m_pEvalMethodManager->getEvalMethodByName( regulation );
        if( pEvalMethod == 0 ) return false;

        EvalResultGenerator* pEvalResultGenerator = CreateEvalResultGenerator( pEvalMethod );

        // �������ݿ��������
        CDaoDatabase* pDB = createDB( m_evalResultDataBasePath, false ); // ÿ�����۶��ؽ����ݿ�(ɾ���Ѵ��ڵ����ݿ�)
        TableCreator* pTableCreator = new TableCreator( pDB );
        DataWriter* pDataWriter = new DataWriter( pDB );

        int len = clauses.length();
        for( int i = 0; i < len; i++ )
        {
            pEvalResultGenerator->startEval();
            pEvalResultGenerator->setClauseNum( clauses[i] );
            pEvalResultGenerator->initEvalData();
            pEvalResultGenerator->createTable( pTableCreator );
            pEvalResultGenerator->initEvalObject();
            pEvalResultGenerator->writeToTable( pDataWriter );
            pEvalResultGenerator->endEval();
        }

        delete pTableCreator;
        delete pDataWriter;

        pDB->Close();
        delete pDB;

        delete pEvalResultGenerator;

        return true;
    }
}