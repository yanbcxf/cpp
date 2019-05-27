#include "StdAfx.h"
#include "CalcStatsGovCn.h"

CCalcStatsGovCn::CCalcStatsGovCn(HWND hwnd, int logType)
:CCalculateTask(hwnd, logType)
{
}

CCalcStatsGovCn::~CCalcStatsGovCn(void)
{
}




void CCalcStatsGovCn::Execute1(void* firstArg)
{
	CCalcStatsGovCn * sink = (CCalcStatsGovCn *)firstArg;
	sink->Execute();
}



void CCalcStatsGovCn::Execute()
{
	/*汇总需要下载财务报表的股票*/

	if(m_vec_zb_code.size()<=0)
		return;

	stringstream ss;
	zb_code.clear();
	zb_name.clear();
	sj_code.clear();

	data.clear();
	unit.clear();

	try{

		session sql(g_MysqlPool);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");

		if(m_str_db_code.find("hg")!=string::npos)
		{
			//	全国数据
			ss << "select  zb_code, zb_name, sj_code, data, unit, '86' as reg_code, '中国' as reg_name "
				<< " from  statsgov" <<  m_str_db_code  << "  where zb_code  in ( ";

			for(int m=0; m< m_vec_zb_code.size(); m++)
			{
				ss << "\'" << m_vec_zb_code[m] << "\'" ;

				if(m!=m_vec_zb_code.size()-1)
					ss << ",";
				else
					ss << " ) ";
			}

			ss << "  ORDER BY sj_code, zb_code  ";

		}
		else 
		{
			// 分省数据
			ss << "select  zb_code, zb_name, sj_code, data, unit, reg_code, reg_name  "
				<< " from  statsgov" <<  m_str_db_code  << "  where zb_code  in ( ";
			for(int m=0; m< m_vec_zb_code.size(); m++)
			{
				ss << "\'" << m_vec_zb_code[m] << "\'" ;

				if(m!=m_vec_zb_code.size()-1)
					ss << ",";
				else
					ss << " ) ";
			}

			ss << " and reg_code in ( ";
			for(int m=0; m< m_vec_reg_code.size(); m++)
			{
				ss << "\'" << m_vec_reg_code[m] << "\'" ;

				if(m!=m_vec_reg_code.size()-1)
					ss << ",";
				else
					ss << " ) ";
			}

			if(m_vec_zb_code.size()==1)
				ss << "  ORDER BY sj_code, reg_code ";
			else
				ss << "  order by sj_code, zb_code ";

		}
		
		string test = ss.str();
		//test = Gbk2Utf8(test);

		row r;
		statement st = (sql.prepare << test,into(r)) ;
		st.execute();

		int cnt = 0;
		while (st.fetch())
		{
			try{

				if(title_code.empty())
				{
					if(m_vec_zb_code.size()==1)
					{
						title_code = r.get<string>("zb_code");
						title_name =  /*Utf8_GBK*/(r.get<string>("zb_name"));
					}
					else
					{
						title_code = r.get<string>("reg_code");
						title_name =  /*Utf8_GBK*/(r.get<string>("reg_name"));
					}
				}
				
			}
			catch(...)
			{
				continue;
			}


			//	时间为 “横坐标”
			string strSjCode = r.get<string>("sj_code");
			if(m_nOutput==1)
			{
				if(m_str_db_code=="hgyd" || m_str_db_code=="fsyd")
				{
					string startDate = strSjCode.substr(0,4) + "-" + strSjCode.substr(4,2) + "-01";
					string endDate = strSjCode.substr(4,2);
					if(endDate == "02")
						endDate = strSjCode.substr(0,4) + "-" + strSjCode.substr(4,2) + "-28";
					else if(endDate == "01" || endDate=="03" || endDate=="05" || endDate=="07" 
						|| endDate=="08" || endDate=="10" || endDate=="12")
						endDate = strSjCode.substr(0,4) + "-" + strSjCode.substr(4,2) + "-31";
					else
						endDate = strSjCode.substr(0,4) + "-" + strSjCode.substr(4,2) + "-30";

					if(m_StartDate.m_dt > 0 && m_EndDate.m_dt > m_StartDate.m_dt )
					{
						COleDateTime tmpStart, tmpEnd;
						tmpStart.ParseDateTime(startDate.c_str());
						tmpEnd.ParseDateTime(endDate.c_str());
						if(tmpEnd<m_StartDate || tmpStart > m_EndDate)
							continue;
					}

					sj_code.insert(sj_code.end(),startDate);
					sj_code.insert(sj_code.end(), endDate);
				}
				else if(m_str_db_code=="hgjd" || m_str_db_code=="fsjd")
				{
					string startDate, endDate;
					if(strSjCode.substr(4,1)=="A")
					{
						startDate = strSjCode.substr(0,4) + "-01-01";
						endDate = strSjCode.substr(0,4) + "-03-31";
					}
					if(strSjCode.substr(4,1)=="B")
					{
						startDate = strSjCode.substr(0,4) + "-04-01";
						endDate = strSjCode.substr(0,4) + "-06-30";
					}
					if(strSjCode.substr(4,1)=="C")
					{
						startDate = strSjCode.substr(0,4) + "-07-01";
						endDate = strSjCode.substr(0,4) + "-09-30";
					}
					if(strSjCode.substr(4,1)=="D")
					{
						startDate = strSjCode.substr(0,4) + "-10-01";
						endDate = strSjCode.substr(0,4) + "-12-31";
					}

					if(m_StartDate.m_dt > 0 && m_EndDate.m_dt > m_StartDate.m_dt )
					{
						COleDateTime tmpStart, tmpEnd;
						tmpStart.ParseDateTime(startDate.c_str());
						tmpEnd.ParseDateTime(endDate.c_str());
						if(tmpEnd<m_StartDate || tmpStart > m_EndDate)
							continue;
					}

					sj_code.insert(sj_code.end(),startDate);
					sj_code.insert(sj_code.end(), endDate);
				}
				else if(m_str_db_code=="hgnd" || m_str_db_code=="fsnd")
				{
					string startDate = strSjCode.substr(0,4) + "-01-01";
					string endDate = strSjCode.substr(0,4) + "-12-31";

					if(m_StartDate.m_dt > 0 && m_EndDate.m_dt > m_StartDate.m_dt )
					{
						COleDateTime tmpStart, tmpEnd;
						tmpStart.ParseDateTime(startDate.c_str());
						tmpEnd.ParseDateTime(endDate.c_str());
						if(tmpEnd<m_StartDate || tmpStart > m_EndDate)
							continue;
					}

					sj_code.insert(sj_code.end(),startDate);
					sj_code.insert(sj_code.end(), endDate);
				}


				// 插入期初记录, 纵坐标 可以能为 zb_code 或者 reg_code
				if(m_vec_zb_code.size()==1)
				{
					zb_code.insert(zb_code.end(), r.get<string>("reg_code"));
					zb_name.insert(zb_name.end(), /*Utf8_GBK*/(r.get<string>("reg_name")));
				}
				else
				{
					zb_code.insert(zb_code.end(), r.get<string>("zb_code"));
					zb_name.insert(zb_name.end(), /*Utf8_GBK*/(r.get<string>("zb_name")));
				}

				data.insert(data.end(),r.get<double>("data"));
				unit.insert(unit.end(), /*Utf8_GBK*/(r.get<string>("unit")));

				// 插入期末记录, 纵坐标 可以能为 zb_code 或者 reg_code
				if(m_vec_zb_code.size()==1)
				{
					zb_code.insert(zb_code.end(), r.get<string>("reg_code"));
					zb_name.insert(zb_name.end(), /*Utf8_GBK*/(r.get<string>("reg_name")));
				}
				else
				{
					zb_code.insert(zb_code.end(), r.get<string>("zb_code"));
					zb_name.insert(zb_name.end(), /*Utf8_GBK*/(r.get<string>("zb_name")));
				}
				data.insert(data.end(),r.get<double>("data"));
				unit.insert(unit.end(), /*Utf8_GBK*/(r.get<string>("unit")));

			}
			else
			{
				sj_code.insert(sj_code.end(),r.get<string>("sj_code"));
				if(m_vec_zb_code.size()==1)
				{
					zb_code.insert(zb_code.end(), r.get<string>("reg_code"));
					zb_name.insert(zb_name.end(), /*Utf8_GBK*/(r.get<string>("reg_name")));
				}
				else
				{
					zb_code.insert(zb_code.end(), r.get<string>("zb_code"));
					zb_name.insert(zb_name.end(), /*Utf8_GBK*/(r.get<string>("zb_name")));
				}
				data.insert(data.end(),r.get<double>("data"));
				unit.insert(unit.end(), /*Utf8_GBK*/(r.get<string>("unit")));
			}
			

			cnt++;
		}

		ss.str("");
		ss << "CCalcStatsGovCn 统计完成";
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
	catch(std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);
	}
}
