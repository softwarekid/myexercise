#pragma once

#include "../GraphTool/BuildNetwork.h"

#include "EdgeData.h"
typedef Digraph::ArcMap<EdgeData*> IDS_EdgeDataMap;

// 识别角联结构
// 实现不完全???
class IdentifyDiagonalStructure
{
public:
	IdentifyDiagonalStructure(void);
	~IdentifyDiagonalStructure(void);

	Digraph::Arc getEdgeById(const AcDbObjectId& objId);       // 根据id查找分支
	AcDbObjectId getIdByEdge(Digraph::Arc e);                  // 根据分支查找id

	bool isDS(const AcDbObjectId& objId);
	bool doIDS(const AcDbObjectId& objId, AcDbObjectIdArray& p1, AcDbObjectIdArray& p2, bool findNST = false);

private:
	bool initNetwok();	                 // 构建网络并提取图元属性数据
	bool isInitNetworkOk() const;        // 初始化网络是否成功
	void setInitNetwokState(bool bState);// 设置网络初始化状态(成功/失败)
	void initAirEdges();                 // 初始化用风地点分支

private:
	Digraph dg;                       // 通风网络(有向图)
	ArcFilter ef;                     // 阻断分支过滤
	Digraph::Node sn, tn;             // 网络虚拟源点和汇点
	NodeGeoDataMap ngm;

	IDS_EdgeDataMap datas;         // 分支的数据(分支的通风类型，非字段)
	EdgeArray airEdges;                 // 用风地点分支集合
	bool bInitNetwokState;            // 网络初始化是否成功(默认成功)
};
