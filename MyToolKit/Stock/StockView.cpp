
// StockView.cpp : CStockView 类的实现
//

#include "stdafx.h"
#include "Stock.h"
#include "CDMGraphCtrl.h"


#include "StockDoc.h"
#include "StockView.h"

#include "StockRangeSet.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
//////////////////////// Xml 文件解析 //////////////////////



class ElementsParser: public XmlParser 
{

public:
	ElementsParser(LPVOID pContext):XmlParser(pContext)
	{
		CStockView * pStockView = (CStockView *)m_pContext;
		CStockDoc * pDoc = (CStockDoc *)pStockView->GetDocument();

		bInMsgType = false;
		msg_type = "";
		
	}

	~ElementsParser()
	{
		CStockView * pStockView = (CStockView *)m_pContext;
		CStockDoc * pDoc = (CStockDoc *)pStockView->GetDocument();
		stringstream ss;
		ss.str("");
		ss << " data 元素个数 = " << pDoc->m_nRowNum;
		sendToOutput(ss.str().c_str());
	}

	virtual void characterData(const XML_Char *s, int len)
	{
		CStockView * pStockView = (CStockView *)m_pContext;
		CStockDoc * pDoc = (CStockDoc *)pStockView->GetDocument();
		if(bInMsgType)
		{
			msg_type = string(s, len);
			if(msg_type=="TradingDaily")
			{
				pDoc->m_nRowNum = 0;
				pDoc->m_vecTradeDate.clear();
				pDoc->m_vecOpen.clear();
			}
			if(msg_type=="Shareholding")
			{
				pDoc->m_nRowNumOfShareholding = 0;
				pDoc->m_vecReportDateOfShareholding.clear();
				pDoc->m_vecTotalShare.clear();
			}
			if(msg_type=="FinanceReport")
			{
				pDoc->m_nRowNumOfFinanceReport = 0;
				pDoc->m_vecReportDateOfFinanceReport.clear();
				pDoc->m_vecNetProfit.clear();
			}
		}
	}

	virtual void startElement(const char *name, const char **atts)
	{
		CStockView * pStockView = (CStockView *)m_pContext;
		CStockDoc * pDoc = (CStockDoc *)pStockView->GetDocument();
		stringstream ss;
		
		if(string(name)=="msg_type")
		{
			bInMsgType = true;
		}
		else if(string(name)=="data")
		{
			if(msg_type=="TradingDaily")
				TradingDaily(atts);
			if(msg_type=="FinanceReport")
				FinanceReport(atts);
			if(msg_type=="Shareholding")
				Shareholding(atts);
		}
	}

	virtual void endElement(const char *name)
	{
		if(bInMsgType)
			bInMsgType = false;
	}

	//	以下具体的数据表 的处理
	void TradingDaily(const char **atts)
	{
		CStockView * pStockView = (CStockView *)m_pContext;
		CStockDoc * pDoc = (CStockDoc *)pStockView->GetDocument();

		const char ** pStr = atts;
		int column = 0;
		string	columnName;
		while(pStr && (*pStr))
		{
			if(column%2==0)
			{
				columnName = string((char *)*pStr);
			}
			else if(column%2==1)
			{
				if(columnName=="TradeDate")
				{
					COleDateTime odt ;
					odt.ParseDateTime((char *)*pStr);
					pDoc->m_vecTradeDate[pDoc->m_nRowNum]=odt.m_dt;
				}
				if(columnName=="Open")
					pDoc->m_vecOpen[pDoc->m_nRowNum]=String2Double(string((char *)*pStr));
			}

			column++;
			pStr++;
		}

		pDoc->m_nRowNum++;
	}

	void Shareholding(const char **atts)
	{
		CStockView * pStockView = (CStockView *)m_pContext;
		CStockDoc * pDoc = (CStockDoc *)pStockView->GetDocument();

		const char ** pStr = atts;
		int column = 0;
		string	columnName;
		while(pStr && (*pStr))
		{
			if(column%2==0)
			{
				columnName = string((char *)*pStr);
			}
			else if(column%2==1)
			{
				if(columnName=="ReportDate")
				{
					COleDateTime odt ;
					odt.ParseDateTime((char *)*pStr);
					pDoc->m_vecReportDateOfShareholding[pDoc->m_nRowNumOfShareholding]=odt.m_dt;
				}
				if(columnName=="TotalShare")
					pDoc->m_vecTotalShare[pDoc->m_nRowNumOfShareholding]=String2Double(string((char *)*pStr));
			}

			column++;
			pStr++;
		}

		pDoc->m_nRowNumOfShareholding++;
	}

