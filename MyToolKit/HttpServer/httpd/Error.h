#pragma once

namespace ZvLibs
{
	class CError
	{
	public:
		CError(void);
		virtual ~CError(void);

		virtual void SetErr(const string & szReason = "", const string & szOperation = "", bool bLogIt = false);
		virtual void LogErr();

		static string GetWinErr();

		string GetReason();
		string GetOperation();
		string GetErr();

		static string m_szReason;
		static string m_szOperation;
	};
}
