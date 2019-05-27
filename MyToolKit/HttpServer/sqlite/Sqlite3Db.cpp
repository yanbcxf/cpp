#include "stdafx.h"

#include "../utils/UtfConverter.h"
#include "Sqlite3Db.h"
#pragma comment(lib, "sqlite3.lib")

namespace ZvLibs
{
	#include <sqlite3.h>

	CSqlite3Db::CSqlite3Db(const tstring & szFilePath)
	{
		m_bOpened = false;
		m_pSqlite3 = NULL;

		if(szFilePath.length()<=0)
		{
			m_szDbPath = GetAppPath()+_T("\\database.db");
		}
		else
			m_szDbPath = szFilePath;
	}

	CSqlite3Db::~CSqlite3Db(void)
	{
		Close();
	}

	bool CSqlite3Db::Open(const tstring & szDbPath)
	{
		if(IsOpened())
			return true;

		if(szDbPath!=_T(""))
			m_szDbPath = szDbPath;

		int nRet = sqlite3_open(TstringToUtf8(m_szDbPath).c_str(), &m_pSqlite3);

		if (nRet != SQLITE_OK)
		{
			return false;
		}

		m_bOpened = true;

		return true;
	}

	tstring CSqlite3Db::Version()
	{
		return StringToTstring(sqlite3_libversion());
	}

	void CSqlite3Db::Close()
	{
		if(m_pSqlite3)
			sqlite3_close(m_pSqlite3);

		m_bOpened = false;
		m_pSqlite3 = NULL;
		m_szDbPath = _T("");
	}

	bool CSqlite3Db::ExecDML(const tstring & szSql, int * pChange)
	{
		if(!Open())
			return false;

		tstring sql = TstringToUtf8(szSql);

		char * szError;

		if(sqlite3_exec(m_pSqlite3, sql.c_str(), NULL, NULL, &szError)==SQLITE_OK)
		{
			if(pChange!=NULL)
				*pChange = sqlite3_changes(m_pSqlite3);
			return true;
		}
		
		sqlite3_free(szError);

		return false;
	}

	CSqlite3Query CSqlite3Db::Execute(const tstring & szSql)
	{
		if(!Open())
			return CSqlite3Query();

		char* szError=0;
		char** paszResults=0;
		int nRet;
		int nRows(0);
		int nCols(0);

		tstring sql = TstringToUtf8(szSql);
		
		nRet = sqlite3_get_table(m_pSqlite3, sql.c_str(), &paszResults, &nRows, &nCols, &szError);
		

		if (nRet == SQLITE_OK)
		{
			return CSqlite3Query(paszResults, nRows, nCols);
		}
		
		sqlite3_free(szError);

		return CSqlite3Query();
	}

	bool CSqlite3Db::BeginTrans()
	{
		return ExecDML(_T("begin transaction;"));
	}

	bool CSqlite3Db::CommitTrans()
	{
		bool b = ExecDML(_T("commit transaction;"), false);
		if(!b)
			RollbackTrans();
		return b;
	}

	bool CSqlite3Db::RollbackTrans()
	{
		return ExecDML(_T("rollback transaction;"), false);
	}

}