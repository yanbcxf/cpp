// SimpleInputBox.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "SimpleInputBox.h"


// CSimpleInputBox 对话框

IMPLEMENT_DYNAMIC(CSimpleInputBox, CDialog)

CSimpleInputBox::CSimpleInputBox(CWnd* pParent /*=NULL*/)
	: CDialog(CSimpleInputBox::IDD, pParent)
	, m_strInput(_T(""))
{
	m_strCaption=_T("输入对话框");
}

CSimpleInputBox::~CSimpleInputBox()
{
}

void CSimpleInputBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ComboBox);
	DDX_CBString(pDX, IDC_COMBO1, m_strInput);
}


BEGIN_MESSAGE_MAP(CSimpleInputBox, CDialog)
END_MESSAGE_MAP()


// CSimpleInputBox 消息处理程序

BOOL CSimpleInputBox::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowText(m_strCaption);

	for(int i = 0; i< m_ComboString.size(); i++)
	{
		m_ComboBox.AddString(m_ComboString[i].c_str());
	}

	/*m_ComboBox.AddString("股权激励");
	m_ComboBox.AddString("股票激励计划（草案）");
	m_ComboBox.AddString("停牌公告");
	m_ComboBox.AddString("停牌进展公告");
	m_ComboBox.AddString("复牌公告");
	m_ComboBox.AddString("股票预案");
	m_ComboBox.AddString("修订稿");*/

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
