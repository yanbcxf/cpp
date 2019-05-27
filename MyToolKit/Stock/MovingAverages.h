#pragma once

#define	MOVING_AVERAGES_SMOOTHING_DAY_1 5
#define	MOVING_AVERAGES_SMOOTHING_DAY_2 10
#define	MOVING_AVERAGES_SMOOTHING_DAY_3 20
#define	MOVING_AVERAGES_SMOOTHING_DAY_4 60


class CMovingAveragesData
{
public:
	string	tradedate;
	double	close[60];
	double	index1, index2, index3, index4;
	int		period_5 , period_10; 
	int     period_20, period_60;			//	0 :未知    1：多头    -1 ：空头

	//	本日发生的事件 
	string	basic_desc, extra_desc;
	string	remark;

	CMovingAveragesData()
	{
		basic_desc = extra_desc = "";
		
		for(int i =0; i< 60; i++)
			close[i] = 0;
		index1= index2= index3= index4 = 0;

		period_5 = period_10 = period_20 = period_60 = 0;
	}
};


class CMovingAverages : public CAbstractIndex
{
public:
	CMovingAverages(bool bAnalysisUsed = false);
	~CMovingAverages(void);

	virtual void   BeginCalculate();
	virtual void   BeginTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday, 
		string strTradedate, string strCode, string strPlate);
	virtual void   EndTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   EndCalculate();

	virtual string  IndexName(){  return "MA 均线指标"; }


	map<string, CMovingAveragesData>		m_mapIndex;

};
