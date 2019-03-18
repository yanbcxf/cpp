


#include "jmutexautolock.h"

/////////////////////////////////////////////////
/*framework框架部分*/
typedef bool Boolean;
//typedef long int64_t;

#define False false
#define True true

enum IOType
{
	IOInitialize, // The client just connected
	IORead, // Read from the client. 
	IOReadCompleted, // Read completed
	IOWrite, // Write to the Client
	IOWriteCompleted, // Write Completed.
	IOZeroByteRead, // Read zero Byte from client (dummy for avoiding The System Blocking error) 
	IOZeroReadCompleted, // Read Zero Byte  completed. (se IOZeroByteRead)
	IOTransmitFileCompleted, //TransmitFileCompleted.
	IOPostedPackage, // Used to post Packages into IOCP port. 

	IOCommand,		//	对应 AbstractCommand 对象
};

// Determines the size of the first bytes who tells you how big the message are. (pakage heap)  
#define MINIMUMPACKAGESIZE sizeof(UINT)
#define MAXIMUMPACKAGESIZE 65536 /*14600*/ /*512*/
#define MAXIMUMSEQUENSENUMBER 5001

#include "IOCPBuffer.h"
#include "BufferManager.h"

#include "AbstractCommand.h"
#include "DelayQueue.h"
#include "UsageEnvironment.h"
#include "Medium.h"
#include "BaseClient.h"



/////////////////////////////////////////////////
#include "owsp_def.h"