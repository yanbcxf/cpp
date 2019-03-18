#include "stdafx.h"
#include "ProgramStreamPacketizer.h"

#include "bitstream.h"


#ifdef __cplusplus
extern "C" {
#endif

	extern unsigned int mpeg_crc32(const unsigned char * data, int len);

#ifdef __cplusplus
}
#endif 

CProgramStreamPacketizer::CProgramStreamPacketizer()
{
	nb_streams = 0;
	audio_bound = 0;
	video_bound = 0;
	get_first_IFrame = 0;

	fp = NULL;
}


CProgramStreamPacketizer::~CProgramStreamPacketizer()
{
}



///////////////////////  ps 流打包工作函数 //////////////////////////////

int put_pack_header(struct _EStream *es, unsigned char *buf, unsigned long timestamp, u32_t mux_rate)
{
	PutBitContext pb;

	init_put_bits(&pb, buf, 128);

	put_bits32(&pb, PACK_START_CODE);
	put_bits(&pb, 2, 0x1);

	put_bits(&pb, 3, (u32_t)((timestamp >> 30) & 0x07));
	put_bits(&pb, 1, 1);
	put_bits(&pb, 15, (u32_t)((timestamp >> 15) & 0x7fff));
	put_bits(&pb, 1, 1);
	put_bits(&pb, 15, (u32_t)((timestamp)& 0x7fff));
	put_bits(&pb, 1, 1);

	put_bits(&pb, 9, 0);

	put_bits(&pb, 1, 1);
	put_bits(&pb, 22, mux_rate);
	put_bits(&pb, 1, 1);
	put_bits(&pb, 1, 1);
	put_bits(&pb, 5, 0x1f); /* reserved */
	put_bits(&pb, 3, 0); /* stuffing length */

	flush_put_bits(&pb);

	return put_bits_ptr(&pb) - pb.buf;
}

int put_avc_video_descriptor(struct _EStream *es, unsigned char *buf)
{
	int size = 0;
	H264SpsInfo *sps = &(es->sps);
	PutBitContext pb;

	init_put_bits(&pb, buf, 128);

	put_bits(&pb, 8, 40);
	put_bits(&pb, 8, 0);	//we fix it later :)

	put_bits(&pb, 8, sps->profile_idc);	//profile_idc
	put_bits(&pb, 1, sps->constraint_set0_flag);
	put_bits(&pb, 1, sps->constraint_set1_flag);
	put_bits(&pb, 1, sps->constraint_set2_flag);

	put_bits(&pb, 5, (sps->constraint_set3_flag << 4) | sps->reserved_zero_4bits);
	put_bits(&pb, 8, sps->level_idc);

	put_bits(&pb, 1, 0);
	put_bits(&pb, 1, 0);

	put_bits(&pb, 6, 0x3f);

	flush_put_bits(&pb);
	size = put_bits_ptr(&pb) - pb.buf;
	buf[1] = (size - 2) & 0xFF;
	return size;
}


int put_padding_pes_packet(unsigned char *buf, u32_t buf_size, u32_t pad_bytes)
{
	int size = 0;
	PutBitContext pb;

	init_put_bits(&pb, buf, buf_size);

	put_bits(&pb, 24, 1);
	put_bits(&pb, 8, 0xBE);

	put_bits(&pb, 16, 0);	//we fix it later :)

	flush_put_bits(&pb);
	if (pad_bytes - 6 > 0)
	{
		//SN_MEMSET(pb.buf_ptr, 0xFF, pad_bytes - 6);
		memset(pb.buf_ptr, 0xFF, pad_bytes - 6);
		pb.buf_ptr += (pad_bytes - 6);
	}

	size = put_bits_ptr(&pb) - pb.buf;
	buf[4] = (size - 6) >> 8;
	buf[5] = (size - 6) & 0xFF;

	return size;
}


