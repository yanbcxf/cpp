#pragma once

#define	OBV_SMOOTHING_DAY 12


class COnBalanceVolumeData
{
public:
	string	tradedate;
	double	obv[12];
	double	vol[12];

	//	本日发生的事件 
	string	remark;
	double  close;

	COnBalanceVolumeData()
	{
		for(int i =0; i< 12; i++)
			obv[i] = 0;
		for(int i =0; i< 12; i++)
			vol[i] = 0;
		close = 0;

		remark = "";
	}
};


class COnBalanceVolume : public CAbstractIndex
{
public:
	COnBalanceVolume(bool bAnalysisUsed = false);
	~COnBalanceVolume(void);

	virtual void   BeginCalculate();
	virtual void   BeginTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday,
		string strTradedate, string strCode, string strPlate);
	virtual void   EndTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   EndCalculate();

	virtual string  IndexName(){  return "OBV 平衡交易量指标"; }

	void ExportFromDatabase(string strCode, double startTime = 0, double endTime = 0);

	//	key 为股票代码
	map<string, COnBalanceVolumeData>		m_mapIndex;

	//	保存 ExportFromDatabase 的结果, 按照交易日排序
	vector<COnBalanceVolumeData>		m_vec_index;

};
