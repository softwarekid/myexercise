#pragma once

#include "../MineGEDraw/BlockDraw.h"

class PPPBlockDraw : public BlockDraw 
{
public:
	ACRX_DECLARE_MEMBERS(PPPBlockDraw) ;

public:
	PPPBlockDraw();
	virtual void regPropertyDataNames(AcStringArray& names) const;
	virtual void readPropertyDataFromGE(const AcStringArray& values);
};

#ifdef GASGEPLUGIN_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(PPPBlockDraw)
#endif
