#pragma once


typedef unsigned int u32_t;
typedef unsigned short u16_t;
typedef int s32_t;
typedef short s16_t;
typedef char s8_6;

#define PES_DATA_SIZE	(64000)
#define MAX_ESTREAM		2	//目前就只有音视频流


#define PACK_START_CODE             ((unsigned int)0x000001ba)
#define SYSTEM_HEADER_START_CODE    ((unsigned int)0x000001bb)
#define PROGRAM_STREAM_MAP			((unsigned int)0x000001bc)
#define SEQUENCE_END_CODE           ((unsigned int)0x000001b7)
#define PACKET_START_CODE_MASK      ((unsigned int)0xffffff00)
#define PACKET_START_CODE_PREFIX    ((unsigned int)0x00000100)
#define ISO_11172_END_CODE          ((unsigned int)0x000001b9)


typedef struct {
	unsigned char profile_idc;

	unsigned char constraint_set0_flag : 1;
	unsigned char constraint_set1_flag : 1;
	unsigned char constraint_set2_flag : 1;
	unsigned char constraint_set3_flag : 1;
	unsigned char reserved_zero_4bits : 4;
	unsigned char level_idc;
}H264SpsInfo;


struct _EStream {
	PLAYER_MEDIA_F_STREAM_TYPE codecId;

	int chn;
	int bitrate;

	/*PS Info*/
	int stream_id;
	int packet_size;

	int max_buffer_size;
	unsigned long long last_src;
	unsigned long long src;

	/*for video*/
	H264SpsInfo sps;

};

class CProgramStreamPacketizer
{
public:
	CProgramStreamPacketizer();
	~CProgramStreamPacketizer();

	/*输入的 pts 是以采用率为单位的*/
	void PackegeH264Data(unsigned char  *pu8PSData, unsigned int u32PSDataLength,
		unsigned long pts, PLAYER_MEDIA_F_STREAM_TYPE streamType);

private:
	void SaveDataToFile(unsigned char * pes, unsigned pesSize, unsigned long pts);


private:
	int put_system_header(unsigned char  *buf);
	int put_program_stream_map(unsigned char *buf);
	int addAudio2PStream(PLAYER_MEDIA_F_STREAM_TYPE id, int bitrate, int chn);
	int addVideo2PStream(PLAYER_MEDIA_F_STREAM_TYPE id, int width, int height, int bitrate);
	int init_ps(unsigned char *nalBuffer, int nalSize);

	/* is_new_packet = 1 说明输入的数据是采集帧的第一个的 nal 单元 */
	int ps_packet_video(unsigned char *nalBuffer, int nalSize, int is_new_packet, unsigned long pts);
	int ps_packet_audio(unsigned char *nalBuffer, int nalSize, int is_new_packet, unsigned long pts);


private:

	FILE * fp;
	
	//RC_FRAME *rcFrame;
	int cur_stream;

	int first;
	int mux_rate;
	int stream_rate[MAX_ESTREAM];
	
	int nb_streams;
	struct _EStream EStream[MAX_ESTREAM];

	int audio_bound;
	int video_bound;

	int get_first_IFrame;		// 是否获取到第一个I帧
};

