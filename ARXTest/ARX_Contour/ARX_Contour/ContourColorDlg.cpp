#include "stdafx.h"
#include "ContourColorDlg.h"

#include "ARX_Gas.h"
#include "ContourHelper.h"

#include "../ArxHelper/HelperClass.h"
#include "../MineGE/HelperClass.h"

#include "../ContourGE/ContourInfo.h"

struct ColorListItemData
{
    double z;
    COLORREF rgb;
};

static unsigned long RGB2LONG( unsigned int r, unsigned int g, unsigned int b )
{
    return RGB( r, g, b );
}

static void LONG2RGB( unsigned long rgbColor, unsigned int& r, unsigned int& g, unsigned int& b )
{
    // ����2�δ����ǵȼ۵�

    /*r = ( rgbColor & 0xffL );
    g = ( rgbColor & 0xff00L ) >> 8;
    b = rgbColor >> 16;*/

    r = GetRValue( rgbColor );
    g = GetGValue( rgbColor );
    b = GetBValue( rgbColor );
}

//static BOOL InitEditor(CWnd** pWnd, int nRow, int nColumn, CString& strSubItemText, DWORD_PTR dwItemData, void* pThis, BOOL bUpdate)
//{
//	ASSERT(*pWnd);
//	switch(nColumn)
//	{
//	case 1:
//		{
//			AfxMessageBox(_T("InitEditor"));
//			CColorCombox *pColor = dynamic_cast<CColorCombox *>(*pWnd);
//			pColor->SetCurColor((COLORREF)_tstol(strSubItemText));
//		}
//		break;
//	}
//	return TRUE;
//}
//
//static BOOL EndEditor(CWnd** pWnd, int nRow, int nColumn, CString& strSubItemText, DWORD_PTR dwItemData, void* pThis, BOOL bUpdate)
//{
//	ASSERT(pWnd);
//	switch(nColumn)
//	{
//	case 1:
//		{
//			AfxMessageBox(_T("EndEditor"));
//			CColorCombox *pColor = dynamic_cast<CColorCombox *>(*pWnd);
//			CListCtrlEx *pList = reinterpret_cast<CListCtrlEx *>(pThis);
//
//			COLORREF rgb = pColor->GetCurColor();
//
//			CString value;
//			value.Format(_T("%ld"), (unsigned long)rgb);
//			strSubItemText = value;
//
//			pList->SetCellColors(nRow, nColumn, rgb, -1/*rgb*/);
//		}
//		break;
//	}
//	return TRUE;
//}

static AcDbObjectId GetContourInfoOnLayer( const CString& layer )
{
    AcDbObjectIdArray allObjIds;
    LayerHelper::GetAllEntitiesOnLayer( layer, allObjIds );

    AcDbObjectIdArray objIds;
    ArxDataTool::FilterEntsByType( _T( "ContourInfo" ), allObjIds, objIds );

    if( objIds.isEmpty() )
        return AcDbObjectId::kNull;
    else
        return objIds[0];
}

static void GetContourZValues( const AcDbObjectId& objId, AcGeDoubleArray& zValues )
{
    AcTransaction* pTrans = actrTransactionManager->startTransaction();

    AcDbObject* pObj;
    pTrans->getObject( pObj, objId, AcDb::kForRead );

    ContourInfo* pInfo = ContourInfo::cast( pObj );
    pInfo->getZValues( zValues );

    actrTransactionManager->endTransaction();
}

static void GetContourColors( const AcDbObjectId& objId, AcArray<COLORREF>& colors )
{
    AcTransaction* pTrans = actrTransactionManager->startTransaction();

    AcDbObject* pObj;
    pTrans->getObject( pObj, objId, AcDb::kForRead );

    ContourInfo* pInfo = ContourInfo::cast( pObj );
    pInfo->getColors( colors );

    actrTransactionManager->endTransaction();
}

static void GetContourDatas( const AcDbObjectId& objId, AcGePoint3dArray& datas )
{
    AcTransaction* pTrans = actrTransactionManager->startTransaction();

    AcDbObject* pObj;
    pTrans->getObject( pObj, objId, AcDb::kForRead );

    ContourInfo* pInfo = ContourInfo::cast( pObj );
    pInfo->getDatas( datas );

    actrTransactionManager->endTransaction();
}

static void GetContourFillFlag( const AcDbObjectId& objId, BOOL& bFill )
{
    AcTransaction* pTrans = actrTransactionManager->startTransaction();

    AcDbObject* pObj;
    pTrans->getObject( pObj, objId, AcDb::kForRead );

    ContourInfo* pInfo = ContourInfo::cast( pObj );
    bFill = ( pInfo->fill() ? TRUE : FALSE );

    actrTransactionManager->endTransaction();
}

