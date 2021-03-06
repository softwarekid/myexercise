#include "StdAfx.h"
#include "MVSSData.h"

MVSSData::MVSSData( const CString& tableName ) : DaoData2( tableName ), objId( INVALID_OBJ_ID )
{
}

void MVSSData::createTableImpl( TableCreator* pTableCreator )
{
    BEGIN_DEFINE_FIELD( TableCreator, pTableCreator )
    DEFINE_INT_FIELD( 对象标识 )
    END_DEFINE_FIELD
}

void MVSSData::writeToTableImpl( DataWriter* pDataWriter )
{
    BEGIN_WRITE_DATA( DataWriter, pDataWriter )
    WRITE_INT_DATA( 对象标识, objId )
    END_WRITE_DATA
}

void MVSSData::readFromTableImpl( DataReader* pDataReader )
{
    BEGIN_READ_DATA( DataReader, pDataReader )
    READ_INT_DATA( 对象标识, objId )
    END_READ_DATA
}

JointData::JointData() : MVSSData( JOINT_TABLE_NAME )
{

}

void JointData::createTableImpl( TableCreator* pTableCreator )
{
    MVSSData::createTableImpl( pTableCreator );

    BEGIN_DEFINE_FIELD( TableCreator, pTableCreator )
    DEFINE_REAL_FIELD( 坐标X )
    DEFINE_REAL_FIELD( 坐标Y )
    DEFINE_REAL_FIELD( 坐标Z )

    DEFINE_STRING_FIELD( 名称 )
    DEFINE_STRING_FIELD( 相对压能 )
    DEFINE_STRING_FIELD( 大气压 )
    DEFINE_STRING_FIELD( 备注 )
    END_DEFINE_FIELD
}

void JointData::writeToTableImpl( DataWriter* pDataWriter )
{
    MVSSData::writeToTableImpl( pDataWriter );

    BEGIN_WRITE_DATA( DataWriter, pDataWriter )
    WRITE_REAL_DATA( 坐标X, x )
    WRITE_REAL_DATA( 坐标Y, y )
    WRITE_REAL_DATA( 坐标Z, z )

    WRITE_STRING_DATA( 名称, name )
    WRITE_STRING_DATA( 相对压能, rp )
    WRITE_STRING_DATA( 大气压, p )
    WRITE_STRING_DATA( 备注, comment )
    END_WRITE_DATA
}

void JointData::readFromTableImpl( DataReader* pDataReader )
{
    MVSSData::readFromTableImpl( pDataReader );

    BEGIN_READ_DATA( DataReader, pDataReader )
    READ_REAL_DATA( 坐标X, x )
    READ_REAL_DATA( 坐标Y, y )
    READ_REAL_DATA( 坐标Z, z )

    READ_STRING_DATA( 名称, name )
    READ_STRING_DATA( 相对压能, rp )
    READ_STRING_DATA( 大气压, p )
    READ_STRING_DATA( 备注, comment )
    END_READ_DATA
}

TunnelData::TunnelData() : MVSSData( TUNNEL_TABLE_NAME ), spt( INVALID_OBJ_ID ), ept( INVALID_OBJ_ID )
{

}

