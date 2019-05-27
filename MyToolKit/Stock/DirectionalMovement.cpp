#include "StdAfx.h"
#include "Stock.h"
#include "DirectionalMovement.h"

CDirectionalMovement::CDirectionalMovement(bool bAnalysisUsed)
:CAbstractIndex(bAnalysisUsed)
{
}

CDirectionalMovement::~CDirectionalMovement(void)
{
}


void CDirectionalMovement::BeginCalculate()
{
	stringstream ss;

	try{
		ss << "create table if not exists daily_dmi (code varchar(32), tradedate date, serial_num int, remark varchar(64), "
			<< " positive_di double , negative_di double,  ema_dx double, dx double, "
			<< " close double, high double, low double, "
			<< " up double, down double, true_range double,  "
			<< " ema_up double, ema_down double, ema_true_range double, "
			<< " PRIMARY KEY ( code, tradedate)) ";

		session sql(g_MysqlPool);
		string test = ss.str();
		sql << ss.str();

		ss.str("");
		ss << "select code , max_date, ";
		ss << "( select serial_num from daily_dmi b where b.code = a.code and b.tradedate = a.max_date ) as serial_num,";

		ss << "( select close from daily_dmi b where b.code = a.code and b.tradedate = a.max_date ) as close,";
		ss << "( select high from daily_dmi b where b.code = a.code and b.tradedate = a.max_date ) as high,";
		ss << "( select low  from daily_dmi b where b.code = a.code and b.tradedate = a.max_date ) as low,";

		ss << "( select up from daily_dmi b where b.code = a.code and b.tradedate = a.max_date ) as up,";
		ss << "( select down from daily_dmi b where b.code = a.code and b.tradedate = a.max_date ) as down,";
		ss << "( select true_range  from daily_dmi b where b.code = a.code and b.tradedate = a.max_date ) as true_range,";

		ss << "( select ema_up  from daily_dmi b where b.code = a.code and b.tradedate = a.max_date ) as ema_up,";
		ss << "( select ema_down  from daily_dmi b where b.code = a.code and b.tradedate = a.max_date ) as ema_down,";
		ss << "( select ema_true_range  from daily_dmi b where b.code = a.code and b.tradedate = a.max_date ) as ema_true_range,";

		ss << "( select positive_di  from daily_dmi b where b.code = a.code and b.tradedate = a.max_date ) as positive_di,";
		ss << "( select negative_di  from daily_dmi b where b.code = a.code and b.tradedate = a.max_date ) as negative_di,";
		ss << "( select dx  from daily_dmi b where b.code = a.code and b.tradedate = a.max_date ) as dx,";
		ss << "( select ema_dx  from daily_dmi b where b.code = a.code and b.tradedate = a.max_date ) as ema_dx ";

		
		ss << "from ( select code, date_format(max(tradedate), '%Y-%m-%d') as max_date from daily_dmi group by code ) a ";

		test = ss.str();

		row r;
		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();
		while(st.fetch())
		{
			//	取所有股票中最大的交易日，因为可能有股票停牌
			string tmp = r.get<string>("max_date");
			if(tmp!="0000-00-00" && tmp > m_strLatest)
			{
				m_strLatest = tmp;

				ss.str("");
				ss << IndexName() << ", m_strLatest = " << m_strLatest;
				LOG4CPLUS_DEBUG(g_logger, ss.str() );
			} 

			string strCode = r.get<string>("code");
			m_mapIndex[strCode].tradedate = tmp;
			m_mapIndex[strCode].serial_num = r.get<int>("serial_num");

			m_mapIndex[strCode].close = r.get<double>("close", 0);
			m_mapIndex[strCode].high = r.get<double>("high", 0);
			m_mapIndex[strCode].low = r.get<double>("low", 0);

			m_mapIndex[strCode].up = r.get<double>("up", 0);
			m_mapIndex[strCode].down = r.get<double>("down", 0);
			m_mapIndex[strCode].true_range = r.get<double>("true_range", 0);
			
			m_mapIndex[strCode].ema_up = r.get<double>("ema_up", 0);
			m_mapIndex[strCode].ema_down = r.get<double>("ema_down", 0);
			m_mapIndex[strCode].ema_true_range = r.get<double>("ema_true_range", 0);

			m_mapIndex[strCode].positive_di = r.get<double>("positive_di", 0);
			m_mapIndex[strCode].negative_di = r.get<double>("negative_di", 0);
			m_mapIndex[strCode].dx = r.get<double>("dx", 0);
			m_mapIndex[strCode].ema_dx = r.get<double>("ema_dx", 0);
		}
	}
	catch(...)
	{

	}
}


void CDirectionalMovement::BeginTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

}


/*****************  计算公式
	U = Hn - Hn-1 

	D = Ln-1 - Ln  

	TR = (Hn - Ln) | (Hn - Cn-1) | (Cn-1 - Ln)  

	EMAUP = EMAUn-1 + ((2 / (n + 1)) * (Un - EMAUn-1)) 

	EMADOWN = EMADn-1 + ((2 / (n + 1)) * (Dn - EMADn-1)) 

	EMATR = EMATRn-1 + ((2 / (n + 1)) * (TRn - EMATRn-1)) 

	+DI = EMAUP / EMATR

	-DI = EMADOWN / EMATR

	DX = ABS(+DI - -DI) / (+DI + -DI)  

	ADX = EMADXn-1 + ((2 / (n + 1)) * (DXn - EMADXn-1))

*/

