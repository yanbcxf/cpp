#include "DBOperator.h"
#include "../InterfaceGlobal.h"
#include "../xml/UrlUtils.hpp"
#include "../xml/XmlUtils.hpp"

#define LEFT_BRACE '['
#define RIGHT_BRACE ']'


string GetNodeProp_Safe(xmlNodePtr tnode ,const char * name)
{
	string result = "";
	if(xmlHasProp(tnode, BAD_CAST name)!=NULL)
	{
		xmlChar* szAttr = xmlGetProp(tnode, BAD_CAST name);
		result =  (char *)szAttr;;
		xmlFree(szAttr);
	}
	return result;
}

int getUserID(string strDBAddr,int nPort,string strUserName,string &strUserID)
{
	char strTmp[10];
	sprintf(strTmp,"%i", nPort);
	string	url="http://"+ strDBAddr + ":" + strTmp +"/snmsDbs/serviceAction.do?";
	string	data="spe_config_file=spe/gbgateway/user.xml";
	data = data + "&user_name=" + strUserName;

	char *c1 = XmlUtils::ConvertEnc("gbk","utf-8", data.c_str());
	string	str=UrlUtils::PostUrl(url,c1);
	free(c1);
	c1 = NULL;
	

	if(str.find("<?xml version=\"1.0\" encoding=\"GBK\"?>")!=string::npos)
	{
		xmlDocPtr generalDoc = XmlUtils::StringToDoc(str);
		printf("Xml encoding : %s \r\n",generalDoc->encoding );


		xmlNode *  tnode =  generalDoc->children->children;
		while(tnode!=NULL){
			if(tnode->type ==XML_ELEMENT_NODE){
				//cout<< tnode->name << "  type = " << tnode->type << endl;
				strUserID =  GetNodeProp_Safe(tnode,  "pers_id");
				break;				
			}
			tnode = tnode->next;
		}
		xmlFreeDoc(generalDoc);

		return 0;
	}
	else
		return -1;
	
}

//	获得 (设备管理 3 ;  中心管理 1) 服务器
int getServer(string server_type_id,string strDBAddr,int nDBPort,string &strServerAddr,int &nServerPort)
{
	try{
		stringstream ss;
		string server_ip;
		int server_port;

		char strTmp[10];
		sprintf(strTmp,"%i", nDBPort);
		string	url="http://"+ strDBAddr + ":" + strTmp +"/snmsDbs/serviceAction.do?";

		string	data="spe_config_file=spe/gbgateway/server.xml";
		data = data + "&server_type_id=" + server_type_id;
		string	str=UrlUtils::PostUrl(url,(char*) data.c_str());

		if(str.find("<?xml version=\"1.0\" encoding=\"GBK\"?>")!=string::npos)
		{
			xmlDocPtr generalDoc = XmlUtils::StringToDoc(str);
			printf("Xml encoding : %s \r\n",generalDoc->encoding );


			xmlNode *  tnode =  generalDoc->children->children;
			while(tnode!=NULL){
				if(tnode->type ==XML_ELEMENT_NODE){
					//cout<< tnode->name << "  type = " << tnode->type << endl;
					server_ip =  GetNodeProp_Safe(tnode,  "server_ip");
					server_port = atoi((char*)GetNodeProp_Safe(tnode,  "server_port").c_str());

					break;				
				}
				tnode = tnode->next;
			}
			xmlFreeDoc(generalDoc);
		}

		vector<string> iplist;
		splitString(server_ip,"," , iplist);
		/*for(vector<string>::iterator it3 =iplist.begin(); it3!=iplist.end(); it3++)
		{*/
		vector<string>::iterator iter;
		for(iter=iplist.begin();iter!=iplist.end();iter++)
		{
			if( MultiSocketTest((*iter).c_str(), server_port)>=0)
			{
				strServerAddr = (*iter);
				nServerPort = server_port;
				
				return 0;
			}
		}

		ss.str("");
		ss<<"【"<<__FUNCTION__<<"】服务器无法连接_"<<server_type_id;
		LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
		return -1;

	}catch(...){
		//LOG4CPLUS_INFO(logger, "main() 程序发送异常\r\n" );
		return -1;
	}
}

