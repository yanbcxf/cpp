
#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "activepsparser.h"

#define CACHE_BUFFSIZE_INC_UNIT (32 * 1024)
#define MAX_CACHE_BUFFSIZE		(32 * 1024 * 20)

#define MERGE_BUFFSIZE_INC_UNIT (128 * 1024)
#define MERGE_BUFFER_SIZE		(65500)
#define MAX_MERGE_BUFFER_SIZE	(128 * 1024 * 10)

#define PACKET_START_CODE_PREFIX	(0x00000100)
#define PACK_START_CODE				(0xBA)
#define SYSTEM_HEADER_START_CODE	(0xBB)
#define MAP_STREAM_ID				(0xBC)

#define PADDING_STREAM				(0xBE)

#define MPEG_PROGRAM_END_CODE		(0xB9)

// typedef struct{
// 	unsigned int pack_start_code;
// 	unsigned int marker_bit1 : 2;
// 	unsigned int system_clock_reference_base_32_30	: 3;
// 	unsigned int marker_bit2 : 1;
// 	unsigned int system_clock_reference_base_29_15	: 15;
// 	unsigned int marker_bit3 : 1;
// 	unsigned int system_clock_reference_base_14_0	: 15;
// 	unsigned int marker_bit4 : 1;
// 	unsigned int system_clock_reference_extension	: 9;
// 	unsigned int marker_bit5 : 1;
// 	unsigned int program_mux_rate					: 22;
// 	unsigned int marker_bit6 : 1;
// 	unsigned int reserved : 5;
// 	unsigned int pack_stuffing_length				: 3;
// }pack_header;

CTParserBufferManager::CTParserBufferManager()
{
	m_pu8CacheBuffer			= NULL;
	m_u32CacheBufferSize		= 0;
	m_u32CacheDataLength		= 0;

	m_pu8ReadPos				= NULL;
	m_pu8PopPos					= NULL;

	m_pu8OutputBuffer				= NULL;
	m_u32OutputBufferSize			= 0;

	m_enReadSegment = m_enPopSegment = READ_CACHE_SEGMENT;
}

CTParserBufferManager::~CTParserBufferManager()
{
	if(m_pu8CacheBuffer)
	{
		delete []m_pu8CacheBuffer;
		m_pu8CacheBuffer = NULL;
	}

	if(m_pu8OutputBuffer)
	{
		delete []m_pu8OutputBuffer;
		m_pu8OutputBuffer = NULL;
	}
}

int CTParserBufferManager::StartVisitBuffer(unsigned char *pu8PSData, unsigned int u32PSDataLength)
{
	m_pu8Data		= pu8PSData;
	m_u32DataLength	= u32PSDataLength;
	
	//	如果缓存 Cach 中有数据，则优先读入缓存中的历史数据
	if(m_u32CacheDataLength > 0)
	{
		m_enPopSegment = m_enReadSegment = READ_CACHE_SEGMENT;
		m_pu8PopPos = m_pu8ReadPos	= m_pu8CacheBuffer;
	}
	else
	{
		m_enPopSegment = m_enReadSegment = READ_CURBUFFER_SEGMENT;
		m_pu8PopPos = m_pu8ReadPos	= m_pu8Data;
	}

	return 0;
}

