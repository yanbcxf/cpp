#ifndef _PARSEXML_HPP
#define _PARSEXML_HPP
#include <string>
#include <list>
#include <map>
using namespace std;


// #define REQ_ID "REQUEST_ID"
// #define SESS_ID "SESSION_ID"

string GetXMLFieldValue(string strXML,string strName,char *cXmlNode);

/**********************************************
*功  能：获取指定节点名的第一个节点
*参  数：strXML[in]--xml字符串，strName[in]--节点名
*返回值：true--成功，false--失败
**********************************************/
bool FindFirstNode(string strXML,string strName="data");

/**********************************************
*功  能：获取当前节点的下一个节点
*参  数：pCurNode[in]--当前节点
*返回值：true--成功，false--失败
**********************************************/
bool FindNextNode1();

/**********************************************
*功  能：获取节点的特定属性值
*参  数：pCurNode[in]--当前节点,strName[in]--节点属性名
*返回值：属性值
**********************************************/
string GetXMLNodePropValue(string strName);

/**********************************************
*功  能：获取所有节点的的所有属性值
*参  数：strXML[in]--当前节点,lstString[in]--节点属性列表
*返回值：所有属性值
**********************************************/
bool GetXMLAllNodePropValues( string strXML,list<string> &lstString,list<map<string,string > > &lstPropMap );

#endif //_PARSEXML_HPP

