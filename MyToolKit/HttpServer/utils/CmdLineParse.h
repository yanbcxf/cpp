#pragma once

namespace ZvLibs
{

	class CCmdLineParse
	{
	public:
		CCmdLineParse(int argc, _TCHAR* argv[]);
		~CCmdLineParse(void);

		tstring GetString(const tstring & szParam);
		int GetInt(const tstring & szParam);
		bool ParamExist(const tstring & szParam);

	private:
		map<tstring, tstring> m_mapParam;
	};

};