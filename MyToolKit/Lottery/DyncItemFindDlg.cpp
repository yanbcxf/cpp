#include "StdAfx.h"
#include "DyncItemFindDlg.h"

CDyncItemFindDlg::CDyncItemFindDlg(vector<item_info> & isEditable)
:m_vecFindItem(isEditable)
{
}

CDyncItemFindDlg::~CDyncItemFindDlg(void)
{
}


//////////////////
// Create string dialog. If no icon specified, use IDI_QUESTION. Note that
// the order in which the controls are added is the TAB order.
//
BOOL CDyncItemFindDlg::Init(LPCTSTR caption, LPCTSTR prompt,  CWnd* pParent, WORD nIDIcon)
{
	const int CXDIALOG  = 250;					 // dialog width
	const int DLGMARGIN = 20;						 // margins all around
	const int CYSTATIC  = 8;						 // height of static text
	const int CXSTATIC  = 100;
	const int CXEDIT    = 100;
	const int CYEDIT    = 12;					 // height of edit control
	const int CXCOMBOX  = 110;
	const int CYCOMBOX  = 33;
	const int CYSPACE   = 5;						 // vertical space between controls
	const int CXBUTTON  = 40;					 // button width...
	const int CYBUTTON  = 15;					 // ..and height

	
	CDlgTemplateBuilder& dtb = m_dtb;

	int nHeight = CYSTATIC +  CYBUTTON + 2 * DLGMARGIN + m_vecFindItem.size()*CYSPACE ;
	for (int i = 0; i < m_vecFindItem.size(); i++)
	{
		if (m_vecFindItem[i].nType == CDlgTemplateBuilder::COMBOBOX)
			nHeight += CYCOMBOX;
		if (m_vecFindItem[i].nType == CDlgTemplateBuilder::EDIT)
			nHeight += CYEDIT;
	}

	CRect rc(
		CPoint(0,0),
		CSize(CXDIALOG, nHeight));

	// create dialog header
	DLGTEMPLATE* pTempl = dtb.Begin(WS_POPUPWINDOW|DS_MODALFRAME|WS_DLGFRAME,rc,caption);

	// shrink main rect by margins
	rc.DeflateRect(CSize(DLGMARGIN,DLGMARGIN));

	// create icon if needed
	if (nIDIcon) {
		if (nIDIcon >= (WORD)IDI_APPLICATION) {
			// if using a system icon, I load it here and set it in OnInitDialog
			// because can't specify system icon in template, only icons from
			// application resource file.
			m_hIcon = ::LoadIcon(NULL, MAKEINTRESOURCE(nIDIcon));
			nIDIcon = 0;
		} else {
			m_hIcon = NULL;
		}

		// The size is calculated in pixels, but it seems to work OK--???
		CSize sz(GetSystemMetrics(SM_CXICON),GetSystemMetrics(SM_CYICON));
		CRect rcIcon(rc.TopLeft(), sz);
		dtb.AddItem(CDlgTemplateBuilder::STATIC, // add icon 
			WS_VISIBLE|WS_CHILD|SS_LEFT|SS_ICON, rc, nIDIcon, IDICON);
		rc.left += sz.cx;  // shrink main rect by width of icon
	}

	// add prompt
	rc.bottom = rc.top + CYSTATIC;					// height = height of static
	dtb.AddItem(CDlgTemplateBuilder::STATIC,		// add it
		WS_VISIBLE|WS_CHILD|SS_LEFT, rc, prompt);

	long oldright= rc.right;
	long oldleft = rc.left;

	for(int i=0; i< m_vecFindItem.size(); i++)
	{
		rc += CPoint(0,rc.Height()+CYSPACE);        //move below static
		rc.bottom = rc.top + CYSTATIC;              //height = height of static
		rc.left = oldleft;
		rc.right = rc.left + CXSTATIC;
		dtb.AddItem(CDlgTemplateBuilder::STATIC,		// add it
			WS_VISIBLE|WS_CHILD|SS_LEFT, rc, m_vecFindItem[i].caption);

		if (m_vecFindItem[i].nType == CDlgTemplateBuilder::COMBOBOX)
		{
			// add combobo control
			rc.right = oldright;
			rc.bottom = rc.top + CYCOMBOX;
			rc.left = rc.right - CXEDIT;

			dtb.AddItem(CDlgTemplateBuilder::COMBOBOX,			// add it
				m_vecFindItem[i].dwStyle, rc, m_strItem[i], IDCOMB + 4 * i);
		}
		else if(m_vecFindItem[i].nType == CDlgTemplateBuilder::EDIT)
		{
			// add edit control
			rc.right = oldright;
			rc.bottom = rc.top + CYEDIT;
			rc.left = rc.right - CXEDIT;

			dtb.AddItem(CDlgTemplateBuilder::EDIT,			// add it
				m_vecFindItem[i].dwStyle, rc, m_strItem[i], IDEDIT + 4 * i);
		}
		
		//rc.right=oldright;
	}
	
	// add OK button
	rc += CPoint(0, rc.Height() + CYSPACE);		// move below combbox control
	rc.bottom = rc.top + CYBUTTON;					// height = button height
	rc.left   = rc.right - CXBUTTON;					// width  = button width
	rc -= CPoint(CXBUTTON + DLGMARGIN,0);			// move left one button width
	dtb.AddItem(CDlgTemplateBuilder::BUTTON,		// add it
		WS_VISIBLE|WS_CHILD|WS_TABSTOP|BS_DEFPUSHBUTTON, rc, _T("&确定"), IDOK);

	// add Cancel button
	rc += CPoint(CXBUTTON + DLGMARGIN,0);			// move right again
	dtb.AddItem(CDlgTemplateBuilder::BUTTON,		// add Cancel button
		WS_VISIBLE|WS_CHILD|WS_TABSTOP , rc, _T("&取消"), IDCANCEL);

	return InitModalIndirect(pTempl, pParent);

}



