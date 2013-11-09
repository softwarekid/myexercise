#pragma once

#include "PolyLineTunnelDraw.h"

// 简单硐室可视化效果基类
class PolyLineStorageDrawBase : public PolyLineTunnelDraw
{
public:
	ACRX_DECLARE_MEMBERS(PolyLineStorageDrawBase);

public:
	virtual void setAllExtraParamsToDefault();
	virtual void configExtraParams();
	virtual void updateExtraParams();	

	virtual void readExtraParam(DrawParamReader& reader);
	virtual void writeExtraParam(DrawParamWriter& writer);

protected:
	virtual Adesk::Boolean subWorldDraw (AcGiWorldDraw *mode);	

public:
	void drawBox(AcGiWorldDraw *mode);
	void updateBoxWidth();
	void drawText(AcGiWorldDraw *mode);

	// extra param
	double m_boxHeight; // 文字框高度,默认为60
	double m_boxWidth;  // 文字框宽度,默认始末点长度的1/3
	double m_minLength; // 最小长度(用于显示文字框)，默认150

protected:
	PolyLineStorageDrawBase(const CString& text);
	CString m_text; // 要显示的文字
};

#ifdef DEFGEPLUGIN_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(PolyLineStorageDrawBase)
#endif