	void FinanceReport(const char **atts)
	{
		CStockView * pStockView = (CStockView *)m_pContext;
		CStockDoc * pDoc = (CStockDoc *)pStockView->GetDocument();

		const char ** pStr = atts;
		int column = 0;
		string	columnName;
		while(pStr && (*pStr))
		{
			if(column%2==0)
			{
				columnName = string((char *)*pStr);
			}
			else if(column%2==1)
			{
				if(columnName=="ReportDate")
				{
					COleDateTime odt ;
					odt.ParseDateTime((char *)*pStr);
					pDoc->m_vecReportDateOfFinanceReport[pDoc->m_nRowNumOfFinanceReport]=odt.m_dt;
				}	
				if(columnName=="NetProfit")
					pDoc->m_vecNetProfit[pDoc->m_nRowNumOfFinanceReport]=String2Double(string((char *)*pStr));
			}

			column++;
			pStr++;
		}

		pDoc->m_nRowNumOfFinanceReport++;
	}

private: 
	string msg_type;
	bool bInMsgType;
};


//////////////////////////////////////////////////////////////////////////
// CStockView

IMPLEMENT_DYNCREATE(CStockView, CView)

BEGIN_MESSAGE_MAP(CStockView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CStockView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()

	ON_MESSAGE(WM_USER_XML, OnUserXmlWParam)
	ON_COMMAND(ID_SELECT_STOCK, &CStockView::OnSelectStock)
	ON_COMMAND(ID_SET_STOCK_RANGE, &CStockView::OnSetStockRange)
END_MESSAGE_MAP()

// CStockView 构造/析构

CStockView::CStockView()
{
	// TODO: 在此处添加构造代码
	m_EndDate = -1.0;
	m_StartDate = -1.0;

	m_strViewType = "Open";

	/*COleDateTime odt ;
	odt.ParseDateTime("2014-1-1");
	m_StartDate = odt.m_dt;

	odt.ParseDateTime("2015-10-1");
	m_EndDate = odt.m_dt;*/
}

CStockView::~CStockView()
{
}

BOOL CStockView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CStockView 绘制

void CStockView::OnDraw(CDC* /*pDC*/)
{
	CStockDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码

	//	过滤 数据
	int nStart = 0, nEnd = pDoc->m_nRowNum - 1;
	if(m_StartDate> 0 && m_EndDate> m_StartDate)
	{
		for(int i=0 ; i<pDoc->m_nRowNum; i++)
		{
			if(pDoc->m_vecTradeDate[i] >= m_StartDate)
			{
				nStart = i;
				break;
			}
		}

		for(int i=pDoc->m_nRowNum - 1; i>=0; i--)
		{
			if(pDoc->m_vecTradeDate[i] <= m_EndDate)
			{
				nEnd = i;
				break;
			}
		}
	}

	//	数据输入到 控件

	PeCurve(nStart,nEnd);
	TradingDailyCurve(nStart, nEnd, "Open");

	m_DMGraph.AutoRange();
}


// CStockView 打印


void CStockView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CStockView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CStockView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CStockView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void CStockView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CStockView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CStockView 诊断

#ifdef _DEBUG
void CStockView::AssertValid() const
{
	CView::AssertValid();
}

void CStockView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CStockDoc* CStockView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CStockDoc)));
	return (CStockDoc*)m_pDocument;
}
#endif //_DEBUG


// CStockView 消息处理程序

int CStockView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码


	RECT rc;
	GetClientRect(&rc);

	CRect rect(100, 10, 200, 110);

	m_DMGraph.Create(NULL, WS_CHILD | WS_VISIBLE, rect, this, 1);

	

	// hr = DispEventAdvise(m_spDMGraph, &__uuidof(_IDMGraphCtrlEvents));
	// if(FAILED(hr))
	{	//non critical error
		//Message(hr, m_spDMGraph, L"Cannot advise for events from DM Graph control");
	}

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
	}

	m_DMGraph.put_LabelColor(RGB(0,0,0));


	return 0;
}



void CStockView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	CRect rect;
	GetClientRect(&rect);

	m_DMGraph.MoveWindow(rect);

	int i= 1;

}




LRESULT CStockView::OnUserXmlWParam(WPARAM wParam, LPARAM lParam)
{
	// we expect the CMessageString in the WParam paramater
	CMessageString* pString = (CMessageString*)wParam;
	CStockDoc * pDoc = (CStockDoc *)GetDocument();

	// make sure passed string is valid (this is important to prevent unexpected
	// results if an invalid string is passed)
	if (CMessageString::IsStringValid(pString))
	{
		ElementsParser parser(this);
		parser.Parse(pString->GetBuffer(), pString->GetLength());
		delete pString;

		pDoc->UpdateAllViews(NULL);
	}
	else
	{
		CString strMessage = "ERROR! Test message using wParam received, but CMessageString passed in wParam is invalid.";
		AfxMessageBox(strMessage);

		ASSERT(FALSE);
		return MESSAGESTRING_NOT_RECEIVED;
	}

	return 0;
}


