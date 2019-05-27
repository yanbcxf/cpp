
#include "stdafx.h"
#include "compileUTF8range.h"

namespace compileUTF8range
{

	char* Utf82Unicode(const char* utf, size_t *unicode_number)  
	{  
		if(!utf || !strlen(utf))  
		{  
			*unicode_number = 0;  
			return NULL;  
		}  
		int dwUnicodeLen = MultiByteToWideChar(CP_UTF8,0,utf,-1,NULL,0);  
		size_t num = dwUnicodeLen*sizeof(wchar_t);  
		wchar_t *pwText = (wchar_t*)malloc(num);  
		memset(pwText,0,num);  
		MultiByteToWideChar(CP_UTF8,0,utf,-1,pwText,dwUnicodeLen);  
		*unicode_number = dwUnicodeLen - 1;  
		return (char*)pwText;  
	}  


	//////////////////////////////////////////////////////////////////////////

	class CUtf8Rule
	{
	public:
		unsigned int	bottomCodePoint;	//	底部码点
		unsigned int	topCodePoint;
		vector<unsigned char>	bottomUCS;	//	底部码点对应的 utf8 编码
		vector<unsigned char>	topUCS;

	};

	CUtf8Rule rules[9];

	vector<unsigned char> Array2Vector(unsigned char array[], int nSize)
	{
		return vector<unsigned char>(array, array + nSize);
	}

	void init()
	{
		rules[0].bottomCodePoint = 0x00;
		rules[0].topCodePoint = 0x7f;
		rules[0].bottomUCS.push_back( 0x00 );
		rules[0].topUCS.push_back( 0x7f );

		rules[1].bottomCodePoint = 0x80;
		rules[1].topCodePoint = 0x7ff;
		unsigned char arb1[] = {0xc2, 0x80};
		unsigned char art1[] = {0xdf, 0xbf};
		rules[1].bottomUCS = Array2Vector( arb1 ,2); 
		rules[1].topUCS = Array2Vector(art1 ,2);

		rules[2].bottomCodePoint = 0x800;
		rules[2].topCodePoint = 0xfff;
		unsigned char arb2[] = {0xe0, 0xa0, 0x80};
		unsigned char art2[] = {0xe0, 0xbf, 0xbf};
		rules[2].bottomUCS = Array2Vector(arb2 ,3); 
		rules[2].topUCS = Array2Vector(art2, 3);

		rules[3].bottomCodePoint = 0x1000;
		rules[3].topCodePoint = 0xcfff;
		unsigned char arb3[] = {0xe1, 0x80, 0x80};
		unsigned char art3[] = {0xec, 0xbf, 0xbf};
		rules[3].bottomUCS = Array2Vector(arb3 ,3); 
		rules[3].topUCS = Array2Vector(art3 ,3);

		rules[4].bottomCodePoint = 0xd000;
		rules[4].topCodePoint = 0xd7ff;
		unsigned char arb4[] = {0xed, 0x80, 0x80};
		unsigned char art4[] = {0xed, 0x9f, 0xbf};
		rules[4].bottomUCS = Array2Vector(arb4, 3); 
		rules[4].topUCS = Array2Vector(art4, 3);

		rules[5].bottomCodePoint = 0xe000;
		rules[5].topCodePoint = 0xffff;
		unsigned char arb5[] = {0xee, 0x80, 0x80};
		unsigned char art5[] = {0xef, 0xbf, 0xbf};
		rules[5].bottomUCS = Array2Vector(arb5, 3); 
		rules[5].topUCS = Array2Vector(art5, 3);

		rules[6].bottomCodePoint = 0x10000;
		rules[6].topCodePoint = 0x3ffff;
		unsigned char arb6[] = {0xf0, 0x90, 0x80, 0x80};
		unsigned char art6[] = {0xf0, 0xbf, 0xbf, 0xbf};
		rules[6].bottomUCS = Array2Vector(arb6, 4); 
		rules[6].topUCS = Array2Vector(art6, 4);

		rules[7].bottomCodePoint = 0x40000;
		rules[7].topCodePoint = 0xfffff;
		unsigned char arb7[] = {0xf1, 0x80, 0x80, 0x80};
		unsigned char art7[] = {0xf3, 0xbf, 0xbf, 0xbf};
		rules[7].bottomUCS = Array2Vector(arb7, 4); 
		rules[7].topUCS = Array2Vector(art7, 4);

		rules[8].bottomCodePoint = 0x100000;
		rules[8].topCodePoint = 0x10ffff;
		unsigned char arb8[] = {0xf4, 0x80, 0x80, 0x80};
		unsigned char art8[] = {0xf4, 0xbf, 0xbf, 0xbf};
		rules[8].bottomUCS = Array2Vector(arb8, 4); 
		rules[8].topUCS = Array2Vector(art8, 4);

	}