void   CDirectionalMovement::Calculate(CTechnicalAnalysis & ta, CTechnicalAnalysisData & today, CTechnicalAnalysisData & yesterday,
												string strTradedate, string strCode, string strPlate)
{
	if(today.close<0)
	{
		// 本日停牌
		if(m_mapIndex.count(strCode)>0)
		{
			m_mapIndex[strCode].remark ="";

			// 本日停牌, 但是 转增、分红、红股发生,指标进行修正
			if(today.bonus_cash >0 || today.bonus_share >0 || today.reserve_to_common_share >0)
			{
				CDirectionalMovementData macd = m_mapIndex[strCode];

				//	先对之前价格按照现有 价位 进行前复权处理
				macd.close = RecoverPrice(macd.close, today);
				macd.high = RecoverPrice(macd.high, today);
				macd.low = RecoverPrice(macd.low, today);
				macd.ema_down = RecoverPrice(macd.ema_down, today, false);
				macd.ema_up = RecoverPrice(macd.ema_up, today, false);
				macd.ema_true_range = RecoverPrice(macd.ema_true_range, today, false);

				m_mapIndex[strCode] = macd;
			}
		}

		return;
	}

	// 逐个计算 每股的情况
	if(m_mapIndex.count(strCode)==0)
	{
		m_mapIndex[strCode].tradedate = strTradedate;
		m_mapIndex[strCode].serial_num = 1;
		m_mapIndex[strCode].close = today.close;
		m_mapIndex[strCode].high = today.high;
		m_mapIndex[strCode].low = today.low;
	}
	else
	{
		CDirectionalMovementData macd = m_mapIndex[strCode];
		
		//	先对之前价格按照现有 价位 进行前复权处理
		macd.close = RecoverPrice(macd.close, today);
		macd.high = RecoverPrice(macd.high, today);
		macd.low = RecoverPrice(macd.low, today);
		macd.ema_down = RecoverPrice(macd.ema_down, today, false);
		macd.ema_up = RecoverPrice(macd.ema_up, today, false);
		macd.ema_true_range = RecoverPrice(macd.ema_true_range, today, false);
		

		CDirectionalMovementData macd_new = macd;
		macd_new.serial_num ++;
		macd_new.tradedate = strTradedate;
		macd_new.remark = "";

		macd_new.close = today.close;
		macd_new.high = today.high;
		macd_new.low = today.low;

		macd_new.up = today.high - macd.high;
		if(macd_new.up <0)	macd_new.up = 0;

		macd_new.down = macd.low - today.low ;
		if(macd_new.down <0)	macd_new.down = 0;

		double tr1 = abs(today.high - today.low);
		double tr2 = abs(today.high - macd.close);
		macd_new.true_range = abs(macd.close - today.low);
		if(tr1 > macd_new.true_range)
			macd_new.true_range = tr1;
		if(tr2 > macd_new.true_range)
			macd_new.true_range = tr2;

		if(macd_new.serial_num == 2)
		{
			macd_new.ema_up = macd_new.up;
			macd_new.ema_down = macd_new.ema_down;
			macd_new.ema_true_range = macd_new.true_range;
		}
		else if(macd_new.serial_num <= DIRECTIONAL_MOVEMENT_DAY)
		{
			macd_new.ema_up = (macd.ema_up * macd.serial_num  + macd_new.up)/ macd_new.serial_num ;
			macd_new.ema_down = (macd.ema_down * macd.serial_num  + macd_new.down)/ macd_new.serial_num ;
			macd_new.ema_true_range = (macd.ema_true_range * macd.serial_num  + macd_new.true_range)/ macd_new.serial_num ;

			macd_new.positive_di = macd_new.ema_up / macd_new.ema_true_range * 100;
			macd_new.negative_di = macd_new.ema_down / macd_new.ema_true_range * 100;

			if((macd_new.positive_di + macd_new.negative_di)>0)
				macd_new.dx = abs(macd_new.positive_di - macd_new.negative_di) / (macd_new.positive_di + macd_new.negative_di) * 100;
			else
				macd_new.dx = 0;

			if(macd_new.serial_num == DIRECTIONAL_MOVEMENT_DAY)
			{
				macd_new.ema_dx =  macd_new.dx;
			}
		}
		else
		{
			macd_new.ema_up = macd.ema_up  + (2.0 / (DIRECTIONAL_MOVEMENT_DAY + 1)) *( macd_new.up - macd.ema_up);
			macd_new.ema_down = macd.ema_down  + (2.0 / (DIRECTIONAL_MOVEMENT_DAY + 1)) *( macd_new.down - macd.ema_down);
			macd_new.ema_true_range = macd.ema_true_range  + (2.0 / (DIRECTIONAL_MOVEMENT_DAY + 1)) *( macd_new.true_range - macd.ema_true_range);

			macd_new.positive_di = macd_new.ema_up / macd_new.ema_true_range * 100;
			macd_new.negative_di = macd_new.ema_down / macd_new.ema_true_range * 100;

			//	计算 上涨 或者 下跌 的强度  ，以便判断是否为 盘政 或者 趋势 行情
			if((macd_new.positive_di + macd_new.negative_di)>0)
				macd_new.dx = abs(macd_new.positive_di - macd_new.negative_di) / (macd_new.positive_di + macd_new.negative_di) * 100;
			else
				macd_new.dx = 0;

			macd_new.ema_dx = macd.ema_dx  + (2.0 / (6 + 1)) *( macd_new.dx - macd.ema_dx);

			if(macd_new.ema_dx > 20)
			{
				if(macd_new.positive_di > macd_new.negative_di && macd.positive_di < macd.negative_di)
					macd_new.remark = _T("趋势金叉买入");
				
				else if(macd_new.positive_di < macd_new.negative_di && macd.positive_di > macd.negative_di)
					macd_new.remark = _T("趋势死叉卖出");

				else if(macd.ema_dx < 20)
				{
					if(macd_new.positive_di > macd_new.negative_di)
						macd_new.remark = _T("盘整 --> 向上（up）趋势");
					else
						macd_new.remark = _T("盘整 --> 向下（down）趋势");
				}
			}
			else 
			{
				if(macd.ema_dx > 20)
					macd_new.remark = _T("趋势 --> 盘整");
			}

		}
			
		m_mapIndex[strCode] = macd_new;

	}

}