void TunnelData::createTableImpl( TableCreator* pTableCreator )
{
    MVSSData::createTableImpl( pTableCreator );

    BEGIN_DEFINE_FIELD( TableCreator, pTableCreator )
    DEFINE_INT_FIELD( 始节点 )
    DEFINE_INT_FIELD( 末节点 )

    DEFINE_STRING_FIELD( 名称 )
    DEFINE_STRING_FIELD( 风量 )
    DEFINE_STRING_FIELD( 摩擦风阻 )
    DEFINE_STRING_FIELD( 局部风阻 )
    DEFINE_STRING_FIELD( 总阻力 )
    DEFINE_STRING_FIELD( 固定风量 )
    DEFINE_STRING_FIELD( 密度 )
    DEFINE_STRING_FIELD( 可调节性 )
    DEFINE_STRING_FIELD( 调节阻力 )
    DEFINE_STRING_FIELD( 调节风阻 )
    DEFINE_STRING_FIELD( 春季密度 )
    DEFINE_STRING_FIELD( 夏季密度 )
    DEFINE_STRING_FIELD( 秋季密度 )
    DEFINE_STRING_FIELD( 冬季密度 )
    DEFINE_STRING_FIELD( 测试风量 )
    DEFINE_STRING_FIELD( 测试阻力 )
    DEFINE_STRING_FIELD( 巷道长度 )
    DEFINE_STRING_FIELD( 摩擦阻力系数 )
    DEFINE_STRING_FIELD( 百米摩擦风阻 )
    DEFINE_STRING_FIELD( 断面形状 )
    DEFINE_STRING_FIELD( 断面积 )
    DEFINE_STRING_FIELD( 周长 )
    DEFINE_STRING_FIELD( 支护方式 )
    DEFINE_STRING_FIELD( 最大允许风速 )
    DEFINE_STRING_FIELD( 巷道用途 )
    DEFINE_STRING_FIELD( 三区 )
    DEFINE_STRING_FIELD( 温度 )
    DEFINE_STRING_FIELD( 湿度 )
    DEFINE_STRING_FIELD( O2 )
    DEFINE_STRING_FIELD( CH4 )
    DEFINE_STRING_FIELD( CO2 )
    DEFINE_STRING_FIELD( CO )
    DEFINE_STRING_FIELD( 可靠性 )
    DEFINE_STRING_FIELD( 类型 )
    DEFINE_STRING_FIELD( 备注 )
    DEFINE_STRING_FIELD( 附加阻力 )
    END_DEFINE_FIELD
}

void TunnelData::writeToTableImpl( DataWriter* pDataWriter )
{
    MVSSData::writeToTableImpl( pDataWriter );

    BEGIN_WRITE_DATA( DataWriter, pDataWriter )
    WRITE_INT_DATA( 始节点, spt )
    WRITE_INT_DATA( 末节点, ept )

    WRITE_STRING_DATA( 名称, name )
    WRITE_STRING_DATA( 风量, q )
    WRITE_STRING_DATA( 摩擦风阻, r )
    WRITE_STRING_DATA( 局部风阻, lr )
    WRITE_STRING_DATA( 总阻力, h )
    WRITE_STRING_DATA( 固定风量, fq )
    WRITE_STRING_DATA( 密度, rho )
    WRITE_STRING_DATA( 可调节性, ad )
    WRITE_STRING_DATA( 调节阻力, ah )
    WRITE_STRING_DATA( 调节风阻, ar )
    WRITE_STRING_DATA( 春季密度, rho1 )
    WRITE_STRING_DATA( 夏季密度, rho2 )
    WRITE_STRING_DATA( 秋季密度, rho3 )
    WRITE_STRING_DATA( 冬季密度, rho4 )
    WRITE_STRING_DATA( 测试风量, tq )
    WRITE_STRING_DATA( 测试阻力, th )
    WRITE_STRING_DATA( 巷道长度, l )
    WRITE_STRING_DATA( 摩擦阻力系数, apha )
    WRITE_STRING_DATA( 百米摩擦风阻, r100 )
    WRITE_STRING_DATA( 断面形状, atype )
    // 在图元中的字段名称："断面面积"
    // 而在mvss数据库中的字段名称："断面积"
    WRITE_STRING_DATA( 断面积, area )
    WRITE_STRING_DATA( 周长, c )
    WRITE_STRING_DATA( 支护方式, st )
    WRITE_STRING_DATA( 最大允许风速, maxV )
    WRITE_STRING_DATA( 巷道用途, tUse )
    WRITE_STRING_DATA( 三区, tRegion )
    WRITE_STRING_DATA( 温度, temp )
    WRITE_STRING_DATA( 湿度, humidity )
    WRITE_STRING_DATA( O2, O2 )
    WRITE_STRING_DATA( CH4, CH4 )
    WRITE_STRING_DATA( CO2, CO2 )
    WRITE_STRING_DATA( CO, CO )
    WRITE_STRING_DATA( 可靠性, reliability )
    WRITE_STRING_DATA( 类型, type )
    WRITE_STRING_DATA( 备注, comment )
    WRITE_STRING_DATA( 附加阻力, eh )
    END_WRITE_DATA
}

