#pragma once

#include "dlimexp.h"

/*
 * 关于删除方法的注意事项:
 *		1) 使用事务机制删除实体，一定要避免事务嵌套!!!
 *		2) 通常情况下删除实体，建议使用常规机制
 */

/*
		  颜色表
	-----------------
	1	|	Red		|
	-----------------
	2	|	Yellow	|
	-----------------
	3	|	Green	|
	-----------------
	4	|	Cyan	|
	-----------------
	5	|	Blue	|
	-----------------
	6	|	Magenta	|
	-----------------
	7	|White/Black|
	-----------------
*/
class ARXHELPER_DLLIMPEXP ArxEntityHelper
{
public:
	// 删除/反删除对象实体(使用事务)
	static void EraseObject(const AcDbObjectId& objId, Adesk::Boolean erasing);

	// 删除/反删除对象实体(使用常规的Open/Close机制)
	static void EraseObject2(const AcDbObjectId& objId, Adesk::Boolean erasing);

	// 删除/反删除多个对象实体(使用事务)
	static void EraseObjects(const AcDbObjectIdArray& objIds, Adesk::Boolean erasing);

	// 删除/反删除多个对象实体(使用常规的Open/Close机制)
	static void EraseObjects2(const AcDbObjectIdArray& objIds, Adesk::Boolean erasing);

	// 对多个实体进行变换(移动、旋转、缩放)
	static void TransformEntities(const AcDbObjectIdArray& objIds, const AcGeMatrix3d & xform);

	// 显示或隐藏多个图元
	static void ShowEntities(const AcDbObjectIdArray& objIds, bool isVisible);

	// 显示或隐藏单个图元
	static void ShowEntity(const AcDbObjectId& objId, bool isVisible);

	// 立即刷新图元
	static void UpdateEntity(const AcDbObjectId& objId);

	// 缩放到图元
	static void ZoomToEntity(const AcDbObjectId& objId);

	// 缩放到多个图元的近似中心
	static void ZoomToEntities(const AcDbObjectIdArray& objIds);

	// 获取单个图元的颜色
	static bool GetEntityColor(const AcDbObjectId& objId, Adesk::UInt16& colorIndex);

	// 获取多个图元的颜色
	static bool GetEntitiesColor(const AcDbObjectIdArray& objIds, AcArray<Adesk::UInt16>& colors);

	// 设置一个图元的颜色
	static bool SetEntityColor(const AcDbObjectId& objId, unsigned short colorIndex);

	// 设置多个图元的颜色
	static bool SetEntitiesColor(AcDbObjectIdArray& objIds, unsigned short colorIndex);

	// 设置多个图元的颜色
	static bool SetEntitiesColor2(const AcDbObjectIdArray& objIds, const AcArray<Adesk::UInt16>& colors);

	// 后续可能添加一些其它的操作
	// 例如修改图元的图层
	// ...
};