int CTParserBufferManager::EndVisitBuffer()
{
	unsigned int u32PrepareCacheSize = 0;

	if(((m_enPopSegment == READ_CACHE_SEGMENT) && (m_pu8PopPos > (m_pu8CacheBuffer + m_u32CacheDataLength)))
		|| ((m_enPopSegment == READ_CURBUFFER_SEGMENT) && (m_pu8PopPos > (m_pu8Data + m_u32DataLength))))
	{
		//ASSERT(0);
		return -1;
	}

	if(m_enPopSegment == READ_CACHE_SEGMENT)
	{
		u32PrepareCacheSize = ((m_u32CacheDataLength - (m_pu8PopPos - m_pu8CacheBuffer)) + m_u32DataLength);
		//	先将 已经处理的数据从缓存 Cach 中删除
		if(m_u32CacheBufferSize < u32PrepareCacheSize)
		{
			unsigned char *pu8Tmp = NULL;
			pu8Tmp = new unsigned char[(u32PrepareCacheSize + CACHE_BUFFSIZE_INC_UNIT) / CACHE_BUFFSIZE_INC_UNIT * CACHE_BUFFSIZE_INC_UNIT];
			//ASSERT(pu8Tmp != NULL);
			m_u32CacheBufferSize = ((u32PrepareCacheSize + CACHE_BUFFSIZE_INC_UNIT) / CACHE_BUFFSIZE_INC_UNIT * CACHE_BUFFSIZE_INC_UNIT);

			memmove(pu8Tmp, m_pu8PopPos, (m_u32CacheDataLength - (m_pu8PopPos - m_pu8CacheBuffer)));
			m_u32CacheDataLength = (m_u32CacheDataLength - (m_pu8CacheBuffer - m_pu8PopPos));

			if(m_pu8CacheBuffer)
			{
				delete []m_pu8CacheBuffer;
			}
			m_pu8CacheBuffer = pu8Tmp;
		}
		else
		{
			if(m_pu8PopPos != m_pu8CacheBuffer)
			{
				memmove(m_pu8CacheBuffer, m_pu8PopPos, (m_u32CacheDataLength - (m_pu8PopPos - m_pu8CacheBuffer)));
				m_u32CacheDataLength = (m_u32CacheDataLength - (m_pu8PopPos - m_pu8CacheBuffer));
			}
		}
		//	将当前数据区中数据 copy 到缓存 Cach 中
		memmove(m_pu8CacheBuffer + m_u32CacheDataLength, m_pu8Data, m_u32DataLength);
		m_u32CacheDataLength += m_u32DataLength;
	}
	else if(m_enPopSegment == READ_CURBUFFER_SEGMENT)
	{
		u32PrepareCacheSize = (m_u32DataLength - (m_pu8PopPos - m_pu8Data));  //输出到当前DataBuffer,则CacheBuffer的数据已全部读出

		//	如果缓存的大小不够，则重新申请一个 大的缓存
		if(m_u32CacheBufferSize < u32PrepareCacheSize)
		{
			//	Cache 最大允许 640 K
			if(u32PrepareCacheSize > MAX_CACHE_BUFFSIZE)
			{
				//ASSERT(0);
				return -1;
			}

			unsigned char *pu8Tmp = NULL;
			pu8Tmp = new unsigned char[(u32PrepareCacheSize + CACHE_BUFFSIZE_INC_UNIT) / CACHE_BUFFSIZE_INC_UNIT * CACHE_BUFFSIZE_INC_UNIT];
			//ASSERT(pu8Tmp != NULL);

			if(m_pu8CacheBuffer)
			{
				delete []m_pu8CacheBuffer;
			}

			m_pu8CacheBuffer = pu8Tmp;
			m_u32CacheBufferSize = ((u32PrepareCacheSize + CACHE_BUFFSIZE_INC_UNIT) / CACHE_BUFFSIZE_INC_UNIT * CACHE_BUFFSIZE_INC_UNIT);
		}
		//	将剩余的数据 copy 到缓存
		memmove(m_pu8CacheBuffer, m_pu8PopPos, u32PrepareCacheSize);
		m_u32CacheDataLength = u32PrepareCacheSize;
	}
}

int CTParserBufferManager::PopParsedBuffer()
{
	m_enPopSegment = m_enReadSegment;
	m_pu8PopPos = m_pu8ReadPos;

	return 0;
}

unsigned char *	CTParserBufferManager::GetOutputBuffer(unsigned int u32NeedBytes)
{
	if(u32NeedBytes > m_u32OutputBufferSize)
	{
		unsigned char *pu8TmpBuffer = new unsigned char[(u32NeedBytes + CACHE_BUFFSIZE_INC_UNIT) / CACHE_BUFFSIZE_INC_UNIT * CACHE_BUFFSIZE_INC_UNIT];
		if(!pu8TmpBuffer)
		{
			return NULL;
		}
		if(m_pu8OutputBuffer)
		{
			delete []m_pu8OutputBuffer;
		}

		m_pu8OutputBuffer = pu8TmpBuffer;
		m_u32OutputBufferSize = (u32NeedBytes + CACHE_BUFFSIZE_INC_UNIT) / CACHE_BUFFSIZE_INC_UNIT * CACHE_BUFFSIZE_INC_UNIT;
	}

	return m_pu8OutputBuffer;
}

