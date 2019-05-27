#ifndef _XMLUTILS_HPP
#define _XMLUTILS_HPP

#include "StandardInc.hpp"
 #include "XmlInc.hpp"
//#include "BoostInc.hpp"

#include <string>
#include <map>

using namespace std;


class  __declspec(dllexport) XmlUtils
{

  public:
  	
  	static xmlDocPtr DocFromFile(string filename);

    static string DocToString(xmlDocPtr doc);

    static string DocToString(xmlDocPtr doc,string encoding);

    static xmlDocPtr StringToDoc(string xmlstring);

    static map<string,string> DocToStringMap(xmlDocPtr doc,string xpath,
      string key,string value);

    static xmlDocPtr StringMapToDoc(map<string,string> tmap,string kname,string vname);

	static xmlDocPtr StringMapToDoc(map<string,string> tmap);

    static char * ConvertEnc(char *encFrom, char *encTo, const char *in);

    static xmlXPathObjectPtr QureyNode(xmlDocPtr doc, string xpathstr);

    static list<string>  QueryAttr(xmlDocPtr doc,string xpathstr,bool distinct);

    static string  QueryFirstAttr(xmlDocPtr doc,string xpathstr,bool distinct);

    static xmlDocPtr  MergeDoc(xmlDocPtr doc1,xmlDocPtr doc2);

    static xmlDocPtr RootAddNode(xmlDocPtr doc,xmlNodePtr node);

    static xmlDocPtr BuildNewDoc(string rootname);

    static xmlNodePtr SetNodeProp(xmlNodePtr data,const char* name,const char* value);

	static xmlChar* GetNodeProp(xmlNodePtr data,const char* name);

	static string BuildXmlData(string rootname,string dataname,map<string,string> dataattr) ;

	static xmlNodePtr StringMapToNode(map<string,string> attrMap,string nodeName);

	static bool  WriteFile(string filename, xmlDocPtr docPtr,const char *encoding);

  private:
    static void ThrowError(string str);
};

#endif //_XMLUTILS_HPP