static void SetContourInfo( const AcDbObjectId& objId, const AcGeDoubleArray& zValues, const AcArray<COLORREF>& colors, BOOL bFill )
{
    AcTransaction* pTrans = actrTransactionManager->startTransaction();

    AcDbObject* pObj;
    pTrans->getObject( pObj, objId, AcDb::kForWrite );

    ContourInfo* pInfo = ContourInfo::cast( pObj );
    pInfo->setZValues( zValues );
    pInfo->setColors( colors );
    pInfo->needFill( bFill == TRUE );

    actrTransactionManager->endTransaction();
}

static void DrawFill( const CString& layer, AcGePoint3dArray& bound, const AcGePoint3dArray& pts, const AcGeDoubleArray& zValues, const AcArray<COLORREF>& colors, BOOL bFillColor )
{
    // ɾ��ͼ���ϵ����е�ֵ�����ͼԪ
    AcDbObjectIdArray objIds;
    ContourHelper::GetContourGEOnLayer( layer, _T( "ContourFill" ), objIds );
    ArxEntityHelper::EraseObjects2( objIds, true );

    if( bFillColor && colors.length() > zValues.length() )
    {
        // ��ʱ�л�ͼ��, ���ͼ�㲻����, ���½�ͼ��
        LayerSwitch ls( layer, true );

        // ���Ƶ�ֵ�����
        ContourHelper::DrawCountourFill( bound, pts, zValues, colors );
    }
}

IMPLEMENT_DYNAMIC( ContourColorDlg, CDialog )

ContourColorDlg::ContourColorDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( ContourColorDlg::IDD, pParent )
    , m_bFillColor( FALSE )
{

}

ContourColorDlg::~ContourColorDlg()
{
    DeleteColorListItemDatas();
}

void ContourColorDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_CSC_LAYER_LIST, m_layerList );
    DDX_Control( pDX, IDC_CONTOUR_COLOR_LIST, m_colorList );
    DDX_Control( pDX, IDC_COLOR_COMBOX, m_colorCombox );
    DDX_Check( pDX, IDC_FILL_COLOR_CHECKBOX, m_bFillColor );
}

BEGIN_MESSAGE_MAP( ContourColorDlg, CDialog )
    ON_BN_CLICKED( IDOK, &ContourColorDlg::OnBnClickedOk )
    ON_CBN_SELCHANGE( IDC_COLOR_COMBOX, &ContourColorDlg::OnCbnSelchangeColorCombox )
    ON_CBN_SELCHANGE( IDC_CSC_LAYER_LIST, &ContourColorDlg::OnCbnSelchangeCscLayerList )
    ON_BN_CLICKED( IDC_FILL_COLOR_CHECKBOX, &ContourColorDlg::OnBnClickedFillColorCheckbox )
END_MESSAGE_MAP()

BOOL ContourColorDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    FillLayerList();
    FillColorCombox();
    AddColorListColumns();

    return TRUE;
}

void ContourColorDlg::OnBnClickedOk()
{
    UpdateData( TRUE );

    // ��ȡͼ������
    CString layer;
    if( !GetLayer( layer ) )
    {
        MessageBox( _T( "û��ѡ�������ֵ�ߵ�ͼ��" ) );
        return;
    }

    AcGeDoubleArray zValues;
    AcArray<COLORREF> colors;
    int n = m_colorList.GetItemCount();
    for( int i = 0; i < n; i++ )
    {
        ColorListItemData* pData = ( ColorListItemData* )m_colorList.GetItemData( i );
        zValues.append( pData->z );
        colors.append( pData->rgb );
    }
    // ɾ�����һ��zֵ
    zValues.removeLast();

    // ɾ��color list�ϵĸ�������
    DeleteColorListItemDatas();

    // ��ȡͼ���ϵĵ�ֵ����ϢͼԪ
    AcDbObjectId objId = GetContourInfoOnLayer( layer );
    SetContourInfo( objId, zValues, colors, m_bFillColor );

    // ��ȡ�߽���������
    AcGePoint3dArray bounds;
    GetBoundaryPoints( bounds );
    if( bounds.isEmpty() )
    {
        MessageBox( _T( "������һ���պϵľ���߽�" ) );
        return;
    }

    // ��ȡ�㼯����
    AcGePoint3dArray datas;
    GetContourDatas( objId, datas );

    //assert((colors.length()-zValues.length()) == 1);
    // �������
    DrawFill( layer, bounds,  datas, zValues, colors, m_bFillColor );

    OnOK();
}

