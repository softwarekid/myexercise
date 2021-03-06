#include "stdafx.h"
#include "EdgeParamDlg.h"
#include "DefParam.h"

IMPLEMENT_DYNAMIC( EdgeParamDlg, CDialog )

EdgeParamDlg::EdgeParamDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( EdgeParamDlg::IDD, pParent )
    , m_arrowWidth( 0 )
    , m_arrowLength( 0 )
    , m_edgeTextHeight( 0 )
    , m_needEdge( TRUE )
{

}

EdgeParamDlg::~EdgeParamDlg()
{
}

void EdgeParamDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Text( pDX, IDC_VNG_GEN_ARROW_WIDTH, m_arrowWidth );
    DDX_Text( pDX, IDC_VNG_GEN_ARROW_LENGTH, m_arrowLength );

    DDX_Text( pDX, IDC_VNG_GEN_EDGE_TEXT_HEIGHT, m_edgeTextHeight );
    DDX_Check( pDX, IDC_VNG_GEN_NEED_EDGE, m_needEdge );
}

BEGIN_MESSAGE_MAP( EdgeParamDlg, CDialog )
    ON_BN_CLICKED( IDOK, &EdgeParamDlg::OnBnClickedOk )
    ON_BN_CLICKED( IDC_EDGE_DEF_BTN, &EdgeParamDlg::OnBnClickedEdgeDefBtn )
END_MESSAGE_MAP()

BOOL EdgeParamDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_arrowWidth = ep.m_arrowWidth;
    m_arrowLength = ep.m_arrowLength;
    m_needEdge = ( int )ep.m_needEdge;
    m_edgeTextHeight = ep.m_edgeTextHeight;

    UpdateData( FALSE );

    return TRUE;
}

void EdgeParamDlg::OnBnClickedOk()
{
    if( UpdateData( TRUE ) )
    {
        ep.m_arrowWidth = m_arrowWidth;
        ep.m_arrowLength = m_arrowLength;
        ep.m_needEdge = ( m_needEdge != 0 );
        ep.m_edgeTextHeight = m_edgeTextHeight;

        OnOK();
    }
}

void EdgeParamDlg::OnBnClickedEdgeDefBtn()
{
    m_arrowWidth = DEF_ARROW_WIDTH;
    m_arrowLength = DEF_ARROW_LENGTH;
    m_needEdge = DEF_NEED_EDGE;
    m_edgeTextHeight = DEF_EDGE_TEXT_HEIGHT;
    UpdateData( FALSE );
}
