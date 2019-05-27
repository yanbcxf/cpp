#pragma once

#include "CDyncDlg.h"


/*
一个对话框包含多个的 GroupBox， 每个 GroupBox 包含多行， 没行包含多个的 Control
*/

#define MAX_GROUP 13
#define MAX_LINE  20
#define MAX_COLUMN	10

class CDyncItemGroupDlg : public CDialog
{
public:
	//	组、行、列
	CString	m_strItem[MAX_GROUP][MAX_LINE][MAX_COLUMN];						 // the string returned [in,out]
	item_info m_vecFindItem[MAX_GROUP][MAX_LINE][MAX_COLUMN];
		
	CDyncItemGroupDlg();
	~CDyncItemGroupDlg(void);
	

	BOOL Init(LPCTSTR caption, LPCTSTR prompt,  CWnd* pParent=NULL,
		WORD nIDIcon=(WORD)IDI_QUESTION);

protected:
	
	CDlgTemplateBuilder m_dtb;				 // place to build/hold the dialog template
	enum { IDICON= WM_USER + 100, IDEDIT ,MYBUTTON, IDCOMB };				 // control IDs
	

	// Generated message map functions

	DECLARE_MESSAGE_MAP()

	// MFC virtual overrides
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);

	void PositionGroupRect(CRect rc, int groupid);
	void PositionLineRect(CRect rc, int groupid, int lineid);
	void PositionItemRect(CRect rcItem, int nGroup, int nLine, int nCol);
	CSize CalcGroupRect(int nGroup);
	CSize CalcLineRect(int nGroup, int nLine);
	CSize CalcItemRect(int nGroup, int nLine, int nCol);

public:
	int DLGMARGIN;						 // margins all around
	int GROUPMARGIN;
	int CYSPACE;						 // vertical space between controls
	int CXSPACE;						 // horizontal space between controls

	int CYCAPTION;						 // height of static text
	int CXCAPTION;

	int CYSTATIC;						 // height of static text
	int CXSTATIC;

	int CXEDIT;
	int CYEDIT;					 // height of edit control

	int CXCHECKBOX;
	int CYCHECKBOX;					 // height of checkbox control

	int CXCOMBOX;
	int CYCOMBOX;

	int CXBUTTON;					 // button width...
	int CYBUTTON;					 // ..and height

	int CXRADIO;
	int CYRADIO;					 // height of checkbox control

	int CXICON;
	int	CYICON;

	int	GROUP_NUM_PER_LINE;	

};
