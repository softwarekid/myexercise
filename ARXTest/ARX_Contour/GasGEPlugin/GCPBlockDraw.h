#pragma once

#include "../MineGEDraw/BlockDraw.h"

class GCPBlockDraw : public BlockDraw 
{
public:
	ACRX_DECLARE_MEMBERS(GCPBlockDraw) ;

public:
	GCPBlockDraw();
	virtual void regPropertyDataNames(AcStringArray& names) const;
	virtual void readPropertyDataFromGE(const AcStringArray& values);
};

#ifdef GASGEPLUGIN_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(GCPBlockDraw)
#endif
