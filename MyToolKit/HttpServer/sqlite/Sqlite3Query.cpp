#include "stdafx.h"
#include "Sqlite3Db.h"
#include "../utils/ZvUtils.h"
#include "../utils/UtfConverter.h"

namespace ZvLibs
{
	#include "sqlite3.h"

	CSqlite3Query::CSqlite3Query(void)
	{
		m_bNull = true;
		m_bEof = true;
		m_paszResults = NULL;
		m_nRows = m_nCols = m_nCurrentRow = 0;
	}

	CSqlite3Query::~CSqlite3Query(void)
	{
		Finalize();
	}

	CSqlite3Query::CSqlite3Query(const CSqlite3Query& Query)
	{
		m_bEof = Query.m_bEof;
		m_bNull = Query.m_bNull;

		m_paszResults = Query.m_paszResults;

		const_cast<CSqlite3Query&>(Query).m_paszResults = NULL;

		m_nRows = Query.m_nRows;
		m_nCols = Query.m_nCols;
		m_nCurrentRow = Query.m_nCurrentRow;
		m_mapFieldIndex = Query.m_mapFieldIndex;
	}


	CSqlite3Query::CSqlite3Query(char** paszResults, int nRows, int nCols)
	{
		m_paszResults = paszResults;
		m_nRows = nRows;
		m_nCols = nCols;
		m_nCurrentRow = 0;
		m_bNull = false;
		m_bEof = (nRows==0?true:false);

		for(int i=0; i<nCols; i++)
		{
			m_mapFieldIndex[FieldName(i)] = i;
		}
	}

	void CSqlite3Query::Finalize()
	{
		if (m_paszResults)
		{
			sqlite3_free_table(m_paszResults);
			m_paszResults = NULL;
		}
	}

	void CSqlite3Query::NextRow()
	{
		m_nCurrentRow++;
		if(m_nCurrentRow>=m_nRows)
			m_bEof = true;
	}

	int CSqlite3Query::GetInt(int nCol, int nDefault)
	{
		if(IsValueNull(nCol)) return nDefault;

		return atoi((char*)GetString(nCol).c_str());
	}

	int CSqlite3Query::GetInt(const tstring & szField, int nDefault)
	{
		if(IsValueNull(szField)) return nDefault;

		return atoi((char*)GetString(szField).c_str());
	}

	long CSqlite3Query::GetLong(int nCol, long nDefault)
	{
		if(IsValueNull(nCol)) return nDefault;

		return atol((char*)GetString(nCol).c_str());
	}

	long CSqlite3Query::GetLong(const tstring & szField, long nDefault)
	{
		if(IsValueNull(szField)) return nDefault;

		return atol((char*)GetString(szField).c_str());
	}

	bool CSqlite3Query::GetBool(int nCol, bool bDefault)
	{
		if(IsValueNull(nCol)) return bDefault;

		return GetInt(nCol)==1;
	}

	bool CSqlite3Query::GetBool(const tstring & szField, bool bDefault)
	{	
		if(IsValueNull(szField)) return bDefault;

		return GetInt(szField)==1;
	}	

	bool CSqlite3Query::IsValueNull(const tstring & szField)
	{
		if(m_mapFieldIndex.find(szField)==m_mapFieldIndex.end())
			return true;

		return IsValueNull(m_mapFieldIndex[szField]);
	}

	bool CSqlite3Query::IsValueNull(int nCol)
	{
		if (nCol < 0 || nCol > m_nCols-1)
		{
			return true;
		}

		int nIndex = (m_nCurrentRow*m_nCols) + m_nCols + nCol;
		
		if(m_paszResults[nIndex]==NULL) return true;

		return false;
	}

	tstring CSqlite3Query::GetString(int nCol, const tstring & szDefault)
	{
		if(IsValueNull(nCol)) return szDefault;

		int nIndex = (m_nCurrentRow*m_nCols) + m_nCols + nCol;

		return StringToTstring(m_paszResults[nIndex]);
	}

	tstring CSqlite3Query::GetString(const tstring & szField, const tstring & szDefault)
	{
		if(m_mapFieldIndex.find(szField)==m_mapFieldIndex.end())
			return szDefault;

		int nCol = m_mapFieldIndex[szField];

		return GetString(nCol, szDefault);
	}

	tstring CSqlite3Query::RowToJson()
	{
		tostringstream ss;
		ss << _T("{");
		
		for(int i=0; i<GetColCount(); i++)
		{
			if(i>0)
				ss << ",";

			ss << _T("\"") << FieldName(i) << _T("\":\"") << EscapeJsonCharacter(GetString(i)) << "\"";
		}

		ss << "}";
		return ss.str();
	}

	void CSqlite3Query::ToXml(tstring szHeader, tostringstream & ss, bool bEscapeHtml)
	{
		int nColCount = GetColCount();

		while(!Eof())
		{
			if(szHeader.length()>0)
			{
				ss << "<";
				ss << szHeader;
				ss << ">";
			}

			for(int i=0; i<nColCount; i++)
			{
				ss << "<";
				ss << FieldName(i);
				ss << ">";

				ss << (bEscapeHtml?EscapeHtmlCharacter(GetString(i)):GetString(i));

				ss << "</";
				ss << FieldName(i);
				ss << ">";
			}

			if(szHeader.length()>0)
			{
				ss << "</";
				ss << szHeader;
				ss << ">";
			}

			NextRow();
		}
	}

	tstring CSqlite3Query::FieldName(int nCol)
	{
		return StringToTstring(m_paszResults[nCol]);
	}

	CSqlite3Query & CSqlite3Query::operator = (const CSqlite3Query & Sqlite3Query)
	{
		Finalize();

		m_bEof = Sqlite3Query.m_bEof;
		m_bNull = Sqlite3Query.m_bNull;

		m_paszResults = Sqlite3Query.m_paszResults;

		const_cast<CSqlite3Query&>(Sqlite3Query).m_paszResults = NULL;

		m_nRows = Sqlite3Query.m_nRows;
		m_nCols = Sqlite3Query.m_nCols;
		m_nCurrentRow = Sqlite3Query.m_nCurrentRow;
		m_mapFieldIndex = Sqlite3Query.m_mapFieldIndex;

		return *this;
	}

}