void TunnelData::readFromTableImpl( DataReader* pDataReader )
{
    MVSSData::readFromTableImpl( pDataReader );

    BEGIN_READ_DATA( DataReader, pDataReader )
    READ_INT_DATA( 始节点, spt )
    READ_INT_DATA( 末节点, ept )

    READ_STRING_DATA( 名称, name )
    READ_STRING_DATA( 风量, q )
    READ_STRING_DATA( 摩擦风阻, r )
    READ_STRING_DATA( 局部风阻, lr )
    READ_STRING_DATA( 总阻力, h )
    READ_STRING_DATA( 固定风量, fq )
    READ_STRING_DATA( 密度, rho )
    READ_STRING_DATA( 可调节性, ad )
    READ_STRING_DATA( 调节阻力, ah )
    READ_STRING_DATA( 调节风阻, ar )
    READ_STRING_DATA( 春季密度, rho1 )
    READ_STRING_DATA( 夏季密度, rho2 )
    READ_STRING_DATA( 秋季密度, rho3 )
    READ_STRING_DATA( 冬季密度, rho4 )
    READ_STRING_DATA( 测试风量, tq )
    READ_STRING_DATA( 测试阻力, th )
    READ_STRING_DATA( 巷道长度, l )
    READ_STRING_DATA( 摩擦阻力系数, apha )
    READ_STRING_DATA( 百米摩擦风阻, r100 )
    READ_STRING_DATA( 断面形状, atype )
    // 在图元中的字段名称："断面面积"
    // 而在mvss数据库中的字段名称："断面积"
    READ_STRING_DATA( 断面积, area )
    READ_STRING_DATA( 周长, c )
    READ_STRING_DATA( 支护方式, st )
    READ_STRING_DATA( 最大允许风速, maxV )
    READ_STRING_DATA( 巷道用途, tUse )
    READ_STRING_DATA( 三区, tRegion )
    READ_STRING_DATA( 温度, temp )
    READ_STRING_DATA( 湿度, humidity )
    READ_STRING_DATA( O2, O2 )
    READ_STRING_DATA( CH4, CH4 )
    READ_STRING_DATA( CO2, CO2 )
    READ_STRING_DATA( CO, CO )
    READ_STRING_DATA( 可靠性, reliability )
    READ_STRING_DATA( 类型, type )
    READ_STRING_DATA( 备注, comment )
    READ_STRING_DATA( 附加阻力, eh )
    END_READ_DATA
}

GateData::GateData() : MVSSData( GATE_TABLE_NAME ), tObjId( INVALID_OBJ_ID )
{

}

void GateData::createTableImpl( TableCreator* pTableCreator )
{
    MVSSData::createTableImpl( pTableCreator );

    BEGIN_DEFINE_FIELD( TableCreator, pTableCreator )
    DEFINE_INT_FIELD( 绑定巷道 )
    DEFINE_REAL_FIELD( 坐标X )
    DEFINE_REAL_FIELD( 坐标Y )
    DEFINE_REAL_FIELD( 坐标Z )

    DEFINE_STRING_FIELD( 名称 )
    DEFINE_STRING_FIELD( 压差 )
    DEFINE_STRING_FIELD( 漏风量 )
    DEFINE_STRING_FIELD( 等效风阻 )
    DEFINE_STRING_FIELD( 等效风窗面积 )
    DEFINE_STRING_FIELD( 测试压差 )
    END_DEFINE_FIELD
}

void GateData::writeToTableImpl( DataWriter* pDataWriter )
{
    MVSSData::writeToTableImpl( pDataWriter );

    BEGIN_WRITE_DATA( DataWriter, pDataWriter )
    WRITE_INT_DATA( 绑定巷道, tObjId )
    WRITE_REAL_DATA( 坐标X, x )
    WRITE_REAL_DATA( 坐标Y, y )
    WRITE_REAL_DATA( 坐标Z, z )

    WRITE_STRING_DATA( 名称, name )
    WRITE_STRING_DATA( 压差, rq )
    WRITE_STRING_DATA( 漏风量, flow )
    WRITE_STRING_DATA( 等效风阻, er )
    WRITE_STRING_DATA( 等效风窗面积, ea )
    WRITE_STRING_DATA( 测试压差, trq )
    END_WRITE_DATA
}

