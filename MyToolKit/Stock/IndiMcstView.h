#pragma once
#include "BaseChartCtlView.h"

// CIndiMcstView 视图

class CIndiMcstView : public CBaseChartCtlView
{
	DECLARE_DYNCREATE(CIndiMcstView)

protected:
public:
	CIndiMcstView();           // 动态创建所使用的受保护的构造函数
	virtual ~CIndiMcstView();

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


private:
	void RedrawDmGraph(CCalculateTask * cse);
	virtual void ProcessXmlWParam(string msgType,string code,string model, string param1,string param2,string param3);

public:
	virtual void OnInitialUpdate();
};


