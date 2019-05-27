#pragma once

namespace ZvLibs
{

	class CSqlite3Query
	{
	public:
		CSqlite3Query(void);
		virtual ~CSqlite3Query(void);

		CSqlite3Query(char** paszResults, int nRows, int nCols);
		CSqlite3Query(const CSqlite3Query& DbRecords);

		inline bool IsNull() {return m_bNull;};
		inline bool Eof() {return m_bEof;};
		void NextRow();

		tstring GetString(int nCol, const tstring & szDefault = _T(""));
		tstring GetString(const tstring & szField, const tstring & szDefault = _T(""));
		int GetInt(int nCol, int nDefault = 0);
		int GetInt(const tstring & szField, int nDefault = 0);

		long GetLong(int nCol, long nDefault = 0);
		long GetLong(const tstring & szField, long nDefault = 0);

		bool GetBool(int nCol, bool bDefault = false);
		bool GetBool(const tstring & szField, bool bDefault = false);

		int GetColCount() {return m_nCols;};
		int GetRowCount();
		inline int GetCurRow(){return m_nCurrentRow;};

		void Finalize();

		void ToXml(tstring szHeader, tostringstream & ss, bool bEscapeHtml = true);
		CSqlite3Query & operator = (const CSqlite3Query & DbRecords);
		tstring FieldName(int nCol);
		bool IsValueNull(int nCol);
		bool IsValueNull(const tstring & szField);
		tstring RowToJson();

	private:

		bool m_bNull;
		bool m_bEof;

		int m_nCols;
		int m_nRows;
		int m_nCurrentRow;
		char** m_paszResults;

		map<tstring, int> m_mapFieldIndex;
	};

}