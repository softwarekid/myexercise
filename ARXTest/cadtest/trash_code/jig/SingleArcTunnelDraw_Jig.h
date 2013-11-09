#pragma once

#include "../MineGEDraw/MineGEDraw_Jig.h"
#include "SingleArcTunnelDraw.h"

class SingleArcTunnelDraw_Jig : public MineGEDraw_Jig
{
public:
	SingleArcTunnelDraw_Jig(const CString& geType, const CString& drawName);

protected:
	virtual Adesk::Boolean doJig(MineGEDraw* pMineGEDraw);

	virtual DragStatus sampler() ;
	virtual Adesk::Boolean update() ;
	virtual AcDbEntity* entity() const ;

private:
	DragStatus getEndPoint();       // ��̬��ȡ������
	DragStatus getSecondPoint();    // ��̬��ȡ��2��
	enum GET_STATE { EPOINT, SEPOINT }; // ��ǰ��̬��ȡ�����ݣ�ĩ�����ꡢ��2�㣩
	GET_STATE gs;

	AcGePoint3d m_secondPt; // ��2�㣬��ʱ����
	SingleArcTunnelDraw *m_pDraw;
};