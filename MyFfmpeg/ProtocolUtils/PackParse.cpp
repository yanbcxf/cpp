// PackParse.cpp: implementation of the CPackParse class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PackParse.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPackParse::CPackParse()
{

}

CPackParse::~CPackParse()
{

}

start_code CPackParse::next_start_code(unsigned long& lOff)
{
	if(!IsInitialized())
		return not_init;

	while(true)
	{
		switch(NextStartCode(lOff))
		{
		case _picture_start_code:	
			return picture_start_code;
		case _user_data_start_code:
			return user_data_start_code;
		case _sequence_header_code:
			return sequence_header_code;
		case _sequence_error_code:
			return sequence_error_code;
		case _extension_start_code:
			return extension_start_code;
		case sequence_end_code:
			return sequence_end_code;
		case group_start_code:
			return group_start_code;
		case 0:
			return not_found;

		case _ps_start_code:
			return ps_start_code;
		default:
			continue;
		}
	}
}

void CPackParse::initialize_parse_pointer()
{
	SetBytePos(0);
	SetBitPos(0);
}

void CPackParse::Setdata(char* pBuffer, unsigned int ilen)
{
	SetBuffer(pBuffer , ilen);
}