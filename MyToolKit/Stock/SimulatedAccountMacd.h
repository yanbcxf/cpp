#pragma once

#define  SHORT_TRADE	0
#define	 LONG_TRADE		1


#include "SimulatedAccount.h"

class CSimulatedAccountMacd : public CSimulatedAccount
{
public:
	CSimulatedAccountMacd(string strCode);
	~CSimulatedAccountMacd(void);

	
	virtual void ProccessEvent(string tradedate,  map<string, CStockPlateData> & mapPlate);

public:
	string m_lastRemark;
	int		m_trade_type;	//	交易类型， 中长期 或者 中短期
};
