#include "StdAfx.h"
#include "EvalResult.h"

namespace PJ
{
    EVAL_BASE_EXPORT_API EvalResult ConvertBoolToEvalResult( bool value )
    {
        return ( value ? Qualified : UnQualified );
    }
}