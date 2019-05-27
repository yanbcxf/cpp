// IndividualDmGraphView.cpp : 实现文件
//

#include "stdafx.h"
#include "Stock.h"
#include "IndividualShareDoc.h"
#include "BaseMessageView.h"
#include "IndividualDmGraphView.h"

#include "StockRangeSet.h"

// CIndividualDmGraphView

IMPLEMENT_DYNCREATE(CIndividualDmGraphView, CBaseMessageView)

CIndividualDmGraphView::CIndividualDmGraphView()
{
	m_EndDate = -1.0;
	m_StartDate = -1.0;
}

CIndividualDmGraphView::~CIndividualDmGraphView()
{
}

BEGIN_MESSAGE_MAP(CIndividualDmGraphView, CBaseMessageView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_SET_STOCK_RANGE, &CIndividualDmGraphView::OnSetStockRange)
END_MESSAGE_MAP()


// CIndividualDmGraphView 绘图

void CIndividualDmGraphView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// CIndividualDmGraphView 诊断

#ifdef _DEBUG
void CIndividualDmGraphView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CIndividualDmGraphView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CIndividualDmGraphView 消息处理程序

int CIndividualDmGraphView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	RECT rc;
	GetClientRect(&rc);

	CRect rect(100, 10, 200, 110);

	m_DMGraph.Create(NULL, WS_CHILD | WS_VISIBLE, rect, this, 1);

	//	设置 X 轴的时间坐标
	LPDISPATCH lpdis = m_DMGraph.get_Axis(0/*HorizontalX*/);
	if(lpdis)
	{
		CDMGraphAxis dmgraphAxis(lpdis);
		dmgraphAxis.put_Format("%Y-%m-%d");
		dmgraphAxis.put_Time(VARIANT_TRUE);
	}

	//	设置 Y 轴的价格坐标
	lpdis = m_DMGraph.get_Axis(1/*HorizontalY*/);
	if(lpdis)
	{
		CDMGraphAxis dmgraphAxis(lpdis);
		dmgraphAxis.put_Format("%.2f");
		dmgraphAxis.put_GridNumber(10);  /* Y Grid Step */
	}

	m_DMGraph.put_LabelColor(RGB(0,0,0));
	//m_DMGraph.put_TrackMode(3 /*TrackModeState.Zoom*/);
	
	//

	return 0;
}

void CIndividualDmGraphView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rect;
	GetClientRect(&rect);

	m_DMGraph.MoveWindow(rect);
}

void CIndividualDmGraphView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	CIndividualShareDoc * pDoc = (CIndividualShareDoc *)GetDocument();

	CStockApp * pApp = (CStockApp *)AfxGetApp();
	//JMutexAutoLock lock(pApp->m_mutexStockinfo);
	StockInfo_Reqeust req;
	req.code = atoi(pDoc->m_nCode.c_str());
	req.hwnd = m_hWnd;
	pApp->m_listStockinfoReq.push_front(req);
}

void CIndividualDmGraphView::ClearGraphData()
{
	long i, nElementCount = 0;
	BOOL bReplace = FALSE;

	LPDISPATCH lpdis = m_DMGraph.get_Elements();
	if(lpdis==NULL)
		return;

	//	当有多条曲线时， nElementCount >1 
	CDMGraphCollection dmgraphCollection(lpdis);
	nElementCount = dmgraphCollection.get_Count();

	for(i=0; i<nElementCount; i++)
	{
		lpdis = dmgraphCollection.get_Item(i);
		if(lpdis==NULL)
			continue;
		dmgraphCollection.Delete(i);	

	}
}


