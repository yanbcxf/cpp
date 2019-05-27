#ifndef _INTERFACE_GLOBAL_H_
#define _INTERFACE_GLOBAL_H_

#include "./thread/jmutex.h"
#include "StandardInc.hpp"
#include "Log4cplusInc.hpp"



extern Logger g_InterfaceLogger;





extern int MultiSocketTest(string ip, int port);
extern void splitString(string s, string dim, vector<string>& resultVector);


#endif