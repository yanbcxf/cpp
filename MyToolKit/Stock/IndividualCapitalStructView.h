#pragma once


#include "BaseGridCtlView.h"
// CIndividualCapitalStructView 视图

class CIndividualCapitalStructView : public CBaseGridCtlView
{
	DECLARE_DYNCREATE(CIndividualCapitalStructView)

protected:
public:
	CIndividualCapitalStructView();           // 动态创建所使用的受保护的构造函数
	virtual ~CIndividualCapitalStructView();

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

	void RedrawGridCtrl();
	virtual void ProcessXmlWParam(string msgType,string code,string model, string param1,string param2,string param3);
};


