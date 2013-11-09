#include "StdAfx.h"
#include "MineGE.h"

#include "config.h"
#include "DataHelperImpl.h"
#include "CurDrawTool.h"

#include "../MineGEDraw/MineGEDrawSystem.h"
#include "../ArxDbgXdata/ArxDbgXdata.h"

/*
 * ÿ��ͼԪ������һ����չ�ʵ䣬ͼԪ�����ݶ���洢����չ�ʵ���
 * Ŀǰ�����ݶ����Ӧ��key�ǹ̶���(�μ�DATA_OBJECT_EXT_DICT_KEY�Ķ���)
 *
 * �жϴʵ����Ƿ�������ݶ���(DataObject)
 * �ʵ���һ�����ݽṹ�������ܶ�"��-ֵ"��(key-value pair)��������C++ STL�е�map
 * key���ַ�����value��DataObject(��AcDbObject�������Զ�����)
 */
static bool HasDataObject( const AcDbObjectId& dictId )
{
    AcDbDictionary* pDict;
    if( Acad::eOk != acdbOpenObject( pDict, dictId, AcDb::kForRead ) ) return false;

    AcDbObjectId objId;
    bool ret = pDict->has( DATA_OBJECT_EXT_DICT_KEY );
    pDict->close();

    return ret;
}

/*
 * ÿ��ͼԪ������һ����չ�ʵ䣬ͼԪ�����ݶ���洢����չ�ʵ���
 * Ŀǰ�����ݶ����Ӧ��key�ǹ̶���(�μ�DATA_OBJECT_EXT_DICT_KEY�Ķ���)
 *
 * ��ʵ����������ݶ���(DataObject)
 */
static AcDbObjectId CreateDataObject( const AcDbObjectId& dictId,
                                      const CString& type,
                                      const AcDbObjectId& objId )
{
    // �򿪴ʵ䣬��ȡ�ʵ����ָ��
    AcDbDictionary* pDict;
    if( Acad::eOk != acdbOpenObject( pDict, dictId, AcDb::kForWrite ) ) return AcDbObjectId::kNull;

    // �������ݶ���
    DataObject* pDO = new DataObject();
    // ���ù�����ͼԪid
    pDO->setGE( objId );
    // ��¼��������
    pDO->setType( type );
    // ��ʼ������
    // ��ϵͳ�ж�ȡtype�����������ֶ�
    // �����ֶεĸ���n������n�����ַ���
    pDO->initData();

    // ��ʵ����������ݶ���
    // ������ɹ���ɾ�����ݶ���ָ�룻
    // �ɹ�����CAD�����ݶ������һ��ID��ͬʱ�ر����ݶ���(!!!����Ҫ�ر�ͼ�ζ���!!!)
    AcDbObjectId dbObjId;
    if( Acad::eOk != pDict->setAt( DATA_OBJECT_EXT_DICT_KEY, pDO, dbObjId ) )
    {
        // ���Ӳ��ɹ���ɾ�����ݶ���ָ��
        delete pDO;
    }
    else
    {
        // ���ӳɹ�����CAD�����ݶ������һ��ID(dbObjId)
        // ͬʱ�ر����ݶ���(!!!����Ҫ�ر�ͼ�ζ���!!!)
        pDO->close();
    }
    // �رմʵ����ָ��
    pDict->close();

    // �������ݶ���ID
    return dbObjId;
}

/*
 * ÿ��ͼԪ������һ����չ�ʵ䣬ͼԪ�����ݶ���洢����չ�ʵ���
 * Ŀǰ�����ݶ����Ӧ��key�ǹ̶���(�μ�DATA_OBJECT_EXT_DICT_KEY�Ķ���)
 *
 * �Ӵʵ��ж�ȡ���ݶ���(DataObject)
 */
static AcDbObjectId GetDataObject( const AcDbObjectId& dictId )
{
    // �ж�ID�Ƿ���Ч
    if( dictId.isNull() ) return AcDbObjectId::kNull;

    // �򿪴ʵ䣬��ȡ�ʵ����ָ��
    AcDbDictionary* pDict;
    if( Acad::eOk != acdbOpenObject( pDict, dictId, AcDb::kForRead ) ) return AcDbObjectId::kNull;

    // ��ȡ���ݶ���ID
    // Ŀǰ���ݶ����Ӧ��KEY�ǹ̶���(key = DATA_OBJECT_EXT_DICT_KEY)
    AcDbObjectId objId;
    pDict->getAt( DATA_OBJECT_EXT_DICT_KEY, objId );

    // �رմʵ����ָ��
    pDict->close();

    return objId;
}

