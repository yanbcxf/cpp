#pragma once


// CBaseMessageView 视图

class CBaseMessageView : public CView
{
	DECLARE_DYNCREATE(CBaseMessageView)

protected:
	CBaseMessageView();           // 动态创建所使用的受保护的构造函数
	virtual ~CBaseMessageView();

	
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

	CBaseMessageControl	m_MessageCtrl;

public:
	

	afx_msg LRESULT OnUserXmlWParam(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserLogWParam_1(WPARAM wParam, LPARAM lParam);
	virtual void ProcessXmlWParam(string msgType,string code,string model, string param1,string param2,string param3){}
	
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);

	virtual void PostFocusChanged(){};
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