int getParentDevice(string strDBAddr,int nPort,map<string,SN_DEVICE> &SnDeviceLst,string strUserName,string strParentDevID)
{
	stringstream ss("");
	{
		try{
			int nTotalDevice = 0;
			bool bFind = false;
			//	查询设备信息
			char strTmp[10];
			sprintf(strTmp,"%i", nPort);
			string	url="http://"+ strDBAddr + ":" + strTmp +"/snmsDbs/serviceAction.do?";

			string	data="spe_config_file=spe/gbgateway/device_parent.xml";
			data = data + "&dev_parent=" + strParentDevID;
			string	str=UrlUtils::PostUrl(url,(char*) data.c_str());

			if(str.find("<?xml version=\"1.0\" encoding=\"GBK\"?>")!=string::npos)
			{
				bFind = true;
				xmlDocPtr generalDoc = XmlUtils::StringToDoc(str);
				//printf("Xml encoding : %s \r\n",generalDoc->encoding );

				SN_DEVICE channel;
				xmlNode *  tnode =  generalDoc->children->children;
				while(tnode!=NULL){
					if(tnode->type ==XML_ELEMENT_NODE){
						//cout<< tnode->name << "  type = " << tnode->type << endl;
						channel.strDevName = GetNodeProp_Safe(tnode,  "dev_name");
						channel.strDevIP =  GetNodeProp_Safe(tnode,  "dev_ip");
						channel.strDevUser = GetNodeProp_Safe(tnode,  "dev_username");
						channel.strDevPass = GetNodeProp_Safe(tnode,  "dev_password");
						channel.nDevPort = atoi((char*)GetNodeProp_Safe(tnode,  "dev_port").c_str());
						channel.nVideoPort = atoi((char*)GetNodeProp_Safe(tnode,  "video_port").c_str());
						channel.strDevFirm = GetNodeProp_Safe(tnode,  "dev_firm");
						channel.nDevTypeChild = atoi((char*)GetNodeProp_Safe(tnode,  "dev_type_child").c_str());
						channel.strDevSN = GetNodeProp_Safe(tnode,  "dev_sn");
						channel.strConnectType = GetNodeProp_Safe(tnode,  "connect_type");
						channel.strParentDevId = GetNodeProp_Safe(tnode,  "parent_dev_id");
						channel.nDevTypeId = atoi((char*)GetNodeProp_Safe(tnode,  "dev_type_id").c_str());
						channel.nMobilePort = atoi((char*)GetNodeProp_Safe(tnode,  "mobile_port").c_str());
						channel.nWebPort = atoi((char*)GetNodeProp_Safe(tnode,  "web_port").c_str());
						channel.nDevState = atoi((char*)GetNodeProp_Safe(tnode,  "dev_state").c_str());
						channel.strInstallTime = GetNodeProp_Safe(tnode,  "dev_install_date");
						channel.strPosID = GetNodeProp_Safe(tnode,  "posi_id");
						channel.nDevFirm = atoi((char*)GetNodeProp_Safe(tnode,  "dev_firm").c_str());

						channel.nChannelCnt = atoi((char*)GetNodeProp_Safe(tnode,  "channel_cnt").c_str());
						channel.nAlarmInIOCnt = atoi((char*)GetNodeProp_Safe(tnode,  "io_in_cnt").c_str());
						channel.nAlarmOutCnt = atoi((char*)GetNodeProp_Safe(tnode,  "io_out_cnt").c_str());

						channel.strGBID = GetNodeProp_Safe(tnode,  "gb_id");
						channel.strDevId = GetNodeProp_Safe(tnode,  "dev_id");
						channel.nChannelNO = atoi((char*)GetNodeProp_Safe(tnode,  "dev_no").c_str());		//	通道序号
						channel.nMode = atoi((char*)GetNodeProp_Safe(tnode,  "mode").c_str());		//	协议

						if(channel.nDevPort==0)
							channel.nDevPort = channel.nWebPort;

						//channel.chanNo =  atoi(dev_no.c_str());
						try{
							char * dataptr = XmlUtils::ConvertEnc("utf-8","gbk", (char * )channel.strDevName.c_str());
							channel.strDevName = string((char * )dataptr);
							free(dataptr);
							dataptr = NULL;
						}
						catch(...)
						{
							ss.str("");
							ss<<"【"<<__FUNCTION__<<"】初始读入 Sn_device , ConvertEnc 异常,  ";
							ss <<"channel.devName =" << channel.strDevName;
							LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
						}
						SnDeviceLst.insert(make_pair(channel.strGBID,channel));



					}
					tnode = tnode->next;
				}
				xmlFreeDoc(generalDoc);
			}

			if(nTotalDevice>0)
			{
				ss.str("");
				ss<<"【"<<__FUNCTION__<<"】初始读入 Sn_device 表记录数  ";
				ss << nTotalDevice;
				LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
			}
			if(!bFind)
				return -1;

		}catch(std::exception &e){
			ss.str("");
			ss<<"【"<<__FUNCTION__<<"】初始读入 Sn_device Exception :  ";
			ss << e.what();
			LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
			return -1;

		}catch(...){
			//LOG4CPLUS_INFO(logger, "main() 程序发送异常\r\n" );
			ss.str("");
			ss<<"【"<<__FUNCTION__<<"】初始读入 Sn_device 异常  ";
			LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
			return -1;
		}
	}

	return 0;
}

