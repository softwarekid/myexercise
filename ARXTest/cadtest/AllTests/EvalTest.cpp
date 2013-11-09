#include "StdAfx.h"
#include "EvalTest.h"

#include "../EvalBase/EvalService.h"

#include "FilePathHelper.h"

void EvalTest::eval_test()
{
    using namespace PJ;
    EvalService* pEvalService = EvalService::CreateInstance();

    // ���ز��
    pEvalService->loadPlugin( BuildPath( GetAppPathDir(), _T( "cmsr2010.arx" ) ) );

    // ����������ݿ�·��
    pEvalService->setEvalResultDataBasePath( BuildPath( GetAppPathDir(), _T( "pj.mdb" ) ) );

    int start = 100, end = 132;
    AcDbIntArray clauses;
    for( int i = start; i <= end; i++ )
    {
        clauses.append( i );
    }
    pEvalService->doRegEval( _T( "cmsr2004" ), clauses );

    EvalService::DestroyInstance();
}