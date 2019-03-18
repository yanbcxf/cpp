// VideoSequence.h: interface for the CVideoSequence class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VIDEOSEQUENCE_H__1640FFE0_FEF5_452A_824F_BB39C6CE8CD4__INCLUDED_)
#define AFX_VIDEOSEQUENCE_H__1640FFE0_FEF5_452A_824F_BB39C6CE8CD4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PackParse.h"

#define no_sequence_header_in 0x0002
#define error_poccess_sequence_header 0x0003
#define no_error 0x0000

enum aspect_ratio
{
	aspect_ratio_square,
	aspect_ratio_3_4,
	aspect_ratio_9_16,
	aspect_ratio_1_2,
	aspect_ratio_reserved,
	aspect_ratio_not_int,
};

class CVideoSequence : public CPackParse  
{
private:
	unsigned long	horizontal_size_value;
	unsigned long	vertical_size_value;
	aspect_ratio	aspect_ratio_information;
	unsigned long	frame_rate_code;
	unsigned long	bit_rate_value;
	unsigned long	vbv_buffer_size_value;

	char* m_video_data_point;
	unsigned long m_video_data_size;
private:
	short sequence_header(); 
	
public:
	short parse_data(const char* , const unsigned long , unsigned long&);
	char* video_data_point();
	unsigned long video_data_size();

	void video_size(unsigned long& h , unsigned long& v);
	unsigned long frame_rate();
	unsigned long bit_rate();
	unsigned long vbb_buffer_size();

public:
	CVideoSequence();
	virtual ~CVideoSequence();
};

#endif // !defined(AFX_VIDEOSEQUENCE_H__1640FFE0_FEF5_452A_824F_BB39C6CE8CD4__INCLUDED_)