int put_av_pes_packet(struct _EStream *es, unsigned char *buf, u32_t buf_size, u32_t pts, u32_t used_pts,
	unsigned char *data, s32_t *data_len, u32_t packSize)   // data 为源数据起始位置
{
	int size = 0;
	int pack_size = 0;
	int len = *data_len;

	PutBitContext pb;

	init_put_bits(&pb, buf, buf_size);

	put_bits(&pb, 24, 1);
	put_bits(&pb, 8, es->stream_id);

	put_bits(&pb, 16, 0);	//we fix it later :)

	put_bits(&pb, 2, 2);
	put_bits(&pb, 2, 0);
	put_bits(&pb, 1, 0);
	put_bits(&pb, 1, 0);
	put_bits(&pb, 1, 0);
	put_bits(&pb, 1, 0);

	if (used_pts)
		put_bits(&pb, 2, 2);	//PTS
	else
		put_bits(&pb, 2, 0);	//PTS
	put_bits(&pb, 1, 0);
	put_bits(&pb, 1, 0);
	put_bits(&pb, 1, 0);
	put_bits(&pb, 1, 0);
	put_bits(&pb, 1, 0);
	put_bits(&pb, 1, 0);

	if (used_pts) {
		put_bits(&pb, 8, 5);

		/*PTS and DTS*/
		put_bits(&pb, 4, 2);
		put_bits(&pb, 3, (u32_t)((pts >> 30) & 0x7));
		put_bits(&pb, 1, 1);
		put_bits(&pb, 15, (u32_t)((pts >> 15) & 0x7fff));
		put_bits(&pb, 1, 1);
		put_bits(&pb, 15, (u32_t)((pts)& 0x7fff));
		put_bits(&pb, 1, 1);
	}
	else {
		put_bits(&pb, 8, 0);
	}

	flush_put_bits(&pb);

	pack_size = put_bits_ptr(&pb) - pb.buf;
	if (pack_size + len > packSize)
	{
		len = packSize - pack_size;
		*data_len = len;
	}

	//SN_MEMCPY(pb.buf_ptr, buf_size-pack_size, data, len, len);
	memcpy(pb.buf_ptr, data, len);
	pb.buf_ptr += len;


	size = put_bits_ptr(&pb) - pb.buf;
	buf[4] = (size - 6) >> 8;
	buf[5] = (size - 6) & 0xFF;


	return put_bits_ptr(&pb) - pb.buf;;

}


int CProgramStreamPacketizer::put_system_header(unsigned char *buf)
{
	CProgramStreamPacketizer * ps = this;
	int size, i;
	PutBitContext pb;

	init_put_bits(&pb, buf, 128);

	put_bits32(&pb, SYSTEM_HEADER_START_CODE);
	put_bits(&pb, 16, 0);	//we fix it later :)
	put_bits(&pb, 1, 1);

	put_bits(&pb, 22, (ps->mux_rate)); /* maximum bit rate of the multiplexed stream */
	put_bits(&pb, 1, 1); /* marker */


	put_bits(&pb, 6, ps->audio_bound);

	put_bits(&pb, 1, 0); /* variable bitrate*/
	put_bits(&pb, 1, 0); /* non constrainted bit stream */

	put_bits(&pb, 1, 0); /* audio locked */
	put_bits(&pb, 1, 0); /* video locked */

	put_bits(&pb, 1, 1); /* marker */

	put_bits(&pb, 5, ps->video_bound);

	put_bits(&pb, 1, 0);    /* packet_rate_restriction_flag */
	put_bits(&pb, 7, 0x7f); /* reserved byte */

	for (i = 0; i<ps->nb_streams; i++)	{
		put_bits(&pb, 8, ps->EStream[i].stream_id);
		put_bits(&pb, 2, 3);

		if (ps->EStream[i].codecId == PLAYER_MEDIA_F_VIDEO_TYPE_H264)	/*Video*/
		{
			put_bits(&pb, 1, 1);
			put_bits(&pb, 13, ps->EStream[i].max_buffer_size / 1024);
		}
		else
		{
			put_bits(&pb, 1, 0);
			put_bits(&pb, 13, ps->EStream[i].max_buffer_size / 128);
		}
	}


	flush_put_bits(&pb);
	size = put_bits_ptr(&pb) - pb.buf;
	/* patch packet size */
	buf[4] = (size - 6) >> 8;
	buf[5] = (size - 6) & 0xff;


	return put_bits_ptr(&pb) - pb.buf;
}

