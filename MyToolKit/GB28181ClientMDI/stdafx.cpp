
// stdafx.cpp : 只包括标准包含文件的源文件
// GB28181ClientMDI.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

JMutex			g_mtxNet;
map<int, HWND>	g_mapNet2Hwnd;
map<int, int>	g_mapNet2Player;

Logger g_Logger;




BOOL DebugTrace(char * lpszFormat,...)
{
	static HWND hwnd = ::FindWindowA(NULL,	"DbgView");
	if(!IsWindow(hwnd))
		hwnd = ::FindWindowA(NULL, "DbgView");
	if(hwnd)
	{
		static char szMsg[512];
		va_list argList;
		va_start(argList, lpszFormat);
		try
		{
			vsprintf(szMsg,lpszFormat, argList);
		}
		catch(...)
		{
			strcpy(szMsg ,"DebugHelper:Invalid string format!");
		}
		// 		OutputDebugString(szMsg);
		va_end(argList);
		//OutputDebugStr(szMsg);
		//OutputDebugStr("\n\r");
		DWORD dwId = GetCurrentProcessId();
		::SendMessage(hwnd,WM_SETTEXT,dwId,(LPARAM)(LPCTSTR)szMsg);
	}

	return TRUE;
}

void CTimeToLDateTime(const CTime &ctTm,LDATETIME &struTm)
{
	struTm.dwYear	= ctTm.GetYear();
	struTm.dwMonth	= ctTm.GetMonth();
	struTm.dwDay	= ctTm.GetDay();
	struTm.dwHour	= ctTm.GetHour();
	struTm.dwMinute	= ctTm.GetMinute();
	struTm.dwSecond	= ctTm.GetSecond();
}

CTime StrToTime(char *str)    
{ 
	int   iY,iMon,iD,iH,iM,iSS;    
	int   len;  
	try{
		string s=str;
		len=s.length();    
		if(len==8)    
		{    
			CTime   t1;    
			t1=CTime::GetCurrentTime();    
			iY=t1.GetYear();    
			iMon=t1.GetMonth();    
			iD=t1.GetDay();    
			string   h,m,ss;    
			h=s.substr(0,2);    
			iH=atoi(h.c_str());    
			m=s.substr(3,2);    
			iM=atoi(m.c_str());    
			ss=s.substr(6,2);    
			iSS=atoi(ss.c_str());    
		}    
		else if(len==19)    
		{    
			string   y,mon,d,h,m,ss;    
			y=s.substr(0,4);    
			iY=atoi(y.c_str());    
			mon=s.substr(5,2);    
			iMon=atoi(mon.c_str());    
			d=s.substr(8,2);    
			iD=atoi(d.c_str());    
			h=s.substr(11,2);    
			iH=atoi(h.c_str());    
			m=s.substr(14,2);    
			iM=atoi(m.c_str());    
			ss=s.substr(17,2);    
			iSS=atoi(ss.c_str());    
		} 
		else if(len==14)    
		{    
			string   y,mon,d,h,m,ss;    
			y=s.substr(0,4);    
			iY=atoi(y.c_str());    
			mon=s.substr(4,2);    
			iMon=atoi(mon.c_str());    
			d=s.substr(6,2);    
			iD=atoi(d.c_str());    
			h=s.substr(8,2);    
			iH=atoi(h.c_str());    
			m=s.substr(10,2);    
			iM=atoi(m.c_str());    
			ss=s.substr(12,2);    
			iSS=atoi(ss.c_str());    
		}	
		else    
		{    
			CTime   t2;    
			t2=CTime::GetCurrentTime();    
			iY=t2.GetYear();    
			iMon=t2.GetMonth();    
			iD=t2.GetDay();    
			iH=t2.GetHour();    
			iM=t2.GetMinute();    
			iSS=t2.GetSecond();    
		} 
	}
	catch (...)
	{
		DebugTrace("StrToTime");
	}   

	CTime t(iY,iMon,iD,iH,iM,iSS); 
	return t; 	
}


void convert_to_utc(const char *cSource,char *cDest)
{
	struct tm gaintime = {0};
	sscanf(cSource,"%04d-%02d-%02dT%02d:%02d:%02d",&gaintime.tm_year,&gaintime.tm_mon,&gaintime.tm_mday,&gaintime.tm_hour,&gaintime.tm_min,&gaintime.tm_sec);
	gaintime.tm_year -= 1900;
	gaintime.tm_mon -= 1;
	time_t utcTime =mktime(&gaintime);
	sprintf(cDest,"%I64d",utcTime);
}