/*
 * ����ͼԪ������������ӻ�ϵͳ(MineGEDrawSystem)������ӻ�Ч������ָ��
 */
static MineGEDraw* GetCurrentDrawPointer( const CString& type )
{
    MineGEDraw* pDraw = 0;

    CString draw;
    if( GetCurDraw( type, draw ) )
    {
        MineGEDrawSystem* pDrawSystem = MineGEDrawSystem::GetInstance();
        if( pDrawSystem != 0 )
        {
            pDraw = pDrawSystem->getGEDraw( type, draw );
        }
    }
    return pDraw;
}

/*
 * �����ݶ���(DataObject)�ж�ȡ����
 * ��ȡ���ݹ�����Ҫͨ��DataHelperImpl�ศ��ʵ��
 * ������
 *		objId  -- ���ݶ���ID
 *		names  -- Ҫ��ȡ���ֶ�����
 *		values -- �ֶ����ݶ�Ӧ����������
 */
static bool GetPropertyDataFromDataObject( const AcDbObjectId& objId, const AcStringArray& names, AcStringArray& values )
{
    // �����ݶ��󣬲���ȡ���ݶ���ָ��
    DataObject* pDO;
    if( Acad::eOk != acdbOpenObject( pDO, objId, AcDb::kForRead ) ) return false;

    // ���ݶ������������
    DataHelperImpl dh( pDO );

    // ���ζ�ȡ����
    int len = names.length();
    for( int i = 0; i < len; i++ )
    {
        // names[i]��������AcString(ARX�ڲ��ṩ��һ���ַ�����)
        // ͨ��AcString::kACharPtr()����ֱ�ӷ����ַ�������
        // ��ȡ����
        CString value;
        bool ret = dh.getPropertyData( names[i].kACharPtr(), value );
        // ��¼������������
        values.append( value );
    }
    // �ر����ݶ���ָ��(!!����Ҫ�رն���ָ��!!)
    pDO->close();

    return true;
}


/* ����������������longת��RGB��ʽ */
// DWORD <==> Adesk::UInt32 <==> unsigned long
// Adesk::UInt8 <==> unsigned char
static void LONG2RGB( Adesk::UInt32 rgbColor, Adesk::UInt8& r, Adesk::UInt8& g, Adesk::UInt8& b )
{
    // ����2�δ����ǵȼ۵�

    /*r = ( rgbColor & 0xffL );
    g = ( rgbColor & 0xff00L ) >> 8;
    b = rgbColor >> 16;*/

    r = GetRValue( rgbColor );
    g = GetGValue( rgbColor );
    b = GetBValue( rgbColor );
}

/* ��ȡCAD��ͼ���ڱ���ɫ(ͨ��������Ǻ�ɫ) */
static bool GetBackgroundColor( Adesk::UInt8& r, Adesk::UInt8& g, Adesk::UInt8& b )
{
    // ��ȡcad��ǰ��������ɫ���� <==> CAD�����Ҽ��˵�"ѡ��"->"��ʾ"->"��ɫ"
    AcColorSettings cs;
    if( !acedGetCurrentColors( &cs ) ) return false;

    // ��ȡģ�Ϳռ�Ĵ��ڱ�����ɫ
    DWORD rgbColor = cs.dwGfxModelBkColor;
    LONG2RGB( rgbColor, r, g, b );

    return true;
}

/* ARX�����ɵĴ��� */
Adesk::UInt32 MineGE::kCurrentVersionNumber = 1 ;

/* ���޸ģ��ú�ʹ��MineGE��Ϊ������ */
ACRX_NO_CONS_DEFINE_MEMBERS ( MineGE, AcDbEntity )

/* ���캯�� */
MineGE::MineGE() : m_pCurrentGEDraw( 0 )
{
    //acutPrintf(_T("\nMineGE::MineGE()..."));
}

/* �������� */
MineGE::~MineGE ()
{
    //acutPrintf(_T("\nMineGE::~MineGE()..."));
    // �����ӻ�Ч������ָ������ΪNULL���������Ұָ��
    m_pCurrentGEDraw = 0;
}

/*
 * ����ͼԪ�����������
 * ע��ʹ�����麯��isA()��isA()������
 *		ACRX_DECLARE_MEMBERS / ACRX_NO_CONS_DEFINE_MEMBERS
 * ��2����������ʵ��
 */
CString MineGE::getTypeName() const
{
    return this->isA()->name();
}

