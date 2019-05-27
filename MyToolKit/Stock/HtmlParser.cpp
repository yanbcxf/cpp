#include "StdAfx.h"
#include "HtmlParser.h"

CHtmlParser::CHtmlParser(tree<HTML::Node> &dom)
{
	m_dom = dom;
	m_time_begin = 0;
}

CHtmlParser::~CHtmlParser(void)
{

}


void CHtmlParser::Parse(tree<HTML::Node>::iterator_base  & it, string xpathOfParent, map<string,string> attrsOfParent)
{
	if(m_time_begin==0)
		m_time_begin = time(NULL);
	else
	{
		/*为了防止CPU 过热，每运行一段时间，休息一下*/
		time_t nowTime = time(NULL);
		if( (nowTime- m_time_begin)>=1)
		{
			m_time_begin = nowTime;
			Sleep(20);
		}
	}

	//	采用“递归”方式，遍历所有的节点
	int nChild = m_dom.number_of_children(it);
	if(nChild==0) return ;

	tree<HTML::Node>::sibling_iterator child_it = m_dom.begin(it);
	tree<HTML::Node>::sibling_iterator child_end = m_dom.end(it);
	int index = 0;
	for( ; child_it!=child_end; child_it++)
	{
		stringstream ss;
		if(child_it->isTag()==false)
		{
			string val = child_it->text() + "" + child_it->closingText();
			trimString(val);
			if(val.empty()==false)
				HtmlNodeCallBack(xpathOfParent, val, attrsOfParent);
		}
		else
		{
			// 是 Html 标签才需要向下递归遍历
			string tagName = child_it->tagName();
			ss << xpathOfParent << "/" << tagName << "[" << index << "]";
			if(tagName=="a")
			{
				child_it->parseAttributes();
				Parse(child_it, ss.str(), child_it->attributes());
			}
			else
			{
				Parse(child_it, ss.str());
			}
			index ++ ;
		}		
	}
}