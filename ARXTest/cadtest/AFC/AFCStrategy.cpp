#include "stdafx.h"
#include "AFCStrategy.h"
#include <xutility>

namespace PJ
{
    AFCStrategy::AFCStrategy( const CString& tableName )
        : DaoData( tableName ), m_caculateQ( 0 ), m_lastQ( 0 )
    {

    }

    void AFCStrategy::setLastQ( double lastQ )
    {
        m_lastQ = lastQ;
    }

    double AFCStrategy::getLastQ() const
    {
        return m_lastQ;
    }

    double AFCStrategy::getMaxQ()
    {
        // ���Ƚ��м���
        m_caculateQ = caculate();

        if( m_caculateQ < 0 || m_lastQ < 0 ) return -1;

        return max( m_lastQ, m_caculateQ );
    }

    void AFCStrategy::setCaculateQ( double q )
    {
        this->m_caculateQ = q;
    }

    void AFCStrategy::createTableImpl( TableCreator* pTableCreator )
    {
        pTableCreator->addRealField( _T( "caculateQ" ) ); // ����һ��caculateQ(�������)�ֶ�
    }

    void AFCStrategy::writeToTableImpl( DataWriter* pDataWriter )
    {
        // д������
        pDataWriter->writeRealData( _T( "caculateQ" ), m_caculateQ );
    }
}