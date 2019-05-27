#include "StdAfx.h"
#include "JsonNotify.h"

CJsonNotify::CJsonNotify(CHttpSvr * pHttpSvr)
:CHttpHandle(pHttpSvr, HTTP_HANDLE_CONTENT, "notify")
{
}

CJsonNotify::~CJsonNotify(void)
{
}

bool CJsonNotify::Handle(CHttpRequest & Request)
{
	SendJsonTree(Request);
	return true;
}



void CJsonNotify::SendJsonTree(CHttpRequest & Request)
{
	// Request.SetContentType("text/json; charset=utf-8");

	tstring szRoot = ToWinPath(Request.m_mapParam[_T("id")]);
	tostringstream ss;

	ss << "[";

	bool b=false;

	if(/*szRoot==_T("0")*/ 1)
	{
		for(TCHAR cLetter = _T('A'); cLetter <= _T('Z'); cLetter++)
		{
			tstring szDrive;

			szDrive += cLetter;
			szDrive += _T(":\\");
			UINT nType = GetDriveType(szDrive.c_str());

			if(!(DRIVE_REMOVABLE <= nType && nType <= DRIVE_RAMDISK ))
				continue;

			if(b) 
				ss << ",";
			else 
				b=true;

			tstring szPath;
			szPath += cLetter;
			szPath += _T(":");

			ss << "{"
			   << "attributes:{id:'" << EscapeJsonCharacter(szPath) << "'},"
			   << "icons:'images/icon_ext/" << ((nType==DRIVE_CDROM)?"drive_cd.gif":"drive.gif") << "',"
			   << "state:'closed',"
			   << "data:'" << EscapeJsonCharacter(szPath) << "'"
			   << "}";
		}
	}
	else
	{
		WIN32_FIND_DATA fd;
		tstring szPath = szRoot+"\\*";

		HANDLE hFind = FindFirstFile(szPath.c_str(), &fd);

		if(hFind!=INVALID_HANDLE_VALUE)
		{
			while(true)
			{
				tstring szFileName = fd.cFileName;

				if(szFileName!=_T(".") && szFileName!=_T(".."))
				{
					if(b) 
						ss << ",";
					else 
						b=true;

					bool bDir = (fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)?true:false;

					tstring szFileExt = _T("");
					if(!bDir)
					{
						int p = szFileName.rfind('.');
						if(p!=tstring::npos)
							szFileExt = szFileName.substr(p+1);
					}


					ss << "{"
					    // << "attributes:{id:'" << (EscapeJsonCharacter(EscapeHtmlCharacter(szRoot+_T("\\")+fd.cFileName))) << "'},"
						<< "attributes:{id:'" << (EscapeJsonCharacter(EscapeHtmlCharacter(szRoot+_T("\\")+fd.cFileName))) << "'},"
						// << "attributes:{id:'" << UriEncode(((szRoot+_T("\\")+fd.cFileName))) << "'},"
						
						<< "icons:'" << (bDir?_T("images/icon_ext/folder.gif"):(_T("?Option=GetFileIcon&Ext=")+szFileExt)) << "',"
						<< "state:'" << (bDir?"closed":"") <<"',"
						<< "data:'" << EscapeJsonCharacter(EscapeHtmlCharacter(szFileName)) << "'"
						<< "}";

				}

				if(!FindNextFile(hFind, &fd)) break;
			}
			FindClose(hFind);
		}
	}

	ss << "]\r\n";
	string szData = ss.str();

	int nLen = szData.length();
	ss.str("");
	ss << nLen;

	Request.SetParam("Content-Type", "Application/json; charset=utf-8");
	Request.SetParam("Content-Length", ss.str());

	Request.SendUtf8Body(szData);
}