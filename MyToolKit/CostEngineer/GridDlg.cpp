// PDFStructureDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CostEngineer.h"
#include "GridDlg.h"


// CPDFStructureDlg 对话框

IMPLEMENT_DYNAMIC(CGridDlg, CDialog)

CGridDlg::CGridDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGridDlg::IDD, pParent)
{

}

CGridDlg::~CGridDlg()
{
}

void CGridDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_GRID, m_Grid);
}


BEGIN_MESSAGE_MAP(CGridDlg, CDialog)
END_MESSAGE_MAP()


// CPDFStructureDlg 消息处理程序

BOOL CGridDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	CCostEngineerApp * pApp = (CCostEngineerApp *)AfxGetApp();

	try {
		m_Grid.SetRowCount(m_vecData.size() + 1);
		m_Grid.SetColumnCount(m_vecHeader.size());
		m_Grid.SetFixedRowCount(1);
		m_Grid.SetFixedColumnCount(0);
		m_Grid.SetHeaderSort(FALSE);
		m_Grid.SetEditable(TRUE);
	}
	catch (CMemoryException* e)
	{
		e->ReportError();
		e->Delete();
		return TRUE;
	}

	for (int row = 0; row < m_Grid.GetRowCount(); row++)
	{
		for (int col = 0; col < m_Grid.GetColumnCount(); col++)
		{
			GV_ITEM Item;
			Item.mask = GVIF_TEXT | GVIF_FORMAT;
			Item.row = row;
			Item.col = col;
			if (row < 1) {
				Item.nFormat = DT_LEFT | DT_WORDBREAK;

				Item.strText.Format(_T("%s"), m_vecHeader[col].c_str());
			}
			else
			{
				if (col == 0)
					Item.nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;
				else
					Item.nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX;

				if (col >0)
				{
					if (!m_Grid.SetCellType(row, col, RUNTIME_CLASS(CGridCellNumeric)))
						return TRUE;
				}

				Item.strText.Format(_T("%s   "), m_vecData[row-1][col].c_str());
			}

			m_Grid.SetItem(&Item);
		}

	}

	m_Grid.ExpandColumnsToFit();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
