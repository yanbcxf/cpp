#include "stdafx.h"
#include "XmlFiles.h"
#include <shellapi.h>

CXmlFiles::CXmlFiles(CHttpSvr * pHttpSvr)
:CXmlBase(pHttpSvr, "FileUpload.xml")
{
}

CXmlFiles::~CXmlFiles(void)
{
}

void CXmlFiles::GetXml(CHttpRequest & Request, string & szOption, ostringstream & ssXml, bool & bHandled, string & szInfo, string & szWarning, string & szError, string & szNewUrl)
{
	if(szOption.length()>0)
	{
		bool b = true;

		// ÉÏ´«
		if(szOption=="Upload" && Request.IsPost())
		{
			string szNewFilePath = "./upload/"+Request.m_mapFileName["Upload"];
			ToWinPath(szNewFilePath);
			
			MakeDirectory("./upload");
			MoveFileEx(Request.m_mapFilePath["Upload"].c_str(), szNewFilePath.c_str(), MOVEFILE_REPLACE_EXISTING);
		}
		// ÏÂÔØ
		else if(szOption=="Download")
		{
			b = Download(Request, true);
			bHandled = true;
			return;
		}
		else if(szOption=="Auth")
		{
			if(Request.m_szUser.length()<=0)
			{
				Request.AuthenticateBasic("auth test");
				bHandled = true;

				return;
			}
		}
	}

	GetXmlVoice(ssXml);
}

bool CXmlFiles::Download(CHttpRequest & Request, bool bDownload)
{
	return Request.SendFile("./upload/"+Request.m_mapParam["Name"], bDownload);
}


bool CXmlFiles::GetXmlVoice(ostringstream & ssXml)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind;

	string szWinPath = "./upload/*.*";
	ToWinPath(szWinPath);

	hFind = FindFirstFile(szWinPath.c_str(), &fd);
	if(hFind==INVALID_HANDLE_VALUE) 
		return false;

	while(FindNextFile(hFind, &fd))
	{
		if(!(FILE_ATTRIBUTE_DIRECTORY&fd.dwFileAttributes))
		{
			ssXml << "<Voice>";
			ssXml << "<Name>" << fd.cFileName << "</Name>";
			ssXml << "<Size>" << ZvLibs::GetStrFileSize((fd.nFileSizeHigh*(MAXDWORD+1))+fd.nFileSizeLow) << "</Size>";
			ssXml << "</Voice>";
		}
	}

	FindClose(hFind);
	return true;
}