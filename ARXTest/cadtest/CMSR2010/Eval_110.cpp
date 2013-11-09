#include "StdAfx.h"
#include "Eval_110.h"

#include "../MineGE/DrawHelper.h"

/* ȫ�ֺ���(ʵ����Tool.cpp��) */
extern int FindShafts( AcDbObjectIdArray& shafts );

/**********************************************************/
/***************** EvalDataExtractor_110 ******************/
/**********************************************************/
EvalDataExtractor_110::EvalDataExtractor_110()
{
    acutPrintf( _T( "\n��Ͳ��������..." ) );
    init();
    acutPrintf( _T( "\n�ҵ����������ݵĸ���:%d" ), ds.size() );
    itr = ds.begin();
}

EvalDataExtractor_110::~EvalDataExtractor_110()
{
    clean();
}

void EvalDataExtractor_110::init()
{
    acutPrintf( _T( "\n110 -- �������о�ͲͼԪ" ) );

    // 1�����ҽ��羮�ͻط羮
    AcDbObjectIdArray shafts;
    int pos = FindShafts( shafts );
    if( shafts.isEmpty() ) return;

    AcDbObjectIdArray objIds;
    objIds.append( shafts );

    // 2������
    int len = objIds.length();
    for( int i = 0; i < len; i++ )
    {
        LiftShaftDataLink* pLsm = new LiftShaftDataLink();
        pLsm->setDataSource( objIds[i] );
        if( !pLsm->updateData( false ) )
        {
            // ����,���˳�
            delete pLsm;
            clean();
            break;
        }

        pLsm->hasLift = ( ( pLsm->sp & SEP_LIFT_COAL ) != 0 );
        pLsm->vt = ( i < pos ) ? VT_IN : VT_OUT; // λ��pos֮ǰ���ǽ��羮��֮������ǻط羮

        ds.push_back( pLsm );
    }
    acutPrintf( _T( "\n110 -- �������" ) );
}

void EvalDataExtractor_110::clean()
{
    for( DataSet::iterator itr = ds.begin(); itr != ds.end(); itr++ )
    {
        delete ( *itr );
    }
    ds.clear();
}

bool EvalDataExtractor_110::hasNext()
{
    return itr != ds.end();
}

void EvalDataExtractor_110::doExtract( EvalData* pEvalData )
{
    LiftShaftDataLink* pLsm = *itr;
    EvalData_110* pLSData = ( EvalData_110* )pEvalData;

    pLSData->name = pLsm->name;
    pLSData->v = pLsm->v;
    pLSData->sp = pLsm->sp;

    pLSData->vt = pLsm->vt;
    pLSData->hasLift = pLsm->hasLift;

    itr++;
}

/**********************************************************/
/****************** EvalData_110 **************/
/**********************************************************/
EvalData_110::EvalData_110()
{

}

void EvalData_110::createTableImpl( TableCreator* pTableCreator )
{
    BEGIN_DEFINE_FIELD( TableCreator, pTableCreator )
    DEFINE_STRING_FIELD( ��Ͳ���� )
    DEFINE_STRING_FIELD( ������ʽ )
    DEFINE_STRING_FIELD( �����ط� )
    DEFINE_REAL_FIELD( ���� )
    //DEFINE_REAL_FIELD(©����)
    DEFINE_STRING_FIELD( ���Ƶķ�մ�ʩ )
    DEFINE_STRING_FIELD( ����ϵ��� )
    DEFINE_STRING_FIELD( ������ʩ )
    DEFINE_STRING_FIELD( �Զ��������װ�� )
    DEFINE_STRING_FIELD( ������· )
    END_DEFINE_FIELD

    EvalData::createTableImpl( pTableCreator );
}

