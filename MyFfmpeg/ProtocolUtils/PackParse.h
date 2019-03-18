// PackParse.h: interface for the CPackParse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PACKPARSE_H__2CAF79C5_5000_41B1_BA12_67BFA830315D__INCLUDED_)
#define AFX_PACKPARSE_H__2CAF79C5_5000_41B1_BA12_67BFA830315D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataOperation.h"

enum start_code
{
	ps_start_code , 
	picture_start_code,	
	user_data_start_code,
	sequence_header_code,
	sequence_error_code,
	extension_start_code,
	sequence_end_code,
	group_start_code,

	not_init,
	not_found,
};

#define _ps_start_code			0x000001BA
#define _picture_start_code		0x00000100
#define _user_data_start_code	0x000001B2
#define _sequence_header_code	0x000001B3
#define _sequence_error_code	0x000001B4
#define _extension_start_code	0x000001B5
#define _sequence_end_code		0x000001B7
#define _group_start_code		0x000001B8



class CPackParse : public CDataOperation  
{
public:
	void Setdata(char* , unsigned int);
	void		initialize_parse_pointer();
	start_code	next_start_code(unsigned long&);
public:
	CPackParse();
	virtual ~CPackParse();

public:
	

};

#endif // !defined(AFX_PACKPARSE_H__2CAF79C5_5000_41B1_BA12_67BFA830315D__INCLUDED_)