void GateData::readFromTableImpl( DataReader* pDataReader )
{
    MVSSData::readFromTableImpl( pDataReader );

    BEGIN_READ_DATA( DataReader, pDataReader )
    READ_INT_DATA( 绑定巷道, tObjId )
    READ_REAL_DATA( 坐标X, x )
    READ_REAL_DATA( 坐标Y, y )
    READ_REAL_DATA( 坐标Z, z )

    READ_STRING_DATA( 名称, name )
    READ_STRING_DATA( 压差, rq )
    READ_STRING_DATA( 漏风量, flow )
    READ_STRING_DATA( 等效风阻, er )
    READ_STRING_DATA( 等效风窗面积, ea )
    READ_STRING_DATA( 测试压差, trq )
    END_READ_DATA
}

FanData::FanData() : MVSSData( FAN_TABLE_NAME ), tObjId( INVALID_OBJ_ID )
{

}

void FanData::createTableImpl( TableCreator* pTableCreator )
{
    MVSSData::createTableImpl( pTableCreator );

    BEGIN_DEFINE_FIELD( TableCreator, pTableCreator )
    DEFINE_INT_FIELD( 绑定巷道 )
    DEFINE_REAL_FIELD( 坐标X )
    DEFINE_REAL_FIELD( 坐标Y )
    DEFINE_REAL_FIELD( 坐标Z )

    DEFINE_STRING_FIELD( 名称 )
    DEFINE_STRING_FIELD( 运行状态 )
    DEFINE_STRING_FIELD( 风量 )
    DEFINE_STRING_FIELD( 风压 )
    DEFINE_STRING_FIELD( 功率 )
    DEFINE_STRING_FIELD( 效率 )
    DEFINE_STRING_FIELD( 风阻 )
    DEFINE_STRING_FIELD( 等积孔 )
    DEFINE_STRING_FIELD( 型号 )
    DEFINE_STRING_FIELD( 叶片角度 )
    DEFINE_STRING_FIELD( 转速 )
    DEFINE_STRING_FIELD( 风机风量 )
    DEFINE_STRING_FIELD( 风网风量 )
    DEFINE_STRING_FIELD( 漏风风量 )
    DEFINE_STRING_FIELD( 风机静压 )
    DEFINE_STRING_FIELD( 风机全压 )
    DEFINE_STRING_FIELD( 出口速压 )
    DEFINE_STRING_FIELD( 入口速压 )
    DEFINE_STRING_FIELD( 入口静压 )
    DEFINE_STRING_FIELD( 测试静压 )
    DEFINE_STRING_FIELD( 耗电功率 )
    DEFINE_STRING_FIELD( 输入功率 )
    DEFINE_STRING_FIELD( 输出功率 )
    DEFINE_STRING_FIELD( 风网功率 )
    DEFINE_STRING_FIELD( 漏风功率 )
    DEFINE_STRING_FIELD( 有用效率 )
    DEFINE_STRING_FIELD( 电机效率 )
    DEFINE_STRING_FIELD( 静压效率 )
    DEFINE_STRING_FIELD( 全压效率 )
    DEFINE_STRING_FIELD( 风机风阻 )
    DEFINE_STRING_FIELD( 风网风阻 )
    DEFINE_STRING_FIELD( 漏风风阻 )
    DEFINE_STRING_FIELD( 风硐断面 )
    DEFINE_STRING_FIELD( 风硐风速 )
    DEFINE_STRING_FIELD( 出口断面 )
    DEFINE_STRING_FIELD( 出口风速 )
    DEFINE_STRING_FIELD( 初始风量 )
    DEFINE_STRING_FIELD( 备注 )
    DEFINE_STRING_FIELD( H_a0 )
    DEFINE_STRING_FIELD( H_a1 )
    DEFINE_STRING_FIELD( H_a2 )
    DEFINE_STRING_FIELD( H_a3 )
    DEFINE_STRING_FIELD( H_a4 )
    DEFINE_STRING_FIELD( H_a5 )
    DEFINE_STRING_FIELD( N_a0 )
    DEFINE_STRING_FIELD( N_a1 )
    DEFINE_STRING_FIELD( N_a2 )
    DEFINE_STRING_FIELD( N_a3 )
    DEFINE_STRING_FIELD( N_a4 )
    DEFINE_STRING_FIELD( N_a5 )
    DEFINE_STRING_FIELD( E_a0 )
    DEFINE_STRING_FIELD( E_a1 )
    DEFINE_STRING_FIELD( E_a2 )
    DEFINE_STRING_FIELD( E_a3 )
    DEFINE_STRING_FIELD( E_a4 )
    DEFINE_STRING_FIELD( E_a5 )
    END_DEFINE_FIELD
}