int getAllGBSnDevice(string strDBAddr,int nPort,map<string,SN_DEVICE> &SnDeviceLst,string strUserName)
{
	stringstream ss("");
	{
		try{
			int nTotalDevice = 0;
			bool bFind = false;
			//	查询设备信息
			char strTmp[10];
			sprintf(strTmp,"%i", nPort);
			string	url="http://"+ strDBAddr + ":" + strTmp +"/snmsDbs/serviceAction.do?";

			string	data="spe_config_file=spe/gbgateway/device.xml";
			data = data + "&user_name=" + strUserName;
			string	str=UrlUtils::PostUrl(url,(char*) data.c_str());

			if(str.find("<?xml version=\"1.0\" encoding=\"GBK\"?>")!=string::npos)
			{
				bFind = true;
				xmlDocPtr generalDoc = XmlUtils::StringToDoc(str);
				//printf("Xml encoding : %s \r\n",generalDoc->encoding );

				SN_DEVICE channel;
				xmlNode *  tnode =  generalDoc->children->children;
				while(tnode!=NULL){
					if(tnode->type ==XML_ELEMENT_NODE){
						//cout<< tnode->name << "  type = " << tnode->type << endl;
						channel.strDevName = GetNodeProp_Safe(tnode,  "dev_name");
						channel.strDevIP =  GetNodeProp_Safe(tnode,  "dev_ip");
						channel.strDevUser = GetNodeProp_Safe(tnode,  "dev_username");
						channel.strDevPass = GetNodeProp_Safe(tnode,  "dev_password");
						channel.nDevPort = atoi((char*)GetNodeProp_Safe(tnode,  "dev_port").c_str());
						channel.nVideoPort = atoi((char*)GetNodeProp_Safe(tnode,  "video_port").c_str());
						channel.strDevFirm = GetNodeProp_Safe(tnode,  "dev_firm");
						channel.nDevTypeChild = atoi((char*)GetNodeProp_Safe(tnode,  "dev_type_child").c_str());
						channel.strDevSN = GetNodeProp_Safe(tnode,  "dev_sn");
						channel.strConnectType = GetNodeProp_Safe(tnode,  "connect_type");
						channel.strParentDevId = GetNodeProp_Safe(tnode,  "parent_dev_id");
						channel.nDevTypeId = atoi((char*)GetNodeProp_Safe(tnode,  "dev_type_id").c_str());
						channel.nMobilePort = atoi((char*)GetNodeProp_Safe(tnode,  "mobile_port").c_str());
						channel.nWebPort = atoi((char*)GetNodeProp_Safe(tnode,  "web_port").c_str());
						channel.nDevState = atoi((char*)GetNodeProp_Safe(tnode,  "dev_state").c_str());
						channel.strInstallTime = GetNodeProp_Safe(tnode,  "dev_install_date");
						channel.strPosID = GetNodeProp_Safe(tnode,  "posi_id");
						channel.nDevFirm = atoi((char*)GetNodeProp_Safe(tnode,  "dev_firm").c_str());

						channel.nChannelCnt = atoi((char*)GetNodeProp_Safe(tnode,  "channel_cnt").c_str());
						channel.nAlarmInIOCnt = atoi((char*)GetNodeProp_Safe(tnode,  "io_in_cnt").c_str());
						channel.nAlarmOutCnt = atoi((char*)GetNodeProp_Safe(tnode,  "io_out_cnt").c_str());

						channel.strGBID = GetNodeProp_Safe(tnode,  "gb_id");
						channel.strDevId = GetNodeProp_Safe(tnode,  "dev_id");
						channel.nChannelNO = atoi((char*)GetNodeProp_Safe(tnode,  "dev_no").c_str());		//	通道序号
						channel.nMode = atoi((char*)GetNodeProp_Safe(tnode,  "mode").c_str());		//	协议

						if(channel.nDevPort==0)
							channel.nDevPort = channel.nWebPort;

						//channel.chanNo =  atoi(dev_no.c_str());
						try{
							char * dataptr = XmlUtils::ConvertEnc("utf-8","gbk", (char * )channel.strDevName.c_str());
							channel.strDevName = string((char * )dataptr);
							free(dataptr);
							dataptr = NULL;
						}
						catch(...)
						{
							ss.str("");
							ss<<"【"<<__FUNCTION__<<"】初始读入 Sn_device , ConvertEnc 异常,  ";
							ss <<"channel.devName =" << channel.strDevName;
							LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
						}
						SnDeviceLst.insert(make_pair(channel.strGBID,channel));
						
						

					}
					tnode = tnode->next;
				}
				xmlFreeDoc(generalDoc);
			}
			
			map<string,SN_DEVICE>::iterator iter1;
			map<string,SN_DEVICE>::iterator iter2;
			string strLostParentID="";
			for(iter1=SnDeviceLst.begin();iter1!=SnDeviceLst.end();iter1++)
			{
				bool IsParent = false;
				if(iter1->second.nDevTypeId==5||iter1->second.nDevTypeId==6)
				{
					for(iter2=SnDeviceLst.begin();iter2!=SnDeviceLst.end();iter2++)
					{
						if(iter1->second.strParentDevId == iter2->second.strDevId)
						{	
							iter1->second.strDevIP = iter2->second.strDevIP;
							iter1->second.strDevUser = iter2->second.strDevUser;
							iter1->second.strDevPass = iter2->second.strDevPass;
							iter1->second.strDevSN = iter2->second.strDevSN;
							iter1->second.nDevFirm = iter2->second.nDevFirm;
							iter1->second.nDevTypeChild = iter2->second.nDevTypeChild;
							iter1->second.nWebPort = iter2->second.nWebPort;
							iter1->second.nDevPort = iter2->second.nDevPort;
							iter1->second.nVideoPort = iter2->second.nVideoPort;
							iter1->second.nDevState = iter2->second.nDevState;
							iter1->second.nMode = iter2->second.nMode;
							IsParent=true;
							break;
						}
					}
					if(!IsParent)
					{
						if(strLostParentID.length()==0)
							strLostParentID="'"+iter1->second.strParentDevId+"'";
						else
							strLostParentID=strLostParentID+","+"'"+iter1->second.strParentDevId+"'";
					}
				}
			}
			
			if(strLostParentID.length()>0)
			{
				getParentDevice(strDBAddr,nPort,SnDeviceLst,strUserName,strLostParentID);
				for(iter1=SnDeviceLst.begin();iter1!=SnDeviceLst.end();iter1++)
				{
					if(iter1->second.nDevTypeId==5||iter1->second.nDevTypeId==6)
					{
						for(iter2=SnDeviceLst.begin();iter2!=SnDeviceLst.end();iter2++)
						{
							if(iter1->second.strParentDevId == iter2->second.strDevId)
							{	
								iter1->second.strDevIP = iter2->second.strDevIP;
								iter1->second.strDevUser = iter2->second.strDevUser;
								iter1->second.strDevPass = iter2->second.strDevPass;
								iter1->second.strDevSN = iter2->second.strDevSN;
								iter1->second.nDevFirm = iter2->second.nDevFirm;
								iter1->second.nDevTypeChild = iter2->second.nDevTypeChild;
								iter1->second.nWebPort = iter2->second.nWebPort;
								iter1->second.nDevPort = iter2->second.nDevPort;
								iter1->second.nVideoPort = iter2->second.nVideoPort;
								iter1->second.nDevState = iter2->second.nDevState;
								iter1->second.nMode = iter2->second.nMode;
								break;
							}
						}
					}
				}
			}

			if(nTotalDevice>0)
			{
				ss.str("");
				ss<<"【"<<__FUNCTION__<<"】初始读入 Sn_device 表记录数  ";
				ss << nTotalDevice;
				LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
			}
			if(!bFind)
				return -1;

		}catch(std::exception &e){
			ss.str("");
			ss<<"【"<<__FUNCTION__<<"】初始读入 Sn_device Exception :  ";
			ss << e.what();
			LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
			return -1;

		}catch(...){
			//LOG4CPLUS_INFO(logger, "main() 程序发送异常\r\n" );
			ss.str("");
			ss<<"【"<<__FUNCTION__<<"】初始读入 Sn_device 异常  ";
			LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
			return -1;
		}
	}

	return 0;
}


