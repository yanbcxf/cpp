// FindStockByCode.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "FindStockByCode.h"


// CFindStockByCode 对话框

IMPLEMENT_DYNAMIC(CFindStockByCode, CDialog)

CFindStockByCode::CFindStockByCode(CWnd* pParent /*=NULL*/)
	: CDialog(CFindStockByCode::IDD, pParent)
	, m_strCode(_T(""))
{

}

CFindStockByCode::~CFindStockByCode()
{
}

void CFindStockByCode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_Combo1);
	DDX_CBString(pDX, IDC_COMBO1, m_strCode);
	DDV_MaxChars(pDX, m_strCode, 32);
}


BEGIN_MESSAGE_MAP(CFindStockByCode, CDialog)
END_MESSAGE_MAP()


// CFindStockByCode 消息处理程序

BOOL CFindStockByCode::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	SetWindowText(m_strCaption);

	int nIndex = CB_ERR;
	m_Combo1.ResetContent();

	CStockApp * pApp = (CStockApp *)AfxGetApp();
	for(int i=0; i< m_pStockInfo->m_nRowNum; i++)
	{
		CString str;
		str.Format("%06d   %s",m_pStockInfo->m_vec_a_code[i],m_pStockInfo->m_vec_abbreviation[i].c_str());
		m_Combo1.AddString(str);
	}
		

	m_Combo1.SetEditTooltip(TRUE);
	m_Combo1.SetListTooltip(TRUE);

	m_Combo1.SetMode(CComboBoxExt::MODE_AUTOCOMPLETE);
	m_Combo1.SetFocus();	// 获得输入焦点

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
