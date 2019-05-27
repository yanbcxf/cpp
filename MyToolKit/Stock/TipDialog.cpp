// TipDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "TipDialog.h"


// CTipDialog 对话框

IMPLEMENT_DYNAMIC(CTipDialog, CDialog)

CTipDialog::CTipDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CTipDialog::IDD, pParent)
{

}

CTipDialog::~CTipDialog()
{
}

void CTipDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRID, m_Grid);
}


BEGIN_MESSAGE_MAP(CTipDialog, CDialog)
END_MESSAGE_MAP()


// CTipDialog 消息处理程序

BOOL CTipDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_Grid.EnableDragAndDrop(TRUE);
	m_Grid.GetDefaultCell(FALSE, FALSE)->SetBackClr(RGB(0xFF, 0xFF, 0xE0));

	m_Grid.SetRowCount(m_lstTip.size() + 1);
	m_Grid.SetColumnCount(4);

	m_Grid.SetFixedRowCount(1);
	m_Grid.SetFixedColumnSelection(FALSE);
	m_Grid.SetFixedRowSelection(FALSE);
	m_Grid.EnableColumnHide();
	m_Grid.SetEditable(FALSE);

	GV_ITEM Item;
	Item.mask = GVIF_TEXT|GVIF_FORMAT;
	Item.row = 0;
	Item.col = 0;
	Item.strText.Format(_T("%s"), "日期");
	m_Grid.SetItem(&Item);

	Item.col = 1;
	Item.strText.Format(_T("%s"), "标题");
	m_Grid.SetItem(&Item);

	Item.col = 2;
	Item.strText.Format(_T("%s"), "URL");
	m_Grid.SetItem(&Item);

	list<CTipRecord>::iterator it_tip = m_lstTip.begin();
	for(int nRow = 0 ; it_tip != m_lstTip.end(); it_tip++, nRow++)
	{
		GV_ITEM Item;
		Item.mask = GVIF_TEXT|GVIF_FORMAT;
		Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
		Item.row = nRow + 1;
		Item.col = 0;
		Item.strText.Format(_T("%s"), it_tip->report_date.c_str());
		m_Grid.SetItem(&Item);

		Item.col = 1;
		Item.strText.Format(_T("%s"), it_tip->title.c_str());
		m_Grid.SetItem(&Item);

		Item.col = 2;
		Item.strText.Format(_T("%s"), it_tip->url.c_str());
		if (!m_Grid.SetCellType(nRow + 1, 2, RUNTIME_CLASS(CGridURLCell)))
			return FALSE;
		m_Grid.SetItem(&Item);
	}


	m_Grid.AutoSize();
	m_Grid.SetCompareFunction(CGridCtrl::pfnCellNumericCompare);
	m_Grid.SetColumnWidth(2, 160);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
