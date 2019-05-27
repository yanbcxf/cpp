// Step2.cpp : implementation file
//

#include "stdafx.h"
#include "KeyGenerator.h"
#include "Step2.h"
#include "KeyGeneratorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStep2 dialog
extern CKeyGeneratorDlg *g_Maindlg; 


CStep2::CStep2(CWnd* pParent /*=NULL*/)
	: CDialog(CStep2::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStep2)
	m_strVersion = _T("");
	//}}AFX_DATA_INIT
}


void CStep2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStep2)
	DDX_Text(pDX, IDC_EDIT_VERSION, m_strVersion);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStep2, CDialog)
	//{{AFX_MSG_MAP(CStep2)
	ON_BN_CLICKED(IDC_NEXT, OnNext)
	ON_BN_CLICKED(IDC_PREV, OnPrev)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUT_BROWSE, OnButBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStep2 message handlers

void CStep2::OnNext() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int nSize;
	int nPos;
	CString Did;
	CString subDid;

	listMac.RemoveAll();
// 	if(m_strVersion)
	subDid.Format("%s",m_strVersion);
	//需要一行一行的读
	nPos = subDid.Find(";");

	while (nPos>0)
	{
		nSize = subDid.GetLength();
		Did.Format("%s",subDid.Left(nPos));
		listMac.AddTail(Did);
		subDid = subDid.Right(nSize-nPos-1);
		nPos = subDid.Find(";");
	}
	//最后一个没有";",直接添加

	if(subDid!="")
		listMac.AddTail(subDid);

	int nWnd = 3;
	::SendMessage(g_Maindlg->m_hWnd,WM_SET_WNDNO,(WPARAM)&nWnd,0);

}

void CStep2::OnPrev() 
{
	// TODO: Add your control notification handler code here
	int nWnd = 1;
	::SendMessage(g_Maindlg->m_hWnd,WM_SET_WNDNO,(WPARAM)&nWnd,0);
}

BOOL CStep2::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CStep2::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	::SendMessage(g_Maindlg->m_hWnd,WM_CLOSE_WND,0,0);
	
	CDialog::OnClose();
}

void CStep2::OnButBrowse() 
{
	// TODO: Add your control notification handler code here
	CStdioFile myfile;
	char szFileName[MAX_PATH]={0};
	
	CFileDialog fd(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"ini files(*.*)|*.*||");
    
    if (IDOK != fd.DoModal())
    {
		return;
    }
	sprintf(szFileName, "%s", fd.GetPathName());
	
	if (myfile.Open(szFileName, CFile::modeRead)==FALSE)
	{
		AfxMessageBox("打开MAC导入文件失败");
		return;
	}
	char szBuff[256]={0};
	int nSize;
	CString mac;
	CString tmp;

	listMac.RemoveAll();
	m_strVersion.Format("");
	//需要一行一行的读
	while(myfile.ReadString(szBuff,256))
	{
		mac.Format("%s",szBuff);
		mac.TrimRight("\n\r");
		listMac.AddTail(mac);
		mac.Format("%s;",szBuff);
		tmp+=mac;
	}
	myfile.Close();
	int pos = tmp.ReverseFind(';');
	m_strVersion=tmp.Left(pos);
	UpdateData(FALSE);
}
