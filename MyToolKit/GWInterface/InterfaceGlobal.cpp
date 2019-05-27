#include "InterfaceGlobal.h"


Logger g_InterfaceLogger=Logger::getInstance( LOG4CPLUS_TEXT("GWInterface") );


//********************************************************************
// function: 解析字符串，如果遇到空格，则填充填充符
// parameter:src[in]-源字符串及待解析字符串
//           reg[in]-分隔符
//           strFil[in]-填充符，默认为"0"
// return:保存得到字符串
//********************************************************************
list<string> splitStr(string src,string reg,string strFil="0")
{
	list<string> listStr;
	int nPos=0,nLen=0,nBegin=0;
	string strSub;
	nBegin = 0;
	while(nPos!=-1)
	{	
		nPos = src.find(reg,nBegin);
		nLen = nPos!=-1 ? nPos-nBegin : src.length()-nBegin;
		strSub.clear();
		strSub = src.substr(nBegin,nLen);
		if (strSub.length() != 0)//找到子字符串
		{
			listStr.push_back(strSub);
		}	 
		else//没找到字符串
		{
			// 			if (nPos != -1)//两个reg之间没有字符串
			// 			{
			listStr.push_back(strFil);
			//			}
		}
		nBegin = nPos+1;
	}
	return listStr;
}


//读配置文件
bool ReadCfg()
{


	return true;
}

void splitString(string s, string dim, vector<string>& resultVector)
{
	string::size_type  position=0; 
	string::size_type  lastPosition=0;  
	while((position=s.find_first_of(dim, position))!=string::npos)  
	{  
		//position=s.find_first_of(flag,position);   
		string elem = s.substr(lastPosition,position-lastPosition);
		resultVector.push_back(elem);
		position++;
		lastPosition = position;
	}  
	string elem = s.substr(lastPosition);
	resultVector.push_back(elem);

}

//	ip 中包含一台机器上的多个 IP 地址，其中一个连接成功立即返回, 用来测试可使用的路由
int MultiSocketTest(string ip, int port)
{
	SOCKET sk;
	SOCKADDR_IN saddr;
	fd_set r;
	//设置超时时间
	unsigned long ul = 1;
	struct timeval timeo = {1, 0};

	sk=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//创建套接口
	if(sk==INVALID_SOCKET)
	{
		printf("Create scoket fail:%d\n",GetLastError());
		return -1;
	}
	saddr.sin_family=AF_INET;
	saddr.sin_port=htons(port);
	saddr.sin_addr.S_un.S_addr=inet_addr(ip.c_str());

	//连接超时判断，否则约秒才能返回
	//设置非阻塞
	if(ioctlsocket(sk, FIONBIO, (unsigned long*)&ul) == SOCKET_ERROR)
	{
		printf("ioctlsocket fail");
		closesocket(sk);
		return -1;
	}
	//发起连接
	if(connect(sk, (struct sockaddr*)&saddr, sizeof(saddr)) == SOCKET_ERROR)
	{
		FD_ZERO(&r);
		FD_SET(sk, &r);
		if(select(0,0,&r,0, &timeo) <= 0) //需要注意select函数第一个参数在winsock被忽略了,在linux必须是sock+1;
		{
			printf("connect fail\n");
			closesocket(sk);
			return -1;
		}
	}
	closesocket(sk);
	return 1;
}