unsigned char *	CTParserBufferManager::ReadBytes(unsigned int u32Size)
{
	unsigned int u32CacheHaveDataLength, u32CurBufferHaveDataLength;
	unsigned int u32NeedBytes = 0;
	unsigned char *pu8OutputBuffer = NULL;
	bool bUseOutputBuffer = false;

	if(u32Size == 0)
	{
		return m_pu8ReadPos;
	}

	if(m_enReadSegment == READ_CACHE_SEGMENT)
	{
		u32CacheHaveDataLength = (m_pu8CacheBuffer + m_u32CacheDataLength - m_pu8ReadPos);
		u32CurBufferHaveDataLength = m_u32DataLength;
	}
	else
	{
		u32CacheHaveDataLength = 0;
		u32CurBufferHaveDataLength = (m_pu8Data + m_u32DataLength - m_pu8ReadPos);
	}

	if(u32Size > (u32CacheHaveDataLength + u32CurBufferHaveDataLength))
	{
		return NULL;		//没有足够的数据满足Size
	}

	u32NeedBytes = u32Size;

	if(m_enReadSegment == READ_CACHE_SEGMENT)		
	{
		if(u32NeedBytes <= u32CacheHaveDataLength)
		{
			//memcpy(GetOutputBuffer(u32NeedBytes), m_pu8ReadPos, u32NeedBytes);
			pu8OutputBuffer = m_pu8ReadPos;		//不跨越CacheBuffer,直接使用CacheBuffer地址
			m_pu8ReadPos += u32NeedBytes;
			
			u32NeedBytes = 0;
		}
		else
		{
			if(u32CacheHaveDataLength > 0)
			{
				//	跨越 CachBuffer ，一部分数据来自 Cach ，一部分来自当前数据区，则调用 GetOutputBuffer 分配一个临时内存
				pu8OutputBuffer = GetOutputBuffer(u32NeedBytes);

				memcpy(pu8OutputBuffer, m_pu8ReadPos, u32CacheHaveDataLength);
				u32NeedBytes -= u32CacheHaveDataLength;
				u32CacheHaveDataLength = 0;

				bUseOutputBuffer = true;
			}

			m_enReadSegment = READ_CURBUFFER_SEGMENT;
			m_pu8ReadPos = m_pu8Data;
		}
	}

	if((u32NeedBytes > 0) && (m_enReadSegment == READ_CURBUFFER_SEGMENT))
	{
		if(u32NeedBytes > m_u32DataLength)
		{
			//ASSERT(0);
			return NULL;
		}
		else
		{
			if(bUseOutputBuffer == true)
			{
				memcpy(pu8OutputBuffer + (u32Size - u32NeedBytes), m_pu8ReadPos, u32NeedBytes);
			}
			else
			{
				pu8OutputBuffer = m_pu8ReadPos;
			}
			
			m_pu8ReadPos += u32NeedBytes;
		}
	}

	return pu8OutputBuffer;
}

//	从缓存中读出 最多四个字节，数值保存在 pu32Out 中
 int CTParserBufferManager::ReadBinary(unsigned int *pu32Out, unsigned int u32Size)
{
	unsigned int x = 0;
	unsigned char c;
	unsigned char *pu8Out = NULL;

	for(unsigned int i = 0; i < u32Size;i++){
		if((pu8Out = ReadBytes(1)) == NULL)
		{
			return -1;
		}

		c = *pu8Out;
		x |= c << ((u32Size-i-1)*8);
	}

	*pu32Out = x;

	return 0;
}

int	CTParserBufferManager::SeekBytes(int s32Size)
{

	if(s32Size > 0)
	{
		if(ReadBytes((unsigned int)s32Size) != 0)
		{
			return -1;
		}
		else
		{
			return 0;
		}
	}
	else if(s32Size < 0)
	{
		//	读出指针进行 回溯
		unsigned int u32Size = abs(s32Size);	//取绝对值
		if(m_enReadSegment == READ_CACHE_SEGMENT)
		{
			if(u32Size <= (m_pu8ReadPos - m_pu8CacheBuffer))
			{
				m_pu8ReadPos -= u32Size;
			}
			else
			{
				return -1;
			}
		}
		else
		{
			if(u32Size <= (m_pu8ReadPos - m_pu8Data))
			{
				m_pu8ReadPos -= u32Size;
			}
			else if(u32Size <= ((m_pu8ReadPos - m_pu8Data) + m_u32CacheDataLength))
			{
				//	读出指针的回溯大小超出了 当前数据区，则继续回溯到缓存 Cach 中
				m_pu8ReadPos = (m_pu8CacheBuffer + (m_u32CacheDataLength - (u32Size - (m_pu8ReadPos - m_pu8Data))));
				m_enReadSegment = READ_CACHE_SEGMENT;
			}
			else
			{
				return -1;
			}
		}
	}

	return 0;
}

