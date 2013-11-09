#pragma once

#include "dlimexp.h"

// ���ݸ���������
class ARXHELPER_DLLIMPEXP ArxDataTool
{
public:
	/* ��չ���ݲ��� */
	static void RegAppName(AcDbDatabase* db, const CString& appName);

	static bool GetDataFromXData(AcDbObject* pObj, const CString& appName, int index, CString& value);

	static bool SetDataToXData(AcDbObject* pObj, const CString& appName, int index, const CString& value);

	static void AddDataToXData(AcDbObject* pObj, const CString& appName);

	static void RemoveDataFromXData(AcDbObject* pObj, const CString& appName, int index);

	static void AddDataToObjects(const AcDbObjectIdArray& objIds, const CString& appName);

	static void RemoveDataFromObjects(const AcDbObjectIdArray& objIds, const CString& appName, int index);

	/* �жϴʵ��Ƿ���� */
	static bool IsDictExist(const CString& dictName);
	
	/* ע�����Ӵʵ� */
	static void RegDict(const CString& dictName);

	/* �ж�ͼԪ�Ƿ�������typeƥ�� */
	static bool IsEqualType(const CString& type, const AcDbObjectId& objId, bool isDerivedFromParent=true);

	/* ��ģ�Ϳռ�Ŀ����¼�в�������ָ�����͵�ͼԪ(��Ļ�Ͽɼ���) */
	static void GetEntsByType(const CString& type, AcDbObjectIdArray& objIds, bool isDerivedFromParent=true);

	/* ��ģ�Ϳռ�Ŀ����¼�в�������ָ�����͵�ͼԪ(��Ļ�ϲ��ɼ��ģ��ѱ�ɾ����) */
	static void GetErasedEntsByType(const CString& type, AcDbObjectIdArray& objIds, bool isDerivedFromParent=true);
};