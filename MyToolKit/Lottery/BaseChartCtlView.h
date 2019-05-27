#pragma once

#include "BaseMessageView.h"
#include "ChartCtrl.h"

#include "ChartCandlestickSerie.h"
#include "ChartBarSerie.h"
#include "ChartSurfaceSerie.h"
#include "ChartLineSerie.h"
#include "ChartPointsSerie.h"
#include "ChartLabel.h"

#include "ChartAxisLabel.h"

// CBaseChartCtlView 视图

class CBaseChartCtlView : public CBaseMessageView
{
	DECLARE_DYNCREATE(CBaseChartCtlView)

protected:
public:
	CBaseChartCtlView();           // 动态创建所使用的受保护的构造函数
	virtual ~CBaseChartCtlView();

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()

protected:
	friend class CBaseChartCtlViewMouseListener;
	CChartCtrl* m_pChartCtrl;
	DATE	m_StartDate;
	DATE	m_EndDate;
	string	m_strTitle;					//	保存视图的子标题 ( 如国家统计局的指标 )
	int		m_nSelectedCurve;		//	选中的曲线名称
	map<string, list<CTipRecord>>	m_mapTip;

	//CStatic *	m_pStaticLeftTip;
	CStatic *	m_pStaticRightTip;
	CButtonST * m_pButtonMenu;
	CButtonST * m_pButtonAll;
	CButtonST * m_pButton6Month;
	CButtonST * m_pButton1Year;
	CButtonST * m_pButton3Year;


	CFont		m_editFont; 
	bool	m_bPagination;		//	是否显示分页栏

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
	virtual void RedrawDmGraph()
	{
		int w = 6;
	} ;

	virtual void RedrawChart(CCalculateTask * cse);

	afx_msg void OnSetStockRange();
	virtual void ProcessXmlWParam(string msgType,string code,string model, string param1,string param2,string param3);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	vector<string>  GetVisibleSeries();

	virtual void PostButtonClicked(int nWinDay){};

	virtual void TipContent(string seriesName, string sj_code, string & strTip){};

protected:
	afx_msg void OnBtnAll();
	afx_msg void OnBtn6Month();
	afx_msg void OnBtn1Year();
	afx_msg void OnBtn3Year();
	afx_msg void OnStnDblclickSaticRightTip();

	afx_msg void OnBtnChartMenu();
	virtual void ProccessChartMenu(){}

};



class CBaseChartCtlViewMouseListener: public CChartSeriesMouseListener<SChartXYPoint>
{
public:

	CBaseChartCtlViewMouseListener(CBaseChartCtlView * pView)
	{
		m_pView = pView;
	}

	void OnMouseEventSeries(CChartMouseListener::MouseEvent mouseEvent, CPoint point,
		CChartSerieBase<SChartXYPoint>* pSerie, unsigned uPointIndex);

private:
	CBaseChartCtlView * m_pView;

};