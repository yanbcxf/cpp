

namespace compileUTF8range
{
	class CUtf8Rule;

	void init();

	string compileUTF8range(unsigned short bot, unsigned short top);

	string expandRange(int ruleNo, unsigned short bot, unsigned short top );
		

	string walk(vector<unsigned char> bRules,vector<unsigned char> tRules,
		vector<unsigned char> botBytes,vector<unsigned char> topBytes, bool & splitB, bool & splitT);

	//# Include all chars between $bot and $top .
	string walkFull(vector<unsigned char> vecBottomUCS, vector<unsigned char> vecTopUCS);
	
	string printTest();

}