void EvalData_110::writeToTableImpl( DataWriter* pDataWriter )
{
    BEGIN_WRITE_DATA( DataWriter, pDataWriter )
    WRITE_STRING_DATA( ��Ͳ����, name )
    if( !hasLift )
    {
        WRITE_STRING_DATA( ������ʽ, _T( "������" ) )
    }
    else if( ( sp & SEP_SKIP_HOIST ) != 0 )
    {
        WRITE_STRING_DATA( ������ʽ, _T( "��������" ) )
    }
    else
    {
        WRITE_STRING_DATA( ������ʽ, _T( "��ʽ����" ) )
    }
    if( vt == VT_IN )
    {
        WRITE_STRING_DATA( �����ط�, _T( "����" ) )
    }
    else
    {
        WRITE_STRING_DATA( �����ط�, _T( "�ط�" ) )
    }
    WRITE_REAL_DATA( ����, v )
    if( !hasLift )
    {
        WRITE_STRING_DATA( ���Ƶķ�մ�ʩ, _T( "-" ) )
    }
    else if( ( sp && SEP_ENCLOSE ) != 0 )
    {
        WRITE_STRING_DATA( ���Ƶķ�մ�ʩ, _T( "��" ) )
    }
    else
    {
        WRITE_STRING_DATA( ���Ƶķ�մ�ʩ, _T( "��" ) )
    }
    if( !hasLift )
    {
        WRITE_STRING_DATA( ����ϵ���, _T( "-" ) )
    }
    else if( ( sp & SEP_CH4_INTERRUPT ) != 0 )
    {
        WRITE_STRING_DATA( ����ϵ���, _T( "��" ) )
    }
    else
    {
        WRITE_STRING_DATA( ����ϵ���, _T( "��" ) )
    }
    if( !hasLift )
    {
        WRITE_STRING_DATA( ������ʩ, _T( "-" ) )
    }
    else if( ( sp & SEP_DUST_PROOF ) != 0 )
    {
        WRITE_STRING_DATA( ������ʩ, _T( "��" ) )
    }
    else
    {
        WRITE_STRING_DATA( ������ʩ, _T( "��" ) )
    }
    if( !hasLift )
    {
        WRITE_STRING_DATA( �Զ��������װ��, _T( "-" ) )
    }
    else if( ( sp & SEP_FIRE_ALARM ) != 0 )
    {
        WRITE_STRING_DATA( �Զ��������װ��, _T( "��" ) )
    }
    else
    {
        WRITE_STRING_DATA( �Զ��������װ��, _T( "��" ) )
    }
    if( !hasLift )
    {
        WRITE_STRING_DATA( ������·, _T( "-" ) )
    }
    else if( ( sp & SEP_FIRE_CONTROL_PIPE ) != 0 )
    {
        WRITE_STRING_DATA( ������·, _T( "��" ) )
    }
    else
    {
        WRITE_STRING_DATA( ������·, _T( "��" ) )
    }
    END_WRITE_DATA

    EvalData::writeToTableImpl( pDataWriter );
}

/**********************************************************/
/******************* Eval_110 *****************/
/**********************************************************/

bool Eval_110::doEval( EvalData* pEvalData )
{
    return eval_once( pEvalData );
}

bool Eval_110::eval_once( EvalData* pEvalData )
{
    EvalData_110* pLSData = ( EvalData_110* )pEvalData;

    bool ret = true;
    if( pLSData->hasLift )
    {
        if( pLSData->vt == VT_IN )
        {
            ret = eval_ventInShaft( pLSData );
        }
        else
        {
            ret = eval_ventOutShat( pLSData );
        }
    }
    return ret;
}

bool Eval_110::eval_ventInShaft( EvalData_110* pLSData )
{
    double v = pLSData->v;
    int ep = pLSData->sp;

    bool ret = true;
    // �ȽϷ���
    if( ( ep & SEP_SKIP_HOIST ) != 0 )          // ��������
    {
        ret = ( ( v > 0 ) && ( v <= 6 ) );
    }
    else if( ( ep & SEP_BELT_CONVEY ) != 0 )    	// ��ʽ����
    {
        //acutPrintf(_T("\n��ʽ���ͣ�����:%.3f"), v);
        ret = ( ( v > 0 ) && ( v <= 4 ) );
    }
    //acutPrintf(_T("\n���۽������,���1:%s"),ret?_T("�ϸ�"):_T("���ϸ�"));
    ret = ret && ( ( ep & SEP_DUST_PROOF ) != 0 );
    ret = ret && ( ( ep & SEP_FIRE_ALARM ) != 0 );
    ret = ret && ( ( ep & SEP_FIRE_CONTROL_PIPE ) != 0 );
    //acutPrintf(_T("\n���۽������,���2:%s"),ret?_T("�ϸ�"):_T("���ϸ�"));

    return ret;
}

bool Eval_110::eval_ventOutShat( EvalData_110* pLSData )
{
    double v = pLSData->v;
    int ep = pLSData->sp;

    bool ret = true;
    if( ( ep & SEP_SKIP_HOIST ) != 0 )		// ������������Ϊ�ط羮
    {
        ret = ret && ( ( ep & SEP_ENCLOSE ) != 0 );
        ret = ret && ( ( ep & SEP_DUST_PROOF ) == 0 );
    }
    else     	                       // ��ʽ���;���Ϊ�ط羮
    {
        ret = ret && ( ( v > 0 ) && ( v <= 6 ) );
        ret = ret && ( ( ep & SEP_CH4_INTERRUPT ) != 0 );
    }
    return ret;
}

/**********************************************************/
/*************** EvalFactory_110 **************/
/**********************************************************/
Eval* EvalFactory_110::createEvalObject()
{
    return new Eval_110();
}

EvalData* EvalFactory_110::createEvalDataObject()
{
    return new EvalData_110();
}

EvalDataExtractor* EvalFactory_110::createEvalDataExtractorObject()
{
    return new EvalDataExtractor_110();
}