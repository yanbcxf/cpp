#pragma once


#include "IndividualDmGraphView.h"
// CIndividualNetProfitsView 视图

class CIndividualNetProfitsView : public CIndividualDmGraphView
{
	DECLARE_DYNCREATE(CIndividualNetProfitsView)

protected:
public:
	CIndividualNetProfitsView();           // 动态创建所使用的受保护的构造函数
	virtual ~CIndividualNetProfitsView();

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

public:
	void RedrawDmGraph();
};


