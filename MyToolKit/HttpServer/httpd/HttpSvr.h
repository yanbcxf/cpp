#pragma once

namespace ZvLibs
{
	class CHttpHandle;

	#define DEFAULT_HTTP_PORT 80

	class CHttpSvr
	{
	public:

		CHttpSvr(const tstring & szRootPath="./", int nPort = DEFAULT_HTTP_PORT);
		virtual ~CHttpSvr();

		bool Start(int nPort = -1);
		void Stop();
		void SetRootPath(const string & szRoot);

		string GetRootPath();

		static void ThreadAccept(LPVOID pParam);

		void AddHandle(CHttpHandle & HttpHandle);

		void InitMimeTypes();
		bool ContentHandleExist(const string & szPath);

		string GetIp();
		void RemoveHandle(CHttpHandle & Httphandle);
		bool SetPort(int nPort);
		void SetDefaultFile(const string szDefaultFile);

		int m_nPort;
		SOCKET m_sListen;
		string m_szRoot;
		string m_szDefaultFile;

		map<string, string> m_mapMimeTypes;

		vector<CHttpHandle*> m_vtHandleFilterBefore;
		map<string, CHttpHandle*> m_mapHandleContent;
		vector<CHttpHandle*> m_vtHandleFilterAfter;
	};

}