	//# Find which rule subsumes the codepoint.
	int find(unsigned int codepoint)
	{
		int i = 0;
		for(; i< 9; i++)
		{
			if(rules[i].bottomCodePoint <= codepoint && codepoint <= rules[i].topCodePoint )
				return i;
		}
		return -1;
	}

	string toHex(unsigned char ch)
	{
		char tmp[16] = { 0 };
		sprintf_s (tmp, 16, "\\x%02X", ch);
		return string(tmp);
	}




	string compileUTF8range(unsigned short bot, unsigned short top)
	{
		int botRuleNo = find(bot);
		int topRuleNo = find(top);
		string ret;
		if (botRuleNo == topRuleNo) {
			ret = expandRange(botRuleNo, bot, top);
		} 
		else 
		{
			
			vector<string> ranges;
			string t;

			//# Expand the bottom rule (including stair-steps)
			t = expandRange(botRuleNo, bot, rules[botRuleNo].topCodePoint );
			ranges.push_back(t);

			//# Expland each rule in between the bottom and top.
			for (int i = botRuleNo+1; i < topRuleNo; i++) {
				t = expandRange(i, rules[i].bottomCodePoint, rules[i].topCodePoint);
				ranges.push_back(t);
			}

			//# Expand the top rule.
			t = expandRange(topRuleNo, rules[topRuleNo].bottomCodePoint, top);
			ranges.push_back(t);

			string strJoin = "";
			for(int i= 0; i< ranges.size(); i++)
			{
				strJoin += ranges[i];
				if(i< ranges.size() -1)
					strJoin += "|";
			}
			ret = strJoin;
		}
		return ret;
	}


