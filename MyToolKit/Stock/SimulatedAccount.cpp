#include "StdAfx.h"
#include "AbstractIndex.h"
#include "TechAnalysisEvent.h"

#include "SimulatedAccount.h"

//////////////////////////////////////////////////////////////////////////
//  
//C++实现求一维离散数据极值（波峰、波谷）  
//  
//参数：数组，数组大小  

void findPeaks(vector<double> num, vector<int> & indMax, vector<int> & indMin)  
{  
    vector<int> sign;  
    for(int i = 1;i<num.size();i++)  
    {  
        /*相邻值做差： 
		 *小于0，赋-1 
         *大于0，赋1 
         *等于0，赋0 
         */  
        int diff = num[i] - num[i-1];  
       if(diff>0)  
        {  
            sign.push_back(1);  
       }  
       else if(diff<0)  
        {  
            sign.push_back(-1);  
        }  
        else  
        {  
            sign.push_back(0);  
        }  
    }  
    //再对sign相邻位做差  
    //保存极大值和极小值的位置  
  
   for(int j = 1;j<sign.size();j++)  
    {  
        int diff = sign[j]-sign[j-1];  
        if(diff<0)  
        {  
            indMax.push_back(j);  
        }  
        else if(diff>0)  
        {  
            indMin.push_back(j);  
        }  
    }  
}  



//////////////////////////////////////////////////////////////////////////

CSimulatedAccount::CSimulatedAccount(string strCode)
{
	m_status.Reset();

	m_strCode = strCode;
	m_status.dbCapital = 10 * 10000;	//	初始为 10 万元
	m_status.nShares = 0;				//	初始为 0 股

	m_status.trend_flag = "unknown";
	m_status.trend_boll = "unknown";
	m_status.trend_close = 0;

	// m_status.obv_accum = 0;
	m_bUpdated = false;
}

CSimulatedAccount::~CSimulatedAccount(void)
{
}

void CSimulatedAccount::SetLastStatus(CSimulatedAccountTradeDetail satd)
{
	m_status = satd;
}

void CSimulatedAccount::PostEvent(CTechAnalysisEventData event)
{
	m_Event = event;
}

