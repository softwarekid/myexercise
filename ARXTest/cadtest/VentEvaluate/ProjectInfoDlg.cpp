#include "stdafx.h"
#include "ProjectInfoDlg.h"

#include "../MineGE/HelperClass.h"

IMPLEMENT_DYNAMIC( ProjectInfoDlg, CDialog )

ProjectInfoDlg::ProjectInfoDlg( CWnd* pParent /*=NULL*/ )
    : CDialog( ProjectInfoDlg::IDD, pParent )
    , m_prepare_solution_begin( COleDateTime::GetCurrentTime() )
    , m_prepare_solution_end( COleDateTime::GetCurrentTime() )
    , m_prepare_info_begin( COleDateTime::GetCurrentTime() )
    , m_prepare_info_end( COleDateTime::GetCurrentTime() )
    , m_prepare_contract_begin( COleDateTime::GetCurrentTime() )
    , m_prepare_contract_end( COleDateTime::GetCurrentTime() )
    , m_survey_learn_begin( COleDateTime::GetCurrentTime() )
    , m_survey_learn_end( COleDateTime::GetCurrentTime() )
    , m_work_do_begin( COleDateTime::GetCurrentTime() )
    , m_work_do_end( COleDateTime::GetCurrentTime() )
    , m_work_report_begin( COleDateTime::GetCurrentTime() )
    , m_work_report_end( COleDateTime::GetCurrentTime() )
{

}

ProjectInfoDlg::~ProjectInfoDlg()
{
}

void ProjectInfoDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_PROJECT_EVAL_BASIS, m_evalBasis );
    DDX_DateTimeCtrl( pDX, IDC_DATETIMEPICKER1, m_prepare_solution_begin );
    DDX_DateTimeCtrl( pDX, IDC_DATETIMEPICKER2, m_prepare_solution_end );
    DDX_DateTimeCtrl( pDX, IDC_DATETIMEPICKER3, m_prepare_info_begin );
    DDX_DateTimeCtrl( pDX, IDC_DATETIMEPICKER4, m_prepare_info_end );
    DDX_DateTimeCtrl( pDX, IDC_DATETIMEPICKER5, m_prepare_contract_begin );
    DDX_DateTimeCtrl( pDX, IDC_DATETIMEPICKER6, m_prepare_contract_end );
    DDX_DateTimeCtrl( pDX, IDC_DATETIMEPICKER7, m_survey_learn_begin );
    DDX_DateTimeCtrl( pDX, IDC_DATETIMEPICKER8, m_survey_learn_end );
    DDX_DateTimeCtrl( pDX, IDC_DATETIMEPICKER13, m_work_do_begin );
    DDX_DateTimeCtrl( pDX, IDC_DATETIMEPICKER14, m_work_do_end );
    DDX_DateTimeCtrl( pDX, IDC_DATETIMEPICKER15, m_work_report_begin );
    DDX_DateTimeCtrl( pDX, IDC_DATETIMEPICKER16, m_work_report_end );
}


BEGIN_MESSAGE_MAP( ProjectInfoDlg, CDialog )
    ON_BN_CLICKED( IDOK, &ProjectInfoDlg::OnBnClickedOk )
END_MESSAGE_MAP()


// ProjectInfoDlg ��Ϣ��������

static void GetTimeFromData( const AcDbObjectId& objId, const CString& field, COleDateTime& mt )
{
    CString value;
    DataHelper::GetPropertyData( objId, field, value );
    mt.ParseDateTime( value );
}

static void SetTimeToData( const AcDbObjectId& objId, const CString& field, const COleDateTime& mt )
{
    CString value = ( COleVariant )mt;
    DataHelper::SetPropertyData( objId, field, value );
}

