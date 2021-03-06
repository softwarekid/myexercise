#include <stdafx.h>
#include "structdef.h"

IMPLEMENT_SERIAL( CTriSet,  CObject, 1 )

CTriSet::CTriSet()
{
    //Do something
}

CTriSet::~CTriSet()
{
    //Do something
    theTriSet.RemoveAll();
}

void CTriSet::Serialize( CArchive& ar )
{
    int nCount;
    CObject::Serialize( ar );
    if( ar.IsStoring() )
        ar << ibcount;
    else
        ar >> ibcount;

    if ( ar.IsStoring() )
    {
        nCount = ( WORD )theTriSet.GetSize();
        ar << nCount;
        for( int i = 0; i < nCount; i++ )
        {
            ar << ( WORD )theTriSet[i].ib1.enumDotSource;
            ar << theTriSet[i].ib1.iFaultID;
            ar << theTriSet[i].ib1.index;
            ar << ( WORD )theTriSet[i].ib2.enumDotSource;
            ar << theTriSet[i].ib2.iFaultID;
            ar << theTriSet[i].ib2.index;
            ar << ( WORD )theTriSet[i].ib3.enumDotSource;
            ar << theTriSet[i].ib3.iFaultID;
            ar << theTriSet[i].ib3.index;
        }
    }
    else
    {
        ar >> nCount;
        for( int i = 0; i < nCount; i++ )
        {
            STriangle tri;
            SDot dot;
            WORD w;
            ar >> w;
            dot.enumDotSource = ( EDotSource )w;
            ar >> dot.iFaultID;
            ar >> dot.index;
            tri.ib1 = dot;
            ar >> w;
            dot.enumDotSource = ( EDotSource )w;
            ar >> dot.iFaultID;
            ar >> dot.index;
            tri.ib2 = dot;
            ar >> w;
            dot.enumDotSource = ( EDotSource )w;
            ar >> dot.iFaultID;
            ar >> dot.index;
            tri.ib3 = dot;
            theTriSet.Add( tri );
        }
    }
}