void MineGE::initPropertyData()
{
    //assertWriteEnabled();

    // �ж����ݶ����Ƿ�Ϊ�գ�
    // �����Ϊ�գ���ʾ�Ѿ����ݶ����Ѿ���ʼ������
    if( !m_dataObjectId.isNull() ) return;

    // ������Ȩ�ޣ���kForWrite״̬��
    Acad::ErrorStatus es = upgradeOpen();
    if( es == Acad::eOk || es == Acad::eWasOpenForWrite )
    {
        // ����������չ�ʵ䣬����ID
        createExtensionDictionary();
        AcDbObjectId dictId = extensionDictionary();
        // �������ݶ������ӵ���չ�ʵ���
        m_dataObjectId = CreateDataObject( dictId, getTypeName(), objectId() );
    }
    if( es == Acad::eOk )
    {
        // �������ǰ��kForRead״̬����ԭ��Ȩ��
        downgradeOpen();
    }
}

/* �������ݶ���ID */
AcDbObjectId MineGE::getDataObject() const
{
    assertReadEnabled();

    return m_dataObjectId;
}

/*
 * ��ʼ��ͼԪ�Ŀ��ӻ�Ч������
 * ִ��2����ʼ��������
 *		1) ��ȡ��ǰ���ӻ�Ч������ָ��(m_pCurrentGEDraw)
 *		2) �������еĿ��ӻ�Ч���ĸ��Ӳ��������浽��չ����
 */
void MineGE::initDraw()
{
    if( m_pCurrentGEDraw == 0 )
    {
        // ������Ȩ�ޣ���kForWrite״̬��
        Acad::ErrorStatus es = upgradeOpen();
        if( es == Acad::eOk || es == Acad::eWasOpenForWrite )
        {
            initNewObjectExtraParams();
        }
        if( es == Acad::eOk )
        {
            // �������ǰ��kForRead״̬����ԭ��Ȩ��
            downgradeOpen();
        }
    }
}

void MineGE::updateDrawParams( MineGEDraw* pGEDraw )
{
    if( pGEDraw != 0 )
    {
        // 1) ���������ж�ȡ�ؼ������������ݸ����ӻ�Ч������
        writeKeyParamToGEDraw( pGEDraw );
        // 2) ���¼��㸽�Ӳ���
        pGEDraw->updateExtraParams();
        // 3) �ӿ��ӻ�Ч�������ж�ȡ������������������չ����
        readExtraParamFromGEDraw( pGEDraw );
    }
}

void MineGE::configDraw( const CString& drawName )
{
    // 1) ���ݿ��ӻ�Ч�������ƣ�����ӻ�Ч��ϵͳ(MineGEDrawSystem)������ӻ�Ч������
    MineGEDraw* pGEDraw = MineGEDrawSystem::GetInstance()->getGEDraw( getTypeName(), drawName );
    // 2) ���¿��ӻ�Ч������
    updateDrawParams( pGEDraw );
}

void MineGE::extractExistedDraw( AcStringArray& existedDraw )
{
    // 1) ʹ��ArxDbgXDataģ���е�ArxDbgAppXdata�������չ����
    ArxDbgAppXdata xdata( DRAW_PARAMS_XDATA_GROUP, acdbHostApplicationServices()->workingDatabase() );
    xdata.getXdata( this );          // ��ȡ��չ���ݵ�xdata������

    if( xdata.isEmpty() ) return;    // û������

    int len = 0;
    xdata.getInteger( 1, len );      // ���ӻ�Ч������
    for( int i = 1; i <= len; i++ )
    {
        CString drawName;
        xdata.getString( 2 * i, drawName ); // ���ӻ�Ч������
        existedDraw.append( drawName );  // ��¼��չ�����еĿ��ӻ�Ч������
    }
}

