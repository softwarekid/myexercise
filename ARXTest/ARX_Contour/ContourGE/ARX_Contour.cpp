#include "stdafx.h"

#include "../Contour/Contour.h"

void ARX_SmoothPoints( const AcGePoint3dArray& inPts, AcGePoint3dArray& outPts )
{
    PointArray inPa, outPa;
    for( int i = 0; i < inPts.length(); i++ )
    {
        DT_Point p = {inPts[i].x, inPts[i].y, 0};
        inPa.push_back( p );
    }

    // 调用Contour.dll中的光滑算法
    SmoothPoints( inPa, outPa, 15 );

    for( int i = 0; i < outPa.size(); i++ )
    {
        outPts.append( AcGePoint3d( outPa[i].x, outPa[i].y, 0 ) );
    }
}

void ARX_Label( const AcGePoint3dArray& pts, AcGePoint3d& tpt, double& angle )
{
    PointArray cnpts;
    for( int i = 0; i < pts.length(); i++ )
    {
        DT_Point p = {pts[i].x, pts[i].y, 0};
        cnpts.push_back( p );
    }

    // 调用Contour.dll中的光滑算法
    DT_Point p;
    SearchLabelPostion( cnpts, p, angle );

    tpt.x = p.x;
    tpt.y = p.y;
    tpt.z = 0;
}