#define READ_BYTE(OUT_UCHAR, FAIL) \
	do{	\
		unsigned char *pTmp; \
		if((pTmp = ReadBytes(1)) == NULL)	\
		{	\
			goto FAIL;	\
		}	\
		else	\
		{	\
			OUT_UCHAR = *pTmp;	\
		}	\
	}while(0) \

#define READ_BYTES(OUT_POINT, NUM, FAIL) \
	do{	\
		if((OUT_POINT = ReadBytes(NUM)) == NULL)	\
		{	\
			goto FAIL;	\
		}	\
	}while(0) \

#define READ_BINARY(OUT_UINT, NUM, FAIL) \
	do{	\
		if(ReadBinary(&OUT_UINT, NUM) < 0)	\
		{	\
			goto FAIL;	\
		}	\
	}while(0) \

#define SEEK_BYTES(NUM, FAIL) \
	do{	\
		if(SeekBytes(NUM) < 0)	\
		{	\
			goto FAIL;	\
		}	\
	}while(0) \

CTActivePSParser::CTActivePSParser(cbDataHandle cbDataHandleFuncs, APSPARSER_USERDATA *pUserData)
: m_cbDataHandleFuncs(cbDataHandleFuncs), m_pUserData(pUserData)
{
	m_bParseSystemHead		= false;
	m_PacketCounter			= 0;
	memset(m_StreamIDMap, 0, sizeof(unsigned char) * STREAMID_MAP_SIZE);

	m_pu8OutputBuffer			= NULL;
	m_u32OutputBufferSize		= 0;

	m_u8MergeID			= 0;
	m_bMergeFlag		= false;
	m_u32OutputBufferDataLength = 0;

}

CTActivePSParser::~CTActivePSParser()
{

}

int CTActivePSParser::ParseSystemHeader()
{
	unsigned char *pu8Point = NULL;
	unsigned int system_header_length = 0;

	unsigned int program_stream_map_length = 0;
	unsigned int elementary_stream_map_length = 0;
	unsigned int program_stream_info_length = 0;

	unsigned int padding_stream_length = 0;
	unsigned char * pesinfo = NULL;

	unsigned char stream_type = 0;
	unsigned char elementary_stream_id = 0;
	unsigned int  elementary_stream_info_length = 0;

	do
	{
		do
		{
			READ_BYTES(pu8Point, 1, ParseSystemHeaderFail);
		}while(*pu8Point != SYSTEM_HEADER_START_CODE);

		SEEK_BYTES(-4, ParseSystemHeaderFail);

		PopParsedBuffer();  //未得到头,则前面的数据都是没用

		READ_BYTES(pu8Point, 4, ParseSystemHeaderFail);

		//if(*((unsigned int *)pu8Point) == (PACKET_START_CODE_PREFIX | SYSTEM_HEADER_START_CODE))
		if(pu8Point[0] == 0x00 && pu8Point[1] == 0x00 && pu8Point[2] == 0x01 && pu8Point[3] == SYSTEM_HEADER_START_CODE)
		{
			READ_BINARY(system_header_length, 2, ParseSystemHeaderFail); //read  system_header.header_length
			if (system_header_length > 65535)
			{
				continue;
			}
			READ_BYTES(pu8Point, system_header_length, ParseSystemHeaderFail);

			READ_BYTES(pu8Point, 4, ParseSystemHeaderFail);

			//Parse  program_stream_map()
			//if(*((unsigned int *)pu8Point) == (PACKET_START_CODE_PREFIX | MAP_STREAM_ID))
			if(pu8Point[0] == 0x00 && pu8Point[1] == 0x00 && pu8Point[2] == 0x01 && pu8Point[3] == MAP_STREAM_ID)
			{
				// skip program_stream_map_length + map_version
				READ_BINARY(program_stream_map_length, 2, ParseSystemHeaderFail);

				READ_BYTES(pu8Point, 2, ParseSystemHeaderFail);

				READ_BINARY(program_stream_info_length, 2, ParseSystemHeaderFail);
				if (program_stream_info_length > 65535)
				{
					continue;
				}

				READ_BYTES(pu8Point, program_stream_info_length, ParseSystemHeaderFail);

				READ_BINARY(elementary_stream_map_length, 2, ParseSystemHeaderFail);
				while (elementary_stream_map_length >= 4)
				{
						/*	stream_type 含义：
						a)  MPEG - 4视频流：  0x10；
						b)  H.264视频流：   0x1B；
						c)  SVAC视频流：    0x80；
						d)  G.711音频流：   0x90；
						e)  G.722.1音频流： 0x92；
						f)  G.723.1音频流： 0x93；
						g)  G.729音频流：   0x99；
						h)  SVAC音频流：    0x9B。*/

					READ_BYTE(stream_type, ParseSystemHeaderFail);
					READ_BYTE(elementary_stream_id, ParseSystemHeaderFail);
					READ_BINARY(elementary_stream_info_length, 2, ParseSystemHeaderFail);

					/*流标识 streamid  ：  1110xxxx for video ， 110 xxxxx for audio*/
					m_StreamIDMap[elementary_stream_id] = stream_type;
					READ_BYTES(pu8Point, elementary_stream_info_length, ParseSystemHeaderFail);	//descriptor()
					elementary_stream_map_length -= 4;
					if (elementary_stream_info_length > 0)
						elementary_stream_map_length -= elementary_stream_info_length;
				}
				m_PacketCounter++;
				m_bParseSystemHead = true;
			}

			else if (pu8Point[0] == 0x00 && pu8Point[1] == 0x00 && pu8Point[2] == 0x01 && pu8Point[3] == PADDING_STREAM)
			{
				/* 新闻联播.mpg 会走到这个分支 */
				padding_stream_length = 0;
				READ_BINARY(padding_stream_length, 2, ParseSystemHeaderFail);
				READ_BYTES(pesinfo, padding_stream_length, ParseSystemHeaderFail);

				m_StreamIDMap[0xE0] = 1;
				m_StreamIDMap[0xC0] = 1;

				m_PacketCounter++;
				m_bParseSystemHead = true;
			}
		}
			
	}while(m_bParseSystemHead == false);

	return 0;

ParseSystemHeaderFail:

	return -1;
}

