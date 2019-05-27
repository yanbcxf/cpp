#pragma once

#pragma warning(disable: 4251 4275 4786 4996)

#define WIN32_LEAN_AND_MEAN 

#include <process.h>
#include <iostream>
#include <deque>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <conio.h>
#include <tchar.h>  
using namespace std;

#include <WinSock2.h>
#include <wininet.h>
#include <mswsock.h>

#pragma comment(lib, "Ws2_32")

#include "tstl.h"
#include "httpd/error.h"
#include "utils/utils.h"
#include "httpd/HttpClient.h"
#include "httpd/Httpd.h"
#include "sqlite/Sqlite3Db.h"
#include "utils/CmdLineParse.h"