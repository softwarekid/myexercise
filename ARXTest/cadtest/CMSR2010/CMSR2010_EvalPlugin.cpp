#include "StdAfx.h"
#include "CMSR2010_EvalPlugin.h"

//  ú��2010���100������
#include "Eval_100.h"

// ú��2010���101������
#include "Eval_101.h"

// ú��2010���102������
#include "Eval_102.h"

// ú��2010���103������
#include "Eval_103.h"

// ú��2010���105������
#include "Eval_105.h"

// ú��2010���106������
#include "Eval_106.h"

// ú��2010���107������
#include "Eval_107.h"

// ú��2010���109������
#include "Eval_109.h"

// ú��2010���110������
#include "Eval_110.h"

// ú��2010���115������
#include "Eval_115.h"

// �ṩһ��Ĭ������(���۽��Ĭ��"δ֪���")
//#include "DefEval.h"

//#include "Eval_130.h"
//#include "Eval_131.h"
//#include "Eval_132.h"

CMSR2010_EvalPlugin::CMSR2010_EvalPlugin()
{
}

CMSR2010_EvalPlugin::~CMSR2010_EvalPlugin( void )
{
}

void CMSR2010_EvalPlugin::initialize( EvalMethod* pEvalMethod )
{
    BEGIN_REGISTER_EVAL_FACTORY( EvalMethod, pEvalMethod )
    REGISTER_EVAL_FACTORY( 100 )
    REGISTER_EVAL_FACTORY( 101 )
    REGISTER_EVAL_FACTORY( 102 )
    REGISTER_EVAL_FACTORY( 103 )
    /*REGISTER_EVAL_FACTORY(104)*/
    REGISTER_EVAL_FACTORY( 105 )
    REGISTER_EVAL_FACTORY( 106 )
    REGISTER_EVAL_FACTORY( 107 )
    /*REGISTER_EVAL_FACTORY(108)*/
    REGISTER_EVAL_FACTORY( 109 )

    REGISTER_EVAL_FACTORY( 110 )
    /*REGISTER_EVAL_FACTORY(111)
    REGISTER_EVAL_FACTORY(112)
    REGISTER_EVAL_FACTORY(113)
    REGISTER_EVAL_FACTORY(114)*/
    REGISTER_EVAL_FACTORY( 115 )
    /*REGISTER_EVAL_FACTORY(116)
    REGISTER_EVAL_FACTORY(117)
    REGISTER_EVAL_FACTORY(118)
    REGISTER_EVAL_FACTORY(119)

    REGISTER_EVAL_FACTORY(120)
    REGISTER_EVAL_FACTORY(121)
    REGISTER_EVAL_FACTORY(122)
    REGISTER_EVAL_FACTORY(123)
    REGISTER_EVAL_FACTORY(124)
    REGISTER_EVAL_FACTORY(125)
    REGISTER_EVAL_FACTORY(126)
    REGISTER_EVAL_FACTORY(127)
    REGISTER_EVAL_FACTORY(128)
    REGISTER_EVAL_FACTORY(129)

    REGISTER_EVAL_FACTORY(130)
    REGISTER_EVAL_FACTORY(131)
    REGISTER_EVAL_FACTORY(132)*/

    END_REGISTER_EVAL_FACTORY
}

void CMSR2010_EvalPlugin::unInitialize( EvalMethod* pEvalMethod )
{
    BEGIN_UNREGISTER_EVAL_FACTORY( EvalMethod, pEvalMethod )
    UNREGISTER_EVAL_FACTORY( 100 )
    UNREGISTER_EVAL_FACTORY( 101 )
    UNREGISTER_EVAL_FACTORY( 102 )
    UNREGISTER_EVAL_FACTORY( 103 )
    /*UNREGISTER_EVAL_FACTORY(104)*/
    UNREGISTER_EVAL_FACTORY( 105 )
    UNREGISTER_EVAL_FACTORY( 106 )
    UNREGISTER_EVAL_FACTORY( 107 )
    /*UNREGISTER_EVAL_FACTORY(108)*/
    UNREGISTER_EVAL_FACTORY( 109 )

    UNREGISTER_EVAL_FACTORY( 110 )
    /*UNREGISTER_EVAL_FACTORY(111)
    UNREGISTER_EVAL_FACTORY(112)
    UNREGISTER_EVAL_FACTORY(113)
    UNREGISTER_EVAL_FACTORY(114)*/
    UNREGISTER_EVAL_FACTORY( 115 )
    /*UNREGISTER_EVAL_FACTORY(116)
    UNREGISTER_EVAL_FACTORY(117)
    UNREGISTER_EVAL_FACTORY(118)
    UNREGISTER_EVAL_FACTORY(119)

    UNREGISTER_EVAL_FACTORY(120)
    UNREGISTER_EVAL_FACTORY(121)
    UNREGISTER_EVAL_FACTORY(122)
    UNREGISTER_EVAL_FACTORY(123)
    UNREGISTER_EVAL_FACTORY(124)
    UNREGISTER_EVAL_FACTORY(125)
    UNREGISTER_EVAL_FACTORY(126)
    UNREGISTER_EVAL_FACTORY(127)
    UNREGISTER_EVAL_FACTORY(128)
    UNREGISTER_EVAL_FACTORY(129)

    UNREGISTER_EVAL_FACTORY(130)
    UNREGISTER_EVAL_FACTORY(131)
    UNREGISTER_EVAL_FACTORY(132)*/

    END_UNREGISTER_EVAL_FACTORY
}

