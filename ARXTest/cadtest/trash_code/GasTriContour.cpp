#include "StdAfx.h"
#include "GasTriContour.h"

ACRX_DXF_DEFINE_MEMBERS (
    GasFlowTriContour, TriContour,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    GASFLOWTRICONTOUR, CONTOURAPP
)

ACRX_DXF_DEFINE_MEMBERS (
    GasContentTriContour, TriContour,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    GASCONTENTTRICONTOUR, CONTOURAPP
)

ACRX_DXF_DEFINE_MEMBERS (
    GasPressureTriContour, TriContour,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation,
    GASPRESSURETRICONTOUR, CONTOURAPP
)

GasFlowTriContour::GasFlowTriContour() : TriContour()
{

}

GasContentTriContour::GasContentTriContour() : TriContour()
{

}

GasPressureTriContour::GasPressureTriContour() : TriContour()
{

}