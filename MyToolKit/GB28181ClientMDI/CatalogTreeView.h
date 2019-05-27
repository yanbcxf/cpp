#pragma once

#include "ViewTree.h"

class CCatalogTreeToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

// CCatalogTreeView

class CCatalogTreeView : public CDockablePane
{
	DECLARE_DYNAMIC(CCatalogTreeView)

public:
	CCatalogTreeView();
	virtual ~CCatalogTreeView();

	void AdjustLayout();
	void CleanupTree();

protected:
	CCatalogTreeToolBar m_wndToolBar;
	CViewTree m_wndClassView;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	void FillClassView();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnRealplay();
	afx_msg void OnProperties();

	afx_msg LRESULT OnUserCatalogComm(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserCatalogData(WPARAM wParam, LPARAM lParam);

protected:
	vector<int>		m_vec_net_port;
	map<string, HTREEITEM>	m_mapTree;
public:
	afx_msg void OnPlayback();
	afx_msg void OnAlarmSubscribe();
	afx_msg void OnDestroy();
};