	//# Expand the characters between $bot and $top. ruleNo must subsume both characters.
	string expandRange(int ruleNo, unsigned short bot, unsigned short top )
	{
		// 从 unicode 码点转换为 utf-8 编码，第一次调用计算长度，第二次实际转换
		unsigned char utf8_str[16] = { 0 };
		int utf8_size = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)&bot, 1, NULL, 0, NULL, NULL) + 1;
		WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)&bot, 1, (char *)utf8_str, utf8_size, NULL, NULL);
		vector<unsigned char>	botBytes = Array2Vector(utf8_str, strlen((char *)utf8_str));

		memset(utf8_str, 0, 16);
		utf8_size = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)&top, 1, NULL, 0, NULL, NULL) + 1;
		WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)&top, 1, (char *)utf8_str, utf8_size, NULL, NULL);
		vector<unsigned char>	topBytes = Array2Vector(utf8_str, strlen((char *)utf8_str));

		bool undef1, undef2;
		return walk(rules[ruleNo].bottomUCS, rules[ruleNo].topUCS, botBytes, topBytes, undef1, undef2);
	}


	string walk(vector<unsigned char> bRules,vector<unsigned char> tRules,
		vector<unsigned char> botBytes,vector<unsigned char> topBytes, bool & splitB, bool & splitT)
	{
		unsigned char bRule = bRules[0];	bRules.erase(bRules.begin());
		unsigned char tRule = tRules[0];	tRules.erase(tRules.begin());

		unsigned char botByte = botBytes[0];	botBytes.erase(botBytes.begin());
		unsigned char topByte = topBytes[0];	topBytes.erase(topBytes.begin());

		string	b = toHex(botByte);
		string	t = toHex(topByte);
		string	rb = toHex(bRule);
		string	rt = toHex(tRule);
		
		if (bRules.empty()) {
			splitB = botByte != bRule;
			splitT = topByte != tRule;
			char tmp[128] = { 0 };
			if(botByte == topByte)
			{
				sprintf_s(tmp, 128, "%s", b.c_str());
			}
			else
			{
				sprintf_s(tmp, 128, "[%s-%s]", b.c_str(), t.c_str());
			}
			return string(tmp);
		}

		//# Do a tentative walk of remaining chars. Sets $splitB and $splitT.
		string range = walk(bRules, tRules, botBytes, topBytes, splitB, splitT);

		if (botByte == topByte) {
			char tmp[128] = { 0 };
			sprintf_s(tmp, 128, "(%s, %s)", b.c_str(), range.c_str());
			return string(tmp);
		}

		vector<string>	vecRet;
		string topStr = "";

		if (splitB) {
			//# Walk the bottom as if the top char is the top rule.
			bool undef1, undef2;
			string bRange = walk(bRules, tRules, botBytes, tRules, undef1, undef2);
			char tmp[128] = { 0 };
			sprintf_s(tmp, 128, "(%s, %s)", b.c_str(), bRange.c_str());
			vecRet.push_back(string(tmp));
			botByte++;
			b = toHex(botByte);
		} 
		else if (botByte > bRule) {
			// # our callers have to worry about stair-stepping
			splitB = true; 
		}


		if (splitT) {
			//# Walk the bottom as if the bottom char is the bottom rule.
			bool undef1, undef2;
			string tRange = walk(bRules, tRules, bRules, topBytes, undef1, undef2);
			char tmp[128] = { 0 };
			sprintf_s(tmp, 128, "(%s, %s)", t.c_str(), tRange.c_str());
			topStr = string(tmp);
			topByte--;
			t = toHex(topByte);
		} 
		else if (topByte > tRule) {
			//# our callers have to worry about stair-stepping
			splitT = true; 
		}

		//# Walk intermediate sub-rules.
		if (topByte > botByte) {
			string r1 = walkFull(bRules, tRules);

			char tmp[128] = { 0 };
			sprintf_s(tmp, 128, "([%s-%s], %s)", b.c_str(), t.c_str(), r1.c_str());
			vecRet.push_back(string(tmp));
		}
		if (!topStr.empty()) 
		{
			vecRet.push_back(topStr);
		}

		//	($splitB, $splitT, join("|", @ret))
		string strJoin = "";
		if(vecRet.size()>0)
		{
			//strJoin += "(";
			for(int i= 0; i< vecRet.size(); i++)
			{
				strJoin += vecRet[i];
				if(i< vecRet.size() -1)
					strJoin += "|";
			}
			//strJoin +=")";
		}
	
		return strJoin;

	}

	

	//# Include all chars between $bot and $top .
	string walkFull(vector<unsigned char> vecBottomUCS, vector<unsigned char> vecTopUCS)
	{

		string b = toHex(vecBottomUCS[0]);
		string t = toHex(vecTopUCS[0]);
		vecBottomUCS.erase(vecBottomUCS.begin());
		vecTopUCS.erase(vecTopUCS.begin());

		char tmp[32] = { 0 };
		sprintf_s(tmp, 32, "[%s-%s]", b.c_str(), t.c_str());
		string ret = b == t ? b : string(tmp);

		if (vecBottomUCS.size()>0) 
		{
			string r = walkFull(vecBottomUCS, vecTopUCS);
			return ret + "," + r;
		} 
		else 
		{
			return ret;
		}
	}


	string printTest()
	{
		//	F4 8D 90 AA   (\uE4, (\uB9, [\u80-\u9D]))
		size_t unicode_number;
		unsigned char tmp[16] = { 0 };
		tmp[0] = 0xd2;	tmp[1] = 0xbb;	//tmp[2] = 0x81;
		string tttttt= string((char *)tmp);
		string tttt2 = "一";
		const char * ttt3 = tttt2.c_str();
		Utf82Unicode((const char *)tmp, &unicode_number);

		//	bytes: [E3 83 8A-E3 84 85]

		// bytes: [E4 B8 81-E9 BE A5]   UCS:   ['\u4E01'-'\u9fA5 ]    UCS:   ['\u0071'-'\u9fA5    ]    chars: [    '丁'-    '龥' ]
		bool splitB, splitT;
		string kkk = compileUTF8range(L'一',  L'严');

		kkk = "(\xE4\xB8\xA5)";

		vector<string> firstMatch;
		vector<int>		firstOffset;
		string val = "你啊，严肃一二三了解具体";
		string val_utf8 = Gbk2Utf8(val);
		const char * ppp = val_utf8.c_str();
		if(Pcre2Grep(kkk, val_utf8.c_str(), firstMatch, firstOffset )>0)
		{
			string gbk = Utf8_GBK(firstMatch[0]);
			int t1 = 1;
			
		}

		return kkk;

		vector<unsigned char>	bottomUCS, topUCS,  botBytes, topBytes;

		botBytes.push_back(0xB8);	botBytes.push_back(0x81);	
		topBytes.push_back(0xE9);	topBytes.push_back(0xBE);	

		bottomUCS.push_back(0x80);	bottomUCS.push_back(0x80);	
		topUCS.push_back(0xBF);		topUCS.push_back(0xBF);	
		string ttt =  walk(bottomUCS, topUCS, botBytes, topUCS, splitB, splitT);

		return ttt;
		//return walkFull(rules[1].bottomUCS, rules[1].topUCS);
	}

}