int CProgramStreamPacketizer::put_program_stream_map(unsigned char *buf)
{
	int i = 0;
	unsigned int crc32 = 0;
	int size = 0;
	int esm_offset = 0, esinfo_offset = 0;
	PutBitContext pb;

	CProgramStreamPacketizer * ps = this;

	init_put_bits(&pb, buf, 128);

	put_bits32(&pb, PROGRAM_STREAM_MAP);

	put_bits(&pb, 16, 0);	//we fix it later :)

	put_bits(&pb, 1, 1);
	put_bits(&pb, 2, 3);	//reserved
	put_bits(&pb, 5, 0);
	put_bits(&pb, 7, 0x7f);	//reserved
	put_bits(&pb, 1, 1);
	put_bits(&pb, 16, 0);	//program info = 0

	flush_put_bits(&pb);
	esm_offset = put_bits_ptr(&pb) - pb.buf;
	put_bits(&pb, 16, 0);	//we fix it later :)

	for (i = 0; i<ps->nb_streams; i++)
	{
		if (ps->EStream[i].codecId == PLAYER_MEDIA_F_VIDEO_TYPE_H264)
		{
			put_bits(&pb, 8, 0x1B);
			put_bits(&pb, 8, ps->EStream[i].stream_id);

			flush_put_bits(&pb);
			esinfo_offset = put_bits_ptr(&pb) - pb.buf;
			put_bits(&pb, 16, 0);	//we fix it later :)

			flush_put_bits(&pb);
			size = put_avc_video_descriptor(&(ps->EStream[i]), pb.buf_ptr);
			pb.buf_ptr += size;
			buf[esinfo_offset] = (size) >> 8;
			buf[esinfo_offset + 1] = (size)& 0xff;
		}
#if 1
		else if (ps->EStream[i].codecId == PLAYER_MEDIA_F_AUDIO_TYPE_G711)
		{
			put_bits(&pb, 8, 0x90);
			put_bits(&pb, 8, ps->EStream[i].stream_id);
			put_bits(&pb, 16, 0);	//FIXME :<
		}
#endif
	}

	flush_put_bits(&pb);
	size = put_bits_ptr(&pb) - pb.buf;
	buf[esm_offset] = (size - esm_offset - 2) >> 8;
	buf[esm_offset + 1] = (size - esm_offset - 2) & 0xff;

	put_bits32(&pb, 0);	//crc32, fix it later

	flush_put_bits(&pb);
	size = put_bits_ptr(&pb) - pb.buf;

	/* patch packet size */
	buf[4] = (size - 6) >> 8;
	buf[5] = (size - 6) & 0xff;

	crc32 = mpeg_crc32(pb.buf, size - 4);
	pb.buf_ptr[-4] = crc32 >> 24;
	pb.buf_ptr[-3] = (crc32 >> 16) & 0xff;
	pb.buf_ptr[-2] = (crc32 >> 8) & 0xff;
	pb.buf_ptr[-1] = (crc32 & 0xff);

	return put_bits_ptr(&pb) - pb.buf;;
}



//////////////////////  ps 流打包 ///////////////////////


int CProgramStreamPacketizer::addAudio2PStream(PLAYER_MEDIA_F_STREAM_TYPE id, int bitrate, int chn)
{
	CProgramStreamPacketizer * ps = this;
	if (ps->nb_streams >= MAX_ESTREAM)
		return -1;

	ps->EStream[ps->nb_streams].stream_id = 0xC0 + ps->audio_bound;
	ps->EStream[ps->nb_streams].codecId = id;
	ps->EStream[ps->nb_streams].bitrate = bitrate;
	ps->EStream[ps->nb_streams].chn = chn;
	ps->EStream[ps->nb_streams].max_buffer_size = 8 * 1024;

	ps->EStream[ps->nb_streams].packet_size = PES_DATA_SIZE;

	bitrate = (bitrate + 50 - 1) / (50);
	ps->stream_rate[ps->nb_streams] = bitrate;

	if (ps->mux_rate < bitrate)
		ps->mux_rate = bitrate;

	ps->nb_streams++;
	ps->audio_bound++;

	return 0;
}



int CProgramStreamPacketizer::addVideo2PStream(PLAYER_MEDIA_F_STREAM_TYPE id, int width, int height, int bitrate)
{

	CProgramStreamPacketizer * ps = this;
	if (ps->nb_streams >= MAX_ESTREAM)
		return -1;

	ps->EStream[ps->nb_streams].stream_id = 0xE0 + ps->video_bound;
	ps->EStream[ps->nb_streams].codecId = id;
	ps->EStream[ps->nb_streams].bitrate = bitrate;
	ps->EStream[ps->nb_streams].max_buffer_size = bitrate / 8;
	ps->EStream[ps->nb_streams].packet_size = PES_DATA_SIZE;
	ps->EStream[ps->nb_streams].last_src = 0;
	ps->EStream[ps->nb_streams].src = 0;


	bitrate = (bitrate + 8 * 50 - 1) / (8 * 50);
	ps->stream_rate[ps->nb_streams] = bitrate;
	if (ps->mux_rate < bitrate)
		ps->mux_rate = bitrate;

	ps->nb_streams++;
	ps->video_bound++;

	return (ps->nb_streams - 1);
}