void FanData::writeToTableImpl( DataWriter* pDataWriter )
{
    MVSSData::writeToTableImpl( pDataWriter );

    BEGIN_WRITE_DATA( DataWriter, pDataWriter )
    WRITE_INT_DATA( 绑定巷道, tObjId )
    WRITE_REAL_DATA( 坐标X, x )
    WRITE_REAL_DATA( 坐标Y, y )
    WRITE_REAL_DATA( 坐标Z, z )

    WRITE_STRING_DATA( 名称, name )
    WRITE_STRING_DATA( 运行状态, rs )
    WRITE_STRING_DATA( 风量, q )
    WRITE_STRING_DATA( 风压, p )
    WRITE_STRING_DATA( 功率, power )
    WRITE_STRING_DATA( 效率, efficiency )
    WRITE_STRING_DATA( 风阻, r )
    WRITE_STRING_DATA( 等积孔, aHole )
    WRITE_STRING_DATA( 型号, model )
    WRITE_STRING_DATA( 叶片角度, angle )
    WRITE_STRING_DATA( 转速, rotateSpeed )
    WRITE_STRING_DATA( 风机风量, fq )
    WRITE_STRING_DATA( 风网风量, nq )
    WRITE_STRING_DATA( 漏风风量, lq )
    WRITE_STRING_DATA( 风机静压, fsp )
    WRITE_STRING_DATA( 风机全压, ffp )
    WRITE_STRING_DATA( 出口速压, outVP )
    WRITE_STRING_DATA( 入口速压, inVP )
    WRITE_STRING_DATA( 入口静压, inSP )
    WRITE_STRING_DATA( 测试静压, tSP )
    WRITE_STRING_DATA( 耗电功率, cPower )
    WRITE_STRING_DATA( 输入功率, inPower )
    WRITE_STRING_DATA( 输出功率, outPower )
    WRITE_STRING_DATA( 风网功率, nPower )
    WRITE_STRING_DATA( 漏风功率, lPower )
    WRITE_STRING_DATA( 有用效率, uPower )
    WRITE_STRING_DATA( 电机效率, ePower )
    WRITE_STRING_DATA( 静压效率, sPower )
    WRITE_STRING_DATA( 全压效率, fPower )
    WRITE_STRING_DATA( 风机风阻, fr )
    WRITE_STRING_DATA( 风网风阻, nr )
    WRITE_STRING_DATA( 漏风风阻, lr )
    WRITE_STRING_DATA( 风硐断面, fma )
    WRITE_STRING_DATA( 风硐风速, fmv )
    WRITE_STRING_DATA( 出口断面, outArea )
    WRITE_STRING_DATA( 出口风速, outV )
    WRITE_STRING_DATA( 初始风量, iq )
    WRITE_STRING_DATA( 备注, comment )
    WRITE_STRING_DATA( H_a0, H_a0 )
    WRITE_STRING_DATA( H_a1, H_a1 )
    WRITE_STRING_DATA( H_a2, H_a2 )
    WRITE_STRING_DATA( H_a3, H_a3 )
    WRITE_STRING_DATA( H_a4, H_a4 )
    WRITE_STRING_DATA( H_a5, H_a5 )
    WRITE_STRING_DATA( N_a0, N_a0 )
    WRITE_STRING_DATA( N_a1, N_a1 )
    WRITE_STRING_DATA( N_a2, N_a2 )
    WRITE_STRING_DATA( N_a3, N_a3 )
    WRITE_STRING_DATA( N_a4, N_a4 )
    WRITE_STRING_DATA( N_a5, N_a5 )
    WRITE_STRING_DATA( E_a0, E_a0 )
    WRITE_STRING_DATA( E_a1, E_a1 )
    WRITE_STRING_DATA( E_a2, E_a2 )
    WRITE_STRING_DATA( E_a3, E_a3 )
    WRITE_STRING_DATA( E_a4, E_a4 )
    WRITE_STRING_DATA( E_a5, E_a5 )
    END_WRITE_DATA
}

