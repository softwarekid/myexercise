// ContourDoc.cpp : implementation of the CContourDoc class
//

#include "stdafx.h"
#include "Contour.h"

#include "ContourDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CContourDoc

IMPLEMENT_DYNCREATE( CContourDoc, CDocument )

BEGIN_MESSAGE_MAP( CContourDoc, CDocument )
    //{{AFX_MSG_MAP(CContourDoc)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CContourDoc construction/destruction

CContourDoc::CContourDoc()
{
    // TODO: add one-time construction code here

}

CContourDoc::~CContourDoc()
{
}

BOOL CContourDoc::OnNewDocument()
{
    if ( !CDocument::OnNewDocument() )
        return FALSE;

    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)

    return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CContourDoc serialization

void CContourDoc::Serialize( CArchive& ar )
{
    if ( ar.IsStoring() )
    {
        // TODO: add storing code here
    }
    else
    {
        // TODO: add loading code here
    }
}

/////////////////////////////////////////////////////////////////////////////
// CContourDoc diagnostics

#ifdef _DEBUG
void CContourDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CContourDoc::Dump( CDumpContext& dc ) const
{
    CDocument::Dump( dc );
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CContourDoc commands

