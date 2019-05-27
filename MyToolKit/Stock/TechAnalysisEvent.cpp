#include "StdAfx.h"
#include "TechAnalysisEvent.h"
#include "DirectionalMovement.h"

CTechAnalysisEvent::CTechAnalysisEvent()
{
}

CTechAnalysisEvent::~CTechAnalysisEvent(void)
{
}



void CTechAnalysisEvent::ExportFromDatabase(string strCode, double startTime, double endTime)
{
	stringstream ss;
	backend_factory const &backEnd = *soci::factory_mysql();

	try
	{
		ss << "CTechAnalysisEvent 开始从 MYSQL 获取数据......";
		//ss << "【" << m_nCode << "】";
		//sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		//////////////////////////////////////////////////////////////////////////
		ss.str("");
		ss << "select code, date_format(tradedate, '%Y-%m-%d') as tradedate, "
			<< " index_name, basic_desc, extra_desc, close, percent "
			<< " from tech_analysis_event where code=\'" << strCode << "\' ";
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
			CTechAnalysisEventData  bbd;
			bbd.tradedate = r.get<string>("tradedate");
			bbd.index_name = r.get<string>("index_name");
			bbd.basic_desc = r.get<string>("basic_desc");
			bbd.close = r.get<double>("close", 0);
			bbd.percent = r.get<double>("percent", 0);

			m_vec_index.insert(m_vec_index.end(), bbd);
		}

		ss.str("");
		ss << "OK, CTechAnalysisEvent 从 MYSQL 获取数据完毕.";
		//return EXIT_SUCCESS;
	}
	catch (std::exception const & e)
	{
		ss.str("");
		ss << e.what() << ' \r\n';
	}
}
