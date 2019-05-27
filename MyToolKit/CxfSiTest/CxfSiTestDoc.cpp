
// CxfSiTestDoc.cpp : CCxfSiTestDoc 类的实现
//

#include "stdafx.h"
#include "CxfSiTest.h"

#include "CxfSiTestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCxfSiTestDoc

IMPLEMENT_DYNCREATE(CCxfSiTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CCxfSiTestDoc, CDocument)
END_MESSAGE_MAP()


// CCxfSiTestDoc 构造/析构

CCxfSiTestDoc::CCxfSiTestDoc()
{
	// TODO: 在此添加一次性构造代码

}

CCxfSiTestDoc::~CCxfSiTestDoc()
{
}

BOOL CCxfSiTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CCxfSiTestDoc 序列化

void CCxfSiTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CCxfSiTestDoc 诊断

#ifdef _DEBUG
void CCxfSiTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCxfSiTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CCxfSiTestDoc 命令
