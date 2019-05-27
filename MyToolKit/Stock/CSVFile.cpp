#include "StdAfx.h"

#include "CSVFile.h"


CCSVFile::CCSVFile(LPCTSTR lpszFilename, Mode mode)
: CStdioFile(lpszFilename, (mode == modeRead) ?
			 CFile::modeRead|CFile::shareDenyWrite|CFile::typeText   
			 :CFile::modeWrite|CFile::shareDenyWrite|CFile::modeCreate|CFile::typeText)
{

#ifdef _DEBUG
	m_nMode = mode;
#endif
}

CCSVFile::~CCSVFile(void)
{
}

bool CCSVFile::ReadData(CStringArray &arr)
{
	// Verify correct mode in debug build
	ASSERT(m_nMode == modeRead);

	// Read next line
	CString sLine;
	if (!ReadString(sLine))
		return false;

	LPCTSTR p = sLine;
	int nValue = 0;

	// Parse values in this line

	while (*p != '\0')
	{
		CString s;  // String to hold this value

		if (*p == '"')
		{
			// Bump past opening quote
			p++;
			// Parse quoted value
			while (*p != '\0')
			{
				// Test for quote character
				if (*p == '"')
				{
					// Found one quote
					p++;
					// If pair of quotes, keep one
					// Else interpret as end of value
					if (*p != '"')
					{
						p++;
						break;
					}
				}
				// Add this character to value
				s.AppendChar(*p++);
			}
		}
		else
		{
			// Parse unquoted value
			while (*p != '\0' && *p != ',')
			{
				s.AppendChar(*p++);
			}
			// Advance to next character (if not already end of string)
			if (*p != '\0')
				p++;
		}
		// Add this string to value array
		if (nValue < arr.GetCount())
			arr[nValue] = s;
		else
			arr.Add(s);
		nValue++;
	}

	//	yangbin : 对于 “逗号” 后面紧跟 “\0”的情况，要额外增加一个空字符串
	if( *p == '\0' && *(p-1)== ',')
	{
		CString s = "";
		if (nValue < arr.GetCount())
			arr[nValue] = s;
		else
			arr.Add(s);
		nValue++;
	}

	// Trim off any unused array values
	if (arr.GetCount() > nValue)
		arr.RemoveAt(nValue, arr.GetCount() - nValue);
	// We return true if ReadString() succeeded--even if no values
	return true;
}

void CCSVFile::WriteData(CStringArray &arr)
{
	static TCHAR chQuote = '"';
	static TCHAR chComma = ',';

	// Verify correct mode in debug build
	ASSERT(m_nMode == modeWrite);

	// Loop through each string in array
	for (int i = 0; i < arr.GetCount(); i++)
	{
		// Separate this value from previous
		if (i > 0)
			WriteString(_T(","));
		// We need special handling if string contains
		// comma or double quote
		bool bComma = (arr[i].Find(chComma) != -1);
		bool bQuote = (arr[i].Find(chQuote) != -1);
		if (bComma || bQuote)
		{
			Write(&chQuote, sizeof(TCHAR));
			if (bQuote)
			{
				for (int j = 0; j < arr[i].GetLength(); i++)
				{
					// Pairs of quotes interpreted as single quote
					if (arr[i][j] == chQuote)
						Write(&chQuote, sizeof(TCHAR));
					TCHAR ch = arr[i][j];
					Write(&ch, sizeof(TCHAR));
				}
			}
			else
			{
				WriteString(arr[i]);
			}
			Write(&chQuote, sizeof(TCHAR));
		}
		else
		{
			WriteString(arr[i]);
		}
	}
	WriteString(_T("\n"));
}


bool CCSVFile::ReadAllData(vector<string>& arrayHeader, vector<vector<string>>& arrayData)
{
	arrayHeader.clear();
	arrayData.clear();

	CStringArray arr;
	//CCSVFile csvFile(szFileName);
	ReadData(arr);
	for(int i=0; i<arr.GetSize(); i++)
	{
		arrayHeader.push_back(arr.ElementAt(i).GetBuffer());
	}

	while(1)
	{

		arr.RemoveAll();
		if(ReadData(arr)==false)
			break;
		else
		{
			vector<string> listData;
			for(int i=0; i<arr.GetSize(); i++)
			{
				listData.push_back(arr.ElementAt(i).GetBuffer());
			}
			arrayData.push_back(listData);
		}

	}
	return true;
}