void CIndividualDmGraphView::SetGraphData(VARIANT* pvarrX, VARIANT* pvarrY, LPCTSTR szName, COLORREF clr, int lineType, int pointSize)
{
	ATLASSERT(pvarrX);
	ATLASSERT(pvarrY);
	ATLASSERT(szName);

	long i, nElementCount = 0;
	BOOL bReplace = FALSE;

	LPDISPATCH lpdis = m_DMGraph.get_Elements();
	if(lpdis==NULL)
		return;

	//	当有多条曲线时， nElementCount >1 
	CDMGraphCollection dmgraphCollection(lpdis);
	nElementCount = dmgraphCollection.get_Count();

	for(i=0; i<nElementCount; i++)
	{
		lpdis = dmgraphCollection.get_Item(i);
		if(lpdis==NULL)
			continue;

		CDMGraphElement dmgraphElement(lpdis);
		CString bsElemName = dmgraphElement.get_Name();

		if(_tcsicmp(bsElemName.GetBuffer(), szName) == 0)
		{
			//	该处代码，正常情况下不会执行到
			bReplace = TRUE;

			//dmgraphCollection.Delete(i);
			
			//	存在同名的曲线，则替换
			dmgraphElement.put_PointSize(pointSize);
			dmgraphElement.put_LineColor(clr);
			dmgraphElement.put_LineWidth(1);
			dmgraphElement.Plot(*pvarrX, *pvarrY);
			break;
		}

	}

	if(bReplace == FALSE )
	{

		lpdis = dmgraphCollection.Add();
		if(lpdis==NULL)
			return;
		CDMGraphElement dmgraphElement(lpdis);
		dmgraphElement.put_Name(szName);
		dmgraphElement.put_PointSymbol(1 /*Dots*/);
		dmgraphElement.put_PointSize(pointSize);
		dmgraphElement.put_LineColor(clr);
		dmgraphElement.put_LineWidth(1);
		dmgraphElement.put_LineType(lineType);
		dmgraphElement.Plot(*pvarrX, *pvarrY);
	}
	
}

void CIndividualDmGraphView::SetGraphAnnotation(double labelx, double labely, string labelCaption, COLORREF clr)
{
	LPDISPATCH lpdis = m_DMGraph.get_Annotations();
	if(lpdis==NULL)
		return;

	long  nAnnotationCount = 0;
	//	当有多条曲线时， nElementCount >1 
	CDMGraphCollection dmgraphCollection(lpdis);
	nAnnotationCount = dmgraphCollection.get_Count();

	{

		lpdis = dmgraphCollection.Add();
		if(lpdis==NULL)
			return;
		CDMGraphAnnotation dmgraphAnnotation(lpdis);
		dmgraphAnnotation.put_LabelX(labelx);
		dmgraphAnnotation.put_LabelY(labely);
		dmgraphAnnotation.put_LabelColor(RGB(0,0,0));
		dmgraphAnnotation.put_LabelBkColor(clr);
		dmgraphAnnotation.put_LabelCaption(labelCaption.c_str());
	}
}



void CIndividualDmGraphView::OnSetStockRange()
{
	// TODO: 在此添加命令处理程序代码
	CStockRangeSet aboutDlg;
	if(m_StartDate>0 && m_EndDate> m_StartDate)
	{
		aboutDlg.m_StartDate.m_dt = m_StartDate;
		aboutDlg.m_End_Date.m_dt = m_EndDate;
	}
	aboutDlg.DoModal();

	m_StartDate = aboutDlg.m_StartDate;
	m_EndDate = aboutDlg.m_End_Date;
	//m_strViewType = aboutDlg.m_strViewType;

	m_DMGraph.put_TrackMode(0 /*TrackModeState.None*/);
	//ClearGraphData();
	m_DMGraph.ClearGraph();
	RedrawDmGraph();
	m_DMGraph.put_TrackMode(3 /*TrackModeState.Zoom*/);
}

void CIndividualDmGraphView::ProcessXmlWParam(string msgType, string code,string model,\
											  string param1,string param2,string param3)
{
	m_DMGraph.put_TrackMode(0 /*TrackModeState.None*/);
	//ClearGraphData();
	m_DMGraph.ClearGraph();
	RedrawDmGraph();
	m_DMGraph.put_TrackMode(3 /*TrackModeState.Zoom*/);
}