int UpdateSnDeviceByID(string strDBAddr,int nPort,SN_DEVICE &SnDevice)
{
	stringstream ss;
	try{
		//	查询设备信息
		char strTmp[10];
		sprintf(strTmp,"%i",nPort);
		string	url="http://"+ strDBAddr + ":" + strTmp +"/snmsDbs/serviceAction.do?";

		string	data="spe_config_file=spe/gbgateway/device_by_id.xml";
		data = data + "&dev_id=" + SnDevice.strDevId;
		string	str=UrlUtils::PostUrl(url,(char*) data.c_str());
		bool bFind = false;

		if(str.find("<?xml version=\"1.0\" encoding=\"GBK\"?>")!=string::npos)
		{
			bFind = true;
			xmlDocPtr generalDoc = XmlUtils::StringToDoc(str);
			//printf("Xml encoding : %s \r\n",generalDoc->encoding );

			xmlNode *  tnode =  generalDoc->children->children;
			while(tnode!=NULL){
				if(tnode->type ==XML_ELEMENT_NODE){
					//cout<< tnode->name << "  type = " << tnode->type << endl;
					SnDevice.strDevName = GetNodeProp_Safe(tnode,  "dev_name");
					SnDevice.strDevIP =  GetNodeProp_Safe(tnode,  "dev_ip");
					SnDevice.strDevUser = GetNodeProp_Safe(tnode,  "dev_username");
					SnDevice.strDevPass = GetNodeProp_Safe(tnode,  "dev_password");
					SnDevice.nDevPort = atoi((char*)GetNodeProp_Safe(tnode,  "dev_port").c_str());
					SnDevice.nVideoPort = atoi((char*)GetNodeProp_Safe(tnode,  "video_port").c_str());
					SnDevice.strDevFirm = GetNodeProp_Safe(tnode,  "dev_firm");
					SnDevice.nDevTypeChild = atoi((char*)GetNodeProp_Safe(tnode,  "dev_type_child").c_str());
					SnDevice.strDevSN = GetNodeProp_Safe(tnode,  "dev_sn");
					SnDevice.strConnectType = GetNodeProp_Safe(tnode,  "connect_type");
					SnDevice.strParentDevId = GetNodeProp_Safe(tnode,  "parent_dev_id");
					SnDevice.nDevTypeId = atoi((char*)GetNodeProp_Safe(tnode,  "dev_type_id").c_str());
					SnDevice.nMobilePort = atoi((char*)GetNodeProp_Safe(tnode,  "mobile_port").c_str());
					SnDevice.nWebPort = atoi((char*)GetNodeProp_Safe(tnode,  "web_port").c_str());
					SnDevice.nDevState = atoi((char*)GetNodeProp_Safe(tnode,  "dev_state").c_str());
					SnDevice.strInstallTime = GetNodeProp_Safe(tnode,  "dev_install_date");
					SnDevice.strPosID = GetNodeProp_Safe(tnode,  "posi_id");
					SnDevice.nDevFirm = atoi((char*)GetNodeProp_Safe(tnode,  "dev_firm").c_str());

					SnDevice.nChannelCnt = atoi((char*)GetNodeProp_Safe(tnode,  "channel_cnt").c_str());
					SnDevice.nAlarmInIOCnt = atoi((char*)GetNodeProp_Safe(tnode,  "io_in_cnt").c_str());
					SnDevice.nAlarmOutCnt = atoi((char*)GetNodeProp_Safe(tnode,  "io_out_cnt").c_str());

					SnDevice.strGBID = GetNodeProp_Safe(tnode,  "gb_id");
					SnDevice.strDevId = GetNodeProp_Safe(tnode,  "dev_id");
					SnDevice.nChannelNO = atoi((char*)GetNodeProp_Safe(tnode,  "dev_no").c_str());		//	通道序号

					if(SnDevice.nDevPort==0)
						SnDevice.nDevPort = SnDevice.nWebPort;
					//channel.chanNo =  atoi(dev_no.c_str());

					try{
						char * dataptr = XmlUtils::ConvertEnc("utf-8","gbk", (char * )SnDevice.strDevName.c_str());
						SnDevice.strDevName = string((char * )dataptr);
					}
					catch(...)
					{
						ss.str("");
						ss<<"【"<<__FUNCTION__<<"】 , ConvertEnc 异常,  ";
						ss <<"channel.devName =" << SnDevice.strDevName;
						LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
					}

				
					break;
				}
				tnode = tnode->next;
			}
			xmlFreeDoc(generalDoc);
		}
		if(!bFind)
			return -1;
	}catch(...){
		//LOG4CPLUS_INFO(logger, "main() 程序发送异常\r\n" );
		return -1;
	}
	return 0;
}