void FanData::readFromTableImpl( DataReader* pDataReader )
{
    MVSSData::readFromTableImpl( pDataReader );

    BEGIN_READ_DATA( DataReader, pDataReader )
    READ_INT_DATA( 绑定巷道, tObjId )

    READ_REAL_DATA( 坐标X, x )
    READ_REAL_DATA( 坐标Y, y )
    READ_REAL_DATA( 坐标Z, z )

    READ_STRING_DATA( 名称, name )
    READ_STRING_DATA( 运行状态, rs )
    READ_STRING_DATA( 风量, q )
    READ_STRING_DATA( 风压, p )
    READ_STRING_DATA( 功率, power )
    READ_STRING_DATA( 效率, efficiency )
    READ_STRING_DATA( 风阻 , r )
    READ_STRING_DATA( 等积孔, aHole )
    READ_STRING_DATA( 型号, model )
    READ_STRING_DATA( 叶片角度, angle )
    READ_STRING_DATA( 转速, rotateSpeed )
    READ_STRING_DATA( 风机风量, fq )
    READ_STRING_DATA( 风网风量, nq )
    READ_STRING_DATA( 漏风风量, lq )
    READ_STRING_DATA( 风机静压, fsp )
    READ_STRING_DATA( 风机全压, ffp )
    READ_STRING_DATA( 出口速压, outVP )
    READ_STRING_DATA( 入口速压, inVP )
    READ_STRING_DATA( 入口静压, inSP )
    READ_STRING_DATA( 测试静压, tSP )
    READ_STRING_DATA( 耗电功率, cPower )
    READ_STRING_DATA( 输入功率, inPower )
    READ_STRING_DATA( 输出功率, outPower )
    READ_STRING_DATA( 风网功率, nPower )
    READ_STRING_DATA( 漏风功率, lPower )
    READ_STRING_DATA( 有用效率, uPower )
    READ_STRING_DATA( 电机效率, ePower )
    READ_STRING_DATA( 静压效率, sPower )
    READ_STRING_DATA( 全压效率, fPower )
    READ_STRING_DATA( 风机风阻, fr )
    READ_STRING_DATA( 风网风阻, nr )
    READ_STRING_DATA( 漏风风阻, lr )
    READ_STRING_DATA( 风硐断面, fma )
    READ_STRING_DATA( 风硐风速, fmv )
    READ_STRING_DATA( 出口断面, outArea )
    READ_STRING_DATA( 出口风速, outV )
    READ_STRING_DATA( 初始风量, iq )
    READ_STRING_DATA( 备注, comment )
    READ_STRING_DATA( H_a0, H_a0 )
    READ_STRING_DATA( H_a1, H_a1 )
    READ_STRING_DATA( H_a2, H_a2 )
    READ_STRING_DATA( H_a3, H_a3 )
    READ_STRING_DATA( H_a4, H_a4 )
    READ_STRING_DATA( H_a5, H_a5 )
    READ_STRING_DATA( N_a0, N_a0 )
    READ_STRING_DATA( N_a1, N_a1 )
    READ_STRING_DATA( N_a2, N_a2 )
    READ_STRING_DATA( N_a3, N_a3 )
    READ_STRING_DATA( N_a4, N_a4 )
    READ_STRING_DATA( N_a5, N_a5 )
    READ_STRING_DATA( E_a0, E_a0 )
    READ_STRING_DATA( E_a1, E_a1 )
    READ_STRING_DATA( E_a2, E_a2 )
    READ_STRING_DATA( E_a3, E_a3 )
    READ_STRING_DATA( E_a4, E_a4 )
    READ_STRING_DATA( E_a5, E_a5 )
    END_READ_DATA
}