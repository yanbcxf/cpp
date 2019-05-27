#pragma once
#include "basegridctlview.h"

// CHexunResearchPaperListView 视图

class CCninfoAnnouncementListView : public CBaseGridCtlView
{
	DECLARE_DYNCREATE(CCninfoAnnouncementListView)

protected:
public:
	CCninfoAnnouncementListView();           // 动态创建所使用的受保护的构造函数
	virtual ~CCninfoAnnouncementListView();

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
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnTableAdvancedQuery();
	afx_msg void OnUpdateTableAdvancedQuery(CCmdUI *pCmdUI);
	void OnPageUpdate();

private:
	bool	m_bCalculating;
	string	m_strLike;
	int		m_nPlateCode;
	string	m_sPlateName;
public:
	
	afx_msg void OnTableAddToSelfSelect();
};


