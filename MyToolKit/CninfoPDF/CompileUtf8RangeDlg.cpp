// CompileUtf8RangeDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "CninfoPDF.h"
#include "CompileUtf8RangeDlg.h"

#include "compileUTF8range.h"

// CCompileUtf8RangeDlg 对话框

IMPLEMENT_DYNAMIC(CCompileUtf8RangeDlg, CMyDialog)

CCompileUtf8RangeDlg::CCompileUtf8RangeDlg(CWnd* pParent /*=NULL*/)
	: CMyDialog(CCompileUtf8RangeDlg::IDD, pParent)
	, m_strStart(_T(""))
	, m_strEnd(_T(""))
	, m_strResult(_T(""))
{

}

CCompileUtf8RangeDlg::~CCompileUtf8RangeDlg()
{
}

void CCompileUtf8RangeDlg::DoDataExchange(CDataExchange* pDX)
{
	CMyDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_START, m_strStart);
	DDX_Text(pDX, IDC_EDIT_END, m_strEnd);
	DDX_Text(pDX, IDC_EDIT_RESULT, m_strResult);
}


BEGIN_MESSAGE_MAP(CCompileUtf8RangeDlg, CMyDialog)
	ON_BN_CLICKED(IDC_BTN_CACULATE, &CCompileUtf8RangeDlg::OnBnClickedBtnCaculate)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CCompileUtf8RangeDlg 消息处理程序

void CCompileUtf8RangeDlg::OnBnClickedBtnCaculate()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();

	m_strStart.Trim();
	m_strEnd.Trim();
	if(m_strStart.IsEmpty()==FALSE && m_strEnd.IsEmpty()==FALSE)
	{
		string  strStart = Gbk2Utf8(m_strStart.GetBuffer());
		string	strEnd = Gbk2Utf8(m_strEnd.GetBuffer());

		size_t unicode_number;
		int idx = 0;
		char * pStart = compileUTF8range::Utf82Unicode((const char *)strStart.c_str(), &unicode_number);
		char * pEnd = compileUTF8range::Utf82Unicode((const char *)strEnd.c_str(), &unicode_number);

		string strResult = compileUTF8range::compileUTF8range(*((unsigned short *)pStart), *((unsigned short *)pEnd));

		char * pGbk = m_strStart.GetBuffer();
		
		strResult +=  string("    " ) +  string(m_strStart.GetBuffer()) + " GBK :";
		int nLen = strlen(pGbk);
		for(int  i=0; i< nLen; i++)
		{
			strResult += compileUTF8range::toHex(pGbk[i]);
		}

		m_strResult.Format("%s", strResult.c_str());
		UpdateData(FALSE);
	}
}

BOOL CCompileUtf8RangeDlg::OnInitDialog()
{
	CMyDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	compileUTF8range::init();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CCompileUtf8RangeDlg::OnSize(UINT nType, int cx, int cy)
{
	CMyDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
}
