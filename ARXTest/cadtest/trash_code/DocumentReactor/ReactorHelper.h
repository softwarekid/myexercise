#pragma once

class ReactorHelper
{
public:

	// 管理文档相关的reactor
	// 这2个方法用于kLoadAppMsg和kUnloadAppMsg消息
	static void CreateDocumentReactorMap();
	static void RemoveDocumentReactorMap();

	// 这2个方法用于kLoadDwgMsg和kUnloadDwgMsg消息
	static void AddDocumentReactor(AcApDocument* pDoc);
	static void RemoveDocumentReactor(AcApDocument* pDoc);
};
