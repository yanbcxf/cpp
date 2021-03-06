// HttpServer.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "XmlFiles.h"
#include "XmlExplorer.h"
#include "JsonNotify.h"

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	CHttpSvr HttpSvr;

	if (!(HttpSvr.Start(80) || HttpSvr.Start(81) || HttpSvr.Start(82)))
		return 0;

	cout << "http://" << HttpSvr.GetIp() << ":" << HttpSvr.m_nPort;

	HttpSvr.SetDefaultFile("index.html");

	CXmlFiles XmlFiles(&HttpSvr);
	CXmlExplorer XmlExplorer(&HttpSvr);

	// yangbin
	CJsonNotify JsonNotify(&HttpSvr);
	HttpSvr.AddHandle(JsonNotify);

	HttpSvr.AddHandle(XmlFiles);
	HttpSvr.AddHandle(XmlExplorer);

	Sleep(INFINITE);
	return 0;
}

