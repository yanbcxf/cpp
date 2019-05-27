#pragma once


#include "BaseGridCtlView.h"
// CIndividualSummaryView 视图

class CIndividualSummaryView : public CBaseGridCtlView
{
	DECLARE_DYNCREATE(CIndividualSummaryView)

protected:
public:
	CIndividualSummaryView();           // 动态创建所使用的受保护的构造函数
	virtual ~CIndividualSummaryView();

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
	virtual void OnInitialUpdate();
	
private:
	void RedrawGridCtrl();
	int SearchStockInfo(CStockInfo * pStockinfo, int code);
	virtual void ProcessXmlWParam(string msgType,string code,string model, string param1,string param2,string param3);
};