// ���������Ƚϸ���
// ����Ҫ��֤xdata���draw��ȫ�ֵ�draw��ͬ����
// ���ܴ������¼��������
// 1��xdata���draw ���� ȫ�ֵ�draw
// 2��xdata���draw ���� ȫ�ֵ�draw(������draw���)
// 3��xdata���draw С�� ȫ�ֵ�draw(ɾ����draw���)
// 4��xata��Ĳ���draw����ȫ�ֵ�draw��û�У�ȫ�ֵĲ���draw��xdata��û��
// Ŀǰ������򵥵İ취������draw�������ȫ�����ϲ���xdata��
// Ҳ����˵xdata�п��ܴ�������ġ�����ʹ�õ�draw����
// ���ڿ��Կ���ר�ű�дһ������draw�ĳ���
void MineGE::initAllExtraParamsToXData()
{
    // ͨ��assertWriteEnabled�޶�ֻ����write״̬�²���
    assertWriteEnabled();

    // ��ȡ�Ѵ��ڵ�draw
    AcStringArray existedDraw;
    extractExistedDraw( existedDraw );

    MineGEDrawSystem* pGEService = MineGEDrawSystem::GetInstance();
    AcStringArray drawList;
    pGEService->getAllGEDrawsByGEType( getTypeName(), drawList );

    ArxDbgAppXdata xdata( DRAW_PARAMS_XDATA_GROUP, acdbHostApplicationServices()->workingDatabase() );
    xdata.setString( 0, _T( "" ) );                           // ��ǰ���ӻ�Ч������
    xdata.setInteger( 1, 0 );                                 // ���ӻ�Ч������(��ʱ����0)

    bool foundNewDraw = false;                                // �Ƿ����µ�draw
    int i = existedDraw.length() + 1;
    int len = drawList.length();
    for( int j = 0; j < len; j++ )
    {
        if( existedDraw.contains( drawList.at( j ) ) ) continue;

        foundNewDraw = true;                                   // �������µ�draw

        MineGEDraw* pGEDraw = pGEService->getGEDraw( getTypeName(), drawList.at( j ).kACharPtr() );
        writeKeyParamToGEDraw( pGEDraw );                          // д��ؼ�������draw��
        // ��Щ���Ӳ�������Ҫ���ݹؼ������������
        //pGEDraw->setAllExtraParamsToDefault();                   // ������ΪĬ��ֵ
        pGEDraw->updateExtraParams();                              // ���㸽�Ӳ���
        xdata.setString( 2 * i, pGEDraw->isA()->name() );          // ���ӻ�Ч������

        ArxDbgXdataList dataList;
        DrawParamWriter writer( &dataList );
        pGEDraw->writeExtraParam( writer );
        xdata.setList( 2 * i + 1, dataList );                      // д�븽�Ӳ���
        i++;
    }
    xdata.setInteger( 1, i - 1 );                                  // ����draw��ʵ�ʸ���

    if( foundNewDraw ) xdata.setXdata( this );                     // ֻ�з����µ�draw�Ż����xdata
}

/*
 * ��ͼԪ�Ĺؼ����������仯ʱ��ͼԪ�ĸ��Ӳ���Ҳ����Ҫ��֮�仯
 * ���������ʼĩ������仯����˫���������������������Ҳ��Ҫ���¼���
 * MineGE���������ж�ȡ�ؼ����������ݸ����ӻ�Ч�������������㸽�Ӳ���
 * Ȼ���ٴӿ��ӻ�Ч�������н����º�ĸ��Ӳ�����ȡ��MineGEͼԪ����չ������
 */
void MineGE::updateDraw()
{
    assertWriteEnabled();

    //m_pCurrentGEDraw = GetCurrentDrawPointer(getTypeName());
    if( m_pCurrentGEDraw != 0 )
    {
        updateDrawParams( m_pCurrentGEDraw );
    }
}

/*
 * ��ʼ��ͼԪ�Ŀ��ӻ�Ч������
 * ִ��2����ʼ��������
 *		1) ��ȡ��ǰ���ӻ�Ч������ָ��(m_pCurrentGEDraw)
 *		2) �������еĿ��ӻ�Ч���ĸ��Ӳ��������浽��չ����
 */
void MineGE::initNewObjectExtraParams()
{
    assertWriteEnabled();

    // 1) ��ȡ��ǰ���ӻ�Ч������ָ��(m_pCurrentGEDraw)
    m_pCurrentGEDraw = GetCurrentDrawPointer( getTypeName() );

    // 2) �������еĿ��ӻ�Ч���ĸ��Ӳ��������浽��չ����
    //     ��ͼԪ������δ���ӵ����ݿ�(Ҳ����δ����ID)
    if( isNewObject() )
    {
        initAllExtraParamsToXData();
    }
}

/* ��ȡ��ǰ���ӻ�Ч������ָ�� */
MineGEDraw* MineGE::getCurrentDraw() const
{
    return m_pCurrentGEDraw;
}

/*
 * ��ͼԪ��չ�����ж�ȡ���Ӳ��������ݸ����ӻ�Ч������
 */
