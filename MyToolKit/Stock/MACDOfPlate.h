#pragma once

#define	RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY 5

class CMACDOfPlate : public CMAConvergenceDivergence
{
public:
	CMACDOfPlate(bool bAnalysisUsed = false);
	~CMACDOfPlate(void);

	virtual void   BeginCalculate();
	virtual void   EndTradedate(CTechnicalAnalysis & ta, string strTradedate);

	virtual string  IndexName(){  return "MACD °å¿éÖ¸±ê"; }


	map<string, CMAConvergenceDivergenceData>		m_mapIndex;

};
