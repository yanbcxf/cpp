#pragma once

class CHtmlParser
{
public:
	CHtmlParser(tree<HTML::Node> &dom);
	virtual ~CHtmlParser(void);

	void Parse(tree<HTML::Node>::iterator_base & it, string xpathOfParent, map<string,string> attrsOfParent = map<string,string>());

	virtual void HtmlNodeCallBack(string xpath, string val, map<string,string> attrsOfParent) = 0;

protected:
	LPVOID m_pContext;
	tree<HTML::Node>  m_dom;

	time_t	m_time_begin;
};
