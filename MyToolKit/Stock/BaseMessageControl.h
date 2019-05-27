#pragma once

class CBaseMessageControl
{
public:
	CBaseMessageControl(void);
	~CBaseMessageControl(void);

	enum Search_option{
		Search_Industry = 0,
		Search_Stock,
		Search_Child,
		Search_All
	};

	void SetOwnerView(CView * pView){ m_pOwnerView = pView; };

	LRESULT OnUserXmlWParam(WPARAM wParam, LPARAM lParam);
	LRESULT OnUserLogWParam_1(WPARAM wParam, LPARAM lParam);
	static void FocusStockOnLeftPane(string strCode);
	void SetStockRangeForCalculateTask(CIndividualShareDoc * pDoc, CCalculateTask * pCse, int option = Search_Stock);
	void OnSetFocus(CString strTitle);

protected:
	CView * m_pOwnerView;
};
