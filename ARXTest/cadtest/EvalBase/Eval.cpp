#include "StdAfx.h"
#include "Eval.h"
#include "HelperMacro.h"

#include "EvalNames.h"

namespace PJ
{
    EvalData::EvalData()
    {

    }

    void EvalData::createTableImpl( TableCreator* pTableCreator )
    {
        pTableCreator->addIntField( EVAL_RESULT_FIELD );
    }

    void EvalData::writeToTableImpl( DataWriter* pDataWriter )
    {
        pDataWriter->writeIntData( EVAL_RESULT_FIELD, eval_result );
    }
}