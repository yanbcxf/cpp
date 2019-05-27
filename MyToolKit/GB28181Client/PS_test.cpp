
#include "StdAfx.h"

FILE * fp_h265 = NULL;

int PesPacketParserCB_general(PS_PACKET_S *pstPSPacket, APSPARSER_H * phParser, APSPARSER_USERDATA *pUserData)
{
	static AVCodec *pCodec = NULL;
	static AVCodecContext *pCodecCtx = NULL;
	static AVCodecParserContext *pCodecParserCtx = NULL;

	AVCodecID codec_id = /*AV_CODEC_ID_HEVC*/ /*AV_CODEC_ID_MPEG2VIDEO*/ AV_CODEC_ID_H264;
	if (pCodec == NULL)
	{
		pCodec = avcodec_find_decoder(codec_id);
		if (!pCodec) {
			printf("Codec not found\n");
		}
		pCodecCtx = avcodec_alloc_context3(pCodec);
		if (!pCodecCtx){
			printf("Could not allocate video codec context\n");
		}

		pCodecParserCtx = av_parser_init(codec_id);
		if (!pCodecParserCtx){
			printf("Could not allocate video parser context\n");
		}
	}

	CTActivePSParser * pParser = (CTActivePSParser *)phParser;
	uint32_t sessionId = (uint32_t)pUserData;

	unsigned char es_id = pstPSPacket->pstPESPacket->es_id & 0xE0;
	if (es_id == 224)
	{
		AVPacket packet;
		av_init_packet(&packet);

		uint8_t *cur_ptr;
		int cur_size;
		cur_ptr = pstPSPacket->pstPESPacket->es_data;
		cur_size = pstPSPacket->pstPESPacket->es_data_length;

		while (cur_size>0){

			int len = av_parser_parse2(
				pCodecParserCtx, pCodecCtx,
				&packet.data, &packet.size,
				cur_ptr, cur_size,
				AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);

			cur_ptr += len;
			cur_size -= len;

			if (packet.size == 0)
				continue;


			if(fp_h265==NULL)
			{
				fp_h265 = fopen("gb_28181_client.h264", "wb");
			}
			if(fp_h265)
				fwrite(packet.data, 1, packet.size, fp_h265);

			//SDL_Delay(40);
		}
	}
	return 0;
}



DWORD WINAPI player_stream_input_data_thread_2(void *arg)
{
	uint32_t sessionId = (uint32_t)arg;

	if(fp_h265)
	{
		fclose(fp_h265);
		fp_h265 = NULL;
	}


	Sleep(40);
	int k = 0;
	for (; k<1; k++)
	{

		FILE *fp_in;

		const int in_buffer_size = 4096;
		uint8_t in_buffer[in_buffer_size + FF_INPUT_BUFFER_PADDING_SIZE] = { 0 };
		uint8_t *cur_ptr;
		int cur_size;

		//char filepath_in[] = "bigbuckbunny_480x272.h264";
		//char filepath_in[] = "ÐÂÎÅÁª²¥.mpg";
		//char filepath_in[] = "ipc133.ps";
		char filepath_in[] = "gb_28181_000141.ps";



		//Input File
		fp_in = fopen(filepath_in, "rb");
		if (!fp_in) {
			printf("Could not open input stream\n");
			break;
		}

		CTActivePSParser * pParser = new CTActivePSParser(PesPacketParserCB_general, (APSPARSER_USERDATA *)sessionId);

		while (1) {

			cur_size = fread(in_buffer, 1, in_buffer_size, fp_in);
			if (cur_size == 0)
				break;
			cur_ptr = in_buffer;

			pParser->ParsePSData(in_buffer, cur_size, 1);

			//player_stream_input_data(sessionId, AVMEDIA_TYPE_VIDEO, packet.data, packet.size, AV_NOPTS_VALUE);
			Sleep(5);

		}
	}
	return 0;
}

void Dll_PS_test()
{
	DWORD m_ThreadId_Callback;
	CreateThread(NULL,  0 , player_stream_input_data_thread_2 , (LPVOID)NULL,  0 , &m_ThreadId_Callback);
}