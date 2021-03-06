#include "StdAfx.h"
#include "DataDlgManager.h"

DataDlgManager::DataDlgManager () : m_last_pos( -1 )
{
}

DataDlgManager::~DataDlgManager ()
{
    int n = m_dlgArray.length();
    for( int i = 0; i < n; i++ )
    {
        m_dlgArray[i]->DestroyWindow();
        delete m_dlgArray[i];
    }
    m_dlgArray.removeAll();
}

void DataDlgManager::regDlg( const CString& type, PropertyData_DockBarChildDlg* pDlg )
{
    m_types.append( type );
    m_dlgArray.append( pDlg );
}

static CString GetTypeName( const AcDbObjectId& objId )
{
    AcTransaction* pTrans = actrTransactionManager->startTransaction();
    if( pTrans == 0 ) return _T( "" );

    AcDbObject* pObj;
    if( Acad::eOk != pTrans->getObject( pObj, objId, AcDb::kForRead ) )
    {
        actrTransactionManager->abortTransaction();
        return _T( "" );
    }

    CString type = pObj->isA()->name();

    actrTransactionManager->endTransaction();

    return type;
}

void DataDlgManager::load( const AcDbObjectId& objId )
{
    // 保存原来的数据
    if( m_last_pos != -1 )
    {
        m_dlgArray[m_last_pos]->updatePropertyData( true );
        m_dlgArray[m_last_pos]->ShowWindow( SW_HIDE );
    }

    if( !objId.isNull() )
    {
        int pos = m_types.find( GetTypeName( objId ) );
        if( pos != -1 )
        {
            m_dlgArray[pos]->m_objId = objId;
            m_dlgArray[pos]->updatePropertyData( false );

            // 跟随鼠标
            POINT pt;
            GetCursorPos( &pt );
            CRect rect;
            m_dlgArray[pos]->GetWindowRect( &rect );
            m_dlgArray[pos]->MoveWindow( pt.x + 100, pt.y - 100, rect.Width(), rect.Height() );
            m_dlgArray[pos]->ShowWindow( SW_SHOW );

            m_last_pos = pos;
            m_last_objId = objId;
        }
    }
}