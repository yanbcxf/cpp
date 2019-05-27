#include "StdAfx.h"
#include "AbstractIndex.h"

CAbstractIndex::CAbstractIndex(bool bAnalysisUsed)
{
	if(bAnalysisUsed)
	{
		CTechnicalAnalysis::getInstance().m_vecIndex.insert(CTechnicalAnalysis::getInstance().m_vecIndex.end(), this);
	}

	m_strLatest = "2016-01-01";
}

CAbstractIndex::~CAbstractIndex(void)
{
}


double  CAbstractIndex::RecoverPrice(double price, CTechnicalAnalysisData tad, bool subCash)
{
	if(tad.bonus_cash >0 || tad.bonus_share >0 || tad.reserve_to_common_share >0)
	{
		//	前复权： 按照现有价位来复权以前的价格
		double ret = (price - (subCash? (tad.bonus_cash /10):0) )/(1 + tad.reserve_to_common_share/10 + tad.bonus_share/10);
		return ret;
	}
	else
		return price;
}


double  CAbstractIndex::RecoverVolume(double volume, CTechnicalAnalysisData tad)
{
	if(tad.bonus_cash >0 || tad.bonus_share >0 || tad.reserve_to_common_share >0)
	{
		//	前复权： 按照现有价位来复权以前的成交量
		double ret = (1 + tad.reserve_to_common_share/10 + tad.bonus_share/10) * volume;
		return ret;
	}
	else
		return volume;
}