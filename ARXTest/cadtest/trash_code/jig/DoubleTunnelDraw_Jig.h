#pragma once

#include "../MineGEDraw/MineGEDraw_Jig.h"
#include "DoubleTunnelDraw.h"

class DoubleTunnelDraw_Jig : public MineGEDraw_Jig
{
public:
	DoubleTunnelDraw_Jig(const CString& geType, const CString& drawName);

protected:
	virtual Adesk::Boolean doJig(MineGEDraw* pMineGEDraw);

	//- input sampler
	virtual DragStatus sampler() ;

	//- jigged entity update
	virtual Adesk::Boolean update() ;

	virtual AcDbEntity* entity() const ;

private:
	DragStatus getEndPoint(); // 动态获取点坐标

	DoubleTunnelDraw *m_pDraw;
};
