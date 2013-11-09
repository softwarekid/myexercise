#pragma once

#include "dlimexp.h"
#include "GraphInfo.h"
#include "../GraphDef.h"

/*
 * Ventilation Network Graph Generation ==> VNGGen
 * ͨ������ͼ����
 *
 * �������:
 *		dg    --   ͼ(������)

 * �������:
 *		gi    --   ͼ�ļ�����Ϣ
 *		nis   --   ���нڵ�ļ�����Ϣ
 *		eis   --   ���з�֧�ļ�����Ϣ
 * ע1: �����֧Digraph::Arc����һ��id==>dg.id(e)
 * ע2: ��֧��ϢEdgeInfoҲ��һ��id ==> ei.id
 * ע3: ����ͨ��id���й���
 *
 * Ŀǰ���������������ӳ�һ����֧
 * �������߱༭��̫���㣬����֧Ӧ����һ��ͼԪ���ö���ֱ�߱�ʾ��֧������
 *
 * ��һ���Ĺ���:
 *     1) ���Ը������еĵ����ݣ���Բ�����Ʒ�֧(δʵ��???)
 */

extern "C" VNC_GEN_EXPORT_API bool VNGGen(Digraph& dg, 
										  double m_nodeSep, double m_rankSep,
										  double m_graphRatio, double m_graphWidth, double m_graphHeight, bool m_useDefWH,
										  double m_nodeWidth, double m_nodeHeight, double m_nodeTextHeight, double m_edgeTextHeight, 
										  GraphInfo& gi, NodeInfoArray& nis, EdgeInfoArray& eis);


extern "C" VNC_GEN_EXPORT_API void ClearNodeInfoArray(NodeInfoArray& nis);
extern "C" VNC_GEN_EXPORT_API void ClearEdgeInfoArray(EdgeInfoArray& eis);