#include "StdAfx.h"
#include "MVSSDataLink.h"

MVSSDataLink::MVSSDataLink() : DataLink(), objId( -1 )
{

}

void JointDataLink::regDatas()
{
    /*linkDoubleData(_T("坐标X"), &x);
    linkDoubleData(_T("坐标Y"), &y);
    linkDoubleData(_T("坐标Z"), &z);
    linkStringData(_T("名称"), &name);
    linkStringData(_T("相对压能"), &rp);
    linkStringData(_T("大气压"), &p);
    linkStringData(_T("备注"), &comment);*/
}

void TunnelDataLink::regDatas()
{
    linkStringData( _T( "名称" ), &name );
    linkStringData( _T( "风量" ), &q );
    linkStringData( _T( "摩擦风阻" ), &r );
    linkStringData( _T( "局部风阻" ), &lr );
    linkStringData( _T( "总阻力" ), &h );
    linkStringData( _T( "固定风量" ), &fq );
    linkStringData( _T( "密度" ), &rho );
    linkStringData( _T( "可调节性" ), &ad );
    linkStringData( _T( "调节阻力" ), &ah );
    linkStringData( _T( "调节风阻" ), &ar );
    linkStringData( _T( "春季密度" ), &rho1 );
    linkStringData( _T( "夏季密度" ), &rho2 );
    linkStringData( _T( "秋季密度" ), &rho3 );
    linkStringData( _T( "冬季密度" ), &rho4 );
    linkStringData( _T( "测试风量" ), &tq );
    linkStringData( _T( "测试阻力" ), &th );
    linkStringData( _T( "巷道长度" ), &l );
    linkStringData( _T( "摩擦阻力系数" ), &apha );
    linkStringData( _T( "百米摩擦风阻" ), &r100 );
    linkStringData( _T( "断面形状" ), &atype );
    linkStringData( _T( "断面面积" ), &area );
    linkStringData( _T( "周长" ), &c );
    linkStringData( _T( "支护方式" ), &st );
    linkStringData( _T( "最大允许风速" ), &maxV );
    linkStringData( _T( "巷道用途" ), &tUse );
    linkStringData( _T( "三区" ), &tRegion );
    linkStringData( _T( "温度" ), &temp );
    linkStringData( _T( "湿度" ), &humidity );
    linkStringData( _T( "O2" ), &O2 );
    linkStringData( _T( "CH4" ), &CH4 );
    linkStringData( _T( "CO2" ), &CO2 );
    linkStringData( _T( "CO" ), &CO );
    linkStringData( _T( "可靠性" ), &reliability );
    linkStringData( _T( "类型" ), &type );
    linkStringData( _T( "备注" ), &comment );
    linkStringData( _T( "附加阻力" ), &eh );
}

void GateDataLink::regDatas()
{
    linkStringData( _T( "名称" ), &name );
    linkStringData( _T( "压差" ), &rq );
    linkStringData( _T( "漏风量" ), &flow );
    linkStringData( _T( "等效风阻" ), &er );
    linkStringData( _T( "等效风窗面积" ), &ea );
    linkStringData( _T( "测试压差" ), &trq );
}

void FanDataLink::regDatas()
{
    linkStringData( _T( "名称" ), &name );
    linkStringData( _T( "运行状态" ), &rs );
    linkStringData( _T( "风量" ), &q );
    linkStringData( _T( "风压" ), &p );
    linkStringData( _T( "功率" ), &power );
    linkStringData( _T( "效率" ), &efficiency );
    linkStringData( _T( "风阻" ), &r );
    linkStringData( _T( "等积孔" ), &aHole );
    linkStringData( _T( "型号" ), &model );
    linkStringData( _T( "叶片角度" ), &angle );
    linkStringData( _T( "转速" ), &rotateSpeed );
    linkStringData( _T( "风机风量" ), &fq );
    linkStringData( _T( "风网风量" ), &nq );
    linkStringData( _T( "漏风风量" ), &lq );
    linkStringData( _T( "风机静压" ), &fsp );
    linkStringData( _T( "风机全压" ), &ffp );
    linkStringData( _T( "出口速压" ), &outVP );
    linkStringData( _T( "入口速压" ), &inVP );
    linkStringData( _T( "入口静压" ), &inSP );
    linkStringData( _T( "测试静压" ), &tSP );
    linkStringData( _T( "耗电功率" ), &cPower );
    linkStringData( _T( "输入功率" ), &inPower );
    linkStringData( _T( "输出功率" ), &outPower );
    linkStringData( _T( "风网功率" ), &nPower );
    linkStringData( _T( "漏风功率" ), &lPower );
    linkStringData( _T( "有用效率" ), &uPower );
    linkStringData( _T( "电机效率" ), &ePower );
    linkStringData( _T( "静压效率" ), &sPower );
    linkStringData( _T( "全压效率" ), &fPower );
    linkStringData( _T( "风机风阻" ), &fr );
    linkStringData( _T( "风网风阻" ), &nr );
    linkStringData( _T( "漏风风阻" ), &lr );
    linkStringData( _T( "风硐断面" ), &fma );
    linkStringData( _T( "风硐风速" ), &fmv );
    linkStringData( _T( "出口断面" ), &outArea );
    linkStringData( _T( "出口风速" ), &outV );
    linkStringData( _T( "初始风量" ), &iq );
    linkStringData( _T( "备注" ), &comment );
    linkStringData( _T( "H_a0" ), &H_a0 );
    linkStringData( _T( "H_a1" ), &H_a1 );
    linkStringData( _T( "H_a2" ), &H_a2 );
    linkStringData( _T( "H_a3" ), &H_a3 );
    linkStringData( _T( "H_a4" ), &H_a4 );
    linkStringData( _T( "H_a5" ), &H_a5 );
    linkStringData( _T( "N_a0" ), &N_a0 );
    linkStringData( _T( "N_a1" ), &N_a1 );
    linkStringData( _T( "N_a2" ), &N_a2 );
    linkStringData( _T( "N_a3" ), &N_a3 );
    linkStringData( _T( "N_a4" ), &N_a4 );
    linkStringData( _T( "N_a5" ), &N_a5 );
    linkStringData( _T( "E_a0" ), &E_a0 );
    linkStringData( _T( "E_a1" ), &E_a1 );
    linkStringData( _T( "E_a2" ), &E_a2 );
    linkStringData( _T( "E_a3" ), &E_a3 );
    linkStringData( _T( "E_a4" ), &E_a4 );
    linkStringData( _T( "E_a5" ), &E_a5 );
}