void CMSR2010_EvalPlugin::getPluginInfo( EvalPluginInfo& epi ) const
{
    epi.name = _T( "cmsr2010" ); // ����Ҫ
    epi.author = _T( "anheihb03dlj" );
    epi.date = _T( "2011_03_15" );
    epi.regulation = _T( "ú��ȫ���2010��" );
    epi.clauseImpl = _T( "100-132" );
}

// ���۲����������ʵ��
EVAL_PLUGIN_CREATE_FUNCTION_IMPL( CMSR2010_EvalPlugin )

// ú��2010���100�������۹�����������ʵ��
EVAL_FACTORY_CREATE_FUNCTION_IMPL( 100, EvalFactory_100 )
EVAL_FACTORY_CREATE_FUNCTION_IMPL( 101, EvalFactory_101 )
EVAL_FACTORY_CREATE_FUNCTION_IMPL( 102, EvalFactory_102 )
EVAL_FACTORY_CREATE_FUNCTION_IMPL( 103, EvalFactory_103 )
/*
EVAL_FACTORY_CREATE_FUNCTION_IMPL(104, EvalFactory_104)
*/
EVAL_FACTORY_CREATE_FUNCTION_IMPL( 105, EvalFactory_105 )
EVAL_FACTORY_CREATE_FUNCTION_IMPL( 106, EvalFactory_106 )
EVAL_FACTORY_CREATE_FUNCTION_IMPL( 107, EvalFactory_107 )
/*EVAL_FACTORY_CREATE_FUNCTION_IMPL(108, EvalFactory_108)
*/
EVAL_FACTORY_CREATE_FUNCTION_IMPL( 109, EvalFactory_109 )
EVAL_FACTORY_CREATE_FUNCTION_IMPL( 110, EvalFactory_110 )
/*
EVAL_FACTORY_CREATE_FUNCTION_IMPL(111, EvalFactory_111)
EVAL_FACTORY_CREATE_FUNCTION_IMPL(112, EvalFactory_112)
EVAL_FACTORY_CREATE_FUNCTION_IMPL(113, EvalFactory_113)
EVAL_FACTORY_CREATE_FUNCTION_IMPL(114, EvalFactory_114)*/
EVAL_FACTORY_CREATE_FUNCTION_IMPL( 115, EvalFactory_115 )
/*EVAL_FACTORY_CREATE_FUNCTION_IMPL(116, EvalFactory_116)
EVAL_FACTORY_CREATE_FUNCTION_IMPL(117, EvalFactory_117)
EVAL_FACTORY_CREATE_FUNCTION_IMPL(118, EvalFactory_118)
EVAL_FACTORY_CREATE_FUNCTION_IMPL(119, EvalFactory_119)

EVAL_FACTORY_CREATE_FUNCTION_IMPL(120, EvalFactory_120)
EVAL_FACTORY_CREATE_FUNCTION_IMPL(121, EvalFactory_121)
EVAL_FACTORY_CREATE_FUNCTION_IMPL(122, EvalFactory_122)
EVAL_FACTORY_CREATE_FUNCTION_IMPL(123, EvalFactory_123)
EVAL_FACTORY_CREATE_FUNCTION_IMPL(124, EvalFactory_124)
EVAL_FACTORY_CREATE_FUNCTION_IMPL(125, EvalFactory_125)
EVAL_FACTORY_CREATE_FUNCTION_IMPL(126, EvalFactory_126)
EVAL_FACTORY_CREATE_FUNCTION_IMPL(127, EvalFactory_127)
EVAL_FACTORY_CREATE_FUNCTION_IMPL(128, EvalFactory_128)
EVAL_FACTORY_CREATE_FUNCTION_IMPL(129, EvalFactory_129)

EVAL_FACTORY_CREATE_FUNCTION_IMPL(130, EvalFactory_130)
EVAL_FACTORY_CREATE_FUNCTION_IMPL(131, EvalFactory_131)
EVAL_FACTORY_CREATE_FUNCTION_IMPL(132, EvalFactory_132)
*/