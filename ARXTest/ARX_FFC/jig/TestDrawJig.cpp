#include "StdAfx.h"

#include "DragJig.h"

// ��Դ:
// http://www.objectarx.net/forum.php?mod=viewthread&tid=2363&highlight=jig

BOOL PickUpOneEnt( const TCHAR* strPrompt, AcDbObjectId& entId, AcGePoint3d& pickPnt )
{
    ads_name entname;
    int rc = acedEntSel( strPrompt, entname, asDblArray( pickPnt ) );
    switch( rc )
    {
    case RTERROR || RTNONE:
        acutPrintf( _T( "\nѡ�������" ) );
        return FALSE;
    case RTCAN:
        acutPrintf( _T( "\n�û�������ESC����" ) );
        return FALSE;
    case RTNORM:
        break;
    default:
        acutPrintf( _T( "\nδ֪����" ) );
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
    if(!PickUpOneEnt(_T("\nѡ��һ�������"),crvId,pickPnt)) return;
    if(!PickUpOneEnt(_T("\nѡ��һԲ"),C1Id,pickPnt)) return;
    if(!PickUpOneEnt(_T("\nѡ��һԲ"),C2Id,pickPnt)) return;

    DragJig jig(crvId,C1Id,C2Id);
    jig.doIt();*/
}