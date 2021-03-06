#pragma once

#include <deque>
#include "../EvalBase/Eval.h"
using namespace PJ;

#include "FipDataLink.h"

class EvalData_107 : public EvalData
{
public:
	EvalData_107();

protected:
	virtual void createTableImpl(TableCreator* pTableCreator);
	virtual void writeToTableImpl(DataWriter* pDataWriter);

public:
	CString name;        // 名称
	AcStringArray p1;
	AcStringArray p2;
	bool includeAir;    // 是否包含用风地点(串联风)
};

class EvalDataExtractor_107 : public EvalDataExtractor
{
public:
	EvalDataExtractor_107();
	~EvalDataExtractor_107();

	virtual bool hasNext();
	virtual void doExtract(EvalData* pEvalData);	

private:
	void init();
	void clean();
	typedef std::deque<FipDataLink*> DataSet;
	DataSet::iterator itr;
	DataSet ds;
};

class Eval_107 : public Eval
{
public:
	virtual bool doEval(EvalData* pEvalData);

private:
	bool eval_once(EvalData* pEvalData);
};

class EvalFactory_107 : public EvalFactory
{
public:
	virtual Eval* createEvalObject();
	virtual EvalData* createEvalDataObject();
	virtual EvalDataExtractor* createEvalDataExtractorObject();
};
