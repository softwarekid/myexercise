#pragma once

#include "dlimexp.h"

/*
 * ����ɾ��������ע������:
 *		1) ʹ���������ɾ��ʵ�壬һ��Ҫ��������Ƕ��!!!
 *		2) ͨ�������ɾ��ʵ�壬����ʹ�ó������
 */

/*
		  ��ɫ��
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
	// ɾ��/��ɾ������ʵ��(ʹ������)
	static void EraseObject(const AcDbObjectId& objId, Adesk::Boolean erasing);

	// ɾ��/��ɾ������ʵ��(ʹ�ó����Open/Close����)
	static void EraseObject2(const AcDbObjectId& objId, Adesk::Boolean erasing);

	// ɾ��/��ɾ���������ʵ��(ʹ������)
	static void EraseObjects(const AcDbObjectIdArray& objIds, Adesk::Boolean erasing);

	// ɾ��/��ɾ���������ʵ��(ʹ�ó����Open/Close����)
	static void EraseObjects2(const AcDbObjectIdArray& objIds, Adesk::Boolean erasing);

	// �Զ��ʵ����б任(�ƶ�����ת������)
	static void TransformEntities(const AcDbObjectIdArray& objIds, const AcGeMatrix3d & xform);

	// ��ʾ�����ض��ͼԪ
	static void ShowEntities(const AcDbObjectIdArray& objIds, bool isVisible);

	// ��ʾ�����ص���ͼԪ
	static void ShowEntity(const AcDbObjectId& objId, bool isVisible);

	// ����ˢ��ͼԪ
	static void UpdateEntity(const AcDbObjectId& objId);

	// ���ŵ�ͼԪ
	static void ZoomToEntity(const AcDbObjectId& objId);

	// ���ŵ����ͼԪ�Ľ�������
	static void ZoomToEntities(const AcDbObjectIdArray& objIds);

	// ��ȡ����ͼԪ����ɫ
	static bool GetEntityColor(const AcDbObjectId& objId, Adesk::UInt16& colorIndex);

	// ��ȡ���ͼԪ����ɫ
	static bool GetEntitiesColor(const AcDbObjectIdArray& objIds, AcArray<Adesk::UInt16>& colors);

	// ����һ��ͼԪ����ɫ
	static bool SetEntityColor(const AcDbObjectId& objId, unsigned short colorIndex);

	// ���ö��ͼԪ����ɫ
	static bool SetEntitiesColor(AcDbObjectIdArray& objIds, unsigned short colorIndex);

	// ���ö��ͼԪ����ɫ
	static bool SetEntitiesColor2(const AcDbObjectIdArray& objIds, const AcArray<Adesk::UInt16>& colors);

	// ������������һЩ�����Ĳ���
	// �����޸�ͼԪ��ͼ��
	// ...
};