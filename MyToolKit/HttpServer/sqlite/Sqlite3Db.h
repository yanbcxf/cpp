#pragma once

#include "Sqlite3Query.h"

namespace ZvLibs
{
	struct sqlite3;

	class CSqlite3Db
	{
	public:
		CSqlite3Db(const tstring & szFilePath = _T(""));
		virtual ~CSqlite3Db(void);

		bool ExecDML(const tstring & szSql, int * pChange = NULL);
		CSqlite3Query Execute(const tstring & szSql);

		bool Open(const tstring & szDbName = _T(""));
		void Close();
		bool IsOpened() {return m_bOpened;};

		bool BeginTrans();
		bool CommitTrans();
		bool RollbackTrans();

		tstring Version();

		tstring m_szDbPath;

	private:

		HANDLE m_hMutex;
		bool m_bOpened;
		sqlite3* m_pSqlite3;
	};

}