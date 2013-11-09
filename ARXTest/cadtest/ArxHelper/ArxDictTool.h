#pragma once

#include "dlimexp.h"

// ��Ч��entry����
#define INVALID_ENTRY 0

// CAD�ʵ��������������
// AcDbDictionay��key���Ǵ�Сд���е�
// ����NAME, name, Name, naMe�ȶ���һ����

// һ��key->XRecord(��¼��)
// XRecordʵ����һ��resbuf list, ���Ա���һ�����ݣ�
// ����ʹ��XRecord����һЩ�򵥵��ı�������(������1����Ҳ�����Ƕ��)
class ARXHELPER_DLLIMPEXP ArxDictTool
{
public:
	static ArxDictTool* GetDictTool(const CString& dictName);

	bool findKey(const CString& key);
	void removeKey(const CString& key);
	void getAllKeys(AcStringArray& keys);

	int addEntry(const CString& key, const CString& entry);         // ����entry,���key�����ڣ����Զ�����
	int removeEntry(const CString& key, const CString& entry);      // ɾ��entry
	int findEntry(const CString& key, const CString& entry);        // ����entry������λ��
	void getAllEntries(const CString& key, AcStringArray& entries); // ��ȡkey�����ĵ�����entry
	int countEntries(const CString& key);                           // ͳ��key����������entry����

	// index����Ҫ����0
	bool modifyEntry(const CString& key, int index, const CString& newEntry); // �޸�key�е�index���ֵ
	bool getEntry(const CString& key, int index, CString& entry);             // ��ȡkey�е�index���ֵ

private:
	ArxDictTool(const CString& dictName);
	CString m_dictName;
};

// һ��key->AcDbObject
class ARXHELPER_DLLIMPEXP ArxDictTool2
{
public:
	static ArxDictTool2* GetDictTool(const CString& dictName);

	// ���ӵ��ʵ��У�ͬʱ�Զ��رն���pObj->close();
	bool addEntry(const CString& key, AcDbObject* pObj);         // ����
	bool removeEntry(const CString& key);                        // ɾ��
	bool findEntry(const CString& key, AcDbObjectId& objId);     // ����
	void getAllKeys(AcStringArray& keys);
	void getAllEntries(AcDbObjectIdArray& objIds);               // ��ȡ���еĶ���

private:
	ArxDictTool2(const CString& dictName);
	CString m_dictName;
};

class ARXHELPER_DLLIMPEXP ArxDictHelper
{
public:
	static void GetAllKeys( const CString& dictName, AcStringArray& keys);
	static bool GetAllEntries(const CString& dictName, const CString& key, AcStringArray& entries);
	static bool AddEntry(const CString& dictName, const CString& key, const CString& entry);
	static void RemoveAllEntries(const CString& dictName, const CString& key);
	static bool RemoveEntry(const CString& dictName, const CString& key, const CString& entry);
};