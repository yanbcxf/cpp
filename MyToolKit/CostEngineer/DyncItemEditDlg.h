#pragma once

#include "CDyncDlg.h"



class CDyncItemEditDlg : public CDialog
{
public:
	CString	m_strItem[64];						 // the string returned [in,out]
	CString m_strComb;
	HICON		m_hIcon;							 // icon if not supplied
	
	CDyncItemEditDlg(void);
	~CDyncItemEditDlg(void);

	afx_msg void OnCbnSelchangeCombo1();

	BOOL Init(LPCTSTR caption, LPCTSTR prompt, vector<item_info> isEditable, CWnd* pParent=NULL,
		WORD nIDIcon=(WORD)IDI_QUESTION);

protected:
	vector<item_info>		m_ItemInfo;
	CDlgTemplateBuilder m_dtb;				 // place to build/hold the dialog template
	enum { IDICON=1, IDEDIT ,MYBUTTON, IDCOMB };				 // control IDs

	// Generated message map functions

	DECLARE_MESSAGE_MAP()

	// MFC virtual overrides
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX)
	{
		for(int i=0; i< m_ItemInfo.size(); i++)
		{
			DDX_Text(pDX, IDEDIT + i * 4,	 m_strItem[i]);
		}
		DDX_Text(pDX, IDCOMB, m_strComb);
	}
};