unsigned long get_PTS_DTS( unsigned char *data )
{
	unsigned long t = (*data & 0x0e) << 29 |
		((((*(data+1)) << 8) | (*(data+2))) >> 1) << 15 |
		(((*(data+3)) << 8) | (*(data+4))) >> 1;
	return t;
}

int CTActivePSParser::ParsePESPacket()
{
	unsigned char u8Value = 0;
	unsigned char *pu8Point = NULL;

	PS_PACKET_S		stPSPacket;
	PES_PACKET_S	stPESPacket;

	unsigned int PES_packet_length = 0;
	unsigned char PTS_DTS_flags = 0;
	unsigned int PES_header_data_length = 0;
	unsigned char * pesinfo = NULL;

	unsigned int es_data_len = 0;
	unsigned char * esdata = NULL;

	do 
	{	
		do 
		{
			READ_BYTE(u8Value, ParsePESPacketFail);
		} while (u8Value != 0x01);

		SEEK_BYTES(-3, ParsePESPacketFail);

		PopParsedBuffer();

		READ_BYTES(pu8Point, 3, ParsePESPacketFail);
		if (pu8Point[0] == 0x00 && pu8Point[1] == 0x00 && pu8Point[2] == 0x01)
		{
			READ_BYTE(u8Value, ParsePESPacketFail); // 224 : E0 | 192 : C0
			if(u8Value == 0xBA)
			{
				m_PacketCounter++;
			}
			if(m_StreamIDMap[u8Value] != 0)
			{
				PES_packet_length = 0;
				READ_BINARY(PES_packet_length, 2, ParsePESPacketFail);

				// parse pts_dts flag
				READ_BYTES(pu8Point, 1, ParsePESPacketFail);

				PTS_DTS_flags			= 0;
				PES_header_data_length	= 0;
				pesinfo					= NULL;

				READ_BYTE(PTS_DTS_flags, ParsePESPacketFail);
				READ_BINARY(PES_header_data_length, 1, ParsePESPacketFail);
				READ_BYTES(pesinfo, PES_header_data_length, ParsePESPacketFail);

				if ( PTS_DTS_flags & 0x80)
				{
					// pts
					stPESPacket.pts = get_PTS_DTS(pesinfo);
					if (PTS_DTS_flags & 0x40)
					{
						// dts
						stPESPacket.dts = get_PTS_DTS(pesinfo + 5);
					}
				}

				// 17126238 unknown why length not correct,skip
				if(PES_header_data_length > PES_packet_length)
				{
					continue;
				}
				if((PES_packet_length - PES_header_data_length) <= 3)
				{
					continue;
				}

				es_data_len = PES_packet_length - PES_header_data_length - 3;
				esdata = NULL;

				//	但收到的数据不足时，会跳到 ParsePESPacketFail
				READ_BYTES(esdata, es_data_len, ParsePESPacketFail);
				
				if(m_bMergeFlag && u8Value == m_u8MergeID && es_data_len < MERGE_BUFFER_SIZE)
				{

					WriteInOutputBuffer(esdata, es_data_len);

					stPESPacket.es_type = m_StreamIDMap[u8Value];
					stPESPacket.es_id = u8Value;
					stPESPacket.es_data = m_pu8OutputBuffer;
					stPESPacket.es_data_length = m_u32OutputBufferDataLength + es_data_len;
					stPSPacket.pstPESPacket = &stPESPacket;
					stPSPacket.pack_id = m_PacketCounter;
					m_bMergeFlag = false;
					m_u8MergeID = 0;
					m_u32OutputBufferDataLength = 0;

				}
				else
				{
					stPESPacket.es_type = m_StreamIDMap[u8Value];
					stPESPacket.es_id = u8Value;
					stPESPacket.es_data = esdata;
					stPESPacket.es_data_length = es_data_len;
					stPSPacket.pstPESPacket = &stPESPacket;
					stPSPacket.pack_id = m_PacketCounter;
					if (es_data_len == MERGE_BUFFER_SIZE)
					{
						WriteInOutputBuffer(esdata, es_data_len);
						m_bMergeFlag = true;
						m_u8MergeID = u8Value;
						continue;
					}
				}

				if (m_cbDataHandleFuncs)
				{
					m_cbDataHandleFuncs(&stPSPacket, this, m_pUserData);
				}

				PopParsedBuffer();

				// printf("read es data length [%d],counter [%x]\n",es_data_len,rsb.counter);
				break;
			}
			// audio data
			if ( u8Value >= 0xC0 && u8Value <= 0xDF)
			{
				break;
			}
			// video data
			if ( u8Value >= 0xE0 && u8Value <= 0xEF)
			{
				break;
			}
		}
		else
		{
			continue;
		}
	}while (1);

	return 0;

ParsePESPacketFail:

	return -1;
}

