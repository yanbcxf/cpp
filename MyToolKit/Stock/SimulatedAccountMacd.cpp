#include "StdAfx.h"
#include "AbstractIndex.h"
#include "TechAnalysisEvent.h"

#include "SimulatedAccountMacd.h"

//////////////////////////////////////////////////////////////////////////

CSimulatedAccountMacd::CSimulatedAccountMacd(string strCode)
:CSimulatedAccount(strCode)
{
	m_lastRemark = "";
	m_trade_type = SHORT_TRADE;
}

CSimulatedAccountMacd::~CSimulatedAccountMacd(void)
{
}


void CSimulatedAccountMacd::ProccessEvent(string tradedate, map<string, CStockPlateData> & mapPlate)
{
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

	if(m_Event.tradedate == "0000-00-00" || m_Event.close <= 0)
	{

		return;
	}

	m_status.remark = "";

	if(m_status.nShares >0)
	{
		//	计算本次卖出的 损益
		double earning = m_status.nShares * m_Event.close - m_status.dbCost;

		if(earning/m_status.dbCost < -0.1)
			m_status.remark = "建议卖出, 止损单";
	}

	m_status.remark = "";


	do{
		if(m_status.remark.empty() == false)
			break;

		//////////////////////////////////////////////////////////////////////////
		//	汇总所属板块的 
		string desc_plate = "";
		for(int i=0; i<vecParent.size(); i++)
		{
			if(mapPlate.count(vecParent[i])>0)
			{
				CStockPlateData  spd = mapPlate[vecParent[i]];
				

			}
		}


		//////////////////////////////////////////////////////////////////////////
		//	过滤事件的接收 

		// 记录 MA 系统当前的长短期状态
		if(m_Event.ma.empty()==false)		m_status.desc_ma = m_Event.ma;


		if(m_Event.macd.empty() /*&& m_Event.dmi.empty() */
			&& m_Event.candlesticks.empty() /*&& m_Event.boll.empty() */
			/*&& m_Event.ma.empty()*/ && m_Event.obv.empty())
			break;

		if(m_Event.macd.empty()==false 
			&& (m_Event.macd.find("背离")!=string::npos
			|| m_Event.macd.find("叉")!=string::npos))		
		{
			m_status.desc_macd = m_Event.macd;
		}
		// if(m_Event.dmi.empty()==false)		m_status.desc_dmi = m_Event.dmi;
		//if(m_Event.kdj.empty()==false)		m_status.desc_kdj = m_Event.kdj;
		if(m_Event.candlesticks.empty()==false)		m_status.desc_k = m_Event.candlesticks;
		// if(m_Event.ma.empty()==false)		m_status.desc_ma = m_Event.ma;
		if(m_Event.obv.empty()==false)		m_status.desc_obv = m_Event.obv;


		//////////////////////////////////////////////////////////////////////////
		//	每日做出买卖决策
		double width = 4 * m_Event.standard / m_Event.middle;

		
		if(m_trade_type == SHORT_TRADE)
		{
			// 买入信号
			if(m_Event.macd.find("水上金叉")!=string::npos)
			{
				/*if(m_Event.macd.find("1次水上金叉")!=string::npos||
					m_Event.macd.find("2次水上金叉")!=string::npos)
				{
					m_status.remark = "建议买入";
				}
				else
				{
					if(m_Event.dif < m_Event.close * 0.05 )
						m_status.remark = "建议买入";
					else
						m_status.remark = "dif处于高位，持币观望";
				}*/

				if( m_Event.k_value < 80)
				{
					m_status.remark = "建议买入";
				}
				else
					m_status.remark = "KDJ超买，持币观望";

				if(m_status.remark.find("建议买入")!=string::npos )
				{
					/*if(m_Event.change_rate>9.00)
					{
						m_status.remark = "金叉日涨幅过大，停止交易";
					}*/
				}
				
			}

			if(m_Event.macd.find("水下金叉")!=string::npos ||
				m_Event.macd.find("底背离")!=string::npos )
			{

				/*if(abs(m_Event.dif) < m_Event.close * 0.05 )
				{
					m_status.remark = "建议买入";
					
				}
				else
					m_status.remark = "dif处于0轴下较远处，持币观望";*/

				if( m_Event.k_value < 80)
				{
					m_status.remark = "建议买入";
				}
				else
					m_status.remark = "KDJ超买，持币观望";

				
			}

			//	卖出信号
			if( m_Event.macd.find("顶背离")!=string::npos)
			{
				m_status.remark = "建议卖出";
			}

			if(m_Event.macd.find("水上跌破前次金叉")!=string::npos)
			{
				m_status.remark = "建议卖出";
			}

			if(m_Event.macd.find("水下跌破前次金叉")!=string::npos)
			{
				m_status.remark = "建议卖出";
			}

			if(m_Event.macd.find("dif下穿0轴")!=string::npos)
			{
				//m_status.remark = "建议卖出";
				/*if(m_Event.ma.find("长期多头")==string::npos)
					m_status.remark = "建议卖出";
				else
					m_status.remark = "MA不达标, 持股等待";*/
			}
		}
		else
		{
			//	买入信号
			if(m_Event.macd.find("dif上穿0轴")!=string::npos)
			{
				if(m_Event.ma.find("长期多头")!=string::npos)
					m_status.remark = "建议买入";
				else
					m_status.remark = "MA不达标, 持币观望";
			}

			//	卖出信号
			if(m_Event.macd.find("dif下穿0轴")!=string::npos)
			{
				if(m_Event.ma.find("长期多头")==string::npos)
					m_status.remark = "建议卖出";
				else
					m_status.remark = "MA不达标, 持股等待";
			}
		}

		if(m_status.remark.find("建议买入")!=string::npos )
		{
			
			
		}

		if(m_status.remark.find("建议买入")!=string::npos 
			&& (m_Event.dynamic_pe > 50 || m_Event.dynamic_pe <=0))
		{
			m_status.remark = "PE过大，停止交易";
		}

		if(m_Event.candlesticks.find("熊市窗口图")!=string::npos
			||m_Event.candlesticks.find("墓碑")!=string::npos
			||m_Event.candlesticks.find("穿头破脚阴包阳")!=string::npos
			||m_Event.candlesticks.find("晚星")!=string::npos)
		{
			m_status.remark = "建议卖出, " + m_Event.candlesticks;

			m_status.desc_macd = "";
			m_status.desc_dmi = "";
			m_status.desc_kdj = "";
		}
		
		if(m_Event.obv.find("股价放量大跌")!=string::npos)
		{
			m_status.remark = "建议卖出, 股价放量大跌";

			m_status.desc_macd = "";
			m_status.desc_dmi = "";
			m_status.desc_kdj = "";
		}

		if(m_status.remark.find("建议买入")!=string::npos )
		{
			for(int i=0; i<vecParent.size(); i++)
			{
				if(mapPlate.count(vecParent[i])>0)
				{
					CStockPlateData  spd = mapPlate[vecParent[i]];

					stringstream ss;
					ss << "[" << spd.name << "] ";
					
					if(spd.dif>0)	ss << "水上";
					else			
					{
						ss << "水下";
					}

					if(spd.dif> spd.dea)	ss << "已金叉";
					if(spd.dif < spd.dea)	ss << "已死叉";
					
					if(i< vecParent.size() -1)	ss << ",";

					m_status.remark += ss.str();
				}
			}

		}


		//	如果这次决策与上次不同，则记录变更
		if( m_status.remark.empty()==false && m_lastRemark != m_status.remark)
		{
			m_lastRemark = m_status.remark;
		}
	

		m_status.desc_k = "";		//	该指标的持续意义不大
		m_status.desc_obv = "";
	}while(0);

	//////////////////////////////////////////////////////////////////////////
	//	对账户实际 买卖操作

	double  close = m_Event.close ;

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

			//	记录购买成本
			m_status.dbCost += nShare * close;
			m_status.dbLossEarning = 0;

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

			//	计算本次卖出的 损益
			double earning = m_status.nShares * close - m_status.dbCost;

			m_status.dbLossEarning = earning/m_status.dbCost;
			m_status.dbCost = 0;
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
	

	//////////////////////////////////////////////////////////////////////////
	//	记录本日数据，以便明日使用
	m_Event_Of_Yesterday = m_Event;

	//	标志该日无数据
	m_Event.tradedate = "0000-00-00";

}