int ContourColorDlg::GetColorListCurSel() const
{
    int pos = LB_ERR;
    for( int i = 0; i < m_colorList.GetItemCount(); i++ )
    {
        if( m_colorList.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
        {
            pos = i;
            break;
        }
    }
    return pos;
}

void ContourColorDlg::OnCbnSelchangeColorCombox()
{
    int pos = GetColorListCurSel();
    if( pos == LB_ERR ) return;

    ColorListItemData* pData = ( ColorListItemData* )m_colorList.GetItemData( pos );
    pData->rgb = m_colorCombox.GetCurColor();
    m_colorList.SetItemData( pos, ( DWORD_PTR )pData );
    m_colorList.SetCellColors( pos, 1, pData->rgb, -1 );
    m_colorList.Update( pos );
}

void ContourColorDlg::FillLayerList()
{
    // ��ȡ����ͼ���б�
    AcStringArray layers;
    GetEntityLayers( _T( "ContourInfo" ), layers );

    m_layerList.ResetContent();

    // ����б�
    int n = layers.length();
    for( int i = 0; i < n; i++ )
    {
        m_layerList.AddString( layers[i].kACharPtr() );
    }
}

void ContourColorDlg::FillColorCombox()
{
    m_colorCombox.InitColorCB();
}

static COLORREF GetColorRGB( const AcArray<COLORREF>& colors, int i )
{
    if( colors.isEmpty() )
        return RGB( 255, 255, 255 );
    else
        return colors[i];
}

void ContourColorDlg::AddColorListColumns()
{
    m_colorList.Reset();

    CRect rect;
    m_colorList.GetWindowRect( &rect );
    ScreenToClient( &rect );
    double c = 0.4;

    m_colorList.InsertColumn( 0, _T( "��ֵ��ֵ" ), LVCFMT_CENTER, ( int )( c * rect.Width() ) );
    m_colorList.InsertColumn( 1, _T( "�����ɫ" ), LVCFMT_CENTER, ( int )( ( 1 - c )*rect.Width() ) );
    //m_colorList.SetColumnEditor(1, &InitEditor, &EndEditor, &m_colorCombox);
    m_colorList.SetColumnColors( 1, RGB( 255, 255, 255 ), -1 );
}

void ContourColorDlg::FillColorList( const AcGeDoubleArray& zValues, const AcArray<COLORREF>& colors )
{
    DeleteColorListItemDatas();
    AddColorListColumns();

    // ��������һ��zֵ(��ʾ���zֵ֮��ķ�Χ)
    AcGeDoubleArray new_zValues;
    new_zValues.append( zValues );
    new_zValues.append( zValues.last() );

    int n = new_zValues.length();
    for( int i = 0; i < n; i++ )
    {
        m_colorList.InsertItem( i, _T( "xx" ) );

        CString str;
        if( i == 0 )
        {
            str.Format( _T( "<%.3f" ), new_zValues[i] );
        }
        else if( i == n - 1 )
        {
            str.Format( _T( ">%.3f" ), new_zValues[i - 1] );
        }
        else
        {
            str.Format( _T( "%.3f ~ %.3f" ), new_zValues[i - 1], new_zValues[i] );
        }
        m_colorList.SetItemText( i, 0, str );
        m_colorList.SetItemText( i, 1, _T( "�����ɫ" ) );

        COLORREF rgb = GetColorRGB( colors, i );
        m_colorList.SetCellColors( i, 1, rgb, -1 );

        ColorListItemData* pData = new ColorListItemData();
        pData->z = new_zValues[i];
        pData->rgb = rgb;;
        m_colorList.SetItemData( i, ( DWORD_PTR )pData );
    }
}

bool ContourColorDlg::GetLayer( CString& layer )
{
    int pos = m_layerList.GetCurSel();
    bool ret = ( pos != CB_ERR );
    if( ret )
    {
        m_layerList.GetLBText( pos, layer );
    }
    return ret;
}

void ContourColorDlg::OnCbnSelchangeCscLayerList()
{
    // ��ȡͼ������
    CString layer;
    if( !GetLayer( layer ) )
    {
        MessageBox( _T( "û��ѡ�������ֵ�ߵ�ͼ��" ) );
        return;
    }

    // ��ȡͼ���ϵĵ�ֵ����ϢͼԪ
    AcDbObjectId objId = GetContourInfoOnLayer( layer );

    // ��ȡzֵ
    AcGeDoubleArray zValues;
    GetContourZValues( objId, zValues );

    // ��ȡ��ɫ
    AcArray<COLORREF> colors;
    GetContourColors( objId, colors );

    // �Ƿ����
    GetContourFillFlag( objId, m_bFillColor );

    FillColorList( zValues, colors );

    enableColorCtrl();

    UpdateData( FALSE );
}

void ContourColorDlg::OnBnClickedFillColorCheckbox()
{
    UpdateData( TRUE );
    enableColorCtrl();
}

void ContourColorDlg::DeleteColorListItemDatas()
{
    int n = m_colorList.GetItemCount();
    for( int i = 0; i < n; i++ )
    {
        ColorListItemData* pData = ( ColorListItemData* )m_colorList.GetItemData( i );
        delete pData;
    }
}

void ContourColorDlg::enableColorCtrl()
{
    m_colorList.EnableWindow( m_bFillColor );
    m_colorCombox.EnableWindow( m_bFillColor );
}