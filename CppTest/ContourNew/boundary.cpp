// boundary.cpp: implementation of the CBoundary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "contour.h"
#include "boundary.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBoundary::CBoundary()
{

}

CBoundary::~CBoundary()
{

}

//通过鼠标点击来确定边界点
void CBoundary::BoundByMousePts()
{
    SPoint theSPt;
    theSPt.x = 10;
    theSPt.y = 100;
    theSPt.z = 1000;
    this->m_Points.Add( theSPt );

}