#pragma once

#include "CDyncDlg.h"




class CDyncItemFindDlg : public CDialog
{
public:
	CString	m_strItem[64];						 // the string returned [in,out]
	vector<item_info> & m_vecFindItem;
	HICON		m_hIcon;							 // icon if not supplied
	
	CDyncItemFindDlg(vector<item_info> & isEditable);
	~CDyncItemFindDlg(void);
	

	BOOL Init(LPCTSTR caption, LPCTSTR prompt,  CWnd* pParent=NULL,
		WORD nIDIcon=(WORD)IDI_QUESTION);

protected:
	
	CDlgTemplateBuilder m_dtb;				 // place to build/hold the dialog template
	enum { IDICON=1, IDEDIT ,MYBUTTON, IDCOMB };				 // control IDs

	// Generated message map functions

	DECLARE_MESSAGE_MAP()

	// MFC virtual overrides
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX)
	{
		for(int i=0; i< m_vecFindItem.size(); i++)
		{
			if(m_vecFindItem[i].nType== CDlgTemplateBuilder::EDIT)
				DDX_Text(pDX, IDEDIT + i * 4,	 m_strItem[i]);

			if (m_vecFindItem[i].nType == CDlgTemplateBuilder::COMBOBOX)
				DDX_Text(pDX, IDCOMB + i * 4, m_strItem[i]);
		}
		
	}
};
