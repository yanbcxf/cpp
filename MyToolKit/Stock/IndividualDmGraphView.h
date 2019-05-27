#pragma once

#include "BaseMessageView.h"
// CIndividualDmGraphView 视图

class CIndividualDmGraphView : public CBaseMessageView
{
	DECLARE_DYNCREATE(CIndividualDmGraphView)

protected:
	CIndividualDmGraphView();           // 动态创建所使用的受保护的构造函数
	virtual ~CIndividualDmGraphView();

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif


protected:
	CDMGraphCtrl	m_DMGraph;
	DATE	m_StartDate;
	DATE	m_EndDate;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void OnInitialUpdate();
	virtual void RedrawDmGraph() {} ;
	void ClearGraphData();
public:
	afx_msg void OnSetStockRange();
	virtual void ProcessXmlWParam(string msgType,string code,string model, string param1,string param2,string param3);

protected:
	/*
	enum __MIDL___MIDL_itf_DMGraph_0000_0000_0001
	{	Solid	= 0,
	Dash	= ( Solid + 1 ) ,
	Dot	= ( Dash + 1 ) ,
	DashDot	= ( Dot + 1 ) ,
	DashDotDot	= ( DashDot + 1 ) ,
	Null	= ( DashDotDot + 1 ) ,
	XYStep	= ( Null + 1 ) ,
	YXStep	= ( XYStep + 1 ) ,
	Bars	= ( YXStep + 1 ) ,
	Stick	= ( Bars + 1 ) 
	} 	LineType;
	*/
	void SetGraphData(VARIANT* pvarrX, VARIANT* pvarrY, LPCTSTR szName, COLORREF clr, int lineType = 0, int pointSize =1);
	void SetGraphAnnotation(double labelx, double labely, string labelCaption, COLORREF clr);
};



#define CURRENT_ASSET_CURVE(tableItem, itemName, lineColor, lineType, pointSize) \
	isHaveData = false; \
	for(long i=0; i<  nEnd - nStart +1; i++) \
	{\
		map_summary_in[i] += String2Double(balance.m_vec_row[nStart + i][itemName]);\
		if( String2Double(balance.m_vec_row[nStart + i][itemName])>0.0)\
		isHaveData = true;\
	}\
	if(isHaveData)\
	{\
		for(long i=0; i<  nEnd - nStart +1; i++)\
		{\
			double sum = map_summary_in[i]/10000.0;\
			hr = SafeArrayPutElement(varrY.parray, &i, &sum);\
		}\
		COLORREF clr = lineColor;\
		SetGraphData(&varrX, &varrY, itemName, clr, lineType, pointSize);\
		vec_clr.push_back( clr);\
		vec_annotion.push_back( itemName);\
		vec_percent.push_back(String2Double(balance.m_vec_row[nEnd][itemName]));\
	}