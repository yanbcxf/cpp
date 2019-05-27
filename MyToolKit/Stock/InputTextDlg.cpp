// InputTextDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "InputTextDlg.h"


// CInputTextDlg 对话框

IMPLEMENT_DYNAMIC(CInputTextDlg, CDialog)

CInputTextDlg::CInputTextDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputTextDlg::IDD, pParent)
	, m_strInputText(_T(""))
{

}

CInputTextDlg::~CInputTextDlg()
{
}

void CInputTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strInputText);
	DDV_MaxChars(pDX, m_strInputText, 64);
}


BEGIN_MESSAGE_MAP(CInputTextDlg, CDialog)
END_MESSAGE_MAP()


// CInputTextDlg 消息处理程序
