#pragma once

#define	RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_1 6
#define	RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_2 12
#define	RELATIVE_STRENGTH_INDEX_SMOOTHING_DAY_3 24


class CRelativeStrengthIndexData
{
public:
	string	tradedate;
	int		serial_num;
	double	close;
	double	advance_average1, advance_average2, advance_average3 ;
	double	decline_average1, decline_average2, decline_average3;

	double	index1, index2, index3;

	//	本日发生的事件 
	string	basic_desc, extra_desc;

	CRelativeStrengthIndexData()
	{
		basic_desc = extra_desc = "";
		serial_num = 0;
		advance_average1= advance_average2= advance_average3= 0;
		decline_average1= decline_average2= decline_average3= 0;
		close = 0;
		index1= index2= index3= 0;
	}
};


class CRelativeStrengthIndex : public CAbstractIndex
{
public:
	CRelativeStrengthIndex(bool bAnalysisUsed = false);
	~CRelativeStrengthIndex(void);

	virtual void   BeginCalculate();
	virtual void   BeginTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday, 
		string strTradedate, string strCode, string strPlate);
	virtual void   EndTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   EndCalculate();

	virtual string  IndexName(){  return "RSI 相对强弱指标"; }


	map<string, CRelativeStrengthIndexData>		m_mapIndex;

};
