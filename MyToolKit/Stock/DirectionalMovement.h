#pragma once

#define	DIRECTIONAL_MOVEMENT_DAY 14

class CDirectionalMovementData
{
public:
	int		serial_num;
	string	tradedate, good_tradedate, bad_tradedate, good_desc, bad_desc;
	double	close, high, low;
	double	up, down, true_range;
	double  ema_up, ema_down, ema_true_range; 
	double  positive_di, negative_di, dx, ema_dx;

	//	本日发生的事件 
	string	remark;

	CDirectionalMovementData()
	{
		close = high = low = 0;
		up = down = true_range = 0;
		ema_up = ema_down = ema_true_range = 0;
		positive_di = negative_di = dx = ema_dx = 0;

		remark = "";
	}
};


class CDirectionalMovement : public CAbstractIndex
{
public:
	CDirectionalMovement(bool bAnalysisUsed = false);
	~CDirectionalMovement(void);

	virtual void   BeginCalculate();
	virtual void   BeginTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday,
		string strTradedate, string strCode, string strPlate);
	virtual void   EndTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   EndCalculate();

	virtual string  IndexName(){  return "DMI 动向指标"; }

	void ExportFromDatabase(string strCode, double startTime = 0, double endTime = 0);


	map<string, CDirectionalMovementData>		m_mapIndex;

	//	保存 ExportFromDatabase 的结果, 按照交易日排序
	vector<CDirectionalMovementData>		m_vec_index;

};