BEGIN_MESSAGE_MAP(CDyncItemFindDlg, CDialog)
	//{{AFX_MSG_MAP(CStringDialog)
	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////
// Initialize dialog: if I loaded a system icon, set it in static control.
//
BOOL CDyncItemFindDlg::OnInitDialog()
{
	if (m_hIcon) {
		CStatic* pStatic = (CStatic*)GetDlgItem(IDICON);
		ASSERT(pStatic);
		pStatic->SetIcon(m_hIcon);
		SetIcon(m_hIcon, TRUE);			// Set big icon
		SetIcon(m_hIcon, FALSE);		// Set small icon
	}

	for(int i=0; i< m_vecFindItem.size(); i++)
	{
		//	将原有的值 进行展示
		if (m_vecFindItem[i].nType == CDlgTemplateBuilder::EDIT)
			m_strItem[i].Format("%s", m_vecFindItem[i].strValue);
		else if (m_vecFindItem[i].nType == CDlgTemplateBuilder::COMBOBOX)
		{
			CComboBox * pComBox = (CComboBox*)GetDlgItem(IDCOMB + 4 * i);
			if (pComBox)
			{
				pComBox->ResetContent();
				pComBox->AddString("Yes");
				pComBox->AddString("No");
			}
		}
	}
			
	BOOL bResult =  CDialog::OnInitDialog();

	for (int i = 0; i< m_vecFindItem.size(); i++)
	{
		//	将原有的值 进行展示
		if (m_vecFindItem[i].nType == CDlgTemplateBuilder::COMBOBOX)
		{
			CComboBox * pComBox = (CComboBox*)GetDlgItem(IDCOMB + 4 * i);
			if (pComBox)
				pComBox->SetCurSel(1);
		}
	}

	return bResult;
}



/////////////////
// User pressed OK: check for empty string if required flag is set.
//
void CDyncItemFindDlg::OnOK()
{
	UpdateData(TRUE);

	for(int i=0; i< m_vecFindItem.size(); i++)
	{
		// 保存本次的 修改结果
		sprintf_s(m_vecFindItem[i].strValue, 128, "%s", m_strItem[i].Trim().GetBuffer());
	}

	CDialog::OnOK();
}

