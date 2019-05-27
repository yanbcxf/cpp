// PDFStructureDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "PDFStructureDlg.h"


// CPDFStructureDlg 对话框

IMPLEMENT_DYNAMIC(CPDFStructureDlg, CDialog)

CPDFStructureDlg::CPDFStructureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPDFStructureDlg::IDD, pParent)
{

}

CPDFStructureDlg::~CPDFStructureDlg()
{
}

void CPDFStructureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_GRID, m_Grid);
}


BEGIN_MESSAGE_MAP(CPDFStructureDlg, CDialog)
END_MESSAGE_MAP()


// CPDFStructureDlg 消息处理程序

BOOL CPDFStructureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	vector<int>		vecCode;
	vector<string>	vecContent;
	vector<int>		vecParentCode;
	string			strAnnouncementTitle;
	string			strAdjuncUrl;

	try{
		string test = "select code, content, parent_code, announcementTitle, adjunctUrl ";
		test += "from cpdfanalysis where announcementId = '" + m_announcementId + "' order by code ";
		session sql(g_MysqlPool);
		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		while(st.fetch())
		{
			vecCode.insert(vecCode.end(), r.get<int>("code"));
			vecContent.insert(vecContent.end(), r.get<string>("content"));
			vecParentCode.insert(vecParentCode.end(), r.get<int>("parent_code"));
		}
	}
	catch(...){}

	if(vecCode.size()==0 || vecCode.size()!=vecContent.size() ||
		vecCode.size()!=vecParentCode.size())
		return FALSE;

	// Tree related stuff
	GTV_HTREENODE hItem1;
	
	map<int, GTV_HTREENODE>		mapHitem;
	
	{        
        m_Grid.SetAutoSizeStyle();

		TRY {
		    //m_Grid.SetRowCount(m_nRows);
		    //m_Grid.SetColumnCount(m_nCols);
		    m_Grid.SetFixedRowCount(1);
		    m_Grid.SetFixedColumnCount(1);
	    }
	    CATCH (CMemoryException, e)
	    {
	    	e->ReportError();
    		return FALSE;
	    }
        END_CATCH
		
		m_Grid.SetRedraw(FALSE);

		m_Grid.SetTreeColumnCellTypeID(CT_CHECKBOX);	

		m_Grid.InsertColumn(_T("结构层次"));
		m_Grid.InsertColumn(_T("内容"));
			
		m_Grid.GetDefaultCell(FALSE,FALSE)->SetFormat(DT_WORDBREAK|DT_EDITCONTROL|DT_NOPREFIX|DT_END_ELLIPSIS);		
		m_Grid.EnableTitleTips(FALSE);		//	关闭每个单元格的文本超长的 气球提示
		m_Grid.SetEditable(FALSE);

		m_Grid.SetRedraw(TRUE);
		
		m_Grid.SetTreeColumnCellTypeID(CT_DEFAULT);
		for(int i=0; i<vecCode.size(); i++)
		{
			CString str;
			str.Format("%d", i);
			if(vecParentCode[i]==-1)
			{
				hItem1 = m_Grid.InsertItem(str);
				mapHitem[vecCode[i]] = hItem1;
			}
			else
			{
				hItem1 = m_Grid.InsertItem(str, mapHitem[vecParentCode[i]]);
				mapHitem[vecCode[i]] = hItem1;
			}
		}

		m_Grid.SetColumnWidth(0, 18);
		m_Grid.SetColumnWidth(2, 700);
		  	    	    	
		GV_ITEM Item;

		Item.mask = GVIF_TEXT;
		Item.row = 0;
		Item.col = 0;
		Item.strText = _T("Root Item3");
		/*Item.iImage = rand()%m_ImageList.GetImageCount();
		Item.mask  |= (GVIF_IMAGE);     */           
		
		
		//**************************FILL / REFILL TABLE***************************************
		// The Fill/Refill code can be in a separate function to be called whenever necessary
		m_Grid.SetRedraw(FALSE);
		
		
		// fill rows/cols with Data
		for (int row = 1; row < m_Grid.GetItemCount(); row++)
		{
    		for (int col = 2; col < m_Grid.GetColumnCount(); col++)
			{ 

				CString strrr = m_Grid.GetItemText(row,1);
				int idx = atoi(strrr.GetBuffer());

				Item.mask = GVIF_TEXT;
    			Item.row = row;
	    		Item.col = col;

				CString str;
				str.Format("%s", vecContent[idx].c_str());
				Item.strText = str;

				m_Grid.SetItem(&Item);
			}
		}
		
	}

	m_Grid.SetRedraw(TRUE);

	// Write extra codes for setting up the treelistctrl for sorting
	m_Grid.SetHeaderSort(FALSE);

	//	确定行的高度
	for (int row = 1; row < m_Grid.GetItemCount(); row++)
	{
		CString strrr = m_Grid.GetItemText(row,2);
		CGridCellBase* pCell = m_Grid.GetCell(row,2);
		if(pCell)
		{
			CSize si = pCell->GetTextExtentByWidth(strrr, 700);
			m_Grid.SetRowHeight(row, si.cy);
		}

	}
	

	//m_Grid.AutoSize(GVS_HEADER);
	//m_Grid.AutoSizeRows();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