void CDirectionalMovement::EndTradedate(CTechnicalAnalysis & ta, string strTradedate)
{

	stringstream ss;
	try
	{
		ss << "insert into daily_dmi (code , tradedate, serial_num,  "
			<< " close, high, low, up, down, true_range, "
			<< " ema_up, ema_down, ema_true_range,  positive_di, negative_di, dx, ema_dx, remark "
			<< " ) values ";

		map<string, CDirectionalMovementData>::iterator it = m_mapIndex.begin();
		while(it!=m_mapIndex.end())
		{
			CDirectionalMovementData rsi = it->second;
			ss << "(\'" << it->first << "\' ,\'" << strTradedate << "\'"
				<< ", " << rsi.serial_num
				<< ", " << Double2String(rsi.close, "%.3f")
				<< ", " << Double2String(rsi.high, "%.3f")
				<< ", " << Double2String(rsi.low, "%.3f")
				<< ", " << Double2String(rsi.up, "%.3f")
				<< ", " << Double2String(rsi.down, "%.3f")
				<< ", " << Double2String(rsi.true_range, "%.3f")
				<< ", " << Double2String(rsi.ema_up, "%.3f")
				<< ", " << Double2String(rsi.ema_down, "%.3f")
				<< ", " << Double2String(rsi.ema_true_range, "%.3f")
				<< ", " << Double2String(rsi.positive_di, "%.3f")
				<< ", " << Double2String(rsi.negative_di, "%.3f")
				<< ", " << Double2String(rsi.dx, "%.3f")
				<< ", " << Double2String(rsi.ema_dx, "%.3f")
				<< ", \'" << rsi.remark << "\'";
			ss << ")";

			it++;
			if( it!=m_mapIndex.end())
				ss << ",";
		}

		session sql(g_MysqlPool);
		string test = ss.str();
		sql << test;

		
		((CStockApp *)AfxGetApp())->m_StockDataLog.UpdateTable("daily_dmi", strTradedate);
	}
	catch(...)
	{

	}
}

void CDirectionalMovement::EndCalculate()
{
	
}



void CDirectionalMovement::ExportFromDatabase(string strCode, double startTime, double endTime)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CDirectionalMovement 开始从 MYSQL 获取数据......";
		//ss << "【" << m_nCode << "】";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select code, date_format(tradedate, '%Y-%m-%d') as tradedate, "
			<< " positive_di, negative_di, ema_dx "
			<< " from daily_dmi where code=\'" << strCode << "\' ";
		if(endTime > startTime)
		{
			COleDateTime start, end;
			start.m_dt = startTime;
			end.m_dt = endTime;
			ss << " and tradedate >= \'" << start.Format("%Y-%m-%d") << "\'";
			ss << " and tradedate <= \'" << end.Format("%Y-%m-%d") << "\'";
		}
		ss	<< " order by tradedate";

		row r;
		//session sql(backEnd, m_ConnectString);
		session sql(g_MysqlPool);

		statement st = (sql.prepare << ss.str(),into(r)) ;
		st.execute();

		while (st.fetch())
		{
			CDirectionalMovementData  bbd;
			bbd.tradedate = r.get<string>("tradedate");
			bbd.positive_di = r.get<double>("positive_di");
			bbd.negative_di = r.get<double>("negative_di");
			bbd.ema_dx = r.get<double>("ema_dx");

			m_vec_index.insert(m_vec_index.end(), bbd);
		}

		ss.str("");
		ss << "OK, CDirectionalMovement 从 MYSQL 获取数据完毕.";
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
	}
}