void CSimulatedAccount::ProccessEvent(string tradedate,  map<string, CStockPlateData> & mapPlate)
{
	if(m_Event.tradedate == "0000-00-00")
	{
		if(tradedate> m_time_to_market && m_time_to_market!="0000-00-00")
		{
			stringstream ss;
			ss << "[" << m_strCode << "] Error， ProccessEvent 本日无数据";
			// LOG4CPLUS_DEBUG(g_logger, ss.str() );
		}
		return;
	}

	bool	bExitFromSellOrBuy = false;

	//////////////////////////////////////////////////////////////////////////
	//	过滤事件的接收 

	if(m_status.nShares >0)
	{
		if(m_Event.bonus_cash > 0)
			m_status.dbCapital +=  m_status.nShares/10 * m_Event.bonus_cash;
		if(m_Event.bonus_share > 0)
			m_status.nShares += m_Event.bonus_share  * m_status.nShares/10;
		if(m_Event.reserve_to_common_share > 0)
			m_status.nShares += m_Event.reserve_to_common_share  * m_status.nShares/10;

		if(m_Event.bonus_cash > 0 || m_Event.bonus_share > 0 || m_Event.reserve_to_common_share > 0)
		{
			CSimulatedAccountTradeDetail  satd = m_status;
			satd.remark = "转增、红股";
			m_vec_detail.insert(m_vec_detail.end(), satd);
		}
		
	}

	if(m_status.trend_flag != "range" && m_Event.boll.find(_T("进入"))!=string::npos)
	{
		//m_status.Reset();
		m_status.trend_flag = "range";
		m_status.trend_boll = "unknown";
		m_status.boll_channel_num = 0;

		//close = event.close;

		CSimulatedAccountTradeDetail  satd = m_status;
		satd.remark = m_Event.boll;
		satd.action = "";
		m_vec_detail.insert(m_vec_detail.end(), satd);

	}
	else if(m_Event.boll.find(_T("退出"))!=string::npos)
	{
		bExitFromSellOrBuy = true;
	}

	if(m_Event.macd.empty()==false )
	{
		m_status.desc_macd = m_Event.macd;
	}
	if(m_Event.kdj.empty()==false )
	{
		m_status.desc_kdj = m_Event.kdj;
	}
	if(m_Event.dmi.empty()==false )
	{
		m_status.desc_dmi = m_Event.dmi;
	}
	
	if(m_Event.candlesticks.empty()==false )
	{
		m_status.desc_k = m_Event.candlesticks;
	}

	//	记录进入boll 通道的天数
	if(m_status.trend_flag=="range")
	{
		m_status.boll_channel_num++;
	}

	//////////////////////////////////////////////////////////////////////////
	//	每日做出买卖决策
	m_status.remark = "";

	if(tradedate == "2016-09-26" && m_strCode =="603555" )
	{
		int k = 1;
	}
	
	double close = m_Event.close;
	int nScore = 0;
	if(m_status.desc_dmi.find(_T("向上（up）趋势"))!=string::npos || m_status.desc_dmi.find(_T("趋势金叉买入"))!=string::npos )
		nScore ++;
	else if(m_status.desc_dmi.find(_T("向下（down）趋势"))!=string::npos || m_status.desc_dmi.find(_T("趋势死叉卖出"))!=string::npos ) 
		nScore --;

	if(m_status.desc_macd.find(_T("金叉"))!=string::npos)
		nScore ++;
	else if(m_status.desc_macd.find(_T("死叉"))!=string::npos)
		nScore --;

	if(m_status.desc_k.find(_T("穿头破脚阳包阴"))!=string::npos)
		nScore ++;
	else if(m_status.desc_k.find(_T("穿头破脚阴包阳"))!=string::npos)
		nScore --;

	if(nScore >=2)
	{
		if(m_status.trend_flag=="unknown")
		{

		}
		else if(m_status.trend_flag=="down")
		{
			//	先前判断为 DOWN 趋势
			if(close > m_status.trend_close)
			{
				//	先前趋势 判断失误
				// m_status.remark = "建议买入";

				m_status.trend_flag = "unknown";
				m_status.trend_boll = "unknown";
			}
		}
		else if(m_status.trend_flag=="range" )
		{
			if(m_Event.standard > m_Event_Of_Yesterday.standard 
				&& (4 * m_Event.standard / m_Event.middle) > 0.085
				&& (4 * m_Event.standard / m_Event.middle) < 0.095
				&& m_Event.dynamic_pe < 100
				&& m_status.boll_channel_num > 10)
			{
				if( m_Event.ma5 >= m_Event.ma10 &&  m_Event.ma10 >= m_Event.ma20)
				{
					m_status.remark = "建议买入";
					m_status.trend_boll = "up";
				}
				else
				{
					m_status.remark = "均线不达标，持币观望";
				}
			}
			else
			{
				if(m_Event.standard <= m_Event_Of_Yesterday.standard)
					m_status.remark = "BOLL 开口缩小，持币观望";
				else if((4 * m_Event.standard / m_Event.middle) <= 0.085)
					m_status.remark = "BOLL 小于 8.5% ，持币观望";
				else if((4 * m_Event.standard / m_Event.middle) >= 0.095)
					m_status.remark = "BOLL 大于 9.5% ，持币观望";
				else if( m_Event.dynamic_pe >= 100)
					m_status.remark = "BOLL 动态 PE > 100 ，持币观望";
				else
					m_status.remark = "BOLL 通道天数小于 10 ，持币观望";
			}
		}
	}
	else if(nScore <=-2)
	{
		if(m_status.trend_flag=="unknown")
		{

		}
		else if(m_status.trend_flag=="up")
		{
			//	先前判断为 UP 趋势
			if( m_Event.ma5 < m_Event.ma10)
			{
				//	先前判断失误
				m_status.remark = "建议卖出, 先前判断失误";

				m_status.trend_flag = "unknown";
				m_status.trend_boll = "unknown";
			}
			else
			{
				m_status.remark = "均线不达标，持股等待";
			}
		}
		else if(m_status.trend_flag=="range")
		{
			if(m_Event.standard > m_Event_Of_Yesterday.standard 
				&& (4 * m_Event.standard / m_Event.middle) > 0.085
				&& (4 * m_Event.standard / m_Event.middle) < 0.095
				&& m_status.boll_channel_num > 10)
			{
				if( m_Event.ma5 <= m_Event.ma10 && m_Event.ma10 <= m_Event.ma20)
				{
					m_status.remark = "建议卖出";
					m_status.trend_boll = "down";
				}
				else
				{
					m_status.remark = "均线不达标，持股等待";
				}
			}
			else
			{
				if(m_Event.standard <= m_Event_Of_Yesterday.standard)
					m_status.remark = "BOLL 开口缩小，持股等待";
				else if((4 * m_Event.standard / m_Event.middle) <= 0.085)
					m_status.remark = "BOLL 小于 8.5% ，持股等待";
				else if((4 * m_Event.standard / m_Event.middle) >= 0.095)
					m_status.remark = "BOLL 大于 9.5% ，持币观望";
				else
					m_status.remark = "BOLL 通道天数小于 10 ，持股等待";
				
			}
		}
	}

	if(bExitFromSellOrBuy)
	{		
		if(m_status.trend_boll=="unknown")
			m_status.remark = "建议卖出, 未得出结论";
		
	}
	

	//////////////////////////////////////////////////////////////////////////
	//	对账户实际 买卖操作
	if(m_status.remark.find("建议买入")!=string::npos)
	{
		int nShare =  (m_status.dbCapital/close)/100;
		if(nShare * 100 * close > m_status.dbCapital)
			nShare--;


		if(nShare > 0)
		{
			stringstream ss;
			nShare = nShare * 100;

			//	买人转账
			m_status.nShares += nShare ;
			m_status.dbCapital -=  nShare * close;
			double dbTotal = m_status.nShares * close + m_status.dbCapital;

			ss << "买入" << (nShare) << "股 ";
			ss << ", 每股 " << Double2String(close)  << "元";
			ss << "， 总资产为 " << Double2String(dbTotal) << "元";

			if(dbTotal> 10 * 10000)
				ss << ", earning ！";
			else if(dbTotal< 10 * 10000)
				ss << ", loss ！";
			m_status.action = ss.str();

		}
		else
			m_status.action = "";

	}
	else if(m_status.remark.find("建议卖出")!=string::npos)
	{
		if(m_status.nShares > 0)
		{
			stringstream ss;
			m_status.dbCapital  += (m_status.nShares  * close);

			ss << "卖出" << m_status.nShares  << "股 ";
			ss << ", 每股 " << Double2String(close) << "元";
			ss << "， 总资产为 " << Double2String(m_status.dbCapital) << "元";

			if(m_status.dbCapital> 10 * 10000)
				ss << ", earning ！";
			else if(m_status.dbCapital< 10 * 10000)
				ss << ", loss ！";

			m_status.action = ss.str();

			m_status.nShares = 0;
		}
		else
			m_status.action = "";

	}
	else
	{
		m_status.action = "";

	}

	//	将本日的 状态变化进行保存
	if(m_status.remark.empty()==false)
		m_vec_detail.insert(m_vec_detail.end(), m_status);

	if(bExitFromSellOrBuy)
	{
		//m_status.Reset();
		if(m_status.trend_flag=="range")
		{
			m_status.trend_flag = m_status.trend_boll;
			m_status.trend_close = close;
			m_status.trend_ma5 = m_Event.ma5;
			m_status.trend_ma10 = m_Event.ma10;
			m_status.trend_ma20 = m_Event.ma20;
		}
		else
		{
			stringstream ss;
			ss << "[" << m_strCode << "] BOLL 指标 Error";
			LOG4CPLUS_DEBUG(g_logger, ss.str() );
		}

		CSimulatedAccountTradeDetail  satd = m_status;
		satd.remark = "====> 退出买入卖出时机";
		satd.action = "";
		m_vec_detail.insert(m_vec_detail.end(), satd);
	}

	m_status.desc_k = "";		//	该指标的持续意义不大

	//////////////////////////////////////////////////////////////////////////
	//	记录本日数据，以便明日使用
	m_Event_Of_Yesterday = m_Event;

	//	标志该日无数据
	m_Event.tradedate = "0000-00-00";

}

