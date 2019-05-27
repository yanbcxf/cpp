#pragma once

class CAccumulationDistributionLine : public CAbstractIndex
{
public:
	CAccumulationDistributionLine(bool bAnalysisUsed = false);
	~CAccumulationDistributionLine(void);

	virtual void   BeginCalculate();
	virtual void   BeginTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday,
		string strTradedate, string strCode, string strPlate);
	virtual void   EndTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   EndCalculate();

	virtual string  IndexName(){  return "ÀÛ»ý/ÅÉ·¢Ïß"; }

	map<string, double>		m_mapIndex;

};
