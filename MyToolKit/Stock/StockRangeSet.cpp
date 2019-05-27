// StockRangeSet.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "StockRangeSet.h"


// CStockRangeSet 对话框

IMPLEMENT_DYNAMIC(CStockRangeSet, CDialog)

CStockRangeSet::CStockRangeSet(CWnd* pParent /*=NULL*/)
	: CDialog(CStockRangeSet::IDD, pParent)
	, m_StartDate(COleDateTime::GetCurrentTime())
	, m_End_Date(COleDateTime::GetCurrentTime())
{

}

CStockRangeSet::~CStockRangeSet()
{
}

void CStockRangeSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_DateTimeCtrl(pDX, IDC_START_DATE, m_StartDate);
	DDX_DateTimeCtrl(pDX, IDC_END_DATE, m_End_Date);
	DDX_Control(pDX, IDC_LIST_VIEWTYPE, m_listViewType);
}


BEGIN_MESSAGE_MAP(CStockRangeSet, CDialog)
END_MESSAGE_MAP()


// CStockRangeSet 消息处理程序

BOOL CStockRangeSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	LONG lStyle;
	lStyle = GetWindowLong(m_listViewType.m_hWnd, GWL_STYLE);//获取当前窗口style
	lStyle &= ~LVS_TYPEMASK;	//清除显示方式位
	lStyle |= LVS_REPORT;		//设置style
	SetWindowLong(m_listViewType.m_hWnd, GWL_STYLE, lStyle);//设置style

	DWORD dwStyle = m_listViewType.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;	//选中某行使整行高亮（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_GRIDLINES;		//网格线（只适用与report风格的listctrl）
	dwStyle |= LVS_EX_CHECKBOXES;		//item前生成checkbox控件
	m_listViewType.SetExtendedStyle(dwStyle);	//设置扩展风格

	m_listViewType.InsertColumn(0,"曲线类型",LVCFMT_LEFT,100);
	
	int nRow = m_listViewType.InsertItem(0, "Open");//插入行
	m_listViewType.InsertItem(1, "High");
	m_listViewType.InsertItem(2, "Low");
	m_listViewType.InsertItem(3, "Close");
	m_listViewType.InsertItem(4, "PE");
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CStockRangeSet::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	int k = m_listViewType.GetItemCount();
	m_strViewType = "";
	for(int i=0; i<k; i++)
	{
		if(m_listViewType.GetCheck(i))
		{
			char tmp[128];
			memset(tmp, 0 ,128);
			m_listViewType.GetItemText(i, 0, tmp,128);
			if(m_strViewType.empty()==false)
				m_strViewType += ",";
			m_strViewType += string(tmp);
		}
	}

	CDialog::OnOK();
}
