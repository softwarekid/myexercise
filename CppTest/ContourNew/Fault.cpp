// Fault.cpp: implementation of the CFault class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "contour.h"
#include "Fault.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFault::CFault()
{

}

CFault::~CFault()
{
    x.RemoveAll();
    y.RemoveAll();
    z.RemoveAll();
}

