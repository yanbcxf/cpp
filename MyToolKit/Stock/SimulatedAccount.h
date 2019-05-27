#pragma once


class CSimulatedAccountTradeDetail
{
public:
	string	trend_boll ;	//	处于 boll 窄通道中，预判断的 趋势
	string	trend_flag;		//	down 向下，  range 盘整  ， up 向上 ， unknown 未知

	int		boll_channel_num;		//	进入 boll 通道的天数

	double	trend_close;	//  
	double	trend_ma5;		//	同上
	double  trend_ma10;		//	同上
	double	trend_ma20;		//	同上


	double	dbCapital;		//	账户资金
	int		nShares;		//	持有股份

	double	dbCost;			//	记录买入所花费成本
	double	dbLossEarning;	//	记录每次卖出的 损益

	string	desc_dmi;			
	string	desc_macd;
	string	desc_kdj;
	string	desc_k;
	string	desc_ma;
	string	desc_obv;

	//double	obv_accum;
	int		score;

	string	action;			//	操作： 买入 或者 卖出
	string	remark;			//	操作说明

	void Reset()
	{
		desc_dmi = "";
		desc_kdj = "";
		desc_macd = "";
		desc_k = "";
		action = "";
		remark = "";

		score = 0;

		dbCost = 0;
		dbLossEarning = 0;
	}
};


class CSimulatedAccount 
{
public:
	CSimulatedAccount(string strCode);
	~CSimulatedAccount(void);

	void SetLastStatus(CSimulatedAccountTradeDetail satd);
	void PostEvent(CTechAnalysisEventData event);
	virtual void ProccessEvent(string tradedate, map<string, CStockPlateData> & mapPlate);

public:
	string	m_strCode;			//	股票代码
	string	m_time_to_market;	//	
	bool	m_bUpdated;			//	账户是否发生状态改变, 改变则需要保存数据库	

public:
	CTechAnalysisEventData			m_Event;		//	本日收到的数据
	CTechAnalysisEventData			m_Event_Of_Yesterday;

	vector<CSimulatedAccountTradeDetail>	m_vec_detail;	//	本日事件处理明细

	CSimulatedAccountTradeDetail			m_status;		//	账户当前状态

	vector<string>	vecParent;	//	所属板块 ID
};