void MineGE::writeExtraParamToGEDraw( MineGEDraw* pGEDraw ) const
{
    assertReadEnabled();

    // ����ArxDbgXDataģ���е�ArxDbgAppXdata�������չ����
    ArxDbgAppXdata xdata( DRAW_PARAMS_XDATA_GROUP, acdbHostApplicationServices()->workingDatabase() );
    xdata.getXdata( ( MineGE* )this ); // ��ȡ��չ���ݵ�xdata������(ǿ��ȥ��const����)

    int len = 0;
    xdata.getInteger( 1, len );     // ���ӻ�Ч������
    for( int i = 1; i <= len; i++ )
    {
        CString drawName;
        xdata.getString( 2 * i, drawName ); // ���ӻ�Ч������

        if( drawName.CompareNoCase( pGEDraw->isA()->name() ) == 0 )
        {
            ArxDbgXdataList dataList;
            xdata.getList( 2 * i + 1, dataList ); // ���ӻ�Ч���Ĳ���

            ArxDbgXdataListIterator paramList( dataList );
            DrawParamReader reader( &paramList );
            pGEDraw->readExtraParam( reader ); // ����չ�����ж�ȡ����
        }
    }
}

/*
 * �ӿ��ӻ�Ч�������ж�ȡ���Ӳ����������浽ͼԪ����չ������
 */
void MineGE::readExtraParamFromGEDraw( MineGEDraw* pGEDraw )
{
    assertWriteEnabled();

    // ����ArxDbgXDataģ���е�ArxDbgAppXdata�������չ����
    ArxDbgAppXdata xdata( DRAW_PARAMS_XDATA_GROUP, acdbHostApplicationServices()->workingDatabase() );
    xdata.getXdata( this ); // ��ȡ��չ���ݵ�xdata������

    // �����滻����չ����
    // ˼·����ԭ�е���չ���ݸ���һ�ݣ������޸ĵ�draw�������Ƚ����޸ģ�Ȼ���ٱ��浽�µ���չ������
    ArxDbgAppXdata xdata2( DRAW_PARAMS_XDATA_GROUP, acdbHostApplicationServices()->workingDatabase() );
    xdata2.setString( 0, _T( "" ) ); // ��ǰ���ӻ�Ч������

    int len = 0;
    xdata.getInteger( 1, len );     // ���ӻ�Ч������
    xdata2.setInteger( 1, len );

    for( int i = 1; i <= len; i++ )
    {
        CString drawName;
        xdata.getString( 2 * i, drawName ); // ���ӻ�Ч������
        xdata2.setString( 2 * i, drawName );

        if( drawName.CompareNoCase( pGEDraw->isA()->name() ) == 0 )
        {
            ArxDbgXdataList dataList;
            DrawParamWriter writer( &dataList );
            pGEDraw->writeExtraParam( writer );

            xdata2.setList( 2 * i + 1, dataList ); // �޸Ĳ�������ӻ�Ч���Ĳ���
        }
        else
        {
            ArxDbgXdataList dataList;
            xdata.getList( 2 * i + 1, dataList );
            xdata2.setList( 2 * i + 1, dataList );
        }
    }
    xdata2.setXdata( this );
}

/*
 * �ӿ��ӻ�Ч�������ж�ȡ�ؼ������������µ�ͼԪ�Ĺؼ�����(�ؼ�������ͼԪ�����ඨ��)
 */
void MineGE::readKeyParamFromGEDraw( MineGEDraw* pGEDraw )
{
    assertWriteEnabled();

    // �ӿ��ӻ�Ч�������ж�ȡ�ؼ�����
    ArxDbgXdataList dataList;
    DrawParamWriter writer( &dataList );
    pGEDraw->writeKeyParam( writer );

    // �����麯������ͼԪ������Ĺؼ�����
    ArxDbgXdataListIterator dataListIter( dataList );
    DrawParamReader reader( &dataListIter );
    readKeyParam( reader );
}

/*
 * ��ͼԪ��չ�����ж�ȡ�ؼ����������ݸ����ӻ�Ч������
 */
void MineGE::writeKeyParamToGEDraw( MineGEDraw* pGEDraw ) const
{
    assertReadEnabled();

    // ��ͼԪ�������ж�ȡ�ؼ�����
    // �����ݱ��浽��չ��������(ArxDbgXdataList)��
    // ͨ��DrawParamWriter��Ϊ��ת
    ArxDbgXdataList dataList;
    DrawParamWriter writer( &dataList );
    writeKeyParam( writer );

    // ���ؼ��������ݸ����ӻ�Ч������
    // ͨ��DrawParamReader��Ϊ��ת
    ArxDbgXdataListIterator dataListIter( dataList );
    DrawParamReader reader( &dataListIter );
    pGEDraw->readKeyParam( reader );
}

/*
 * ��ͼԪ���������ݶ����ж�ȡ�������ݣ������ݸ����ӻ�Ч������
 */
