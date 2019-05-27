// FindStockByPinyin.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "MainFrm.h"

#include "FindStockByPinyin.h"


// CFindStockByPinyin 对话框

IMPLEMENT_DYNAMIC(CFindStockByPinyin, CDialog)

CFindStockByPinyin::CFindStockByPinyin(CWnd* pParent /*=NULL*/)
	: CDialog(CFindStockByPinyin::IDD, pParent)
	, m_strPinyin(_T(""))
{

}

CFindStockByPinyin::~CFindStockByPinyin()
{
}

void CFindStockByPinyin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_Combo1);
	DDX_CBString(pDX, IDC_COMBO1, m_strPinyin);
	DDV_MaxChars(pDX, m_strPinyin, 32);
}


BEGIN_MESSAGE_MAP(CFindStockByPinyin, CDialog)
END_MESSAGE_MAP()


// CFindStockByPinyin 消息处理程序

BOOL CFindStockByPinyin::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	if(m_strPinyin.IsEmpty()==FALSE)
	{
		int nIndex = CB_ERR;
		m_Combo1.ResetContent();

		CStockApp * pApp = (CStockApp *)AfxGetApp();
		// 过滤创业版
		for(int i=0; i< pApp->m_CEntrepreneurship.m_nRowNum; i++)
		{
			if(pApp->m_CEntrepreneurship.m_vec_cninfo_pinyin[i][0] == m_strPinyin[0])
			{
				CString str;
				str.Format("%-4s %06d %s",pApp->m_CEntrepreneurship.m_vec_cninfo_pinyin[i].c_str(), \
					pApp->m_CEntrepreneurship.m_vec_a_code[i], \
					pApp->m_CEntrepreneurship.m_vec_abbreviation[i].c_str());
				m_Combo1.AddString(str);
			}
		}

		// 过滤中小版
		for(int i=0; i< pApp->m_CSmallMidEnterprise.m_nRowNum; i++)
		{
			if(pApp->m_CSmallMidEnterprise.m_vec_cninfo_pinyin[i][0] == m_strPinyin[0])
			{
				CString str;
				str.Format("%-4s %06d %s",pApp->m_CSmallMidEnterprise.m_vec_cninfo_pinyin[i].c_str(), \
					pApp->m_CSmallMidEnterprise.m_vec_a_code[i], \
					pApp->m_CSmallMidEnterprise.m_vec_abbreviation[i].c_str());
				m_Combo1.AddString(str);
			}
		}

		// 过滤深圳A股
		for(int i=0; i< pApp->m_CShenzhenAshares.m_nRowNum; i++)
		{
			if(pApp->m_CShenzhenAshares.m_vec_cninfo_pinyin[i][0] == m_strPinyin[0])
			{
				CString str;
				str.Format("%-4s %06d %s",pApp->m_CShenzhenAshares.m_vec_cninfo_pinyin[i].c_str(), \
					pApp->m_CShenzhenAshares.m_vec_a_code[i], \
					pApp->m_CShenzhenAshares.m_vec_abbreviation[i].c_str());
				m_Combo1.AddString(str);
			}
		}

		// 过滤上海A股
		for(int i=0; i< pApp->m_CShanghaiStock.m_nRowNum; i++)
		{
			if(pApp->m_CShanghaiStock.m_vec_cninfo_pinyin[i][0] == m_strPinyin[0])
			{
				CString str;
				str.Format("%-4s %06d %s",pApp->m_CShanghaiStock.m_vec_cninfo_pinyin[i].c_str(), \
					pApp->m_CShanghaiStock.m_vec_a_code[i], \
					pApp->m_CShanghaiStock.m_vec_abbreviation[i].c_str());
				m_Combo1.AddString(str);
			}
		}

		// 模拟键盘，向 m_Combo1 下拉框输入刚才的按键字母
		m_Combo1.PostMessage(WM_CHAR,  m_strPinyin[0]);
	}

	m_Combo1.SetEditTooltip(TRUE);
	m_Combo1.SetListTooltip(TRUE);

	m_Combo1.SetMode(CComboBoxExt::MODE_AUTOCOMPLETE);
	m_Combo1.SetFocus();	// 获得输入焦点

	return FALSE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CFindStockByPinyin::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	UpdateData(TRUE);


	CDialog::OnOK();
}
