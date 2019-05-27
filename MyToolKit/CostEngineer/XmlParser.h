
#ifndef _XMLPARSER_HH
#define _XMLPARSER_HH

class XmlParser
{
public:
	XmlParser(LPVOID pContext);
	~XmlParser(void);

	bool Parse(char const *reqStr, unsigned reqStrSize);

	static void XMLCALL startElement1(void *userData, const char *name, const char **atts);
	static void XMLCALL endElement1(void *userData, const char *name);
	static void XMLCALL characterData1(void *userData, const XML_Char *s, int len);
	
	virtual void startElement(const char *name, const char **atts) = 0;
	virtual void endElement(const char *name) = 0;
	virtual void characterData(const XML_Char *s, int len) = 0;

protected:
	LPVOID m_pContext;

};


#endif