#include "stdafx.h"

#include "../MineGE/LinkedGE.h"
#include "CADDrawHelper.h"

static bool GetSEPt( const AcDbObjectId& objId, AcGePoint3d& spt, AcGePoint3d& ept )
{
    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    if( pTrans == 0 ) return false;

    AcDbObject* pObj;
    if( Acad::eOk != pTrans->getObject( pObj, objId, AcDb::kForRead ) )
    {
        actrTransactionManager->abortTransaction();
        return false;
    }

    LinkedGE* pEdge = LinkedGE::cast( pObj );
    if( pEdge == 0 )
    {
        actrTransactionManager->abortTransaction();
        return false;
    }

    pEdge->getSEPoint( spt, ept );

    actrTransactionManager->endTransaction();

    return true;
}

void DisplayIntValue( const AcDbObjectIdArray& objIds, const AcDbIntArray& ivs )
{
    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        AcGePoint3d spt, ept;
        if( GetSEPt( objIds[i], spt, ept ) )
        {
            AcGeVector3d v = ept - spt;

            AcGeVector3d v2( v );
            v2.normalize();
            v2.rotateBy( PI / 2, AcGeVector3d::kZAxis );

            AcGePoint3d pt = spt + v * 0.5 + v2 * 100;

            //DrawEllipse(pt, 100, 100);
            CString str;
            //str.Format(_T("e%d-%d"), i, ivs[i]);
            str.Format( _T( "%d" ), ivs[i] );
            DrawMText( pt, 0, str, 120 );
        }
    }
}

void DisplayIntValue2( const AcDbObjectIdArray& objIds, const AcDbIntArray& ivs, const AcDbIntArray& ivs2 )
{
    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        AcGePoint3d spt, ept;
        if( GetSEPt( objIds[i], spt, ept ) )
        {
            AcGeVector3d v = ept - spt;
            double angle = v.angleTo( AcGeVector3d::kXAxis, -AcGeVector3d::kZAxis );
            if( v.x < 0 ) angle += PI;

            AcGeVector3d v2( v );
            v2.normalize();
            v2.rotateBy( PI / 2, AcGeVector3d::kZAxis );

            AcGePoint3d pt = spt + v * 0.5 + v2 * 50;

            //DrawEllipse(pt, 100, 100);
            CString str;
            //str.Format(_T("e%d-%d"), i, ivs[i]);
            str.Format( _T( "%d/%d" ), ivs[i], ivs2[i] );
            DrawMText( pt, angle, str, 80 );
        }
    }
}