#pragma once



class CCandlesticksJapanData
{
public:
	string	tradedate;
	double	open, open_1, open_2, open_3;
	double	close,close_1, close_2, close_3;
	double	low, low_1, low_2, low_3;
	double	high, high_1, high_2, high_3;

	string	picture;

	//	本日发生的事件 
	string	remark;

	CCandlesticksJapanData()
	{
		open = open_1 = open_2 = open_3 = 0;
		close= close_1= close_2= close_3 = 0;
		low = low_1 = low_2 = low_3 = 0;
		high = high_1 = high_2 = high_3 = 0;

		remark = "";
	}
};


class CCandlesticksJapan : public CAbstractIndex
{
public:
	CCandlesticksJapan(bool bAnalysisUsed = false);
	~CCandlesticksJapan(void);

	virtual void   BeginCalculate();
	virtual void   BeginTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday,
		string strTradedate, string strCode, string strPlate);
	virtual void   EndTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   EndCalculate();

	virtual string  IndexName(){  return _T("K 线图（蜡烛图）"); }


	map<string, CCandlesticksJapanData>		m_mapIndex;

};
