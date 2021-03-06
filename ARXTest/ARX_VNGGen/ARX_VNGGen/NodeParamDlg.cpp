#include "stdafx.h"
#include "NodeParamDlg.h"
#include "DefParam.h"

IMPLEMENT_DYNAMIC( NodeParamDlg, CDialog )

NodeParamDlg::NodeParamDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( NodeParamDlg::IDD, pParent )
    , m_nodeWidth( 0 )
    , m_nodeHeight( 0 )
    , m_nodeTextHeight( 0 )
{

}

NodeParamDlg::~NodeParamDlg()
{
}

void NodeParamDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Text( pDX, IDC_VNG_GEN_NODE_TEXT_HEIGHT, m_nodeTextHeight );
    DDX_Text( pDX, IDC_VNG_GEN_NODE_WIDTH, m_nodeWidth );
    DDX_Text( pDX, IDC_VNG_GEN_NODE_HEIGHT, m_nodeHeight );
}


BEGIN_MESSAGE_MAP( NodeParamDlg, CDialog )
    ON_BN_CLICKED( IDOK, &NodeParamDlg::OnBnClickedOk )
    ON_BN_CLICKED( IDC_NODE_DEF_BTN, &NodeParamDlg::OnBnClickedNodeDefBtn )
END_MESSAGE_MAP()


BOOL NodeParamDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_nodeWidth = np.m_nodeWidth;
    m_nodeHeight = np.m_nodeHeight;
    m_nodeTextHeight = np.m_nodeTextHeight;

    UpdateData( FALSE );

    return TRUE;
}

void NodeParamDlg::OnBnClickedOk()
{
    if( UpdateData( TRUE ) )
    {
        np.m_nodeWidth = m_nodeWidth;
        np.m_nodeHeight = m_nodeHeight;
        np.m_nodeTextHeight = m_nodeTextHeight;

        OnOK();
    }
}

void NodeParamDlg::OnBnClickedNodeDefBtn()
{
    m_nodeWidth = DEF_NODE_WIDTH;
    m_nodeHeight = DEF_NODE_HEIGHT;
    m_nodeTextHeight = DEF_NODE_TEXT_HEIGHT;
    UpdateData( FALSE );
}
