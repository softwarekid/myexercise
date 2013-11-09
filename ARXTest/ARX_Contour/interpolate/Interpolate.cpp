#include "StdAfx.h"
#include "Interpolate.h"

#include "../Contour/Interpolate.h"

#include "KrigingInterpolate.h"
#include "NaturalNeighborInterpolate.h"
#include "RadialInterpolate.h"

// 4�ַ���
// 1 - LinearInterpolatePoints
// 2 - KrigingInterpolatePoints
// 3 - NaturalNeighInterpolatePoints
// 4 - RadialInterpolatePoints
#define USE_IPM 2

double ARX_InterpolatePoint( const AcGePoint3dArray& datas, const AcGePoint3d& pt )
{
    if( USE_IPM == 1 ) return LinearInterpolatePoint( datas, pt );
    if( USE_IPM == 2 ) return RadialInterpolatePoint( datas, pt );
    if( USE_IPM == 3 ) return KrigingInterpolatePoint( datas, pt );
    if( USE_IPM == 4 ) return NaturalNeighInterpolatePoint( datas, pt );
}

void ARX_InterpolatePoints( const AcGePoint3dArray& datas, AcGePoint3dArray& pts )
{
    if( USE_IPM == 1 ) LinearInterpolatePoints( datas, pts );
    if( USE_IPM == 2 ) RadialInterpolatePoints( datas, pts );
    if( USE_IPM == 3 ) KrigingInterpolatePoints( datas, pts );
    if( USE_IPM == 4 ) NaturalNeighInterpolatePoints( datas, pts );
}