int CTActivePSParser::WriteInOutputBuffer(unsigned char *pu8Data, unsigned int u32DataLength)
{
	if(!pu8Data)
	{
		return -1;
	}

	if(!m_pu8OutputBuffer)
	{
		m_pu8OutputBuffer = new unsigned char[m_u32OutputBufferSize = ((u32DataLength + MERGE_BUFFSIZE_INC_UNIT) / MERGE_BUFFSIZE_INC_UNIT * MERGE_BUFFSIZE_INC_UNIT)];
	}
	else
	{
		if(m_u32OutputBufferSize < (m_u32OutputBufferDataLength + u32DataLength))
		{
			unsigned char *pu8Tmp = new unsigned char[m_u32OutputBufferSize = (((m_u32OutputBufferDataLength + u32DataLength) + MERGE_BUFFSIZE_INC_UNIT) / MERGE_BUFFSIZE_INC_UNIT * MERGE_BUFFSIZE_INC_UNIT)];
		
			//ASSERT(pu8Tmp);
			memcpy(pu8Tmp, m_pu8OutputBuffer, m_u32OutputBufferDataLength);

			delete []m_pu8OutputBuffer;
			m_pu8OutputBuffer = pu8Tmp;
		}
		
		memcpy(m_pu8OutputBuffer + m_u32OutputBufferDataLength, pu8Data, u32DataLength);
		m_u32OutputBufferDataLength += u32DataLength;
	}

	return 0;	
}

//	该解析器的 唯一入口
int CTActivePSParser::ParsePSData(unsigned char *pu8PSData, unsigned int u32PSDataLength)
{
	if((!pu8PSData) || (u32PSDataLength == 0))
	{
		return -1;
	}

	StartVisitBuffer(pu8PSData, u32PSDataLength);

	do
	{
		if(!m_bParseSystemHead)
		{
			if(ParseSystemHeader() < 0)
			{
				break;
			}
		}
		
		if(ParsePESPacket() < 0)
		{
			break;
		}
	}while(1);

	EndVisitBuffer();

	return 0;
}

