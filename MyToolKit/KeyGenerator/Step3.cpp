// Step3.cpp : implementation file
//

#include "stdafx.h"
#include "KeyGenerator.h"
#include "Step3.h"
#include "KeyGeneratorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStep3 dialog

extern CKeyGeneratorDlg *g_Maindlg; 

CStep3::CStep3(CWnd* pParent /*=NULL*/)
	: CDialog(CStep3::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStep3)
	m_strDid = _T("");
	//}}AFX_DATA_INIT
}


void CStep3::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStep3)
	DDX_Text(pDX, IDC_EDIT_CAMNUM, m_strDid);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CStep3, CDialog)
	//{{AFX_MSG_MAP(CStep3)
	ON_BN_CLICKED(IDC_PREV, OnPrev)
	ON_BN_CLICKED(IDC_ENYG, OnEnyg)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUT_BROWSE, OnButBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStep3 message handlers

void CStep3::OnPrev() 
{
	// TODO: Add your control notification handler code here
	int nWnd = 2;
	::SendMessage(g_Maindlg->m_hWnd,WM_SET_WNDNO,(WPARAM)&nWnd,0);
	
}

void CStep3::OnEnyg() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	int nSize;
	int nPos;
	CString Did;
	CString subDid;
	CString smg;

	listDVID.RemoveAll();
	subDid.Format("%s",m_strDid);
	//需要一行一行的读
	nPos = subDid.Find(";");

	while (nPos>0)
	{
		nSize = subDid.GetLength();
		Did.Format("%s",subDid.Left(nPos));
 		Did.TrimLeft("");	//去掉首尾空格
 		Did.TrimRight("");
		listDVID.AddTail(Did);
		subDid = subDid.Right(nSize-nPos-1);
		nPos = subDid.Find(";");
	}
	//最后一个没有";",直接添加
	if(subDid!="")
		listDVID.AddTail(subDid);

	::SendMessage(g_Maindlg->m_hWnd,WM_GET_SLC,0,0);
	
}

BOOL CStep3::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CStep3::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	::SendMessage(g_Maindlg->m_hWnd,WM_CLOSE_WND,0,0);
	
	CDialog::OnClose();
}

void CStep3::OnButBrowse() 
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
	CString did;
	CString tmp;
	listDVID.RemoveAll();
	m_strDid.Format("");

	//需要一行一行的读
	while(myfile.ReadString(szBuff,256))
	{
		did.Format("%s",szBuff);
// 		did=did.Left(6);
		did.TrimRight("\n\r");
		listDVID.AddTail(did);
		did.Format("%s;",did);
		tmp+=did;
	}
	myfile.Close();
	int pos = tmp.ReverseFind(';');
	m_strDid=tmp.Left(pos);
	UpdateData(FALSE);
}
