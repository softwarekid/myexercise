#include "StdAfx.h"
#include "GambitParamHelper.h"
#include "../ArxHelper/HelperClass.h"

void GambitParamHelper::InitParams()
{
    // 词典存在
    if( ArxDataTool::IsDictExist( FFC_GAMBIT_PARAM_DICT ) ) return;

    ArxDataTool::RegDict( FFC_GAMBIT_PARAM_DICT );

    CString str;

    ArxDictTool* pDictTool = ArxDictTool::GetDictTool( FFC_GAMBIT_PARAM_DICT );

    ArxUtilHelper::DoubleToString( DEF_FFC_GAMBIT_WIDTH, str );
    pDictTool->addEntry( FFC_GAMBIT_WIDTH_KEY, str );

    delete pDictTool;
}

// 设置默认风门参数
static void SetDefGambitParam( GambitParam& gp )
{
    gp.width = DEF_FFC_GAMBIT_WIDTH;
}

void GambitParamHelper::ReadGambitParam( GambitParam& gp )
{
    SetDefGambitParam( gp );

    ArxDictTool* pDictTool = ArxDictTool::GetDictTool( FFC_GAMBIT_PARAM_DICT );

    CString str;

    pDictTool->getEntry( FFC_GAMBIT_WIDTH_KEY, 1, str );
    ArxUtilHelper::StringToDouble( str, gp.width );

    delete pDictTool;
}

void GambitParamHelper::WriteGambitParam( const GambitParam& gp )
{
    CString str;

    ArxDictTool* pDictTool = ArxDictTool::GetDictTool( FFC_GAMBIT_PARAM_DICT );

    ArxUtilHelper::DoubleToString( gp.width, str );
    pDictTool->modifyEntry( FFC_GAMBIT_WIDTH_KEY, 1, str );

    delete pDictTool;
}