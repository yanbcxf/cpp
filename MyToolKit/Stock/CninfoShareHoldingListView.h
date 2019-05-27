#pragma once
#include "basegridctlview.h"

// CHexunResearchPaperListView 视图

class CCninfoShareHoldingListView : public CBaseGridCtlView
{
	DECLARE_DYNCREATE(CCninfoShareHoldingListView)

protected:
public:
	CCninfoShareHoldingListView();           // 动态创建所使用的受保护的构造函数
	virtual ~CCninfoShareHoldingListView();

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
	void RedrawGridCtrl(CCalculateTask * cse);
	virtual void ProcessXmlWParam(string msgType,string code,string model, string param1,string param2,string param3);
	void Startup();

public:
	virtual void OnInitialUpdate();
	afx_msg void OnTableAdvancedQuery();
	afx_msg void OnUpdateTableAdvancedQuery(CCmdUI *pCmdUI);
	void OnPageUpdate();

private:
	bool	m_bCalculating;
	string	m_strLike;
	int		m_nPlateCode;
	string	m_sPlateName;

	vector<item_info>  m_vecFindItem;		//	保存查找 对话框 的款项内容
public:
	
	afx_msg void OnTableAddToSelfSelect();

	virtual void PostGridDblClick(int nRow);
};