UINT SelectStockThreadFunc(LPVOID param)
{
	HWND hwnd = (HWND)param;
	string connectString = "dbname=stock  port=3366   host=127.0.0.1 user=root  ";
	backend_factory const &backEnd = *soci::factory_mysql();

	stringstream ss;
	try
	{

		ss << "\n 开始从 MYSQL 获取数据。。。。。。\n\n";
		sendToOutput(ss.str().c_str());

		//////////////////////////////////////////////////////////////////////////
		
		row r;
		//session sql(backEnd, connectString);
		session sql(g_MysqlPool);

		statement st = (sql.prepare << "select TradeDate,Open,High,Low,Close,Volume,Adj_Close from TradingDaily where Code=300067 order by TradeDate",into(r)) ;
		st.execute();

		ss.str("");
		ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
		ss << "<root>";
		ss << "<msg_type>TradingDaily</msg_type>\n";
		int cnt = 0;
		while (st.fetch())
		{

			ss << "<data ";
			std::tm  tm = r.get<std::tm>("TradeDate");
			ss << " TradeDate=\"" << tm.tm_year + 1900 << "-" << tm.tm_mon + 1 << "-" << tm.tm_mday  << "\"";
			ss << " Open=\"" << r.get<double>("Open") << "\"";
			ss << " High=\"" << r.get<double>("High") << "\"";
			ss << " Low=\"" << r.get<double>("Low") << "\"";
			ss << " Close=\"" << r.get<double>("Close") << "\"";
			ss << " Volume=\"" << r.get<int>("Volume") << "\"";
			ss << " Adj_Close=\"" << r.get<double>("Adj_Close") << "\"";
			ss << " ></data>\n";
			cnt++;
		}
		ss << "</root>\n";

		if(cnt>0)
		{
			string xml = ss.str();
			CMessageString* pString = new CMessageString(xml.c_str());

			pString->PostAsWParam(hwnd, WM_USER_XML, 0);

		}

		//////////////////////////////////////////////////////////////////////////

		//session sql(backEnd, connectString);
		st = (sql.prepare << "select ReportDate, NetProfit from FinanceReport where Code=300067 order by ReportDate",into(r)) ;
		st.execute();

		ss.str("");
		ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
		ss << "<root>";
		ss << "<msg_type>FinanceReport</msg_type>\n";
		cnt = 0;
		while (st.fetch())
		{

			ss << "<data ";
			std::tm  tm = r.get<std::tm>("ReportDate");
			ss << " ReportDate=\"" << tm.tm_year + 1900 << "-" << tm.tm_mon + 1 << "-" << tm.tm_mday  << "\"";
			ss << " NetProfit=\"" << r.get<double>("NetProfit") << "\"";
			ss << " ></data>\n";
			cnt++;
		}
		ss << "</root>\n";

		if(cnt>0)
		{
			string xml = ss.str();
			CMessageString* pString = new CMessageString(xml.c_str());

			pString->PostAsWParam(hwnd, WM_USER_XML, 0);

		}

		//////////////////////////////////////////////////////////////////////////

		//session sql(backEnd, connectString);
		st = (sql.prepare << "select ReportDate, TotalShare from Shareholding where Code=300067 order by ReportDate",into(r)) ;
		st.execute();

		ss.str("");
		ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
		ss << "<root>";
		ss << "<msg_type>Shareholding</msg_type>\n";
		cnt = 0;
		while (st.fetch())
		{

			ss << "<data ";
			std::tm  tm = r.get<std::tm>("ReportDate");
			ss << " ReportDate=\"" << tm.tm_year + 1900 << "-" << tm.tm_mon + 1 << "-" << tm.tm_mday  << "\"";
			ss << " TotalShare=\"" << r.get<double>("TotalShare") << "\"";
			ss << " ></data>\n";
			cnt++;
		}
		ss << "</root>\n";

		if(cnt>0)
		{
			string xml = ss.str();
			CMessageString* pString = new CMessageString(xml.c_str());

			pString->PostAsWParam(hwnd, WM_USER_XML, 0);

		}


		ss.str("");
		ss << "\nOK, 从 MYSQL 获取数据完毕.\n\n";
		sendToOutput(ss.str().c_str());
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << '\n';
		sendToOutput(ss.str().c_str());
	}
	return 0 ;
}



void CStockView::OnSelectStock()
{
	// TODO: 在此添加命令处理程序代码
	((CStockApp *)AfxGetApp())->StartWorkerThread(SelectStockThreadFunc,m_hWnd);
	
	CStockDoc * pDoc = (CStockDoc *)GetDocument();
	pDoc->SetTitle("300067(安诺其)");
}


