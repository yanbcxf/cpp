
// GB28181ClientMDIDoc.cpp : CGB28181ClientMDIDoc 类的实现
//

#include "stdafx.h"
#include "GB28181ClientMDI.h"

#include "GB28181ClientMDIDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CGB28181ClientMDIDoc

IMPLEMENT_DYNCREATE(CGB28181ClientMDIDoc, CDocument)

BEGIN_MESSAGE_MAP(CGB28181ClientMDIDoc, CDocument)
END_MESSAGE_MAP()


// CGB28181ClientMDIDoc 构造/析构

CGB28181ClientMDIDoc::CGB28181ClientMDIDoc()
{
	// TODO: 在此添加一次性构造代码

}

CGB28181ClientMDIDoc::CGB28181ClientMDIDoc(string strDeviceId)
{
	m_strDeviceId = strDeviceId;
}

CGB28181ClientMDIDoc::~CGB28181ClientMDIDoc()
{
}

BOOL CGB28181ClientMDIDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CGB28181ClientMDIDoc 序列化

void CGB28181ClientMDIDoc::Serialize(CArchive& ar)
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


// CGB28181ClientMDIDoc 诊断

#ifdef _DEBUG
void CGB28181ClientMDIDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CGB28181ClientMDIDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CGB28181ClientMDIDoc 命令
