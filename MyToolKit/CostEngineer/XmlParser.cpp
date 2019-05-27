#include "StdAfx.h"
#include "XmlParser.h"


#if defined(__amigaos__) && defined(__USE_INLINE__)
#include <proto/expat.h>
#endif

#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

void XMLCALL
XmlParser::startElement1(void *userData, const char *name, const char **atts)
{
	XmlParser *  pXmlParser = (XmlParser *)userData;
	if(pXmlParser)
		pXmlParser->startElement(name, atts);
}

void XMLCALL
XmlParser::endElement1(void *userData, const char *name)
{
	XmlParser *  pXmlParser = (XmlParser *)userData;
	if(pXmlParser)
		pXmlParser->endElement(name);
}

void XMLCALL 
XmlParser::characterData1(void *userData, const XML_Char *s, int len)
{
	XmlParser *  pXmlParser = (XmlParser *)userData;
	if(pXmlParser)
		pXmlParser->characterData(s, len);
}


XmlParser::XmlParser(LPVOID pContext)
{
	m_pContext = pContext;
}

XmlParser::~XmlParser(void)
{
}


bool XmlParser::Parse(char const *reqStr, unsigned reqStrSize)
{
	XML_Parser parser = XML_ParserCreate(NULL);
	if(parser)
	{
		XML_SetUserData(parser, this);
		XML_SetElementHandler(parser, XmlParser::startElement1, XmlParser::endElement1);
		XML_SetCharacterDataHandler(parser, XmlParser::characterData1);

		if (XML_Parse(parser, reqStr, reqStrSize, 1) == XML_STATUS_ERROR)
		{
			char tmp[1024];
			sprintf(tmp,
				"XmlParser::Parse, %s at line %" XML_FMT_INT_MOD "u column %" XML_FMT_INT_MOD "u\n",
				XML_ErrorString(XML_GetErrorCode(parser)),
				XML_GetCurrentLineNumber(parser),
				XML_GetCurrentColumnNumber(parser));
			// sendToOutput(tmp, NULL, WM_USER_LOG_1);	
			return false;
		}
		XML_ParserFree(parser);
	}
	return true;
}