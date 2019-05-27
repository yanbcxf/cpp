#include "StdAfx.h"
#include "StockDataModel.h"

CStockDataModel::CStockDataModel(void)
{
	m_nCode = -1;
	m_nRowNum = 0;
	m_hWnd = NULL;
	m_nLogType = WM_USER_LOG_1;
	m_ConnectString = "dbname=stock  port=3366   host=127.0.0.1 user=root  charset=utf8 ";
}

CStockDataModel::~CStockDataModel(void)
{
}

void CStockDataModel::NotifyDatabaseImportedToWindow()
{
	/*stringstream ss;
	ss << "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
	ss << "<root>";
	ss << "<msg_type code=\"" << m_nCode << "\" ";
	ss << " model=\"" << ModelType() << "\" ";
	ss << " >DatabaseImported</msg_type>";
	ss << "<data ";
	ss << " >";
	ss << "</data></root>";

	sendToOutput(ss.str().c_str(), m_hWnd);*/
}


long  CStockDataModel::CheckDownloadFileInfo(string middleFileName)
{
	stringstream ss;
	long	nfilesize = 0;

	CFileFind finder;
	BOOL bWorking = finder.FindFile(middleFileName.c_str());
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CTime ftime;

		ss  <<"\r\nGetFileName: "<<finder.GetFileName();

		finder.GetCreationTime(ftime);
		ss <<"\r\nGetCreationTime:"<<ftime.Format("[%Y-%m-%d %H:%M:%S]");

		finder.GetLastAccessTime(ftime);
		ss <<"\r\nGetLastAccessTime:"<<ftime.Format("[%Y-%m-%d %H:%M:%S]");

		finder.GetLastWriteTime(ftime);
		ss <<"\r\nGetLastWriteTime: "<<ftime.Format("[%Y-%m-%d %H:%M:%S]");

		ss << "\r\n";

		sendToOutput(ss.str().c_str(), m_hWnd, m_nLogType);

		nfilesize = finder.GetLength();
		if(nfilesize >0)
		{
			// 本日已经下载，则不重复下载

			// 当前时间前的最后一个交易日
			time_t curtime = time (NULL);
			struct tm  tm1;
			while(1)
			{
				//tm1 = gmtime (&curtime);
				localtime_s(&tm1, &curtime);
				if(tm1.tm_wday==0 || tm1.tm_wday==6)
				{
					curtime = curtime - 24 * 3600;
					continue;
				}

				char temp[256] = { 0 };
				snprintf (temp, 255, "%4d-%02d-%02d", 1900 + tm1.tm_year, tm1.tm_mon+1, tm1.tm_mday);	
				//currentLastTradeDate = string(temp);
				break;
			}

			if(tm1.tm_year + 1900 > ftime.GetYear())
				nfilesize = 0;	//	重新下载
			else if(tm1.tm_year + 1900 == ftime.GetYear() && tm1.tm_mon + 1 > ftime.GetMonth())
				nfilesize = 0;	//	重新下载
			else if(tm1.tm_year + 1900 == ftime.GetYear() && tm1.tm_mon + 1 == ftime.GetMonth() \
				&& tm1.tm_mday > ftime.GetDay())
				nfilesize = 0;	//	重新下载

		}
	}

	return nfilesize;
}




long  CStockDataModel::GetDownloadFileSize(string middleFileName)
{
	stringstream ss;
	long	nfilesize = 0;

	CFileFind finder;
	BOOL bWorking = finder.FindFile(middleFileName.c_str());
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CTime ftime;


		finder.GetCreationTime(ftime);
		finder.GetLastAccessTime(ftime);
		finder.GetLastWriteTime(ftime);


		nfilesize = finder.GetLength();

	}

	return nfilesize;
}