BOOL ProjectInfoDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // ��ȡ���ݶ���
    AcDbObjectId objId;
    SingleDataObjectHelper::GetObjectId( _T( "������Ŀ��Ϣ" ), objId );

    // ����"��������"��Ŀ
    m_evalBasis.AddString( _T( "ú��ȫ���" ) );

    CString value;
    DataHelper::GetPropertyData( objId, _T( "��������" ), value );
    int pos = m_evalBasis.FindString( 0, value );
    m_evalBasis.SetCurSel( pos );
    /*m_evalBasis.SetWindowText(value);*/

    GetTimeFromData( objId, _T( "�ƶ����۷�����ʼʱ��" ), m_prepare_solution_begin );
    GetTimeFromData( objId, _T( "�ƶ����۷�������ʱ��" ), m_prepare_solution_end );
    GetTimeFromData( objId, _T( "�ռ��������Ͽ�ʼʱ��" ), m_prepare_info_begin );
    GetTimeFromData( objId, _T( "�ռ��������Ͻ���ʱ��" ), m_prepare_info_end );
    GetTimeFromData( objId, _T( "ǩ�����ۺ�ͬ��ʼʱ��" ), m_prepare_contract_begin );
    GetTimeFromData( objId, _T( "ǩ�����ۺ�ͬ����ʱ��" ), m_prepare_contract_end );
    GetTimeFromData( objId, _T( "���������᳹��ʼʱ��" ), m_survey_learn_begin );
    GetTimeFromData( objId, _T( "���������᳹����ʱ��" ), m_survey_learn_end );
    GetTimeFromData( objId, _T( "��չ���۹�����ʼʱ��" ), m_work_do_begin );
    GetTimeFromData( objId, _T( "��չ���۹�������ʱ��" ), m_work_do_end );
    GetTimeFromData( objId, _T( "�������۱��濪ʼʱ��" ), m_work_report_begin );
    GetTimeFromData( objId, _T( "�������۱������ʱ��" ), m_work_report_end );

    UpdateData( FALSE );

    return TRUE;  // return TRUE unless you set the focus to a control
    // �쳣: OCX ����ҳӦ���� FALSE
}

CString ProjectInfoDlg::getEvalBasis()
{
    CString varName;
    int sel = m_evalBasis.GetCurSel();
    if( CB_ERR != sel )
    {
        m_evalBasis.GetLBText( sel, varName );
    }
    return varName;
}

void ProjectInfoDlg::OnBnClickedOk()
{
    UpdateData( TRUE );

    AcDbObjectId objId;
    SingleDataObjectHelper::GetObjectId( _T( "������Ŀ��Ϣ" ), objId );
    DataHelper::SetPropertyData( objId, _T( "��������" ), getEvalBasis() );

    SetTimeToData( objId, _T( "�ƶ����۷�����ʼʱ��" ), m_prepare_solution_begin );
    SetTimeToData( objId, _T( "�ƶ����۷�������ʱ��" ), m_prepare_solution_end );
    SetTimeToData( objId, _T( "�ռ��������Ͽ�ʼʱ��" ), m_prepare_info_begin );
    SetTimeToData( objId, _T( "�ռ��������Ͻ���ʱ��" ), m_prepare_info_end );
    SetTimeToData( objId, _T( "ǩ�����ۺ�ͬ��ʼʱ��" ), m_prepare_contract_begin );
    SetTimeToData( objId, _T( "ǩ�����ۺ�ͬ����ʱ��" ), m_prepare_contract_end );
    SetTimeToData( objId, _T( "���������᳹��ʼʱ��" ), m_survey_learn_begin );
    SetTimeToData( objId, _T( "���������᳹����ʱ��" ), m_survey_learn_end );
    SetTimeToData( objId, _T( "��չ���۹�����ʼʱ��" ), m_work_do_begin );
    SetTimeToData( objId, _T( "��չ���۹�������ʱ��" ), m_work_do_end );
    SetTimeToData( objId, _T( "�������۱��濪ʼʱ��" ), m_work_report_begin );
    SetTimeToData( objId, _T( "�������۱������ʱ��" ), m_work_report_end );

    OnOK();
}