int GetSnPosition(string strDBAddr,int nPort,map<string,SN_Position> &SnPositionLst)
{
	stringstream ss("");

	{
		try{
			int nTotalDevice = 0;
			bool bFind = false;
			//	查询位置信息
			char strTmp[10];
			sprintf(strTmp,"%i", nPort);
			string	url="http://"+ strDBAddr + ":" + strTmp +"/snmsDbs/serviceAction.do?";

			string	data="spe_config_file=spe/gbgateway/postion.xml";
			string	str=UrlUtils::PostUrl(url,(char*) data.c_str());

			if(str.find("<?xml version=\"1.0\" encoding=\"GBK\"?>")!=string::npos)
			{
				bFind = true;
				xmlDocPtr generalDoc = XmlUtils::StringToDoc(str);
				//printf("Xml encoding : %s \r\n",generalDoc->encoding );

				SN_Position position;
				xmlNode *  tnode =  generalDoc->children->children;
				while(tnode!=NULL){
					if(tnode->type ==XML_ELEMENT_NODE){
						//cout<< tnode->name << "  type = " << tnode->type << endl;
						position.strPosID = GetNodeProp_Safe(tnode,  "posi_id");
						position.strParentPosID =  GetNodeProp_Safe(tnode,  "parent_posi_id");
						position.strPosName = GetNodeProp_Safe(tnode,  "posi_name");
						position.strPosAddr = GetNodeProp_Safe(tnode,  "posi_addr");
						position.nPosLevel = atoi((char*)GetNodeProp_Safe(tnode,  "posi_level").c_str());
						position.strDoMail = atoi((char*)GetNodeProp_Safe(tnode,  "domain").c_str());
						
						//channel.chanNo =  atoi(dev_no.c_str());
						char * dataptr = NULL;
						try{
							dataptr = XmlUtils::ConvertEnc("utf-8","gbk", (char * )position.strPosName.c_str());
							if(dataptr!=NULL)
							{
								position.strPosName = string((char * )dataptr);
								free(dataptr);
								dataptr = NULL;
							}
							

							dataptr = XmlUtils::ConvertEnc("utf-8","gbk", (char * )position.strPosAddr.c_str());
							if(dataptr!=NULL)
							{
								position.strPosAddr = string((char * )dataptr);
								free(dataptr);
								dataptr = NULL;
							}
						}
						catch(...)
						{
							if(dataptr!=NULL)
								free(dataptr);
							ss.str("");
							ss<<"【"<<__FUNCTION__<<"】初始读入 Sn_Position , ConvertEnc 异常,  ";
							LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
						}
						position.strAbsoluteAddr = "";
						position.strAbsoluteAddr+=position.strPosName;
						SnPositionLst.insert(make_pair(position.strPosID,position));

					}
					tnode = tnode->next;
				}
				xmlFreeDoc(generalDoc);
			}
			
			
			if(SnPositionLst.size()>1)
			{
				string strKey="";
				map<string,SN_Position>::iterator iter,iter1;
				map<string,string>::iterator str_iter;
				for(iter=SnPositionLst.begin();iter!=SnPositionLst.end();iter++)
				{
					if(iter->second.strParentPosID=="0")
					{
						//iter->second.strAbsoluteAddr+=iter->second.strPosName;
						continue;
					}
					
					strKey = iter->second.strParentPosID;
					while(1)
					{
						iter1 = SnPositionLst.find(strKey);
						if(iter1!=SnPositionLst.end())
						{
							iter->second.strAbsoluteAddr+=",";
							iter->second.strAbsoluteAddr+=iter1->second.strPosName;
							strKey = iter1->second.strParentPosID;
						}
						else
							break;

					}
				}
			}

			if(nTotalDevice>0)
			{
				ss.str("");
				ss<<"【"<<__FUNCTION__<<"】初始读入 Sn_Position 表记录数  ";
				ss << nTotalDevice;
				LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
			}
			if(!bFind)
				return -1;
			
		}catch(std::exception &e){
			ss.str("");
			ss<<"【"<<__FUNCTION__<<"】初始读入 Sn_Position Exception :  ";
			ss << e.what();
			LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
			return -1;

		}catch(...){
			//LOG4CPLUS_INFO(logger, "main() 程序发送异常\r\n" );
			ss.str("");
			ss<<"【"<<__FUNCTION__<<"】初始读入 Sn_Position 异常  ";
			LOG4CPLUS_DEBUG(g_InterfaceLogger,ss.str().c_str());
			return -1;
		}
	}

	return 0;
}

