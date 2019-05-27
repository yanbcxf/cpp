#pragma once
#include "basegridctlview.h"

// CMainProductQueryView 视图

class CMainProductQueryView : public CBaseGridCtlView
{
	DECLARE_DYNCREATE(CMainProductQueryView)

protected:
public:
	CMainProductQueryView();           // 动态创建所使用的受保护的构造函数
	virtual ~CMainProductQueryView();

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
	void RedrawGridCtrl(CCalculateTask * cse);
	virtual void ProcessXmlWParam(string msgType,string code,string model, string param1,string param2,string param3);

public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTableAddToSelfSelect();
};


