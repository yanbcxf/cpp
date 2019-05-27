// Newmat11TestView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "CalcNewmat11Test.h"
#include "Newmat11TestView.h"


// CNewmat11TestView

IMPLEMENT_DYNCREATE(CNewmat11TestView, CBaseGridCtlView)

CNewmat11TestView::CNewmat11TestView()
{

}

CNewmat11TestView::~CNewmat11TestView()
{
}

BEGIN_MESSAGE_MAP(CNewmat11TestView, CBaseGridCtlView)
END_MESSAGE_MAP()


// CNewmat11TestView 绘图

void CNewmat11TestView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CNewmat11TestView 诊断

#ifdef _DEBUG
void CNewmat11TestView::AssertValid() const
{
	CBaseGridCtlView::AssertValid();
}

#ifndef _WIN32_WCE
void CNewmat11TestView::Dump(CDumpContext& dc) const
{
	CBaseGridCtlView::Dump(dc);
}
#endif
#endif //_DEBUG


// CNewmat11TestView 消息处理程序



void CNewmat11TestView::OnInitialUpdate()
{
	CBaseGridCtlView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();


	Startup();	
}



void CNewmat11TestView::Startup()
{
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();
	CCalcNewmat11Test * cse =  new CCalcNewmat11Test(m_hWnd, WM_USER_LOG_1);

	
	cse->ScheduleTask(&CCalcNewmat11Test::Execute1);
}


void CNewmat11TestView::ProcessXmlWParam(string msgType,string code,string model,\
										 string param1,string param2,string param3)
{
	if(msgType=="TaskFinished")
	{
		stringstream ss;
		ss << "任务【" << (param1) << "】 计算完成. ";

		CCalculateTask * cse = CCalculateTask::FetchTask(atol(param1.c_str()));
		RedrawGridCtrl(cse);
		if(cse!=NULL)
			delete cse;
		sendToOutput(ss.str().c_str(), NULL, WM_USER_LOG_1);
		
	}
	//RedrawGridCtrl();
}


void CNewmat11TestView::RedrawGridCtrl(CCalculateTask * cse)
{

	CCalcNewmat11Test * pStockInfoEx = (CCalcNewmat11Test *)cse;
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	if(m_pGridCtrl)
	{
		CStockApp * pApp = (CStockApp *)AfxGetApp();

		try {
			m_pGridCtrl->SetRowCount(pStockInfoEx->m_result.nrows()+1);
			m_pGridCtrl->SetColumnCount(pStockInfoEx->m_result.ncols()+1);
			m_pGridCtrl->SetFixedRowCount(1);
			m_pGridCtrl->SetFixedColumnCount(1);
			m_pGridCtrl->SetHeaderSort(TRUE);
		}
		catch (CMemoryException* e)
		{
			e->ReportError();
			e->Delete();
			return;
		}

		for (int row = 0; row < m_pGridCtrl->GetRowCount(); row++)
		{
			for (int col = 0; col < m_pGridCtrl->GetColumnCount(); col++)
			{ 
				GV_ITEM Item;
				Item.mask = GVIF_TEXT|GVIF_FORMAT;
				Item.row = row;
				Item.col = col;
				if (row < 1) {
					Item.nFormat = DT_LEFT|DT_WORDBREAK;
					string val;
					val = Int2String(col);
					Item.strText.Format(_T("%s"), val.c_str());
				} 
				else 
				{
					if(col==0)
						Item.nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;
					else
						Item.nFormat = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX;

					

					string val ;
					if(col==0)		
						val = Int2String(row);
					else
						val = Double2String(pStockInfoEx->m_result(row, col));

					Item.strText.Format(_T("%s"), val.c_str());

					/*if(col==10 )
					{
						Item.mask    |= (GVIF_FGCLR);
						if(pStockInfoEx->m_vec_recent_forecast_rate[row-1]>0)
							Item.crFgClr = RGB(255, 64, 64);
						else
							Item.crFgClr = RGB(0, 163, 0);
					}*/


				}
				m_pGridCtrl->SetItem(&Item);
			}
			int k = 1;
		}

		// m_pGridCtrl->AutoSize();
		//m_pGridCtrl->SetColumnWidth(2, 80);
	}

}


