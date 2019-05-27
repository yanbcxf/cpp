#include "StdAfx.h"
#include "DyncItemEditDlg.h"

CDyncItemEditDlg::CDyncItemEditDlg(void)
{
}

CDyncItemEditDlg::~CDyncItemEditDlg(void)
{
}


//////////////////
// Create string dialog. If no icon specified, use IDI_QUESTION. Note that
// the order in which the controls are added is the TAB order.
//
BOOL CDyncItemEditDlg::Init(LPCTSTR caption, LPCTSTR prompt, vector<item_info> isEditable, CWnd* pParent, WORD nIDIcon)
{
	const int CXDIALOG  = 400;					 // dialog width
	const int DLGMARGIN = 7;						 // margins all around
	const int CYSTATIC  = 8;						 // height of static text
	const int CXSTATIC  = 80;
	const int CXEDIT    = 300;
	const int CYEDIT    = 12;					 // height of edit control
	const int CXCOMBOX  = 110;
	const int CYCOMBOX  = 45;
	const int CYSPACE   = 5;						 // vertical space between controls
	const int CXBUTTON  = 40;					 // button width...
	const int CYBUTTON  = 15;					 // ..and height

	m_ItemInfo = isEditable;

	CDlgTemplateBuilder& dtb = m_dtb;
	CRect rc(
		CPoint(0,0),
		CSize(CXDIALOG, CYSTATIC + m_ItemInfo.size()*CYEDIT + CYBUTTON + 2*DLGMARGIN + m_ItemInfo.size()*CYSPACE + CYCOMBOX + CYSPACE) );

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

	for(int i=0; i< m_ItemInfo.size(); i++)
	{
		rc += CPoint(0,rc.Height()+CYSPACE);        //move below static
		rc.bottom = rc.top + CYSTATIC;              //height = height of static
		rc.left = oldleft;
		rc.right = rc.left + CXSTATIC;
		dtb.AddItem(CDlgTemplateBuilder::STATIC,		// add it
			WS_VISIBLE|WS_CHILD|SS_LEFT, rc, m_ItemInfo[i].caption);


		// add edit control
		rc.right = oldright;
		rc.bottom = rc.top + CYEDIT;
		rc.left = rc.right - CXEDIT;
		
		dtb.AddItem(CDlgTemplateBuilder::EDIT,			// add it
			m_ItemInfo[i].dwStyle , rc, m_strItem[i], IDEDIT + 4 * i);
		//rc.right=oldright;
	}

	// add combbox control
	rc += CPoint(0,rc.Height()+CYSPACE);        //move below static
	rc.bottom = rc.top + CYCOMBOX;              //height = height of static
	dtb.AddItem(CDlgTemplateBuilder::COMBOBOX,		// add it
		WS_VISIBLE|WS_CHILD|SS_LEFT|WS_BORDER|CBS_DROPDOWN, rc, m_strComb,IDCOMB);

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
		WS_VISIBLE|WS_CHILD|WS_TABSTOP, rc, _T("&取消"), IDCANCEL);

	return InitModalIndirect(pTempl, pParent);

}



BEGIN_MESSAGE_MAP(CDyncItemEditDlg, CDialog)
	//{{AFX_MSG_MAP(CStringDialog)
	ON_CBN_SELCHANGE(IDCOMB, &CDyncItemEditDlg::OnCbnSelchangeCombo1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////
// Initialize dialog: if I loaded a system icon, set it in static control.
//
BOOL CDyncItemEditDlg::OnInitDialog()
{
	if (m_hIcon) {
		CStatic* pStatic = (CStatic*)GetDlgItem(IDICON);
		ASSERT(pStatic);
		pStatic->SetIcon(m_hIcon);
		SetIcon(m_hIcon, TRUE);			// Set big icon
		SetIcon(m_hIcon, FALSE);		// Set small icon
	}
	CComboBox * pComBox = (CComboBox*)GetDlgItem(IDCOMB);
	if(pComBox)
	{
		pComBox->ResetContent();
		pComBox->AddString("修改（Update）");
		pComBox->AddString("增加（Insert）");
		pComBox->AddString("删除（Delete）");
	}
	
	BOOL bResult =  CDialog::OnInitDialog();
	if(pComBox)
		pComBox->SetCurSel(0);
	return bResult;
}



/////////////////
// User pressed OK: check for empty string if required flag is set.
//
void CDyncItemEditDlg::OnOK()
{
	UpdateData(TRUE);

	CDialog::OnOK();
}

void CDyncItemEditDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	CComboBox * pComBox = (CComboBox*)GetDlgItem(IDCOMB);
	if(pComBox)
	{
		int nSel = pComBox->GetCurSel();

		if(nSel == 1)
		{
			//	新增时，关键域 变成可编辑
			for(int i=0; i< m_ItemInfo.size(); i++)
			{
				CEdit * pEdit = (CEdit *)GetDlgItem(IDEDIT + 4 * i);
				if(pEdit)
				{
					if(m_ItemInfo[i].bKey)
					{
						// BOOL ModifyStyle( DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0 );
						/*pEdit->ModifyStyle(ES_READONLY, 0);
						pEdit->Invalidate(FALSE);*/
						ModifyControlStyle(pEdit,"Edit",ES_READONLY,0,0,0);
					}
				}
				
			}
		}
		else
		{
			for(int i=0; i< m_ItemInfo.size(); i++)
			{
				CEdit * pEdit = (CEdit *)GetDlgItem(IDEDIT + 4 * i);
				if(pEdit)
				{
					if(m_ItemInfo[i].bKey)
					{
						/*pEdit->ModifyStyle(0, ES_READONLY);
						pEdit->Invalidate(FALSE);*/
						ModifyControlStyle(pEdit,"Edit",0, ES_READONLY, 0,0);

					}
				}

			}
		}

	}
}
