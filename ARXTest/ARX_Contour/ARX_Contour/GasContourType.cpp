#include "stdafx.h"
#include "GasContourType.h"

static CString GasTypeName[3] =
{
    _T( "GFP" ),
    _T( "GCP" ),
    _T( "GPP" )
};

static CString GasFieldName[3] =
{
    _T( "绝对瓦斯涌出量" ),
    _T( "瓦斯含量" ),
    _T( "瓦斯压力值" )
};

CString GetGasTypeName( GasContourType gct )
{
    return GasTypeName[gct];
}

CString GetGasFieldName( GasContourType gct )
{
    return GasFieldName[gct];
}
