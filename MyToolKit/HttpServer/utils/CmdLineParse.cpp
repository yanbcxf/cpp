#include "StdAfx.h"
#include "CmdLineParse.h"

namespace ZvLibs
{

	CCmdLineParse::CCmdLineParse(int argc, _TCHAR* argv[])
	{
		tstring szArg;

		for(int i=0; i<argc; i++)
		{
			if(argv[i][0]==_T('-'))
			{
				szArg = argv[i]+1;
				m_mapParam[szArg]=_T("");
			}
			else
			{
				if(szArg.length()>0)
				{
					m_mapParam[szArg] = argv[i];
					szArg = _T("");
				}
			}
		}
	}

	CCmdLineParse::~CCmdLineParse(void)
	{
	}

	tstring CCmdLineParse::GetString(const tstring & szParam)
	{
		if(!ParamExist(szParam))
			return _T("");

		return m_mapParam[szParam];
	}

	int CCmdLineParse::GetInt(const tstring & szParam)
	{
		if(!ParamExist(szParam))
			return 0;

		return atoi(m_mapParam[szParam].c_str());
	}

	bool CCmdLineParse::ParamExist(const tstring & szParam)
	{
		return m_mapParam.find(szParam)!=m_mapParam.end();
	}

};