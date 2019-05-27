#pragma once

#include "afxcmn.h"

#include "../GridCtrl_src/GridCtrl.h"


// CBaseRichEditFormView 窗体视图

class CBaseRichEditFormView : public CFormView
{
	DECLARE_DYNCREATE(CBaseRichEditFormView)

protected:
	CBaseRichEditFormView();           // 动态创建所使用的受保护的构造函数
	virtual ~CBaseRichEditFormView();

public:
	enum { IDD = IDD_BASE_RICHEDIT_FORMVIEW };

	CBaseMessageControl	m_MessageCtrl;

#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	int		m_nPageNo;
	int		m_nMaxPage;
	bool	m_bPagination;		//	是否显示分页栏

public:
	CGridCtrl	m_GridCtrl;
	CRichEditCtrl m_RichEdit;

public:
	void ReLayout();
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	virtual void RedrawDmGraph()
	{
		int w = 6;
	} ;

	virtual void RedrawGridCtrl(CCalculateTask * cse){};

	afx_msg LRESULT OnUserXmlWParam(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserLogWParam_1(WPARAM wParam, LPARAM lParam);
	virtual void ProcessXmlWParam(string msgType,string code,string model, string param1,string param2,string param3);

	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);

	virtual void PostFocusChanged(){};
	virtual void OnPageUpdate(){};

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnFirst();
	afx_msg void OnBnClickedBtnPageup();
	afx_msg void OnBnClickedBtnPagedown();
	afx_msg void OnBnClickedBtnLast();

	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
};


