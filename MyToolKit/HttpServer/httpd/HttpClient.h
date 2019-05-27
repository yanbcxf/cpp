#pragma once

#include "HttpSocket.h"

namespace ZvLibs
{

	class CHttpClient : public CHttpSocket
	{
	public:
		CHttpClient();
		virtual ~CHttpClient();

		bool Connect(const string & szAddress, int nPort);

		void Close();
		bool Request(const string & szUrl, bool bSaveToFile = false);
		bool RequestUrl(const string & szUrl, bool bSaveToFile = false);

		void AddHeader(const string & szName, const string & szValue);
		void AddParamUrl(const string & szName, const string & szValue);
		void AddParamPost(const string & szName, const string & szValue);
		void SetAuthBasic(const string & szUser, const string & szPass);

		bool SendHeader();
		bool IsPost();
		string GetPostLength();
		bool AddFile(const string & szName, const string & szPath);
		bool SendData();
		void Clearup();
		bool RecvHeader();
		bool RecvData();

		int GetStateCode();
		string GetStateLine();

		void DeleteTmpData();
		void SetKeepAlive();
		void SetPost(bool bPost);
		bool HeaderExists(const string & szHeader);

		string m_szData;
		string m_szFilePath;
		string m_szFileName;

	private:
		
		string m_szStateLine;
		string m_szUrl;

		bool m_bSaveToFile;

		map<string, string> m_mapHeaderOut;
		map<string, string> m_mapHeaderIn;
		map<string, string> m_mapParamUrl;
		map<string, string> m_mapParamPost;
		map<string, string> m_mapFiles;

		int m_nStateCode;

		string m_szBoundary;
		bool m_bUploadFile;
		bool m_bPost;

		// string m_szAddress;
		// int m_nPort;
		DWORD m_dwLenFiles;
	};

}