int CProgramStreamPacketizer::init_ps(unsigned char *nalBuffer, int nalSize)
{
	int ret = 0;
	int i = 0, idx = 0;
	int start = 0, len = 0;
	int *p = NULL;
	unsigned char *nal_buf = NULL;
	//Codec_H264_Info info;
	GetBitContext gb;
	H264SpsInfo sps;

	memset(&sps, 0, sizeof(sps));
	/*提取SPS信息*/
	{
		len = nalSize;
		nal_buf = nalBuffer;
		/*skip the start code*/
		if (nal_buf[0] == 0 && nal_buf[1] == 0 && nal_buf[2] == 0 && nal_buf[3] == 1) {
			nal_buf += 4;
			len -= 4;
		}
		else if (nal_buf[0] == 0 && nal_buf[1] == 0 && nal_buf[2] == 1) {
			nal_buf += 3;
			len -= 3;
		}


		init_get_bits(&gb, nal_buf, len);
		if ((get_bits(&gb, 8) & 0x1f) != 7)
		{ 
			//envir() << "Not SPS in I Frame \n";
			return -1;
		}

		sps.profile_idc = get_bits(&gb, 8);
		sps.constraint_set0_flag = get_bits(&gb, 1);
		sps.constraint_set1_flag = get_bits(&gb, 1);
		sps.constraint_set2_flag = get_bits(&gb, 1);
		sps.constraint_set3_flag = get_bits(&gb, 1);
		sps.reserved_zero_4bits = get_bits(&gb, 4);
		sps.level_idc = get_bits(&gb, 8);
	}

	
	idx = addVideo2PStream(PLAYER_MEDIA_F_VIDEO_TYPE_H264, 0, 0, 90000 /*VAM_GetBitrate(venc_stream)*/);/*Video 0*/
	if (idx < 0) {
		//envir() << "addVideo2PStream Failed \n";
		return -1;
	}
	idx = addAudio2PStream(PLAYER_MEDIA_F_AUDIO_TYPE_G711, 8000, 1); /*Audio-1*/
	if (idx < 0) {
		//envir() << "addAudio2PStream Failed \n";
		return -1;
	}

	this->first = 1;
	this->cur_stream = 0; /*Video-0 Audio-1*/
	//this->rcFrame = NULL;
	this->EStream[idx].sps = sps;

	return 0;

}

/* is_new_packet = 1 说明输入的数据是采集帧的第一个的 nal 单元 */
/* 每个pes包大小为64000， 如果视频数据帧的大小超过了，会被分割成多个pes包 */
int CProgramStreamPacketizer::ps_packet_video(unsigned char *nalBuffer, int nalSize, int is_new_packet, unsigned long pts)
{
	int idx = 0;
	int remainLen = 0, packetSize = 0;
	int offset = 0, len = 0, start = 0;
	int mux_rate = 0;
	
	CProgramStreamPacketizer *s = this;
	
	if (is_new_packet)
	{
		s->cur_stream = 0; /*video*/
	}

	idx = s->cur_stream;
	packetSize = s->EStream[idx].packet_size;
	
	mux_rate = s->stream_rate[idx];
	
	unsigned char *rc_ptr;
	rc_ptr = nalBuffer;

	/*封包*/
	while (rc_ptr < nalBuffer + nalSize)
	{
		unsigned char pes[PES_DATA_SIZE];
		memset(pes, 0, sizeof(pes));
		offset = 0;
		remainLen = PES_DATA_SIZE;

		/*我们将每帧打入一个pack中*/
		if (is_new_packet)
		{
			s->EStream[idx].src = s->EStream[idx].last_src;
			s->EStream[idx].src = pts;
			s->EStream[idx].last_src = s->EStream[idx].src;
			offset += put_pack_header(&(s->EStream[idx]), (unsigned char*)(pes + offset), s->EStream[idx].src, mux_rate);
		}
		if (s->first)
		{
			offset += put_system_header((unsigned char*)(pes + offset));
			offset += put_program_stream_map((unsigned char*)(pes + offset));
			s->first = 0;
		}

		/*Access unit delimiter*/
		if (is_new_packet)
		{
			unsigned char delimiter[6] = { 0x00, 0x00, 0x00, 0x01, 0x09, 0xF0 };
			len = sizeof(delimiter);
			offset += put_av_pes_packet(&(s->EStream[idx]), (unsigned char*)(pes + offset), PES_DATA_SIZE - offset, pts, 1,
				delimiter, &len, remainLen);
			remainLen = PES_DATA_SIZE - offset;

			is_new_packet = 0;
		}

		remainLen = PES_DATA_SIZE - offset;
		len = (nalBuffer + nalSize) - rc_ptr;

		offset += put_av_pes_packet(&(s->EStream[idx]), (unsigned char*)(pes + offset), PES_DATA_SIZE - offset, pts, 0,
			rc_ptr, &len, remainLen);

		rc_ptr += len;

		if (offset > 0)
		{
			SaveDataToFile(pes, offset, pts);
		}

	}

	return 1;
}

