#include "Interpolate.h"

#include "KrigingInterpolate.h"
#include "NaturalNeighborInterpolate.h"
#include "RadialInterpolate.h"

// 4�ַ���
// 1 - LinearInterpolatePoints
// 2 - KrigingInterpolatePoints
// 3 - NaturalNeighInterpolatePoints
// 4 - RadialInterpolatePoints
#define USE_IPM 2

double InterpolatePoint( const PointArray& datas, const DT_Point& pt )
{
    if( USE_IPM == 1 ) return LinearInterpolatePoint( datas, pt );
    if( USE_IPM == 2 ) return RadialInterpolatePoint( datas, pt );
    if( USE_IPM == 3 ) return KrigingInterpolatePoint( datas, pt );
    if( USE_IPM == 4 ) return NaturalNeighInterpolatePoint( datas, pt );
}

void InterpolatePoints( const PointArray& datas, PointArray& pts )
{
    if( USE_IPM == 1 ) LinearInterpolatePoints( datas, pts );
    if( USE_IPM == 2 ) RadialInterpolatePoints( datas, pts );
    if( USE_IPM == 3 ) KrigingInterpolatePoints( datas, pts );
    if( USE_IPM == 4 ) NaturalNeighInterpolatePoints( datas, pts );
}