static int newline(char c)
{
	return ('\n' == c ||  '\r' == c )? 1 : 0;
}
static int end_of_string(char c)
{
	return '\0'==c? 1 : 0;
}
static int left_barce(char c)
{
	return LEFT_BRACE == c? 1 : 0;
}
static int isright_brace(char c )
{
	return RIGHT_BRACE == c? 1 : 0;
}

static int parse_file(const char *section, const char *key, const char *buf,int *sec_s,int *sec_e,
					  int *key_s,int *key_e, int *value_s, int *value_e)
{
	const char *p = buf;
	int i=0;

	assert(buf!=NULL);
	assert(section != NULL && strlen(section));
	assert(key != NULL && strlen(key));

	*sec_e = *sec_s = *key_e = *key_s = *value_s = *value_e = -1;

	while( !end_of_string(p[i]) ) {
		//find the section
		if( ( 0==i ||  newline(p[i-1]) ) && left_barce(p[i]) )
		{
			int section_start=i+1;

			//find the ']'
			do {
				i++;
			} while( !isright_brace(p[i]) && !end_of_string(p[i]));

			if( 0 == strncmp(p+section_start,section, i-section_start)) {
				int newline_start=0;

				i++;

				//Skip over space char after ']'
				while(isspace(p[i])) {
					i++;
				}

				//find the section
				*sec_s = section_start;
				*sec_e = i;

				while( ! (newline(p[i-1]) && left_barce(p[i])) 
					&& !end_of_string(p[i]) ) {
						int j=0;
						//get a new line
						newline_start = i;

						while( !newline(p[i]) &&  !end_of_string(p[i]) ) {
							i++;
						}

						//now i  is equal to end of the line
						j = newline_start;

						if(';' != p[j]) //skip over comment
						{
							while(j < i && p[j]!='=') {
								j++;
								if('=' == p[j]) {
									if(strncmp(key,p+newline_start,j-newline_start)==0)
									{
										//find the key ok
										*key_s = newline_start;
										*key_e = j-1;

										*value_s = j+1;
										*value_e = i;

										return 1;
									}
								}
							}
						}

						i++;
				}
			}
		}
		else
		{
			i++;
		}
	}
	return 0;
}

