#pragma once

class CTechnicalAnalysisMarketIndexData_1
{
public:

	CTechnicalAnalysisMarketIndexData_1()
	{
		absolute_breadth_index = 0;
		advance_decline_line = 0;
		advance_decline_ratio = 0;
	}

public:
	int	absolute_breadth_index;
	int	advance_decline_line;
	double advance_decline_ratio;
};

class CTechnicalAnalysisMarketIndexData 
{
public:
	CTechnicalAnalysisMarketIndexData_1  shenzhen_ashare;
	CTechnicalAnalysisMarketIndexData_1  middle_small;
	CTechnicalAnalysisMarketIndexData_1  entrepreneurship;
	CTechnicalAnalysisMarketIndexData_1  shanghai;
	CTechnicalAnalysisMarketIndexData_1	 total;
};


class CAbsoluteBreadthIndex : public CAbstractIndex
{
public:
	CAbsoluteBreadthIndex(bool bAnalysisUsed = false);
	~CAbsoluteBreadthIndex(void);

	virtual void   BeginCalculate();
	virtual void   BeginTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday,
		string strTradedate, string strCode, string strPlate);
	virtual void   EndTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   EndCalculate();

	virtual string  IndexName(){  return "绝对幅度指标"; }

	map<string, CTechnicalAnalysisMarketIndexData>	m_mapIndex;
	int		m_nAdvance;
	int		m_nDecline;

	int		m_nAdvance_of_shenzhen_ashare;
	int		m_nDecline_of_shenzhen_ashare;

	int		m_nAdvance_of_middle_small;
	int		m_nDecline_of_middle_small;

	int		m_nAdvance_of_entrepreneurship;
	int		m_nDecline_of_entrepreneurship;

	int		m_nAdvance_of_shanghai;
	int		m_nDecline_of_shanghai;

};