void MineGE::writePropertyDataToGEDraw( MineGEDraw* pGEDraw ) const
{
    assertReadEnabled();

    // ��ȡҪ��ѯ���ֶ����Ƽ���
    AcStringArray names;
    pGEDraw->regPropertyDataNames( names );
    if( names.isEmpty() ) return;

    // ��ѯ���ݣ���д�뵽��������values��
    AcStringArray values;
    if( !GetPropertyDataFromDataObject( getDataObject(), names, values ) )
    {
        int len = names.length();
        for( int i = 0; i < len; i++ )
        {
            // �����ȡ����ʧ�ܣ�������names�ȳ��Ŀ��ַ���
            values.append( _T( "" ) );
        }
    }

    // ����ѯ�������ݷ��ص����ӻ�Ч������(pGEDraw)
    pGEDraw->readPropertyDataFromGE( values );
}

/*
 * ��ͼԪ�ж�ȡ�ؼ������͸��Ӳ���
 * �����¿��ӻ�Ч������(pGEDraw)�Ĺؼ������͸��Ӳ���
 */
void MineGE::writeParamToGEDraw( MineGEDraw* pGEDraw ) const
{
    assertReadEnabled();

    // 1�����ؼ��������µ�MineGEDraw��
    writeKeyParamToGEDraw( pGEDraw );

    // 2������չ��������ȡ����
    writeExtraParamToGEDraw( pGEDraw );

    // 3����ȡ�������ݣ������ݸ�MineGEDraw
    writePropertyDataToGEDraw( pGEDraw );
}

/*
 * �ӿ��ӻ�Ч������(pGEDraw)�ж�ȡ�ؼ������͸��Ӳ���
 * ������ͼԪ�Ĺؼ������͸��Ӳ���
 */
void MineGE::readParamFromGEDraw( MineGEDraw* pGEDraw )
{
    assertWriteEnabled();

    // 1����MineGEDraw�ж�ȡ���º�Ĺؼ�����
    readKeyParamFromGEDraw( pGEDraw );

    // 2����draw�Ĳ������浽��չ������
    readExtraParamFromGEDraw( pGEDraw );
}

void MineGE::updateDrawParam( bool readOrWrite ) const
{
    if( readOrWrite )
    {
        ( ( MineGE* )this )->readParamFromGEDraw( m_pCurrentGEDraw ); // ǿ��ȥ��const����
    }
    else
    {
        writeParamToGEDraw( m_pCurrentGEDraw );
    }
}

/* ��ͼԪ������д��DWG�ļ��� */
Acad::ErrorStatus MineGE::dwgOutFields( AcDbDwgFiler* pFiler ) const
{
    assertReadEnabled () ;

    //acutPrintf(_T("\nid:%d call MineGE::dwgOutFields()..."), objectId());

    /* ARX�����ɵĹ̶����� */

    // ���û����dwgOutFields()������д��һЩDWG��ʽ�ڲ���Ϣ(���Ǹ��˲²�)
    // ����ԭ��Ӧ��������MFC�����л�����(��ο� ���--������ǳ��MFC��)
    Acad::ErrorStatus es = AcDbEntity::dwgOutFields ( pFiler ) ;
    if ( es != Acad::eOk )
        return ( es ) ;

    // д����汾��
    if ( ( es = pFiler->writeUInt32 ( MineGE::kCurrentVersionNumber ) ) != Acad::eOk )
        return ( es ) ;

    /* �ֶ����Ӵ��� */

    // д�����ݶ���ID(��ָ��)
    pFiler->writeSoftPointerId( m_dataObjectId );

    return ( pFiler->filerStatus () ) ;
}

Acad::ErrorStatus MineGE::dwgInFields( AcDbDwgFiler* pFiler )
{
    assertWriteEnabled();

    //acutPrintf(_T("\nid:%d call MineGE::dwgInFields()..."), objectId());

    /* ARX�����ɵĹ̶����� */

    // ���û����dwgInFields()��������ȡDWG��ʽ�ļ���һЩ�ڲ���Ϣ(���Ǹ��˲²�)
    // ����ԭ��Ӧ��������MFC�����л�����(��ο� ���--������ǳ��MFC��)
    Acad::ErrorStatus es = AcDbEntity::dwgInFields ( pFiler );
    if ( es != Acad::eOk )
        return ( es ) ;

    /* ARX�����ɵĹ̶����� */

    // ��ȡ��汾��
    Adesk::UInt32 version = 0 ;
    if ( ( es = pFiler->readUInt32 ( &version ) ) != Acad::eOk )
        return ( es ) ;
    if ( version > MineGE::kCurrentVersionNumber )
        return ( Acad::eMakeMeProxy ) ;

    /* �ֶ����Ӵ��� */

    // ��ȡ���ݶ���ID
    AcDbSoftPointerId id;
    pFiler->readSoftPointerId( &id );
    m_dataObjectId = id;

    /*
     * ��ʼ��ͼԪ�Ŀ��ӻ�Ч������
     * ִ��2����ʼ��������
     *		1) ��ȡ��ǰ���ӻ�Ч������ָ��(m_pCurrentGEDraw)
     *		2) �������еĿ��ӻ�Ч���ĸ��Ӳ��������浽��չ����
     */
    initDraw();

    return ( pFiler->filerStatus () ) ;
}

