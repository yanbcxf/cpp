#pragma once


// CTabSheet

#define MAXPAGES	20

class CTabSheet : public CTabCtrl
{
	DECLARE_DYNAMIC(CTabSheet)

public:
	CTabSheet();
	virtual ~CTabSheet();

protected:
	DECLARE_MESSAGE_MAP()

	CMyDialog * m_pPages[MAXPAGES];
	UINT	m_IDD[MAXPAGES];
	LPCTSTR m_Title[MAXPAGES];

	int		m_nNumOfPages;
	int		m_nCurrentPage;
public:
	BOOL AddPage(LPCTSTR title, CMyDialog * pDialog, UINT ID);
	void Show(void);
	void SetRect(void);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	void ProcessMessage(string strMsg);
};