void CStockView::SetGraphData(VARIANT* pvarrX, VARIANT* pvarrY, LPCTSTR szName)
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
			bReplace = TRUE;

			dmgraphCollection.Delete(i);
			
			//	存在同名的曲线，则替换
			//dmgraphElement.put_PointSymbol(1 /*Dots*/);
			//dmgraphElement.put_PointSize(1);
			//dmgraphElement.Plot(*pvarrX, *pvarrY);
			break;
		}
		
	}
	//if(bReplace == FALSE )
	{

		lpdis = dmgraphCollection.Add();
		if(lpdis==NULL)
			return;
		CDMGraphElement dmgraphElement(lpdis);
		dmgraphElement.put_Name(szName);
		dmgraphElement.put_PointSymbol(1 /*Dots*/);
		dmgraphElement.put_PointSize(3);
		dmgraphElement.Plot(*pvarrX, *pvarrY);
	}

}
void CStockView::OnSetStockRange()
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
	m_strViewType = aboutDlg.m_strViewType;

	Invalidate();
}

void CStockView::TradingDailyCurve(int nStart, int nEnd, string strViewType)
{
	CStockDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CComVariant varrX, varrY;
	varrX.parray = SafeArrayCreateVector(VT_R8, 0, nEnd - nStart + 1);
	if(varrX.parray == NULL)
		return ;
	varrX.vt = VT_ARRAY|VT_R8;

	varrY.parray = SafeArrayCreateVector(VT_R8, 0, nEnd - nStart + 1);
	if(varrY.parray == NULL)
		return ;
	varrY.vt = VT_ARRAY|VT_R8;

	HRESULT hr;
	for(long i=0; i< nEnd - nStart + 1; i++)
	{
		hr = SafeArrayPutElement(varrX.parray, &i, &pDoc->m_vecTradeDate[nStart + i]);
		hr = SafeArrayPutElement(varrY.parray, &i, &pDoc->m_vecOpen[nStart + i]);

	}
	SetGraphData(&varrX, &varrY, strViewType.c_str());
}

void CStockView::PeCurve(int nStart, int nEnd)
{
	CStockDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CComVariant varrX, varrY;
	varrX.parray = SafeArrayCreateVector(VT_R8, 0, nEnd - nStart + 1);
	if(varrX.parray == NULL)
		return ;
	varrX.vt = VT_ARRAY|VT_R8;

	varrY.parray = SafeArrayCreateVector(VT_R8, 0, nEnd - nStart + 1);
	if(varrY.parray == NULL)
		return ;
	varrY.vt = VT_ARRAY|VT_R8;

	HRESULT hr;
	for(long i=0; i< nEnd - nStart + 1; i++)
	{
		hr = SafeArrayPutElement(varrX.parray, &i, &pDoc->m_vecTradeDate[nStart + i]);
		if(pDoc->m_nRowNumOfFinanceReport>0 && pDoc->m_nRowNumOfShareholding > 0)
		{
			// 寻找最近的 股本
			double nShare = -1;
			for(int k = pDoc->m_nRowNumOfShareholding - 1; k>=0; k--)
			{
				if(pDoc->m_vecReportDateOfShareholding[k]<= pDoc->m_vecTradeDate[nStart + i])
				{
					nShare = pDoc->m_vecTotalShare[k];
					break;
				}
			}

			// 寻找最近的 净利润, 估计全年的利润, 
			double nProfit = -1;
			for(int k = pDoc->m_nRowNumOfFinanceReport - 1; k>=0; k--)
			{
				if(pDoc->m_vecReportDateOfFinanceReport[k]<= pDoc->m_vecTradeDate[nStart + i])
				{

					COleDateTime odt ;
					odt.m_dt = pDoc->m_vecReportDateOfFinanceReport[k];
					int mon = odt.GetMonth();
					int day = odt.GetDay();

					nProfit = pDoc->m_vecNetProfit[k];
					if(mon==3)
						nProfit = 4 * nProfit;
					else if(mon==6)
						nProfit = 2 * nProfit;
					else if(mon==9)
						nProfit = 4 * nProfit / 3;
					break;
				}
			}

			// 计算动态市盈率
			if(nProfit> 0 && nShare >0)
			{
				
				double nPe = nProfit/nShare;
				nPe = pDoc->m_vecOpen[nStart + i]/nPe;
				hr = SafeArrayPutElement(varrY.parray, &i, &nPe);
			}
			else 
			{
				double d = 10.0;
				hr = SafeArrayPutElement(varrY.parray, &i, &d);
			}
		}
		else
		{
			double d = 10.0;
			hr = SafeArrayPutElement(varrY.parray, &i, &d);
		}
	}

	SetGraphData(&varrX, &varrY, "PE");
}