/*
 * �ڻ��Ʊպ�ͼ��ʱ��AcGiFillTypeĬ��ΪkAcGiFillAlways(ʼ�����)
 * �պ�ͼ�ΰ�����Բ������Ρ������
 * �μ���AcGiSubEntityTraits::fillType()����˵��
 * ���磬����һ��Բ����ǰ��ɫ�Ǻڵװ�ɫ����ô�����Զ���ʵ�弼�����Ƶ�Բ��2�����:
 *	    1) arx������ص������-- �ױ�+�ڵ����(����Ч������cad��Բ��һ����)
 *		2) arx����ж�أ�cad���ô���ʵ��ģʽ��ʾͼԪ -- �ױ�+�׵����
 * ����μ����������Բ��һЩ˵��.doc
 */

// ����AcCmColor::colorIndex()��������Ϊcolor index�ܹ�ֻ��256�֣��Ұ�/�ڶ�ʹ��7��ʾ���޷�����
// ���Ҫʹ��rgb��ɫ��Ӧʹ��AcCmEntityColor��AcCmColor����
void MineGE::drawBackground( MineGEDraw* pGEDraw, AcGiWorldDraw* mode )
{
    if( isNewObject() ) return;

    // ���ÿ��ӻ�Ч�������caclBackGroundMinPolygon()����
    // �õ����Ƶı��������(���Ƶ���С�����)
    AcGePoint3dArray pts;
    m_pCurrentGEDraw->caclBackGroundMinPolygon( pts );

    // ��������Ƿ���Ч
    // ע������ļ�鷽���Ƚ�ԭʼ������ȫ��
    //     ֻ���ж϶���εĶ����ʽ�Ƿ����2
    if( pts.length() < 3 ) return;

    // ��ȡCAD��ͼ���ڱ���ɫ
    Adesk::UInt8 r, g, b;
    if( !GetBackgroundColor( r, g, b ) ) return;

    // �����������
    // ʹ��CAD���ڱ���ɫ�������
    AcGiSubEntityTraits& traits = mode->subEntityTraits();

    // ����ԭ�е�����
    Adesk::UInt16 cl = traits.color();;
    AcGiFillType ft = traits.fillType();

    AcCmEntityColor bgColor( r, g, b );
    traits.setTrueColor( bgColor );
    traits.setFillType( kAcGiFillAlways ); // ʼ�����
    //acutPrintf(_T("\n��ɫ������%d"), bgColor.colorIndex());
    mode->geometry().polygon( pts.length(), pts.asArrayPtr() );

    // �ָ�ԭ������
    traits.setFillType( ft );
    traits.setColor( cl );
}

Adesk::Boolean MineGE::subWorldDraw( AcGiWorldDraw* mode )
{
    assertReadEnabled () ;

    if( m_pCurrentGEDraw == 0 ) return Adesk::kTrue;

    //acutPrintf(_T("\ncall id:%d MineGE::subWorldDraw()..."), objectId());
    //acutPrintf(_T("\ncall drawname:%s..."), m_pCurrentGEDraw->isA()->name());

    // 1�����²��������ӻ�Ч������
    updateDrawParam( false );

    // 2�����Ʊ���������
    drawBackground( m_pCurrentGEDraw, mode );

    // 3�����ӻ�Ч�����������ͼ��
    m_pCurrentGEDraw->worldDraw( mode );

    return Adesk::kTrue;
}

Acad::ErrorStatus MineGE::subTransformBy( const AcGeMatrix3d& xform )
{
    if( m_pCurrentGEDraw == 0 ) return Acad::eOk;

    //acutPrintf(_T("\ncall id:%d MineGE::subTransformBy()..."), objectId());

    // 1�����²��������ӻ�Ч��������
    updateDrawParam( false );

    // 2�����ӻ�Ч������ִ�б任
    m_pCurrentGEDraw->transformBy( xform );

    // 3���ӿ��ӻ�Ч�������ж�ȡ���º�Ĳ���
    updateDrawParam( true );

    return Acad::eOk;
}

