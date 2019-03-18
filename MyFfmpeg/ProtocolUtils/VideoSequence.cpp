// VideoSequence.cpp: implementation of the CVideoSequence class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "VideoSequence.h"
#include <memory.h>
#include <string.h>
/////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVideoSequence::CVideoSequence()
{
	horizontal_size_value = 0;
	vertical_size_value = 0;
	aspect_ratio_information = aspect_ratio_not_int;
	frame_rate_code = 0;
	bit_rate_value = 0;
	vbv_buffer_size_value = 0;

	m_video_data_point = 0;
	m_video_data_size = 0;
}

CVideoSequence::~CVideoSequence()
{
	horizontal_size_value = 0;
	vertical_size_value = 0;
	aspect_ratio_information = aspect_ratio_not_int;
	frame_rate_code = 0;
	bit_rate_value = 0;
	vbv_buffer_size_value = 0;

	m_video_data_point = 0;
	m_video_data_size = 0;
}

short CVideoSequence::parse_data(const char *pData, const unsigned long lLen, unsigned long &lOff)
{
	SetBuffer(pData , lLen);
	initialize_parse_pointer();
	
	memcpy(&m_video_data_point , &pData , sizeof(char*));
	short hResult = sequence_header();
	unsigned long bitoff;
	GetOffSet(lOff , bitoff);

	m_video_data_size = lLen-lOff;
	
	return hResult;
}

short CVideoSequence::sequence_header()
{
	start_code code = not_init;
	unsigned long off=0;

	while (true)
	{
		code = next_start_code(off);
		if(code == not_found || code == not_init)	return no_sequence_header_in;
		else if(code == sequence_header_code)	break;
		else continue;
	}
	
	m_video_data_point += off;
	if(GetBufLenLeft()<32)	return error_poccess_sequence_header;

	char tmp[20];
	memset(tmp , 0 , 20);

	// horizontal_size_value
	ReadBits(tmp , 12);
	horizontal_size_value = GetValueFromBits(tmp);

	// vertical_size_value
	ReadBits(tmp , 12);
	vertical_size_value = GetValueFromBits(tmp);

	// aspect_ratio_information
	memset(tmp , 0 , 20);
	ReadBits(tmp , 4);
	if(strcmp(tmp , "0000") == 0) return error_poccess_sequence_header;
	else if(strcmp(tmp , "0001") == 0)	aspect_ratio_information = aspect_ratio_square;
	else if(strcmp(tmp , "0010") == 0)	aspect_ratio_information = aspect_ratio_3_4;
	else if(strcmp(tmp , "0011") == 0)	aspect_ratio_information = aspect_ratio_9_16;
	else if(strcmp(tmp , "0100") == 0)	aspect_ratio_information = aspect_ratio_1_2;
	else	aspect_ratio_information = aspect_ratio_reserved;

	// frame_rate_code
	ReadBits(tmp , 4);
	if(strcmp(tmp , "0000") == 0) return error_poccess_sequence_header;
	else if(strcmp(tmp , "0001") == 0)	frame_rate_code = 23.976;
	else if(strcmp(tmp , "0010") == 0)	frame_rate_code = 24;
	else if(strcmp(tmp , "0011") == 0)	frame_rate_code = 25;
	else if(strcmp(tmp , "0100") == 0)	frame_rate_code = 29.97;
	else if(strcmp(tmp , "0101") == 0)	frame_rate_code = 30;
	else if(strcmp(tmp , "0110") == 0)	frame_rate_code = 50;
	else if(strcmp(tmp , "0111") == 0)	frame_rate_code = 59.94;
	else if(strcmp(tmp , "1000") == 0)	frame_rate_code = 60;
	else	frame_rate_code = 0;

	// bit_rate_value
	ReadBits(tmp , 18);
	bit_rate_value = GetValueFromBits(tmp);
	memset(tmp, 0,20);

	// marker bit
	ReadBits(tmp , 1);
	if(tmp[0] != '1')	return error_poccess_sequence_header;

	// vbv_buffer_size_value
	ReadBits(tmp,10);
	vbv_buffer_size_value = GetValueFromBits(tmp);

	ReadBits(tmp , 3);

	return no_error;
}

unsigned long CVideoSequence::bit_rate()
{
	return bit_rate_value;
}

unsigned long CVideoSequence::frame_rate()
{
	return frame_rate_code;
}

void CVideoSequence::video_size(unsigned long &h, unsigned long& v)
{
	h = horizontal_size_value;
	v = vertical_size_value;
}

unsigned long CVideoSequence::vbb_buffer_size()
{
	return vbv_buffer_size_value;
}

char* CVideoSequence::video_data_point()
{
	return m_video_data_point;
}

unsigned long CVideoSequence::video_data_size()
{
	return m_video_data_size;
}