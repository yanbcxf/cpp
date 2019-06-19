#pragma once
#include "afxwin.h"
//#include "e:\workspace_c3\mytoolkit\stock\gridctrl_src\gridctrl.h"
//#include "e:\workspace_c3\mytoolkit\stock\chartctrl\chartctrl.h"


#include "../GridCtrl_src/GridCtrl.h"
#include "ChartCtrl.h"



// CBaseMessageFormView 窗体视图

class CBaseMessageFormView : public CFormView
{
	DECLARE_DYNCREATE(CBaseMessageFormView)

protected:
public:
	CBaseMessageFormView();           // 动态创建所使用的受保护的构造函数
	virtual ~CBaseMessageFormView();

public:
	enum { IDD = IDD_BASEMESSAGEFORMVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	CBaseMessageControl	m_MessageCtrl;
public:
	CButton m_btn1;
	CGridCtrl m_Grid;
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridClick(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnGridDblClick1(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridClick1(NMHDR *pNMHDR, LRESULT *pResult);

	virtual void PostGridDblClick(int gridId, int nRow)
	{

	}

	virtual void PostGridClick(int gridId, int nRow, int nCol) {

	}

	void ReLayout();
	CGridCtrl m_Grid1;

	int m_upper_percent;	//	上表格占比
	int	m_down_percent;		//	下表格占比

public:

	virtual void RedrawDmGraph()
	{
		int w = 6;
	} ;

	virtual void RedrawChart(CCalculateTask * cse){};

	afx_msg LRESULT OnUserXmlWParam(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUserLogWParam_1(WPARAM wParam, LPARAM lParam);
	virtual void ProcessXmlWParam(string msgType,string code,string model, string param1,string param2,string param3);

	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);

	virtual void PostFocusChanged(){};
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