Acad::ErrorStatus MineGE::subGetOsnapPoints (
    AcDb::OsnapMode osnapMode,
    int gsSelectionMark,
    const AcGePoint3d& pickPoint,
    const AcGePoint3d& lastPoint,
    const AcGeMatrix3d& viewXform,
    AcGePoint3dArray& snapPoints,
    AcDbIntArray& geomIds ) const
{
    assertReadEnabled () ;

    if( m_pCurrentGEDraw == 0 ) return Acad::eOk;

    // 1�����²��������ӻ�Ч��������
    updateDrawParam( false );

    // 2���ӿ��ӻ�Ч�������л�ȡ��׽��
    m_pCurrentGEDraw->getOsnapPoints( osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds );

    return Acad::eOk;
}

Acad::ErrorStatus MineGE::subGetGripPoints(
    AcGePoint3dArray& gripPoints,
    AcDbIntArray& osnapModes,
    AcDbIntArray& geomIds ) const
{
    assertReadEnabled () ;

    if( m_pCurrentGEDraw == 0 ) return Acad::eOk;

    // 1�����²��������ӻ�Ч��������
    updateDrawParam( false );

    // 2���ӿ��ӻ�Ч�������л�ȡ�е�
    m_pCurrentGEDraw->getGripPoints( gripPoints, osnapModes, geomIds );

    return Acad::eOk;
}

Acad::ErrorStatus MineGE::subMoveGripPointsAt ( const AcDbIntArray& indices, const AcGeVector3d& offset )
{
    assertWriteEnabled () ;

    if( m_pCurrentGEDraw == 0 ) return Acad::eOk;

    //acutPrintf(_T("\ncall id:%d MineGE::subMoveGripPointsAt()..."), objectId());

    // 1�����²��������ӻ�Ч��������
    updateDrawParam( false );

    // 2�����ӻ�Ч������ִ�ме�༭����
    m_pCurrentGEDraw->moveGripPointsAt( indices, offset );

    // 3���ӿ��ӻ�Ч�������ж�ȡ���º�Ĳ���
    updateDrawParam( true );

    return Acad::eOk;
}

/*
 * ���ݿ��ӻ�Ч�������ж������С�����(���ڱ���������)
 * ����ͼԪ�ļ�������(����ͼԪʱ�����ڼ������ŷ�Χ)
 */
Acad::ErrorStatus MineGE::subGetGeomExtents( AcDbExtents& extents ) const
{
    assertReadEnabled () ;

    if( m_pCurrentGEDraw == 0 ) return AcDbEntity::subGetGeomExtents( extents );

    // 1�����²��������ӻ�Ч��������
    updateDrawParam( false );

    Acad::ErrorStatus es = m_pCurrentGEDraw->getGeomExtents( extents );

    if( Acad::eOk != es )
    {
        // ���ӻ�Ч������û������ʵ��subGetGeomExtents
        //acutPrintf(_T("\nʹ�ñ�����������μ�����������...\n"));
        // ʹ��caclBackGroundMinPolygon()��������Ķ���δ���
        AcGePoint3dArray pts;
        m_pCurrentGEDraw->caclBackGroundMinPolygon( pts );
        if( pts.isEmpty() )
        {
            es = Acad::eInvalidExtents;
        }
        else
        {
            int len = pts.length();
            for( int i = 0; i < len; i++ )
            {
                extents.addPoint( pts[i] );
            }
            es = Acad::eOk;
        }
    }

    return es;
}

// cad��Ƶ���ĵ���subClose
Acad::ErrorStatus MineGE::subClose( void )
{
    //acutPrintf(_T("\nid:%d call MineGE::subClose()...\n"), objectId());

    // ���û����subClose()����(������һЩCAD�ڲ��Ĳ����������˲²�)
    // ע����ARX������ʵ�ֻ��ඨ����麯������õ����������ȵ��û���ķ���
    Acad::ErrorStatus es = AcDbEntity::subClose () ;

    // new������ͼԪ����ɹ��ύ�����ݿ�֮��:
    //		1) ��ʼ�����ӻ�Ч������(��������չ����)
    //		2) ��ʼ�����ݶ���(��������չ�ʵ�)
    if( es == Acad::eOk )
    {
        initDraw();
        initPropertyData();
    }

    return es;
}