static int load_ini_file(const char *file, char *buf,int *file_size)
{
	FILE *in = NULL;
	int i=0;
	*file_size =0;

	assert(file !=NULL);
	assert(buf !=NULL);

	in = fopen(file,"r");
	if( NULL == in) {
		return 0;
	}

	buf[i]=fgetc(in);

	//load initialization file
	while( buf[i]!= (char)EOF) {
		i++;
		assert( i < MAX_FILE_SIZE ); //file too big, you can redefine MAX_FILE_SIZE to fit the big file 
		buf[i]=fgetc(in);
	}

	buf[i]='\0';
	*file_size = i;

	fclose(in);
	return 1;
}


int read_profile_string( const char *section, const char *key,char *value, 
						int size, const char *default_value, const char *file)
{
	char buf[MAX_PATH]={0};
	int file_size;
	int sec_s,sec_e,key_s,key_e, value_s, value_e;

	//check parameters
	assert(section != NULL && strlen(section));
	assert(key != NULL && strlen(key));
	assert(value != NULL);
	assert(size > 0);
	assert(file !=NULL &&strlen(key));

	if(!load_ini_file(file,buf,&file_size))
	{
		if(default_value!=NULL)
		{
			strncpy(value,default_value, size);
		}
		return 0;
	}

	if(!parse_file(section,key,buf,&sec_s,&sec_e,&key_s,&key_e,&value_s,&value_e))
	{
		if(default_value!=NULL)
		{
			strncpy(value,default_value, size);
		}
		return 0; //not find the key
	}
	else
	{
		int cpcount = value_e -value_s;

		if( size-1 < cpcount)
		{
			cpcount =  size-1;
		}

		memset(value, 0, size);
		memcpy(value,buf+value_s, cpcount );
		value[cpcount] = '\0';

		return 1;
	}
}

