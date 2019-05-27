// Step1.cpp : implementation file
//

#include "stdafx.h"
#include "KeyGenerator.h"
#include "Step1.h"
#include "KeyGeneratorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStep1 dialog

extern CKeyGeneratorDlg *g_Maindlg; 

CStep1::CStep1(CWnd* pParent /*=NULL*/)
	: CDialog(CStep1::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStep1)
	m_strSlc = _T("");
	//}}AFX_DATA_INIT
}


void CStep1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStep1)
	DDX_Text(pDX, IDC_EDIT_SLC, m_strSlc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStep1, CDialog)
	//{{AFX_MSG_MAP(CStep1)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_PREV1, OnPrev1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUT_BROWSE, OnButBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStep1 message handlers

void CStep1::OnNext() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	int nWnd = 2;
	::SendMessage(g_Maindlg->m_hWnd,WM_SET_WNDNO,(WPARAM)&nWnd,0);
}

BOOL CStep1::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CStep1::OnPrev1() 
{
	// TODO: Add your control notification handler code here
	int nWnd = 0;
	::SendMessage(g_Maindlg->m_hWnd,WM_SET_WNDNO,(WPARAM)&nWnd,0);
	
}

void CStep1::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	::SendMessage(g_Maindlg->m_hWnd,WM_CLOSE_WND,0,0);
	
	CDialog::OnClose();
}

void CStep1::OnButBrowse() 
{
	// TODO: Add your control notification handler code here
	FILE *fpslc;	
	char szFileName[MAX_PATH]={0};
	
	CFileDialog fd(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"ini files(*.ini)|*.ini||");
    
    if (IDOK == fd.DoModal())
    {
		sprintf(szFileName, "%s", fd.GetPathName());
    }

	fpslc = fopen(szFileName, "rb");
	if (fpslc==NULL)
	{
		fclose(fpslc);
		return;
	}
	char szBuff[512]={0};
	int nSize;
	nSize = fread(szBuff, 1, 512, fpslc);
	if (nSize>0)
	{
		m_strSlc.Format("%s",szBuff);
	}
	UpdateData(FALSE);
}
