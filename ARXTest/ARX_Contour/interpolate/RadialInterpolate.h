#pragma once

extern double RadialInterpolatePoint(const AcGePoint3dArray& datas, const AcGePoint3d& pt);

extern void RadialInterpolatePoints(const AcGePoint3dArray& datas, AcGePoint3dArray& pts);