int ParaseAlarmReport(string strDBAddr,int nPort,char *cAlarmReport,list<AlarmInfo> &alarmLst)
{
	try{
		
		//if(strstr(cAlarmReport,"<?xml version=\"1.0\" encoding=\"GBK\"?>")!=NULL)
		{
			xmlDocPtr generalDoc = XmlUtils::StringToDoc(cAlarmReport);
			//printf("Xml encoding : %s \r\n",generalDoc->encoding );

			AlarmInfo alInfo;
			xmlNode *  tnode =  generalDoc->children->children;
			while(tnode!=NULL){
				if(strstr((char *)tnode->name,"data")!=NULL){
					//cout<< tnode->name << "  type = " << tnode->type << endl;
					alInfo.strAlarmID = GetNodeProp_Safe(tnode,  "id");
					alInfo.nAlarmLevel = atoi((char*)GetNodeProp_Safe(tnode,  "alarmLevel").c_str());
					alarmLst.push_back(alInfo);
				}
				tnode = tnode->next;
			}
			xmlFreeDoc(generalDoc);
		}

		list<AlarmInfo>::iterator iter;
		for(iter=alarmLst.begin();iter!=alarmLst.end();iter++)
		{
			char strTmp[10];
			sprintf(strTmp,"%i",nPort);
			string	url="http://"+ strDBAddr + ":" + strTmp +"/snmsDbs/serviceAction.do?";

			string	data="spe_config_file=spe/gbgateway/alarm.xml";
			data = data + "&alarm_id=" + iter->strAlarmID;
			string	str=UrlUtils::PostUrl(url,(char*) data.c_str());
			bool bFind = false;
			if(str.find("<?xml version=\"1.0\" encoding=\"GBK\"?>")!=string::npos)
			{
				xmlDocPtr generalDoc = XmlUtils::StringToDoc(str);
				xmlNode *  tnode =  generalDoc->children->children;
				while(tnode!=NULL){
					if(tnode->type ==XML_ELEMENT_NODE){
						//cout<< tnode->name << "  type = " << tnode->type << endl;
						iter->nAlarmType = atoi((char*)GetNodeProp_Safe(tnode,  "alarmtype").c_str());
						iter->strParentDevID = GetNodeProp_Safe(tnode,  "parent_dev_id");
						iter->strDevID = GetNodeProp_Safe(tnode,  "son_dev_id");
						iter->strAlarmTime = GetNodeProp_Safe(tnode,  "alarm_time");
						iter->nAlarmLevel = atoi((char*)GetNodeProp_Safe(tnode,  "alarm_level_id").c_str());
						
						string::size_type pos;
						if((pos=iter->strAlarmTime.find(" "))!=string::npos)
							iter->strAlarmTime.replace(pos,1,"T");
						
						pos = iter->strAlarmTime.find_last_of(".");
						if(pos!=string::npos)
							iter->strAlarmTime = iter->strAlarmTime.substr(0,pos);

					}
					tnode = tnode->next;
				}
				xmlFreeDoc(generalDoc);
			}
		}
	}
	catch(...)
	{

	}
	return 0;
}

int GetDevTypeName(int nDevType,char *cDevName)
{
	switch(nDevType)
	{
	case 1:
		strcpy(cDevName,"DVR");
		break;

	case 2:
		strcpy(cDevName,"IPC");
		break;

	case 3:
		strcpy(cDevName,"NVR");
		break;

	case 204:
		strcpy(cDevName,"ALEPH7480E");
		break;

	case 207:
		strcpy(cDevName,"PC");
		break;

	case 212:
		strcpy(cDevName,"AW-BM1600");
		break;

	case 213:
		strcpy(cDevName,"ST");
		break;

	case 214:
		strcpy(cDevName,"SJ");
		break;

	case 215:
		strcpy(cDevName,"PowerMax Plus");
		break;

	case 216:
		strcpy(cDevName,"DT2000");
		break;

	case 217:
		strcpy(cDevName,"HT-N2316");
		break;

	case 205:
		strcpy(cDevName,"IP");
		break;

	case 208:
		strcpy(cDevName,"DS");
		break;

	default:
		strcpy(cDevName,"Unknow");
		break;
	}
	return 0;
}