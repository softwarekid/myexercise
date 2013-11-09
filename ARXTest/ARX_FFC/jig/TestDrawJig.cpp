#include "StdAfx.h"

#include "DragJig.h"

// 来源:
// http://www.objectarx.net/forum.php?mod=viewthread&tid=2363&highlight=jig

BOOL PickUpOneEnt( const TCHAR* strPrompt, AcDbObjectId& entId, AcGePoint3d& pickPnt )
{
    ads_name entname;
    int rc = acedEntSel( strPrompt, entname, asDblArray( pickPnt ) );
    switch( rc )
    {
    case RTERROR || RTNONE:
        acutPrintf( _T( "\n选择出错！" ) );
        return FALSE;
    case RTCAN:
        acutPrintf( _T( "\n用户按下了ESC键！" ) );
        return FALSE;
    case RTNORM:
        break;
    default:
        acutPrintf( _T( "\n未知错误！" ) );
        return FALSE;
    }
    if( acdbGetObjectId( entId, entname ) != Acad::eOk )
        return FALSE;
    return TRUE;
}

void lgqMyJigdragLArc()
{
    /*AcGePoint3d pickPnt;
    AcDbObjectId crvId,C1Id,C2Id;
    if(!PickUpOneEnt(_T("\n选择一条多段线"),crvId,pickPnt)) return;
    if(!PickUpOneEnt(_T("\n选择一圆"),C1Id,pickPnt)) return;
    if(!PickUpOneEnt(_T("\n选择一圆"),C2Id,pickPnt)) return;

    DragJig jig(crvId,C1Id,C2Id);
    jig.doIt();*/
}