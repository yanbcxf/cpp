#pragma once

#define	KDJ_DAY 9

class CStochasticsOscillatorKDJData
{
public:
	int		serial_num;
	string	tradedate, good_tradedate, bad_tradedate, good_desc, bad_desc;
	double	close, high[9], low[9];
	double	k_value, d_value, j_value;

	//	本日发生的事件 
	string	remark;
	
	CStochasticsOscillatorKDJData()
	{
		close = 0;
		k_value = d_value = j_value = 50;

		for(int i=0; i< 9; i++)
		{
			high[i] = low[i] = 0;
		}

		remark = "";
	}
};


class CStochasticsOscillatorKDJ : public CAbstractIndex
{
public:
	CStochasticsOscillatorKDJ(bool bAnalysisUsed = false);
	~CStochasticsOscillatorKDJ(void);

	virtual void   BeginCalculate();
	virtual void   BeginTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday,
		string strTradedate, string strCode, string strPlate);
	virtual void   EndTradedate(CTechnicalAnalysis & ta, string strTradedate);
	virtual void   EndCalculate();

	virtual string  IndexName(){  return "KDJ 指标"; }

	void ExportFromDatabase(string strCode, double startTime = 0, double endTime = 0);


	map<string, CStochasticsOscillatorKDJData>		m_mapIndex;

	//	保存 ExportFromDatabase 的结果, 按照交易日排序
	vector<CStochasticsOscillatorKDJData>		m_vec_index;

};