/* 打包成功后的 PS 流包放置在 pes 缓存中 */
int CProgramStreamPacketizer::ps_packet_audio(unsigned char *nalBuffer, int nalSize, int is_new_packet, unsigned long pts)
{
	int idx = 0;
	int remainLen = 0, packetSize = 0;
	int offset = 0, len = 0/*, start = 0*/;
	int mux_rate = 0;
	
	unsigned char *pk_start = NULL, *pes_end = NULL;
	CProgramStreamPacketizer *s = this;
	
	if (is_new_packet)
	{
		s->cur_stream = 1; /*audio*/
		//i=0;
	}

	idx = s->cur_stream;
	packetSize = s->EStream[idx].packet_size;

	mux_rate = s->stream_rate[idx];

	/*封包*/
	mux_rate = s->stream_rate[idx];

	unsigned char *rc_ptr;
	rc_ptr = nalBuffer;

	while (rc_ptr < nalBuffer + nalSize)
	{

		unsigned char pes[PES_DATA_SIZE];
		memset(pes, 0, sizeof(pes));
		offset = 0;
		remainLen = PES_DATA_SIZE;

		/*我们将每帧打入一个pack中*/
		if (is_new_packet)
		{
			s->EStream[idx].src = s->EStream[idx].last_src;
			s->EStream[idx].src = pts;
			s->EStream[idx].last_src = s->EStream[idx].src;
			offset += put_pack_header(&(s->EStream[idx]), (unsigned char*)(pes + offset), s->EStream[idx].src, mux_rate);

			is_new_packet = 0;
		}
		if (s->first)
		{
			offset += put_system_header((unsigned char*)(pes + offset));
			offset += put_program_stream_map((unsigned char*)(pes + offset));
			s->first = 0;
		}

		remainLen = PES_DATA_SIZE - offset;
		len = (nalBuffer + nalSize) - rc_ptr;

		offset += put_av_pes_packet(&(s->EStream[idx]), (unsigned char*)(pes + offset), PES_DATA_SIZE - offset, pts, 0,
			rc_ptr, &len, remainLen);

		rc_ptr += len;

		if (offset > 0)
		{
			SaveDataToFile(pes, offset, pts);
		}

	}

	return 1;
}

void CProgramStreamPacketizer::SaveDataToFile(unsigned char * pes, unsigned pesSize, unsigned long pts)
{
	if (fp == NULL)
	{
		fp = fopen("StreamPacketizer.ps", "wb");
	}
	if (fp)
	{
		fwrite(pes, 1, pesSize, fp);
	}
}

void CProgramStreamPacketizer::PackegeH264Data(unsigned char  *pu8PSData, unsigned int u32PSDataLength,  
	unsigned long pts, PLAYER_MEDIA_F_STREAM_TYPE streamType)
{
		
	if (streamType == PLAYER_MEDIA_F_VIDEO_TYPE_H264)
	{
		int begin = 0;
		int next = 0;
		int lastBegin = -1;
		int lastNext = -1;
		int is_new_packet = 1;
		while ((begin = SearchTag2(pu8PSData, next, u32PSDataLength - next, next)) >= 0)
		{
			if (lastBegin >= 0)
			{
				/*针对每个的 nal 进行 pes 封包*/
				if (0x09 == (pu8PSData[lastNext] & 0x1f))
				{
					/*h264 的分割器,忽略 */
				}
				else
				{
					if (0 == get_first_IFrame)/*未获取到第一个I帧*/
					{
						
						if (init_ps(pu8PSData + lastBegin, begin - lastBegin) >= 0)
						{
							get_first_IFrame = 1;
						}
						else
							return;
					}
					ps_packet_video(pu8PSData + lastBegin, begin - lastBegin, is_new_packet, pts);
					is_new_packet = 0;
				}
			}

			lastBegin = begin;
			lastNext = next;
		}
		if (lastBegin >= 0)
		{
			if (0 == get_first_IFrame)/*未获取到第一个I帧*/
			{

				if (init_ps(pu8PSData + lastBegin, begin - lastBegin) >= 0)
				{
					get_first_IFrame = 1;
				}
				else
					return;
			}
			ps_packet_video(pu8PSData + lastBegin, u32PSDataLength - lastBegin, is_new_packet, pts);
		}
	}
	else
	{	
		/* 音频 */
		if (1 == get_first_IFrame)
		{
			ps_packet_audio(pu8PSData, u32PSDataLength, 1, pts);
		}
	}
		

}


