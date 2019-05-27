#pragma once

#include "IndividualDmGraphView.h"
// CIndividualNonCurrentAssetView 视图

class CIndividualNonCurrentAssetView : public CIndividualDmGraphView
{
	DECLARE_DYNCREATE(CIndividualNonCurrentAssetView)

protected:
public:
	CIndividualNonCurrentAssetView();           // 动态创建所使用的受保护的构造函数
	virtual ~CIndividualNonCurrentAssetView();

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

public:
	void RedrawDmGraph();
};


#define NON_CURRENT_ASSET_CURVE(tableItem, itemName, lineColor, lineType, pointSize) \
	isHaveData = false; \
	for(long i=0; i<  nEnd - nStart +1; i++) \
	{\
		map_summary_in[i] += balance.tableItem[nStart + i];\
		if( balance.tableItem[nStart + i]>0.0)\
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
		vec_percent.push_back(balance.tableItem[nEnd]);\
	}
