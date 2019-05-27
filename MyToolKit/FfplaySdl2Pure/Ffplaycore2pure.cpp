
#include "stdafx.h"

extern Logger g_player_log;

const char program_name[] = "ffplay";
const int program_birth_year = 2003;

static int64_t sws_flags = SWS_BICUBIC;

static const char *input_filename;
static const char *window_title;
static int fs_screen_width;
static int fs_screen_height;
static int default_width = 640;
static int default_height = 480;
static int screen_width = 0;
static int screen_height = 0;
static int audio_disable;
static int video_disable;
static int subtitle_disable;
static const char* wanted_stream_spec[AVMEDIA_TYPE_NB] = { 0 };
static int seek_by_bytes = -1;
static int display_disable;
static int show_status = 1;
static int av_sync_type = AV_SYNC_AUDIO_MASTER;
static int64_t start_time = AV_NOPTS_VALUE;
static int64_t duration = AV_NOPTS_VALUE;
static int fast = 0;
static int genpts = 0;
static int lowres = 0;
static int decoder_reorder_pts = -1;
static int autoexit;
static int exit_on_keydown;
static int exit_on_mousedown;
static int loop = 1;
static int framedrop = -1;
static int infinite_buffer = -1;
static enum ShowMode show_mode = SHOW_MODE_NONE;
static const char *audio_codec_name;
static const char *subtitle_codec_name;
static const char *video_codec_name;
double rdftspeed = 0.02;
static int64_t cursor_last_shown;
static int cursor_hidden = 0;
#if CONFIG_AVFILTER
static const char **vfilters_list = NULL;
static int nb_vfilters = 0;
static char *afilters = NULL;
#endif
static int autorotate = 1;

/* current context */
static int is_full_screen;
static int64_t audio_callback_time;

static AVPacket flush_pkt;

int seek_bar_pos;
//专门设置的标记，在程序将要退出的时候会置1
static int exit_remark = 0;

int is_stretch = 1;

//视频帧索引
int vframe_index = 0;
//音频帧索引
int aframe_index = 0;
//Packet索引
int packet_index = 0;

//////////////////////////////////////////////////////////////////////////
//


long  rint(double x)
{
	if(x >= 0.)
		return (long)(x + 0.5);
	else
		return (long)(x - 0.5);
}


//////////////////////////////////////////////////////////////////////////
//	


static void videostate_list_init(VideoStateList *q)
{
	memset(q, 0, sizeof(PacketQueue));
	q->mutex = SDL_CreateMutex();
	q->cond = SDL_CreateCond();
	q->abort_request = 0;
}


//	插入到队尾
static int videostate_list_put(VideoStateList *q, VideoState *pkt)
{
	if (q->abort_request)
		return -1;

	SDL_LockMutex(q->mutex);
	pkt->next = NULL;
	pkt->prev = NULL;
	if (!q->last_pkt)
		q->first_pkt = pkt;
	else
	{
		q->last_pkt->next = pkt;
		pkt->prev = q->last_pkt;
	}
	q->last_pkt = pkt;

	/* XXX: should duplicate packet data in DV case */
	SDL_CondSignal(q->cond);

	SDL_UnlockMutex(q->mutex);
	return 0;
}


/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
static int videostate_list_get_by_windowId(VideoStateList *q, VideoState **pkt, uint32_t windowId)
{
	VideoState *pkt1, *pkt2;
	int ret = 0;

	SDL_LockMutex(q->mutex);
	for (pkt2 = q->first_pkt; pkt2; pkt2 = pkt1) {
		pkt1 = pkt2->next;
		if (pkt2->sdlWindow && SDL_GetWindowID(pkt2->sdlWindow) == windowId)
		{
			if (pkt != NULL)
				*pkt = pkt2;
			ret = 1;
			break;
		}
	}
	SDL_UnlockMutex(q->mutex);
	return ret;
}

int videostate_list_get_by_sessionId(VideoStateList *q, VideoState **pkt, uint32_t sessionId)
{
	VideoState *pkt1, *pkt2;
	int ret = 0;

	SDL_LockMutex(q->mutex);
	for (pkt2 = q->first_pkt; pkt2; pkt2 = pkt1) {
		pkt1 = pkt2->next;
		if (pkt2->sessionId == sessionId)
		{
			if (pkt != NULL)
				*pkt = pkt2;
			ret = 1;
			break;
		}
	}
	SDL_UnlockMutex(q->mutex);
	return ret;
}

//	从队列中删除 某个节点
int videostate_list_pop_by_sessionId(VideoStateList *q, VideoState **pkt, uint32_t sessionId)
{
	VideoState *pkt1, *pkt2;
	int ret = 0;

	SDL_LockMutex(q->mutex);
	for (pkt2 = q->first_pkt; pkt2; pkt2 = pkt1) {
		pkt1 = pkt2->next;
		if (pkt2->sessionId == sessionId)
		{
			if (pkt != NULL)
				*pkt = pkt2;
			
			if(q->first_pkt==pkt2)
			{
				//	删除 第一个节点
				q->first_pkt = pkt2->next;
				if(!q->first_pkt)
					q->last_pkt = NULL;
				else
					q->first_pkt->prev = NULL;
			}
			else if(q->last_pkt==pkt2)
			{
				//	删除 最后节点
				q->last_pkt = pkt2->prev;
				if(!q->last_pkt)
					q->first_pkt = NULL;
				else
					q->last_pkt->next = NULL;
			}
			else
			{
				//	删除 中间节点
				pkt2->prev->next = pkt2->next;
				pkt2->next->prev = pkt2->prev;
			}

			ret = 1;
			break;
		}
	}
	SDL_UnlockMutex(q->mutex);
	return ret;
}

static void videostate_list_destroy(VideoStateList *q)
{
	//packet_queue_flush(q);
	SDL_DestroyMutex(q->mutex);
	SDL_DestroyCond(q->cond);
}

//////////////////////////////////////////////////////////////////////////

#if CONFIG_AVFILTER
static int opt_add_vfilter(void *optctx, const char *opt, const char *arg)
{
	GROW_ARRAY(vfilters_list, nb_vfilters);
	vfilters_list[nb_vfilters - 1] = arg;
	return 0;
}
#endif

static inline
int cmp_audio_fmts(enum AVSampleFormat fmt1, int64_t channel_count1,
enum AVSampleFormat fmt2, int64_t channel_count2)
{
	/* If channel count == 1, planar and non-planar formats are the same */
	if (channel_count1 == 1 && channel_count2 == 1)
		return av_get_packed_sample_fmt(fmt1) != av_get_packed_sample_fmt(fmt2);
	else
		return channel_count1 != channel_count2 || fmt1 != fmt2;
}

static inline
int64_t get_valid_channel_layout(int64_t channel_layout, int channels)
{
	if (channel_layout && av_get_channel_layout_nb_channels(channel_layout) == channels)
		return channel_layout;
	else
		return 0;
}

static void free_picture(Frame *vp);

/* 放置 数据包到 队列中， 如果新包 为 flush_pkt, 说明是新的视频序列， 则队列的 serial++  */
static int packet_queue_put_private(PacketQueue *q, AVPacket *pkt)
{
	MyAVPacketList *pkt1;

	if (q->abort_request)
		return -1;

	pkt1 = (MyAVPacketList*)av_malloc(sizeof(MyAVPacketList));
	if (!pkt1)
		return -1;
	pkt1->pkt = *pkt;
	pkt1->next = NULL;
	if (pkt == &flush_pkt)
		q->serial++;
	pkt1->serial = q->serial;

	if (!q->last_pkt)
		q->first_pkt = pkt1;
	else
		q->last_pkt->next = pkt1;
	q->last_pkt = pkt1;
	q->nb_packets++;
	q->size += pkt1->pkt.size + sizeof(*pkt1);
	/* XXX: should duplicate packet data in DV case */
	SDL_CondSignal(q->cond);
	return 0;
}

static int packet_queue_put(PacketQueue *q, AVPacket *pkt)
{
	int ret;

	/* duplicate the packet */
	if (pkt != &flush_pkt && av_dup_packet(pkt) < 0)
		return -1;

	SDL_LockMutex(q->mutex);
	ret = packet_queue_put_private(q, pkt);
	SDL_UnlockMutex(q->mutex);

	if (pkt != &flush_pkt && ret < 0)
		av_free_packet(pkt);

	return ret;
}

static int packet_queue_put_nullpacket(PacketQueue *q, int stream_index)
{
	AVPacket pkt1, *pkt = &pkt1;
	av_init_packet(pkt);
	pkt->data = NULL;
	pkt->size = 0;
	pkt->stream_index = stream_index;
	return packet_queue_put(q, pkt);
}

/* packet queue handling */
static void packet_queue_init(PacketQueue *q)
{
	memset(q, 0, sizeof(PacketQueue));
	q->mutex = SDL_CreateMutex();
	q->cond = SDL_CreateCond();
	q->abort_request = 1;
}

static void packet_queue_flush(PacketQueue *q)
{
	MyAVPacketList *pkt, *pkt1;

	SDL_LockMutex(q->mutex);
	for (pkt = q->first_pkt; pkt; pkt = pkt1) {
		pkt1 = pkt->next;
		av_free_packet(&pkt->pkt);
		av_freep(&pkt);
	}
	q->last_pkt = NULL;
	q->first_pkt = NULL;
	q->nb_packets = 0;
	q->size = 0;
	SDL_UnlockMutex(q->mutex);
}

static void packet_queue_destroy(PacketQueue *q)
{
	packet_queue_flush(q);
	SDL_DestroyMutex(q->mutex);
	SDL_DestroyCond(q->cond);
}

static void packet_queue_abort(PacketQueue *q)
{
	SDL_LockMutex(q->mutex);

	q->abort_request = 1;

	SDL_CondSignal(q->cond);

	SDL_UnlockMutex(q->mutex);
}

static void packet_queue_start(PacketQueue *q)
{
	SDL_LockMutex(q->mutex);
	q->abort_request = 0;
	packet_queue_put_private(q, &flush_pkt);
	SDL_UnlockMutex(q->mutex);
}

//	block = 1 ,队中无数据时会阻塞
/* return < 0 if aborted, 0 if no packet and > 0 if packet.  */
static int packet_queue_get(PacketQueue *q, AVPacket *pkt, int block, int *serial)
{
	MyAVPacketList *pkt1;
	int ret;

	SDL_LockMutex(q->mutex);

	for (;;) {
		if (q->abort_request) {
			ret = -1;
			break;
		}

		pkt1 = q->first_pkt;
		if (pkt1) {
			q->first_pkt = pkt1->next;
			if (!q->first_pkt)
				q->last_pkt = NULL;
			q->nb_packets--;
			q->size -= pkt1->pkt.size + sizeof(*pkt1);
			*pkt = pkt1->pkt;
			if (serial)
				*serial = pkt1->serial;
			av_free(pkt1);
			ret = 1;
			break;
		}
		else if (!block) {
			ret = 0;
			break;
		}
		else {
			SDL_CondWait(q->cond, q->mutex);
		}
	}
	SDL_UnlockMutex(q->mutex);
	return ret;
}

static void decoder_init(Decoder *d, AVCodecContext *avctx, PacketQueue *queue, SDL_cond *empty_queue_cond) {
	memset(d, 0, sizeof(Decoder));
	d->avctx = avctx;
	d->queue = queue;
	d->empty_queue_cond = empty_queue_cond;
	d->start_pts = AV_NOPTS_VALUE;
}

//	根据解码器 Decode 类型做不同的解码处理
static int decoder_decode_frame(Decoder *d, AVFrame *frame, AVSubtitle *sub) {
	int got_frame = 0;

	do {
		int ret = -1;

		if (d->queue->abort_request)
			return -1;

		if (!d->packet_pending || d->queue->serial != d->pkt_serial) {
			AVPacket pkt;
			do {
				if (d->queue->nb_packets == 0)
					SDL_CondSignal(d->empty_queue_cond);
				if (packet_queue_get(d->queue, &pkt, 1, &d->pkt_serial) < 0)
					return -1;
				if (pkt.data == flush_pkt.data) {
					avcodec_flush_buffers(d->avctx);
					d->finished = 0;
					d->next_pts = d->start_pts;
					d->next_pts_tb = d->start_pts_tb;
				}
			} while (pkt.data == flush_pkt.data || d->queue->serial != d->pkt_serial);
			av_free_packet(&d->pkt);
			d->pkt_temp = d->pkt = pkt;
			d->packet_pending = 1;
		}

		switch (d->avctx->codec_type) {
		case AVMEDIA_TYPE_VIDEO:
			ret = avcodec_decode_video2(d->avctx, frame, &got_frame, &d->pkt_temp);
			if (got_frame) {
				if (decoder_reorder_pts == -1) {
					frame->pts = av_frame_get_best_effort_timestamp(frame);
				}
				else if (decoder_reorder_pts) {
					frame->pts = frame->pkt_pts;
				}
				else {
					frame->pts = frame->pkt_dts;
				}
			}
			break;
		case AVMEDIA_TYPE_AUDIO:
			ret = avcodec_decode_audio4(d->avctx, frame, &got_frame, &d->pkt_temp);
			if (got_frame) {
				AVRational tb; //= (AVRational){1, frame->sample_rate};
				tb.num = 1;
				tb.den = frame->sample_rate;
				if (frame->pts != AV_NOPTS_VALUE)
					frame->pts = av_rescale_q(frame->pts, d->avctx->time_base, tb);
				else if (frame->pkt_pts != AV_NOPTS_VALUE)
					frame->pts = av_rescale_q(frame->pkt_pts, av_codec_get_pkt_timebase(d->avctx), tb);
				else if (d->next_pts != AV_NOPTS_VALUE)
					frame->pts = av_rescale_q(d->next_pts, d->next_pts_tb, tb);
				if (frame->pts != AV_NOPTS_VALUE) {
					d->next_pts = frame->pts + frame->nb_samples;
					d->next_pts_tb = tb;
				}
			}
			break;
		case AVMEDIA_TYPE_SUBTITLE:
			ret = avcodec_decode_subtitle2(d->avctx, sub, &got_frame, &d->pkt_temp);
			break;
		}

		if (ret < 0) {
			d->packet_pending = 0;
		}
		else {
			d->pkt_temp.dts =
				d->pkt_temp.pts = AV_NOPTS_VALUE;
			if (d->pkt_temp.data) {
				if (d->avctx->codec_type != AVMEDIA_TYPE_AUDIO)
					ret = d->pkt_temp.size;
				d->pkt_temp.data += ret;
				d->pkt_temp.size -= ret;
				if (d->pkt_temp.size <= 0)
					d->packet_pending = 0;
			}
			else {
				if (!got_frame) {
					d->packet_pending = 0;
					d->finished = d->pkt_serial;
				}
			}
		}
	} while (!got_frame && !d->finished);

	return got_frame;
}

static void decoder_destroy(Decoder *d) {
	av_free_packet(&d->pkt);
}

static void frame_queue_unref_item(Frame *vp)
{
	av_frame_unref(vp->frame);
	avsubtitle_free(&vp->sub);
}

static int frame_queue_init(FrameQueue *f, PacketQueue *pktq, int max_size, int keep_last)
{
	int i;
	memset(f, 0, sizeof(FrameQueue));
	if (!(f->mutex = SDL_CreateMutex()))
		return AVERROR(ENOMEM);
	if (!(f->cond = SDL_CreateCond()))
		return AVERROR(ENOMEM);
	f->pktq = pktq;
	f->max_size = FFMIN(max_size, FRAME_QUEUE_SIZE);
	f->keep_last = !!keep_last;
	for (i = 0; i < f->max_size; i++)
		if (!(f->queue[i].frame = av_frame_alloc()))
			return AVERROR(ENOMEM);
	return 0;
}

static void frame_queue_destory(FrameQueue *f)
{
	int i;
	for (i = 0; i < f->max_size; i++) {
		Frame *vp = &f->queue[i];
		frame_queue_unref_item(vp);
		av_frame_free(&vp->frame);
		free_picture(vp);
	}
	SDL_DestroyMutex(f->mutex);
	SDL_DestroyCond(f->cond);
}

static void frame_queue_signal(FrameQueue *f)
{
	SDL_LockMutex(f->mutex);
	SDL_CondSignal(f->cond);
	SDL_UnlockMutex(f->mutex);
}

//	获取当前要显示的帧，除了第一次video_refresh 调用时 rindex_shown = 0 ,其余 rindex_shown = 1
static Frame *frame_queue_peek(FrameQueue *f)
{
	return &f->queue[(f->rindex + f->rindex_shown) % f->max_size];
}

static Frame *frame_queue_peek_next(FrameQueue *f)
{
	return &f->queue[(f->rindex + f->rindex_shown + 1) % f->max_size];
}

static Frame *frame_queue_peek_last(FrameQueue *f)
{
	return &f->queue[f->rindex];
}

static Frame *frame_queue_peek_writable(FrameQueue *f)
{
	/* wait until we have space to put a new frame */
	SDL_LockMutex(f->mutex);
	while (f->size >= f->max_size &&
		!f->pktq->abort_request) {
		SDL_CondWait(f->cond, f->mutex);
	}
	SDL_UnlockMutex(f->mutex);

	if (f->pktq->abort_request)
		return NULL;

	return &f->queue[f->windex];
}

static Frame *frame_queue_peek_readable(FrameQueue *f)
{
	/* wait until we have a readable a new frame */
	SDL_LockMutex(f->mutex);
	while (f->size - f->rindex_shown <= 0 &&
		!f->pktq->abort_request) {
		SDL_CondWait(f->cond, f->mutex);
	}
	SDL_UnlockMutex(f->mutex);

	if (f->pktq->abort_request)
		return NULL;

	return &f->queue[(f->rindex + f->rindex_shown) % f->max_size];
}

static void frame_queue_push(FrameQueue *f)
{
	if (++f->windex == f->max_size)
		f->windex = 0;
	SDL_LockMutex(f->mutex);
	f->size++;
	SDL_CondSignal(f->cond);
	SDL_UnlockMutex(f->mutex);
}

static void frame_queue_next(FrameQueue *f)
{
	if (f->keep_last && !f->rindex_shown) {
		f->rindex_shown = 1;
		return;
	}
	//	将该帧清空，以便提供给新的帧使用
	frame_queue_unref_item(&f->queue[f->rindex]);
	if (++f->rindex == f->max_size)
		f->rindex = 0;
	SDL_LockMutex(f->mutex);
	f->size--;
	SDL_CondSignal(f->cond);
	SDL_UnlockMutex(f->mutex);
}

/* jump back to the previous frame if available by resetting rindex_shown */
static int frame_queue_prev(FrameQueue *f)
{
	int ret = f->rindex_shown;
	f->rindex_shown = 0;
	return ret;
}

/* return the number of undisplayed frames in the queue */
static int frame_queue_nb_remaining(FrameQueue *f)
{
	return f->size - f->rindex_shown;
}

/* return last shown position */
static int64_t frame_queue_last_pos(FrameQueue *f)
{
	Frame *fp = &f->queue[f->rindex];
	if (f->rindex_shown && fp->serial == f->pktq->serial)
		return fp->pos;
	else
		return -1;
}

static void decoder_abort(Decoder *d, FrameQueue *fq)
{
	packet_queue_abort(d->queue);
	frame_queue_signal(fq);
	SDL_WaitThread(d->decoder_tid, NULL);
	d->decoder_tid = NULL;
	packet_queue_flush(d->queue);
}

static inline void fill_rectangle(VideoState *is,
	int x, int y, int w, int h, int color, int update)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	/*SDL_FillRect(screen, &rect, color);
	if (update && w > 0 && h > 0)
	SDL_UpdateRect(screen, x, y, w, h);*/

	if (update && w > 0 && h > 0)
	{
		SDL_Texture * sdlTextureBorder = SDL_CreateTexture(is->sdlRenderer, SDL_PIXELFORMAT_RGB24,
			SDL_TEXTUREACCESS_STREAMING, w, h);

		SDL_Surface * sdlSurfaceBorder = SDL_CreateRGBSurface(0, w, h, 24,
			0x00FF0000,
			0x0000FF00,
			0x000000FF,
			0xFF000000);

		color = SDL_MapRGB(sdlSurfaceBorder->format, 0x00, 0xff, 0x00);

		SDL_FillRect(sdlSurfaceBorder, NULL, color);
		SDL_UpdateTexture(sdlTextureBorder, NULL, sdlSurfaceBorder->pixels, sdlSurfaceBorder->pitch);
		SDL_RenderCopy(is->sdlRenderer, sdlTextureBorder, NULL, &rect);

		SDL_DestroyTexture(sdlTextureBorder);
	}
}


/* draw only the border of a rectangle */
static void fill_border(VideoState *is, int xleft, int ytop, int width, int height, int x, int y, int w, int h, int color, int update)
{
	int w1, w2, h1, h2;

	/* fill the background */
	w1 = x;
	if (w1 < 0)
		w1 = 0;
	w2 = width - (x + w);
	if (w2 < 0)
		w2 = 0;
	h1 = y;
	if (h1 < 0)
		h1 = 0;
	h2 = height - (y + h);
	if (h2 < 0)
		h2 = 0;
	fill_rectangle(is,
		xleft, ytop,
		w1, height,
		color, update);
	fill_rectangle(is,
		xleft + width - w2, ytop,
		w2, height,
		color, update);
	fill_rectangle(is,
		xleft + w1, ytop,
		width - w1 - w2, h1,
		color, update);
	fill_rectangle(is,
		xleft + w1, ytop + height - h2,
		width - w1 - w2, h2,
		color, update);
}

#define ALPHA_BLEND(a, oldp, newp, s)\
((((oldp << s) * (255 - (a))) + (newp * (a))) / (255 << s))

#define RGBA_IN(r, g, b, a, s)\
{\
    unsigned int v = ((const uint32_t *)(s))[0];\
    a = (v >> 24) & 0xff;\
    r = (v >> 16) & 0xff;\
    g = (v >> 8) & 0xff;\
    b = v & 0xff;\
}

#define YUVA_IN(y, u, v, a, s, pal)\
{\
    unsigned int val = ((const uint32_t *)(pal))[*(const uint8_t*)(s)];\
    a = (val >> 24) & 0xff;\
    y = (val >> 16) & 0xff;\
    u = (val >> 8) & 0xff;\
    v = val & 0xff;\
}

#define YUVA_OUT(d, y, u, v, a)\
{\
    ((uint32_t *)(d))[0] = (a << 24) | (y << 16) | (u << 8) | v;\
}


#define BPP 1

static void blend_subrect(AVPicture *dst, const AVSubtitleRect *rect, int imgw, int imgh)
{
	int wrap, wrap3, width2, skip2;
	int y, u, v, a, u1, v1, a1, w, h;
	uint8_t *lum, *cb, *cr;
	const uint8_t *p;
	const uint32_t *pal;
	int dstx, dsty, dstw, dsth;

	dstw = av_clip(rect->w, 0, imgw);
	dsth = av_clip(rect->h, 0, imgh);
	dstx = av_clip(rect->x, 0, imgw - dstw);
	dsty = av_clip(rect->y, 0, imgh - dsth);
	lum = dst->data[0] + dsty * dst->linesize[0];
	cb = dst->data[1] + (dsty >> 1) * dst->linesize[1];
	cr = dst->data[2] + (dsty >> 1) * dst->linesize[2];

	width2 = ((dstw + 1) >> 1) + (dstx & ~dstw & 1);
	skip2 = dstx >> 1;
	wrap = dst->linesize[0];
	wrap3 = rect->pict.linesize[0];
	p = rect->pict.data[0];
	pal = (const uint32_t *)rect->pict.data[1];  /* Now in YCrCb! */

	if (dsty & 1) {
		lum += dstx;
		cb += skip2;
		cr += skip2;

		if (dstx & 1) {
			YUVA_IN(y, u, v, a, p, pal);
			lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
			cb[0] = ALPHA_BLEND(a >> 2, cb[0], u, 0);
			cr[0] = ALPHA_BLEND(a >> 2, cr[0], v, 0);
			cb++;
			cr++;
			lum++;
			p += BPP;
		}
		for (w = dstw - (dstx & 1); w >= 2; w -= 2) {
			YUVA_IN(y, u, v, a, p, pal);
			u1 = u;
			v1 = v;
			a1 = a;
			lum[0] = ALPHA_BLEND(a, lum[0], y, 0);

			YUVA_IN(y, u, v, a, p + BPP, pal);
			u1 += u;
			v1 += v;
			a1 += a;
			lum[1] = ALPHA_BLEND(a, lum[1], y, 0);
			cb[0] = ALPHA_BLEND(a1 >> 2, cb[0], u1, 1);
			cr[0] = ALPHA_BLEND(a1 >> 2, cr[0], v1, 1);
			cb++;
			cr++;
			p += 2 * BPP;
			lum += 2;
		}
		if (w) {
			YUVA_IN(y, u, v, a, p, pal);
			lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
			cb[0] = ALPHA_BLEND(a >> 2, cb[0], u, 0);
			cr[0] = ALPHA_BLEND(a >> 2, cr[0], v, 0);
			p++;
			lum++;
		}
		p += wrap3 - dstw * BPP;
		lum += wrap - dstw - dstx;
		cb += dst->linesize[1] - width2 - skip2;
		cr += dst->linesize[2] - width2 - skip2;
	}
	for (h = dsth - (dsty & 1); h >= 2; h -= 2) {
		lum += dstx;
		cb += skip2;
		cr += skip2;

		if (dstx & 1) {
			YUVA_IN(y, u, v, a, p, pal);
			u1 = u;
			v1 = v;
			a1 = a;
			lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
			p += wrap3;
			lum += wrap;
			YUVA_IN(y, u, v, a, p, pal);
			u1 += u;
			v1 += v;
			a1 += a;
			lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
			cb[0] = ALPHA_BLEND(a1 >> 2, cb[0], u1, 1);
			cr[0] = ALPHA_BLEND(a1 >> 2, cr[0], v1, 1);
			cb++;
			cr++;
			p += -wrap3 + BPP;
			lum += -wrap + 1;
		}
		for (w = dstw - (dstx & 1); w >= 2; w -= 2) {
			YUVA_IN(y, u, v, a, p, pal);
			u1 = u;
			v1 = v;
			a1 = a;
			lum[0] = ALPHA_BLEND(a, lum[0], y, 0);

			YUVA_IN(y, u, v, a, p + BPP, pal);
			u1 += u;
			v1 += v;
			a1 += a;
			lum[1] = ALPHA_BLEND(a, lum[1], y, 0);
			p += wrap3;
			lum += wrap;

			YUVA_IN(y, u, v, a, p, pal);
			u1 += u;
			v1 += v;
			a1 += a;
			lum[0] = ALPHA_BLEND(a, lum[0], y, 0);

			YUVA_IN(y, u, v, a, p + BPP, pal);
			u1 += u;
			v1 += v;
			a1 += a;
			lum[1] = ALPHA_BLEND(a, lum[1], y, 0);

			cb[0] = ALPHA_BLEND(a1 >> 2, cb[0], u1, 2);
			cr[0] = ALPHA_BLEND(a1 >> 2, cr[0], v1, 2);

			cb++;
			cr++;
			p += -wrap3 + 2 * BPP;
			lum += -wrap + 2;
		}
		if (w) {
			YUVA_IN(y, u, v, a, p, pal);
			u1 = u;
			v1 = v;
			a1 = a;
			lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
			p += wrap3;
			lum += wrap;
			YUVA_IN(y, u, v, a, p, pal);
			u1 += u;
			v1 += v;
			a1 += a;
			lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
			cb[0] = ALPHA_BLEND(a1 >> 2, cb[0], u1, 1);
			cr[0] = ALPHA_BLEND(a1 >> 2, cr[0], v1, 1);
			cb++;
			cr++;
			p += -wrap3 + BPP;
			lum += -wrap + 1;
		}
		p += wrap3 + (wrap3 - dstw * BPP);
		lum += wrap + (wrap - dstw - dstx);
		cb += dst->linesize[1] - width2 - skip2;
		cr += dst->linesize[2] - width2 - skip2;
	}
	/* handle odd height */
	if (h) {
		lum += dstx;
		cb += skip2;
		cr += skip2;

		if (dstx & 1) {
			YUVA_IN(y, u, v, a, p, pal);
			lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
			cb[0] = ALPHA_BLEND(a >> 2, cb[0], u, 0);
			cr[0] = ALPHA_BLEND(a >> 2, cr[0], v, 0);
			cb++;
			cr++;
			lum++;
			p += BPP;
		}
		for (w = dstw - (dstx & 1); w >= 2; w -= 2) {
			YUVA_IN(y, u, v, a, p, pal);
			u1 = u;
			v1 = v;
			a1 = a;
			lum[0] = ALPHA_BLEND(a, lum[0], y, 0);

			YUVA_IN(y, u, v, a, p + BPP, pal);
			u1 += u;
			v1 += v;
			a1 += a;
			lum[1] = ALPHA_BLEND(a, lum[1], y, 0);
			cb[0] = ALPHA_BLEND(a1 >> 2, cb[0], u, 1);
			cr[0] = ALPHA_BLEND(a1 >> 2, cr[0], v, 1);
			cb++;
			cr++;
			p += 2 * BPP;
			lum += 2;
		}
		if (w) {
			YUVA_IN(y, u, v, a, p, pal);
			lum[0] = ALPHA_BLEND(a, lum[0], y, 0);
			cb[0] = ALPHA_BLEND(a >> 2, cb[0], u, 0);
			cr[0] = ALPHA_BLEND(a >> 2, cr[0], v, 0);
		}
	}
}

static void free_picture(Frame *vp)
{
	if (vp->bmp) {
		//SDL_FreeYUVOverlay(vp->bmp);
		SDL_DestroyTexture(vp->bmp);
		vp->bmp = NULL;
	}
}

static void calculate_display_rect(SDL_Rect *rect,
	int scr_xleft, int scr_ytop, int scr_width, int scr_height,
	int pic_width, int pic_height, AVRational pic_sar)
{
	float aspect_ratio;
	int width, height, x, y;

	if (pic_sar.num == 0)
		aspect_ratio = 0;
	else
		aspect_ratio = av_q2d(pic_sar);

	if (aspect_ratio <= 0.0)
		aspect_ratio = 1.0;
	aspect_ratio *= (float)pic_width / (float)pic_height;

	/* XXX: we suppose the screen has a 1.0 pixel ratio */
	height = scr_height;
	width = ((int)rint(height * aspect_ratio)) & ~1;
	if (width > scr_width) {
		width = scr_width;
		height = ((int)rint(width / aspect_ratio)) & ~1;
	}
	x = (scr_width - width) / 2;
	y = (scr_height - height) / 2;
	rect->x = scr_xleft + x;
	rect->y = scr_ytop + y;
	rect->w = FFMAX(width, 1);
	rect->h = FFMAX(height, 1);
}


static void video_image_display(VideoState *is)
{
	Frame *vp;
	Frame *sp;
	AVPicture pict;
	SDL_Rect rect;
	int i;

	vp = frame_queue_peek(&is->pictq);
	if (vp->bmp) {
		
		/* 该函数获得的 尺寸会随着窗口大小改变而改变 */
		int winSizeW, winSizeH;
		SDL_GetWindowSize(is->sdlWindow, &winSizeW, &winSizeH);

		calculate_display_rect(&rect, is->xleft, is->ytop, is->width, is->height, vp->width, vp->height, vp->sar);

		
		/* 以下会将背景刷成 红色，并且画了一条斜对角线, 调试时使用  */
		/*SDL_SetRenderDrawColor(is->sdlRenderer, 0x00, 0xff, 0, 0);
		SDL_RenderDrawLine(is->sdlRenderer, 0, 0, is->width - 1, is->height - 1);
		SDL_SetRenderDrawColor(is->sdlRenderer, 0xff, 0, 0, 0);*/

		int ww, hh;
		SDL_RenderGetLogicalSize(is->sdlRenderer, &ww, &hh);


		SDL_RenderCopy(is->sdlRenderer, vp->bmp, NULL, &rect);

		// 当屏幕的宽高比与  视频的宽高比不同时，将多出的边框部分刷成黑色
		if (rect.x != is->last_display_rect.x || rect.y != is->last_display_rect.y || rect.w != is->last_display_rect.w 
			|| rect.h != is->last_display_rect.h || is->force_refresh) {
			int bgcolor = 0 /*SDL_MapRGB(is->sdlSurfaceBorder->format, 0x00, 0x00, 0x00)*/;
			
			//	该函数不需要调用，因为 边框时靠 SDL_SetRenderDrawColor 
			//fill_border(is, is->xleft, is->ytop, is->width, is->height, rect.x, rect.y, rect.w, rect.h, bgcolor, 1);
			is->last_display_rect = rect;
		}
	}
}

static inline int compute_mod(int a, int b)
{
	return a < 0 ? a%b + b : a%b;
}

static void video_audio_display(VideoState *s)
{

}

void stream_close(VideoState *is)
{
	//	取消视频刷新
	if (is->timer>0)
		SDL_RemoveTimer(is->timer);

	/* XXX: use a special url_shutdown call to abort parse cleanly */
	// 发出 结束命令， 在read_thread 进行结束操作
	is->abort_request = 1;
	SDL_WaitThread(is->read_tid, NULL);
	packet_queue_destroy(&is->videoq);
	packet_queue_destroy(&is->audioq);
	//packet_queue_destroy(&is->subtitleq);

	/* free all pictures */
	frame_queue_destory(&is->pictq);
	frame_queue_destory(&is->sampq);
	// frame_queue_destory(&is->subpq);

	if ( is->sdlRenderer != NULL )
	{
		SDL_DestroyRenderer( is->sdlRenderer );
		is->sdlRenderer = NULL;
	}

	if ( NULL != is->sdlWindow )
	{
		/*注意在SDL_CreateWindowFrom中sdl将窗口的消息处理函数地址给改写成sdl的函数了
		（在sdl源文件中SetupWindowData（）此函数内修改），
		如下函数 函数中会恢复hwnd默认消息处理函数 */
		SDL_DestroyWindow( is->sdlWindow );
		is->sdlWindow = NULL;
	}

	SDL_DestroyCond(is->continue_read_thread);
#if !CONFIG_AVFILTER
	sws_freeContext(is->img_convert_ctx);
#endif
	av_free(is);
}



/*static*/ void do_exit(VideoState *is)
{
	if (is) {
		stream_close(is);
	}
	av_lockmgr_register(NULL);
	// uninit_opts();
#if CONFIG_AVFILTER
	av_freep(&vfilters_list);
#endif
	avformat_network_deinit();
	if (show_status)
		printf("\n");
	SDL_Quit();
	av_log(NULL, AV_LOG_QUIET, "%s", "");
	exit(0);
}

static void sigterm_handler(int sig)
{
	exit(123);
}

static void set_default_window_size(int width, int height, AVRational sar)
{
	SDL_Rect rect;
	calculate_display_rect(&rect, 0, 0, INT_MAX, height, width, height, sar);
	default_width = rect.w;
	default_height = rect.h;
}


Uint32 refresh_callback(Uint32 interval, void *opaque);


static int video_open(VideoState *is, int force_set_video_mode, Frame *vp)
{
	int flags = SDL_WINDOW_OPENGL;
	int w, h;

	if (is_full_screen) flags |= SDL_WINDOW_FULLSCREEN;
	else                flags |= SDL_WINDOW_RESIZABLE;

	// 计算要创建的窗口尺寸
	if (vp && vp->width)
		set_default_window_size(vp->width, vp->height, vp->sar);

	if (is_full_screen && fs_screen_width) {
		w = fs_screen_width;
		h = fs_screen_height;
	}
	else if (!is_full_screen && screen_width) {
		w = screen_width;
		h = screen_height;
	}
	else {
		w = default_width;
		h = default_height;
	}
	w = FFMIN(16383, w);

	//	窗口已经存在且尺寸满足要求，则不创建
	if (is->sdlWindow)
		return 0;
	/*if (screen && is->width == screen->w && screen->w == w
	&& is->height== screen->h && screen->h == h && !force_set_video_mode)
	return 0;*/

	//	创建窗口
	/* screen = SDL_SetVideoMode(w, h, 0, flags);
	if (!screen) {
	av_log(NULL, AV_LOG_FATAL, "SDL: could not set video mode - exiting\n");
	do_exit(is);
	}*/

	is->sdlWindow = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags);
	if (!is->sdlWindow) {
		av_log(NULL, AV_LOG_FATAL, "SDL: could not set window - exiting\n");
		do_exit(is);
	}

	is->sdlRenderer = SDL_CreateRenderer(is->sdlWindow, -1, 0);
	if (!is->sdlRenderer) {
		av_log(NULL, AV_LOG_FATAL, "SDL: could not set renderer - exiting\n");
		do_exit(is);
	}
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");  // make the scaled rendering look smoother.
	SDL_RenderSetLogicalSize(is->sdlRenderer, w, h);


	if (!window_title)
		window_title = input_filename;
	//SDL_WM_SetCaption(window_title, window_title);
	SDL_SetWindowTitle(is->sdlWindow, window_title);

	//	创建边框的填充面
	/*is->sdlTextureBorder = SDL_CreateTexture(is->sdlRenderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING,
	640, 480);

	is->sdlSurfaceBorder = SDL_CreateRGBSurface(0, 640, 480, 24,
	0x00FF0000,
	0x0000FF00,
	0x000000FF,
	0xFF000000);

	if (!is->sdlTextureBorder || !is->sdlSurfaceBorder) {
	av_log(NULL, AV_LOG_FATAL, "SDL: could not set sdlTextureBorder or sdlSurfaceBorder - exiting\n");
	do_exit(is);
	}*/

	is->width = w;
	is->height = h;

	return 0;
}

/* display the current picture, if any */
static void video_display(VideoState *is)
{

	//	窗口未创建，则先建立
	if (!is->sdlWindow) 
		video_open(is, 0, NULL);

	int ret = SDL_RenderClear(is->sdlRenderer);	// SDL 2.0

	if(ret<0)
	{
		int kkk =1;
	}

	if (/*is->audio_st*/ 1 && is->show_mode != SHOW_MODE_VIDEO)
		video_audio_display(is);
	else if (is->video_st)
		video_image_display(is);


	/*
		视频刷新问题， 每打开一路视频，就会在 SDL2 的定时器线程中增加注册一个的 刷新函数 refresh_callback，
		如果有一路的刷新函数执行的慢，就会影响所有的视频。
		视频慢动作的原因 ： 
			SDL_RenderPresent 使用 d3d 时调用慢， 使用 software 时不慢但 cpu 高
		调用哪个驱动由 SDL_CreateRenderer 决定
	*/
	SDL_RenderPresent(is->sdlRenderer);	// SDL 2.0
}

/*
get_clock(&is->vidclk):
获取到的实际上是:最后一帧的pts 加上 从处理最后一帧开始到现在的时间,具体参考set_clock_at 和get_clock的代码
c->pts_drift=最后一帧的pts-从处理最后一帧时间
clock=c->pts_drift+现在的时候
get_clock(&is->vidclk) ==is->vidclk.pts, av_gettime_relative() / 1000000.0 -is->vidclk.last_updated  +is->vidclk.pts
*/

/*yangbin:	返回 当前时间 time 该时钟 Clock 对应的 PTS 时间
speed 应该时快进 或者 慢放 */
static double get_clock(Clock *c)
{
	if (*c->queue_serial != c->serial)
		return NAN;
	if (c->paused) {
		return c->pts;
	}
	else {
		double time = av_gettime_relative() / 1000000.0;
		return c->pts_drift + time - (time - c->last_updated) * (1.0 - c->speed);
	}
}

//	设置时钟 C 的对应关系 ： pts 与 time 对应
static void set_clock_at(Clock *c, double pts, int serial, double time)
{
	c->pts = pts;
	c->last_updated = time;
	c->pts_drift = c->pts - time;
	c->serial = serial;
}

static void set_clock(Clock *c, double pts, int serial)
{
	double time = av_gettime_relative() / 1000000.0;
	set_clock_at(c, pts, serial, time);
}

static void set_clock_speed(Clock *c, double speed)
{
	set_clock(c, get_clock(c), c->serial);
	c->speed = speed;
}

static void init_clock(Clock *c, int *queue_serial)
{
	c->speed = 1.0;
	c->paused = 0;
	c->queue_serial = queue_serial;
	set_clock(c, NAN, -1);
}

/*	将 slave 时钟同步 （赋值到）c 时钟  */
/* 该函数仅在 图片显示（update_video_pts）和 音频播放(sdl_audio_callback) 时，来同步（赋值到）is->extclk */
static void sync_clock_to_slave(Clock *c, Clock *slave)
{
	double clock = get_clock(c);
	double slave_clock = get_clock(slave);
	if (!isnan(slave_clock) && (isnan(clock) || fabs(clock - slave_clock) > AV_NOSYNC_THRESHOLD))
		set_clock(c, slave_clock, slave->serial);
}

static int get_master_sync_type(VideoState *is) {
	if (is->av_sync_type == AV_SYNC_VIDEO_MASTER) {
		if (is->video_st)
			return AV_SYNC_VIDEO_MASTER;
		else
			return AV_SYNC_AUDIO_MASTER;
	}
	else if (is->av_sync_type == AV_SYNC_AUDIO_MASTER) {
		if ( 1 /*is->audio_st*/)
			return AV_SYNC_AUDIO_MASTER;
		else
			return AV_SYNC_EXTERNAL_CLOCK;
	}
	else {
		return AV_SYNC_EXTERNAL_CLOCK;
	}
}

/* get the current master clock value */
static double get_master_clock(VideoState *is)
{
	double val;

	switch (get_master_sync_type(is)) {
	case AV_SYNC_VIDEO_MASTER:
		val = get_clock(&is->vidclk);
		break;
	case AV_SYNC_AUDIO_MASTER:
		val = get_clock(&is->audclk);
		break;
	default:
		val = get_clock(&is->extclk);
		break;
	}
	return val;
}

static void check_external_clock_speed(VideoState *is) {
	if (is->video_stream >= 0 && is->videoq.nb_packets <= MIN_FRAMES / 2 ||
		is->audio_stream >= 0 && is->audioq.nb_packets <= MIN_FRAMES / 2) {
		//	当流中的包数太小时，降低外部的行走速度
		set_clock_speed(&is->extclk, FFMAX(EXTERNAL_CLOCK_SPEED_MIN, is->extclk.speed - EXTERNAL_CLOCK_SPEED_STEP));
	}
	else if ((is->video_stream < 0 || is->videoq.nb_packets > MIN_FRAMES * 2) &&
		(is->audio_stream < 0 || is->audioq.nb_packets > MIN_FRAMES * 2)) {
		//	当流中的包数太多时，加快外部的行走速度
		set_clock_speed(&is->extclk, FFMIN(EXTERNAL_CLOCK_SPEED_MAX, is->extclk.speed + EXTERNAL_CLOCK_SPEED_STEP));
	}
	else {
		double speed = is->extclk.speed;
		if (speed != 1.0)
			set_clock_speed(&is->extclk, speed + EXTERNAL_CLOCK_SPEED_STEP * (1.0 - speed) / fabs(1.0 - speed));
	}
}


void stream_speed(VideoState *is, double speed)
{
	set_clock_speed(&is->vidclk, speed);
	set_clock_speed(&is->audclk, speed);

	if(fabs(speed-1.0) < 0.000001)
		is->av_sync_type = AV_SYNC_AUDIO_MASTER;
	else
		is->av_sync_type = AV_SYNC_VIDEO_MASTER;
}


/* seek in the stream */
static void stream_seek(VideoState *is, int64_t pos, int64_t rel, int seek_by_bytes)
{
	if (!is->seek_req) {
		is->seek_pos = pos;
		is->seek_rel = rel;
		is->seek_flags &= ~AVSEEK_FLAG_BYTE;
		if (seek_by_bytes)
			is->seek_flags |= AVSEEK_FLAG_BYTE;
		is->seek_req = 1;
		SDL_CondSignal(is->continue_read_thread);
	}
}

/* pause or resume the video */
static void stream_toggle_pause(VideoState *is)
{
	if (is->paused) {
		/* last_updated 记录了上一帧视频图像显示时的系统时钟， av_gettime_relative() - last_updated得到的结果
		刚好是pause这段时间间隔，通过这种方式保证了frame_timer永远记录的是ffplay启动后到当前时间点的时间间隔  */

		/* 从暂停到播放，修改 上一帧的播放时间， 以系统时钟为单位 */
		is->frame_timer += av_gettime_relative() / 1000000.0 - is->vidclk.last_updated;
		if (is->read_pause_return != AVERROR(ENOSYS)) {
			is->vidclk.paused = 0;
		}
		/* 将暂停时的 PTS 与 当前系统时钟对应 */
		set_clock(&is->vidclk, get_clock(&is->vidclk), is->vidclk.serial);
	}
	set_clock(&is->extclk, get_clock(&is->extclk), is->extclk.serial);
	is->paused = is->audclk.paused = is->vidclk.paused = is->extclk.paused = !is->paused;
}

static void toggle_pause(VideoState *is)
{
	stream_toggle_pause(is);
	is->step = 0;
}

static void step_to_next_frame(VideoState *is)
{
	/* if the stream is paused unpause it, then step */
	if (is->paused)
		stream_toggle_pause(is);
	is->step = 1;
}

static double compute_target_delay(double delay, VideoState *is)
{
	double sync_threshold, diff = 0;

	/* update delay to follow master synchronisation source */
	if (get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER) {

		/* if video is slave, we try to correct big delays by
		duplicating or deleting a frame */

		/*我们通过复制和删除一帧来纠正大的延时*/
		diff = get_clock(&is->vidclk) - get_master_clock(is);

		/* skip or repeat frame. We take into account the
		delay to compute the threshold. I still don't know
		if it is the best guess */
		sync_threshold = FFMAX(AV_SYNC_THRESHOLD_MIN, FFMIN(AV_SYNC_THRESHOLD_MAX, delay));
		if (!isnan(diff) && fabs(diff) < is->max_frame_duration)
		{
			if (diff <= -sync_threshold)	/*当前视频帧落后于主时钟源*/
				delay = FFMAX(0, delay + diff);
			else if (diff >= sync_threshold && delay > AV_SYNC_FRAMEDUP_THRESHOLD)
			{
				/*大概意思是:本来当视频帧超前的时候,
				我们应该要选择重复该帧或者下面的2倍延时(即加重延时的策略),
				但因为该帧的显示时间大于显示更新门槛,
				所以这个时候不应该以该帧做同步*/
				delay = delay + diff;
			}
			else if (diff >= sync_threshold)
				delay = 2 * delay;
		}
	}

	av_log(NULL, AV_LOG_TRACE, "video: delay=%0.3f A-V=%f\n",
		delay, -diff);

	return delay;
}

static double vp_duration(VideoState *is, Frame *vp, Frame *nextvp) {
	if (vp->serial == nextvp->serial) {
		double duration = nextvp->pts - vp->pts;
		if (isnan(duration) || duration <= 0 || duration > is->max_frame_duration)
			return vp->duration;
		else
			return duration;
	}
	else {
		return 0.0;
	}
}

/* 使用最新要显示的图片帧中 pts，来刷新视频时钟 */
static void update_video_pts(VideoState *is, double pts, int64_t pos, int serial) {
	/* update current video pts */
	set_clock(&is->vidclk, pts, serial);
	sync_clock_to_slave(&is->extclk, &is->vidclk);
}


/* called to display each frame */
static void video_refresh(void *opaque, double *remaining_time)
{
	VideoState *is = (VideoState *)opaque;
	double time;

	Frame *sp, *sp2;

	if (!is->paused && get_master_sync_type(is) == AV_SYNC_EXTERNAL_CLOCK && is->realtime)
		check_external_clock_speed(is);

	if (!display_disable && is->show_mode != SHOW_MODE_VIDEO && is->audio_st) {
		time = av_gettime_relative() / 1000000.0;
		if (is->force_refresh || is->last_vis_time + rdftspeed < time) {
			/*强制刷新视频*/
			video_display(is);
			is->last_vis_time = time;	/* 记录本次的时间 */
		}
		*remaining_time = FFMIN(*remaining_time, is->last_vis_time + rdftspeed - time);
	}

	if (is->video_st) {
		int redisplay = 0;
		if (is->force_refresh)
			redisplay = frame_queue_prev(&is->pictq);
	retry:
		if (frame_queue_nb_remaining(&is->pictq) == 0) {
			// nothing to do, no picture to display in the queue
		}
		else {
			double last_duration, duration, delay;
			Frame *vp, *lastvp;

			/* dequeue the picture */
			//	获取上一帧
			lastvp = frame_queue_peek_last(&is->pictq);
			//	获取本次帧
			vp = frame_queue_peek(&is->pictq);

			if (vp->serial != is->videoq.serial) {
				// 不等，说明 视频的序列 已经发生改变, 移动队列到下一个
				frame_queue_next(&is->pictq);
				redisplay = 0;
				goto retry;
			}

			// lastvp->serial != vp->serial 说明SEEK过,重新调整frame_timer
			if (lastvp->serial != vp->serial && !redisplay)
				is->frame_timer = av_gettime_relative() / 1000000.0;

			if (is->paused)
				goto display;

			/* compute nominal last_duration */
			// 上一帧的持续时间
			last_duration = vp_duration(is, lastvp, vp);
			if (redisplay)
				delay = 0.0;
			else
			{
				/* 通过与主时钟的同步，来修正上一帧的持续时间( 延长或缩短 ) */
				delay = compute_target_delay(last_duration, is);
			}


			/* yangbin 根据快慢放调整延时时间 */
			delay = delay / is->vidclk.speed;

			time = av_gettime_relative() / 1000000.0;

			/*frame_timer实际上就是上一帧的播放时间，而 frame_timer + delay 实际上就是当前这一帧的播放时间*/
			if (time < is->frame_timer + delay && !redisplay) {
				/*remaining 就是在refresh_loop_wait_event 中还需要睡眠的时间，
				其实就是现在还没到这一帧的播放时间，我们需要睡眠等待*/
				*remaining_time = FFMIN(is->frame_timer + delay - time, *remaining_time);

				/* yangbin : 当前帧的播放时间未到，则不做图像更新*/
				return;
			}

			//////////////////////////////////////////////////////////////////////////
			// yangbin ： 执行到此，说明本次调用需要更新图片

			is->frame_timer += delay;
			/*如果当前这一帧播放时间已经过了，并且其和当前系统时间的差值超过AV_SYNC_THRESHOLD_MAX，
			则将当前这一帧的播放时间改为当前系统时间，并在后续判断是否需要丢帧，
			其目的是  为后面帧的播放时间重新调整frame_timer */
			if (delay > 0 && time - is->frame_timer > AV_SYNC_THRESHOLD_MAX)
				is->frame_timer = time;

			/*更新视频的clock，将当前帧的pts和当前系统的时间保存起来，这2个数据将和audio  clock的pts 和系统时间一起计算delay*/
			SDL_LockMutex(is->pictq.mutex);
			if (!redisplay && !isnan(vp->pts))
				update_video_pts(is, vp->pts, vp->pos, vp->serial);
			SDL_UnlockMutex(is->pictq.mutex);

			if (frame_queue_nb_remaining(&is->pictq) > 1) {
				/*如果缓冲中帧数比较多的时候,例如下一帧也已经到了*/
				Frame *nextvp = frame_queue_peek_next(&is->pictq);

				//	当前帧videoframe的持续时间
				duration = vp_duration(is, vp, nextvp);

				/*	frame_timer+duration 当前帧的播放时间+当前帧的持续时间=下一帧的播放时间
				time > is->frame_timer + duration  当前时间>下一帧的播放时间,来不及播放本帧,
				下一帧的播放时间已经到了,说明当前帧可以丢弃了*/
				if (!is->step && (redisplay || framedrop>0 || (framedrop && get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER)) \
					&& time > is->frame_timer + duration)
				{
					/*如果延迟时间超过一帧了,就采取丢掉当前帧*/
					if (!redisplay)
						is->frame_drops_late++;

					/*采取丢帧策略,丢弃迟来的帧,取下一帧*/
					frame_queue_next(&is->pictq);
					redisplay = 0;
					goto retry;
				}
			}

		display:
			/* display picture */
			if (!display_disable && is->show_mode == SHOW_MODE_VIDEO)
				video_display(is);

			frame_queue_next(&is->pictq);

			if (is->step && !is->paused)
				stream_toggle_pause(is);
		}
	}
	is->force_refresh = 0;
	/*if (show_status) {
		static int64_t last_time;
		int64_t cur_time;
		int aqsize, vqsize, sqsize;
		double av_diff;

		cur_time = av_gettime_relative();
		if (!last_time || (cur_time - last_time) >= 30000) {
			aqsize = 0;
			vqsize = 0;
			sqsize = 0;
			if (is->audio_st)
				aqsize = is->audioq.size;
			if (is->video_st)
				vqsize = is->videoq.size;
			if (is->subtitle_st)
				sqsize = is->subtitleq.size;
			av_diff = 0;
			if (is->audio_st && is->video_st)
				av_diff = get_clock(&is->audclk) - get_clock(&is->vidclk);
			else if (is->video_st)
				av_diff = get_master_clock(is) - get_clock(&is->vidclk);
			else if (is->audio_st)
				av_diff = get_master_clock(is) - get_clock(&is->audclk);
			av_log(NULL, AV_LOG_INFO,
				"%7.2f %s:%7.3f fd=%4d aq=%5dKB vq=%5dKB sq=%5dB f=%"PRId64"/%"PRId64"   \r",
				get_master_clock(is),
				(is->audio_st && is->video_st) ? "A-V" : (is->video_st ? "M-V" : (is->audio_st ? "M-A" : "   ")),
				av_diff,
				is->frame_drops_early + is->frame_drops_late,
				aqsize / 1024,
				vqsize / 1024,
				sqsize,
				is->video_st ? is->video_st->codec->pts_correction_num_faulty_dts : 0,
				is->video_st ? is->video_st->codec->pts_correction_num_faulty_pts : 0);
			fflush(stdout);
			last_time = cur_time;
		}
	}*/
}

/* opaque 的类型为 VideoState  */
Uint32 refresh_callback(Uint32 interval, void *opaque)
{
	//	建解码后的图片进行显示， remaining_time 返回下一帧图片的显示时间
	// yangbin：  remaining_time 如果不初始化，则 debug 版正常，但是 release 版本会造成 SDL 定时器工作错误
	double remaining_time = 0;
	VideoState *is = (VideoState *)opaque;

	video_refresh(opaque, &remaining_time);
	if (remaining_time <= 0)
		remaining_time = REFRESH_RATE;
	
	return remaining_time * 1000;
}



/* allocate a picture (needs to do that in main thread to avoid
potential locking problems */
static void alloc_picture(VideoState *is)
{
	Frame *vp;
	int64_t bufferdiff;

	vp = &is->pictq.queue[is->pictq.windex];

	free_picture(vp);

	video_open(is, 0, vp);

	/*vp->bmp = SDL_CreateYUVOverlay(vp->width, vp->height,
	SDL_YV12_OVERLAY,
	screen);*/
	vp->bmp = SDL_CreateTexture(is->sdlRenderer, /*SDL_PIXELFORMAT_YV12*/SDL_PIXELFORMAT_IYUV,
		SDL_TEXTUREACCESS_STREAMING,
		vp->width, vp->height);

	/* SDL allocates a buffer smaller than requested if the video
	* overlay hardware is unable to support the requested size. */
	/* bufferdiff = vp->bmp ? FFMAX(vp->bmp->pixels[0], vp->bmp->pixels[1]) - FFMIN(vp->bmp->pixels[0], vp->bmp->pixels[1]) : 0;
	if (!vp->bmp || vp->bmp->pitches[0] < vp->width || bufferdiff < (int64_t)vp->height * vp->bmp->pitches[0]) {

	av_log(NULL, AV_LOG_FATAL,
	"Error: the video system does not support an image\n"
	"size of %dx%d pixels. Try using -lowres or -vf \"scale=w:h\"\n"
	"to reduce the image size.\n", vp->width, vp->height );
	do_exit(is);
	}*/

	if (!vp->bmp) {
		av_log(NULL, AV_LOG_FATAL,
			"Error: the video system does not support an image\n"
			"size of %dx%d pixels. Try using -lowres or -vf \"scale=w:h\"\n"
			"to reduce the image size.\n", vp->width, vp->height);
		do_exit(is);
	}

	SDL_LockMutex(is->pictq.mutex);
	vp->allocated = 1;
	SDL_CondSignal(is->pictq.cond);
	SDL_UnlockMutex(is->pictq.mutex);
}

//	如果被对齐的每行 YUV 图像还有空间，则加宽右边框，避免留下空白
static void duplicate_right_border_pixels(int nWidth, int nHeight, SDL_Texture *bmp, AVPicture pict) {
	int i, width, height;
	Uint8 *p, *maxp;
	for (i = 0; i < 3; i++) {
		width = nWidth;
		height = nHeight;
		if (i > 0) {
			width >>= 1;
			height >>= 1;
		}
		/* if (bmp->pitches[i] > width) {
		maxp = bmp->pixels[i] + bmp->pitches[i] * height - 1;
		for (p = bmp->pixels[i] + width - 1; p < maxp; p += bmp->pitches[i])
		*(p+1) = *p;
		}*/
		if (pict.linesize[i] > width) {
			maxp = pict.data[i] + pict.linesize[i] * height - 1;
			for (p = pict.data[i] + width - 1; p < maxp; p += pict.linesize[i])
				*(p + 1) = *p;
		}
	}
}

static int queue_picture(VideoState *is, AVFrame *src_frame, double pts, double duration, int64_t pos, int serial)
{
	Frame *vp;

#if defined(DEBUG_SYNC) && 0
	printf("frame_type=%c pts=%0.3f\n",
		av_get_picture_type_char(src_frame->pict_type), pts);
#endif

	//	vp 返回队列中可写的 帧位置
	if (!(vp = frame_queue_peek_writable(&is->pictq)))
		return -1;

	vp->sar = src_frame->sample_aspect_ratio;

	/* alloc or resize hardware picture buffer */
	if (!vp->bmp || vp->reallocate || !vp->allocated ||
		vp->width != src_frame->width ||
		vp->height != src_frame->height) {
		SDL_Event event;

		vp->allocated = 0;
		vp->reallocate = 0;
		vp->width = src_frame->width;
		vp->height = src_frame->height;

		/* the allocation must be done in the main thread to avoid
		locking problems. */
		event.type = FF_ALLOC_EVENT;
		event.user.data1 = is;
		SDL_PushEvent(&event);

		/* wait until the picture is allocated */
		SDL_LockMutex(is->pictq.mutex);
		while (!vp->allocated && !is->videoq.abort_request) {
			SDL_CondWait(is->pictq.cond, is->pictq.mutex);
		}
		/* if the queue is aborted, we have to pop the pending ALLOC event or wait for the allocation to complete */
		if (is->videoq.abort_request && SDL_PeepEvents(&event, 1, SDL_GETEVENT, FF_ALLOC_EVENT,
			FF_ALLOC_EVENT/*SDL_EVENTMASK(FF_ALLOC_EVENT)*/) != 1) {
			while (!vp->allocated && !is->abort_request) {
				SDL_CondWait(is->pictq.cond, is->pictq.mutex);
			}
		}
		SDL_UnlockMutex(is->pictq.mutex);

		if (is->videoq.abort_request)
			return -1;
	}

	//	进行图像的缩放后，然后放置到图片队列
	/* if the frame is not skipped, then display it */
	if (vp->bmp) {
		AVPicture pict = { { 0 } };

		/* get a pointer on the bitmap */
		//SDL_LockYUVOverlay (vp->bmp);
		SDL_LockTexture(vp->bmp, /*&sdlRect*/NULL, (void **)pict.data, pict.linesize);

		/* pict.data[0] = vp->bmp->pixels[0];
		pict.data[1] = vp->bmp->pixels[2];
		pict.data[2] = vp->bmp->pixels[1];

		pict.linesize[0] = vp->bmp->pitches[0];
		pict.linesize[1] = vp->bmp->pitches[2];
		pict.linesize[2] = vp->bmp->pitches[1];*/

		int y_size = pict.linesize[0] * vp->height;
		pict.data[1] = pict.data[0] + y_size;
		pict.data[2] = pict.data[1] + y_size / 4;

		pict.linesize[1] = pict.linesize[0] / 2;
		pict.linesize[2] = pict.linesize[0] / 2;

#if CONFIG_AVFILTER
		// FIXME use direct rendering
		av_picture_copy(&pict, (AVPicture *)src_frame,
			src_frame->format, vp->width, vp->height);
#else
		// av_opt_get_int(sws_opts, "sws_flags", 0, &sws_flags);
		is->img_convert_ctx = sws_getCachedContext(is->img_convert_ctx,
			vp->width, vp->height, (AVPixelFormat)src_frame->format, vp->width, vp->height,
			AV_PIX_FMT_YUV420P, sws_flags, NULL, NULL, NULL);
		if (!is->img_convert_ctx) {
			av_log(NULL, AV_LOG_FATAL, "Cannot initialize the conversion context\n");
			exit(1);
		}
		sws_scale(is->img_convert_ctx, src_frame->data, src_frame->linesize,
			0, vp->height, pict.data, pict.linesize);
#endif
		/* workaround SDL PITCH_WORKAROUND */
		duplicate_right_border_pixels(vp->width, vp->height, vp->bmp, pict);
		/* update the bitmap content */
		SDL_UnlockTexture(vp->bmp);
		//SDL_UnlockYUVOverlay(vp->bmp);

		vp->pts = pts;
		vp->duration = duration;
		vp->pos = pos;
		vp->serial = serial;

		/* now we can update the picture count */
		frame_queue_push(&is->pictq);
	}
	return 0;
}



AVRational _av_guess_sample_aspect_ratio(AVStream *stream, AVFrame *frame)
{
	AVRational undef = { 0, 1 };
	AVRational stream_sample_aspect_ratio = stream ? stream->sample_aspect_ratio : undef;
	AVRational codec_sample_aspect_ratio = stream && stream->codec ? stream->codec->sample_aspect_ratio : undef;
	AVRational frame_sample_aspect_ratio = frame ? frame->sample_aspect_ratio : codec_sample_aspect_ratio;

	av_reduce(&stream_sample_aspect_ratio.num, &stream_sample_aspect_ratio.den,
		stream_sample_aspect_ratio.num, stream_sample_aspect_ratio.den, INT_MAX);
	if (stream_sample_aspect_ratio.num <= 0 || stream_sample_aspect_ratio.den <= 0)
		stream_sample_aspect_ratio = undef;

	av_reduce(&frame_sample_aspect_ratio.num, &frame_sample_aspect_ratio.den,
		frame_sample_aspect_ratio.num, frame_sample_aspect_ratio.den, INT_MAX);
	if (frame_sample_aspect_ratio.num <= 0 || frame_sample_aspect_ratio.den <= 0)
		frame_sample_aspect_ratio = undef;

	if (stream_sample_aspect_ratio.num)
		return stream_sample_aspect_ratio;
	else
		return frame_sample_aspect_ratio;
}

AVRational _av_guess_frame_rate(AVStream *st)
{
	AVRational fr = st->r_frame_rate;
	AVRational codec_fr = st->codec->framerate;
	AVRational   avg_fr = st->avg_frame_rate;

	if (avg_fr.num > 0 && avg_fr.den > 0 && fr.num > 0 && fr.den > 0 &&
		av_q2d(avg_fr) < 70 && av_q2d(fr) > 210) {
		fr = avg_fr;
	}


	if (st->codec->ticks_per_frame > 1) {
		if (codec_fr.num > 0 && codec_fr.den > 0 &&
			(fr.num == 0 || av_q2d(codec_fr) < av_q2d(fr)*0.7 && fabs(1.0 - av_q2d(av_div_q(avg_fr, fr))) > 0.1))
			fr = codec_fr;
	}

	return fr;
}

//	解码一个视频帧，结果放置在 frame 
static int get_video_frame(VideoState *is, AVFrame *frame)
{
	int got_picture;

	if ((got_picture = decoder_decode_frame(&is->viddec, frame, NULL)) < 0)
		return -1;

	if (got_picture) {
		double dpts = NAN;

		if (frame->pts != AV_NOPTS_VALUE)
			dpts = av_q2d(is->video_st->time_base) * frame->pts;

		//	高宽比
		frame->sample_aspect_ratio = _av_guess_sample_aspect_ratio(is->video_st, frame);

		//	framedrop ： 当 cpu 处理太慢时，丢弃图片 
		if (framedrop>0 || (framedrop && get_master_sync_type(is) != AV_SYNC_VIDEO_MASTER)) {
			if (frame->pts != AV_NOPTS_VALUE) {
				double diff = dpts - get_master_clock(is);
				if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD &&
					diff - is->frame_last_filter_delay < 0 &&
					is->viddec.pkt_serial == is->vidclk.serial &&
					is->videoq.nb_packets) {
					is->frame_drops_early++;
					av_frame_unref(frame);
					got_picture = 0;
				}
			}
		}
	}

	return got_picture;
}



static int audio_thread(void *arg)
{
	VideoState *is = (VideoState *)arg;
	AVFrame *frame = av_frame_alloc();
	Frame *af;
#if CONFIG_AVFILTER
	int last_serial = -1;
	int64_t dec_channel_layout;
	int reconfigure;
#endif
	int got_frame = 0;
	AVRational tb;
	int ret = 0;

	if (!frame)
		return AVERROR(ENOMEM);

	do {
		if ((got_frame = decoder_decode_frame(&is->auddec, frame, NULL)) < 0)
			goto the_end;

		if (got_frame) {
			// tb = (AVRational){1, frame->sample_rate};
			tb.num = 1;
			tb.den = frame->sample_rate;

#if CONFIG_AVFILTER
			dec_channel_layout = get_valid_channel_layout(frame->channel_layout, av_frame_get_channels(frame));

			reconfigure =
				cmp_audio_fmts(is->audio_filter_src.fmt, is->audio_filter_src.channels,
				frame->format, av_frame_get_channels(frame)) ||
				is->audio_filter_src.channel_layout != dec_channel_layout ||
				is->audio_filter_src.freq != frame->sample_rate ||
				is->auddec.pkt_serial != last_serial;

			if (reconfigure) {
				char buf1[1024], buf2[1024];
				av_get_channel_layout_string(buf1, sizeof(buf1), -1, is->audio_filter_src.channel_layout);
				av_get_channel_layout_string(buf2, sizeof(buf2), -1, dec_channel_layout);
				av_log(NULL, AV_LOG_DEBUG,
					"Audio frame changed from rate:%d ch:%d fmt:%s layout:%s serial:%d to rate:%d ch:%d fmt:%s layout:%s serial:%d\n",
					is->audio_filter_src.freq, is->audio_filter_src.channels, av_get_sample_fmt_name(is->audio_filter_src.fmt), buf1, last_serial,
					frame->sample_rate, av_frame_get_channels(frame), av_get_sample_fmt_name(frame->format), buf2, is->auddec.pkt_serial);

				is->audio_filter_src.fmt = frame->format;
				is->audio_filter_src.channels = av_frame_get_channels(frame);
				is->audio_filter_src.channel_layout = dec_channel_layout;
				is->audio_filter_src.freq = frame->sample_rate;
				last_serial = is->auddec.pkt_serial;

				if ((ret = configure_audio_filters(is, afilters, 1)) < 0)
					goto the_end;
			}

			if ((ret = av_buffersrc_add_frame(is->in_audio_filter, frame)) < 0)
				goto the_end;

			while ((ret = av_buffersink_get_frame_flags(is->out_audio_filter, frame, 0)) >= 0) {
				tb = is->out_audio_filter->inputs[0]->time_base;
#endif
				if (!(af = frame_queue_peek_writable(&is->sampq)))
					goto the_end;

				af->pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
				af->pos = av_frame_get_pkt_pos(frame);
				af->serial = is->auddec.pkt_serial;
				AVRational avr;
				avr.num = frame->nb_samples;
				avr.den = frame->sample_rate;
				af->duration = av_q2d(avr);
				//af->duration = av_q2d((AVRational){frame->nb_samples, frame->sample_rate});

				av_frame_move_ref(af->frame, frame);
				frame_queue_push(&is->sampq);

#if CONFIG_AVFILTER
				if (is->audioq.serial != is->auddec.pkt_serial)
					break;
			}
			if (ret == AVERROR_EOF)
				is->auddec.finished = is->auddec.pkt_serial;
#endif
		}
	} while (ret >= 0 || ret == AVERROR(EAGAIN) || ret == AVERROR_EOF);
the_end:
#if CONFIG_AVFILTER
	avfilter_graph_free(&is->agraph);
#endif
	av_frame_free(&frame);
	return ret;
}

static void decoder_start(Decoder *d, int(*fn)(void *), void *arg)
{
	packet_queue_start(d->queue);
	d->decoder_tid = SDL_CreateThread(fn, "22222", arg);
}

//	视频线程：解码数据包，然后放置到图片队列
static int video_thread(void *arg)
{
	VideoState *is = (VideoState *)arg;
	AVFrame *frame = av_frame_alloc();
	double pts;
	double duration;
	int ret;
	AVRational tb = is->video_st->time_base;
	AVRational frame_rate = _av_guess_frame_rate(is->video_st);

#if CONFIG_AVFILTER
	AVFilterGraph *graph = avfilter_graph_alloc();
	AVFilterContext *filt_out = NULL, *filt_in = NULL;
	int last_w = 0;
	int last_h = 0;
	enum AVPixelFormat last_format = -2;
	int last_serial = -1;
	int last_vfilter_idx = 0;
	if (!graph) {
		av_frame_free(&frame);
		return AVERROR(ENOMEM);
	}

#endif

	if (!frame) {
#if CONFIG_AVFILTER
		avfilter_graph_free(&graph);
#endif
		return AVERROR(ENOMEM);
	}

	for (;;) {
		/* 返回的 frame 中就保存了解码后获得的图片  */
		ret = get_video_frame(is, frame);
		if (ret < 0)
			goto the_end;
		if (!ret)
			continue;

#if CONFIG_AVFILTER
		if (last_w != frame->width
			|| last_h != frame->height
			|| last_format != frame->format
			|| last_serial != is->viddec.pkt_serial
			|| last_vfilter_idx != is->vfilter_idx) {
			av_log(NULL, AV_LOG_DEBUG,
				"Video frame changed from size:%dx%d format:%s serial:%d to size:%dx%d format:%s serial:%d\n",
				last_w, last_h,
				(const char *)av_x_if_null(av_get_pix_fmt_name(last_format), "none"), last_serial,
				frame->width, frame->height,
				(const char *)av_x_if_null(av_get_pix_fmt_name(frame->format), "none"), is->viddec.pkt_serial);
			avfilter_graph_free(&graph);
			graph = avfilter_graph_alloc();
			if ((ret = configure_video_filters(graph, is, vfilters_list ? vfilters_list[is->vfilter_idx] : NULL, frame)) < 0) {
				SDL_Event event;
				event.type = FF_QUIT_EVENT;
				event.user.data1 = is;
				SDL_PushEvent(&event);
				goto the_end;
			}
			filt_in = is->in_video_filter;
			filt_out = is->out_video_filter;
			last_w = frame->width;
			last_h = frame->height;
			last_format = frame->format;
			last_serial = is->viddec.pkt_serial;
			last_vfilter_idx = is->vfilter_idx;
			frame_rate = filt_out->inputs[0]->frame_rate;
		}

		ret = av_buffersrc_add_frame(filt_in, frame);
		if (ret < 0)
			goto the_end;

		while (ret >= 0) {
			is->frame_last_returned_time = av_gettime_relative() / 1000000.0;

			ret = av_buffersink_get_frame_flags(filt_out, frame, 0);
			if (ret < 0) {
				if (ret == AVERROR_EOF)
					is->viddec.finished = is->viddec.pkt_serial;
				ret = 0;
				break;
			}

			is->frame_last_filter_delay = av_gettime_relative() / 1000000.0 - is->frame_last_returned_time;
			if (fabs(is->frame_last_filter_delay) > AV_NOSYNC_THRESHOLD / 10.0)
				is->frame_last_filter_delay = 0;
			tb = filt_out->inputs[0]->time_base;
#endif

			//SaveAsJPEG(frame, frame->width, frame->height, 5555);

			// 计算该图片帧持续的时长 duration，调用 queue_picture 放置到图片队列，同时进行缩放
			AVRational avr;
			avr.num = frame_rate.den; avr.den = frame_rate.num;
			duration = (frame_rate.num && frame_rate.den ? av_q2d(avr/*(AVRational){frame_rate.den, frame_rate.num}*/) : 0);
			pts = (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts * av_q2d(tb);
			ret = queue_picture(is, frame, pts, duration, av_frame_get_pkt_pos(frame), is->viddec.pkt_serial);
			av_frame_unref(frame);
#if CONFIG_AVFILTER
		}
#endif

		if (ret < 0)
			goto the_end;
	}
the_end:
#if CONFIG_AVFILTER
	avfilter_graph_free(&graph);
#endif
	av_frame_free(&frame);
	return 0;
}



/* copy samples for viewing in editor window */
static void update_sample_display(VideoState *is, short *samples, int samples_size)
{
	int size, len;

	size = samples_size / sizeof(short);
	while (size > 0) {
		len = SAMPLE_ARRAY_SIZE - is->sample_array_index;
		if (len > size)
			len = size;
		memcpy(is->sample_array + is->sample_array_index, samples, len * sizeof(short));
		samples += len;
		is->sample_array_index += len;
		if (is->sample_array_index >= SAMPLE_ARRAY_SIZE)
			is->sample_array_index = 0;
		size -= len;
	}
}

/* return the wanted number of samples to get better sync if sync_type is video
* or external master clock */
static int synchronize_audio(VideoState *is, int nb_samples)
{
	int wanted_nb_samples = nb_samples;

	/* if not master, then we try to remove or add samples to correct the clock */
	if (get_master_sync_type(is) != AV_SYNC_AUDIO_MASTER) {
		double diff, avg_diff;
		int min_nb_samples, max_nb_samples;

		diff = get_clock(&is->audclk) - get_master_clock(is);

		if (!isnan(diff) && fabs(diff) < AV_NOSYNC_THRESHOLD) {
			//	audio_diff_cum 保存多次累积的 音视频时差
			//	它基本上是一个使用等比级数的加权平均值
			is->audio_diff_cum = diff + is->audio_diff_avg_coef * is->audio_diff_cum;
			if (is->audio_diff_avg_count < AUDIO_DIFF_AVG_NB) {
				/* not enough measures to have a correct estimate */
				is->audio_diff_avg_count++;
			}
			else {
				/* estimate the A-V difference */
				avg_diff = is->audio_diff_cum * (1.0 - is->audio_diff_avg_coef);

				if (fabs(avg_diff) >= is->audio_diff_threshold) {
					// freq 为 采样数/每秒, diff 为与主时钟相差的秒数(有正负号)，nb_samples 为现有音频帧的采样数
					//	wanted_nb_samples 为音视频同步后 希望得到的采样数
					wanted_nb_samples = nb_samples + (int)(diff * is->audio_src.freq);
					min_nb_samples = ((nb_samples * (100 - SAMPLE_CORRECTION_PERCENT_MAX) / 100));
					max_nb_samples = ((nb_samples * (100 + SAMPLE_CORRECTION_PERCENT_MAX) / 100));
					wanted_nb_samples = av_clip(wanted_nb_samples, min_nb_samples, max_nb_samples);
				}
				av_log(NULL, AV_LOG_TRACE, "diff=%f adiff=%f sample_diff=%d apts=%0.3f %f\n",
					diff, avg_diff, wanted_nb_samples - nb_samples,
					is->audio_clock, is->audio_diff_threshold);
			}
		}
		else {
			/* too big difference : may be initial PTS errors, so
			reset A-V filter */
			is->audio_diff_avg_count = 0;
			is->audio_diff_cum = 0;
		}
	}

	return wanted_nb_samples;
}

/**
* Decode one audio frame and return its uncompressed size.
*
* The processed audio frame is decoded, converted if required, and
* stored in is->audio_buf, with size in bytes given by the return
* value.
*/
static int audio_decode_frame(VideoState *is)
{
	int data_size, resampled_data_size;
	int64_t dec_channel_layout;
	av_unused double audio_clock0;
	int wanted_nb_samples;
	Frame *af;

	if (is->paused)
		return -1;

	do {
		if (!(af = frame_queue_peek_readable(&is->sampq)))
			return -1;
		frame_queue_next(&is->sampq);
	} while (af->serial != is->audioq.serial);

	data_size = av_samples_get_buffer_size(NULL, av_frame_get_channels(af->frame),
		af->frame->nb_samples,
		(AVSampleFormat)af->frame->format, 1);

	dec_channel_layout =
		(af->frame->channel_layout && av_frame_get_channels(af->frame) == av_get_channel_layout_nb_channels(af->frame->channel_layout)) ?
		af->frame->channel_layout : av_get_default_channel_layout(av_frame_get_channels(af->frame));
	wanted_nb_samples = synchronize_audio(is, af->frame->nb_samples);

	if (af->frame->format != is->audio_src.fmt ||
		dec_channel_layout != is->audio_src.channel_layout ||
		af->frame->sample_rate != is->audio_src.freq ||
		(wanted_nb_samples != af->frame->nb_samples && !is->swr_ctx)) {
		swr_free(&is->swr_ctx);
		is->swr_ctx = swr_alloc_set_opts(NULL,
			is->audio_tgt.channel_layout, is->audio_tgt.fmt, is->audio_tgt.freq,
			dec_channel_layout, (AVSampleFormat)af->frame->format, af->frame->sample_rate,
			0, NULL);
		if (!is->swr_ctx || swr_init(is->swr_ctx) < 0) {
			av_log(NULL, AV_LOG_ERROR,
				"Cannot create sample rate converter for conversion of %d Hz %s %d channels to %d Hz %s %d channels!\n",
				af->frame->sample_rate, av_get_sample_fmt_name((AVSampleFormat)af->frame->format), av_frame_get_channels(af->frame),
				is->audio_tgt.freq, av_get_sample_fmt_name(is->audio_tgt.fmt), is->audio_tgt.channels);
			swr_free(&is->swr_ctx);
			return -1;
		}
		is->audio_src.channel_layout = dec_channel_layout;
		is->audio_src.channels = av_frame_get_channels(af->frame);
		is->audio_src.freq = af->frame->sample_rate;
		is->audio_src.fmt = (AVSampleFormat)af->frame->format;
	}

	if (is->swr_ctx) {
		const uint8_t **in = (const uint8_t **)af->frame->extended_data;
		uint8_t **out = &is->audio_buf1;
		int out_count = (int64_t)wanted_nb_samples * is->audio_tgt.freq / af->frame->sample_rate + 256;
		int out_size = av_samples_get_buffer_size(NULL, is->audio_tgt.channels, out_count, is->audio_tgt.fmt, 0);
		int len2;
		if (out_size < 0) {
			av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size() failed\n");
			return -1;
		}
		if (wanted_nb_samples != af->frame->nb_samples) {
			//	当希望得到的采样数 与 音频帧中采样数不等时， 应该采样 插入 Silent 数据或者 丢弃某些采样来解决 音视频同步问题
			if (swr_set_compensation(is->swr_ctx, (wanted_nb_samples - af->frame->nb_samples) * is->audio_tgt.freq / af->frame->sample_rate,
				wanted_nb_samples * is->audio_tgt.freq / af->frame->sample_rate) < 0) {
				av_log(NULL, AV_LOG_ERROR, "swr_set_compensation() failed\n");
				return -1;
			}
		}
		av_fast_malloc(&is->audio_buf1, &is->audio_buf1_size, out_size);
		if (!is->audio_buf1)
			return AVERROR(ENOMEM);
		//	len2 = 每声道采样数
		len2 = swr_convert(is->swr_ctx, out, out_count, in, af->frame->nb_samples);
		if (len2 < 0) {
			av_log(NULL, AV_LOG_ERROR, "swr_convert() failed\n");
			return -1;
		}
		if (len2 == out_count) {
			av_log(NULL, AV_LOG_WARNING, "audio buffer is probably too small\n");
			if (swr_init(is->swr_ctx) < 0)
				swr_free(&is->swr_ctx);
		}
		is->audio_buf = is->audio_buf1;
		//	每声道采样数 x 声道数 x 每个采样字节数
		resampled_data_size = len2 * is->audio_tgt.channels * av_get_bytes_per_sample(is->audio_tgt.fmt);
	}
	else {
		is->audio_buf = af->frame->data[0];
		resampled_data_size = data_size;
	}

	audio_clock0 = is->audio_clock;
	/* update the audio clock with the pts */
	//  1/af->frame->sample_rate=采样一个样本点所需要的时间
	if (!isnan(af->pts))
		/*  计算最后一个采样点的 PTS 时间 */
		is->audio_clock = af->pts + (double)af->frame->nb_samples / af->frame->sample_rate;
	else
		is->audio_clock = NAN;
	is->audio_clock_serial = af->serial;
#ifdef DEBUG
	{
		static double last_clock;
		printf("audio: delay=%0.3f clock=%0.3f clock0=%0.3f\n",
			is->audio_clock - last_clock,
			is->audio_clock, audio_clock0);
		last_clock = is->audio_clock;
	}
#endif
	return resampled_data_size;
}

/* prepare a new audio buffer */
static void sdl_audio_callback(void *opaque, Uint8 *stream, int len)
{
	VideoState *is = (VideoState *)opaque;
	int audio_size, len1;

	audio_callback_time = av_gettime_relative();

	/*   len是由SDL传入的SDL缓冲区的大小，如果这个缓冲未满，我们就一直往里填充数据 */
	while (len > 0)
	{
		/*  audio_buf_index 和 audio_buf_size 标示我们自己用来放置解码出来的数据的缓冲区，*/

		/*	这些数据等待copy 到 SDL缓冲区， 当audio_buf_index >= audio_buf_size的时候意味着我们的缓冲为空，
		没有数据可供copy，这时候需要调用audio_decode_frame来解码出更多的桢数据 */

		if (is->audio_buf_index >= is->audio_buf_size) {
			audio_size = audio_decode_frame(is);
			if (audio_size < 0) {
				/* if error, just output silence */
				is->audio_buf = is->silence_buf;
				is->audio_buf_size = sizeof(is->silence_buf) / is->audio_tgt.frame_size * is->audio_tgt.frame_size;
			}
			else {
				if (is->show_mode != SHOW_MODE_VIDEO)
					update_sample_display(is, (int16_t *)is->audio_buf, audio_size);
				is->audio_buf_size = audio_size;
			}
			is->audio_buf_index = 0;
		}

		/*  查看stream可用空间，决定一次copy多少数据，剩下的下次继续copy */
		len1 = is->audio_buf_size - is->audio_buf_index;
		if (len1 > len)
			len1 = len;
		memcpy(stream, (uint8_t *)is->audio_buf + is->audio_buf_index, len1);
		len -= len1;
		stream += len1;
		is->audio_buf_index += len1;
	}
	is->audio_write_buf_size = is->audio_buf_size - is->audio_buf_index;
	/* Let's assume the audio driver that is used by SDL has two periods. */
	if (!isnan(is->audio_clock)) {
		/* is->audio_clock 是已经解码的音频数据最后一个采样点的 PTS ,因此减去未放入 SDL 的采样点的时间得出
		已经开始播放的音频数据的PTS 时间 */
		set_clock_at(&is->audclk, is->audio_clock - (double)(2 * is->audio_hw_buf_size + is->audio_write_buf_size) / is->audio_tgt.bytes_per_sec, is->audio_clock_serial, audio_callback_time / 1000000.0);
		sync_clock_to_slave(&is->extclk, &is->audclk);
	}
}

static int audio_open(void *opaque, int64_t wanted_channel_layout, int wanted_nb_channels, int wanted_sample_rate, struct AudioParams *audio_hw_params)
{
	SDL_AudioSpec wanted_spec, spec;
	const char *env;
	static const int next_nb_channels[] = { 0, 0, 1, 6, 2, 6, 4, 6 };
	static const int next_sample_rates[] = { 0, 44100, 48000, 96000, 192000 };
	int next_sample_rate_idx = FF_ARRAY_ELEMS(next_sample_rates) - 1;

	env = SDL_getenv("SDL_AUDIO_CHANNELS");
	if (env) {
		wanted_nb_channels = atoi(env);
		wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
	}
	if (!wanted_channel_layout || wanted_nb_channels != av_get_channel_layout_nb_channels(wanted_channel_layout)) {
		wanted_channel_layout = av_get_default_channel_layout(wanted_nb_channels);
		wanted_channel_layout &= ~AV_CH_LAYOUT_STEREO_DOWNMIX;
	}
	wanted_nb_channels = av_get_channel_layout_nb_channels(wanted_channel_layout);
	wanted_spec.channels = wanted_nb_channels;
	wanted_spec.freq = wanted_sample_rate;
	if (wanted_spec.freq <= 0 || wanted_spec.channels <= 0) {
		av_log(NULL, AV_LOG_ERROR, "Invalid sample rate or channel count!\n");
		return -1;
	}
	while (next_sample_rate_idx && next_sample_rates[next_sample_rate_idx] >= wanted_spec.freq)
		next_sample_rate_idx--;
	wanted_spec.format = AUDIO_S16SYS;
	wanted_spec.silence = 0;
	wanted_spec.samples = FFMAX(SDL_AUDIO_MIN_BUFFER_SIZE, 2 << av_log2(wanted_spec.freq / SDL_AUDIO_MAX_CALLBACKS_PER_SEC));
	wanted_spec.callback = sdl_audio_callback;
	wanted_spec.userdata = opaque;
	while (SDL_OpenAudio(&wanted_spec, &spec) < 0) {
		av_log(NULL, AV_LOG_WARNING, "SDL_OpenAudio (%d channels, %d Hz): %s\n",
			wanted_spec.channels, wanted_spec.freq, SDL_GetError());
		wanted_spec.channels = next_nb_channels[FFMIN(7, wanted_spec.channels)];
		if (!wanted_spec.channels) {
			wanted_spec.freq = next_sample_rates[next_sample_rate_idx--];
			wanted_spec.channels = wanted_nb_channels;
			if (!wanted_spec.freq) {
				av_log(NULL, AV_LOG_ERROR,
					"No more combinations to try, audio open failed\n");
				return -1;
			}
		}
		wanted_channel_layout = av_get_default_channel_layout(wanted_spec.channels);
	}
	if (spec.format != AUDIO_S16SYS) {
		av_log(NULL, AV_LOG_ERROR,
			"SDL advised audio format %d is not supported!\n", spec.format);
		return -1;
	}
	if (spec.channels != wanted_spec.channels) {
		wanted_channel_layout = av_get_default_channel_layout(spec.channels);
		if (!wanted_channel_layout) {
			av_log(NULL, AV_LOG_ERROR,
				"SDL advised channel count %d is not supported!\n", spec.channels);
			return -1;
		}
	}

	audio_hw_params->fmt = AV_SAMPLE_FMT_S16;
	audio_hw_params->freq = spec.freq;
	audio_hw_params->channel_layout = wanted_channel_layout;
	audio_hw_params->channels = spec.channels;
	audio_hw_params->frame_size = av_samples_get_buffer_size(NULL, audio_hw_params->channels, 1, audio_hw_params->fmt, 1);
	audio_hw_params->bytes_per_sec = av_samples_get_buffer_size(NULL, audio_hw_params->channels, audio_hw_params->freq, audio_hw_params->fmt, 1);
	if (audio_hw_params->bytes_per_sec <= 0 || audio_hw_params->frame_size <= 0) {
		av_log(NULL, AV_LOG_ERROR, "av_samples_get_buffer_size failed\n");
		return -1;
	}
	return spec.size;
}

/* open a given stream. Return 0 if OK */
static int stream_component_open(VideoState *is, int stream_index)
{
	AVFormatContext *ic = is->ic;
	AVCodecContext *avctx;
	AVCodec *codec;
	const char *forced_codec_name = NULL;
	AVDictionary *opts;
	AVDictionaryEntry *t = NULL;
	int sample_rate, nb_channels;
	int64_t channel_layout;
	int ret = 0;
	int stream_lowres = lowres;

	if (stream_index < 0 || stream_index >= ic->nb_streams)
		return -1;
	avctx = ic->streams[stream_index]->codec;

	codec = avcodec_find_decoder(avctx->codec_id);

	switch (avctx->codec_type){
	case AVMEDIA_TYPE_AUDIO: is->last_audio_stream = stream_index; forced_codec_name = audio_codec_name; break;
	//case AVMEDIA_TYPE_SUBTITLE: is->last_subtitle_stream = stream_index; forced_codec_name = subtitle_codec_name; break;
	case AVMEDIA_TYPE_VIDEO: is->last_video_stream = stream_index; forced_codec_name = video_codec_name; break;
	}
	if (forced_codec_name)
		codec = avcodec_find_decoder_by_name(forced_codec_name);
	if (!codec) {
		if (forced_codec_name) av_log(NULL, AV_LOG_WARNING,
			"No codec could be found with name '%s'\n", forced_codec_name);
		else                   av_log(NULL, AV_LOG_WARNING,
			"No codec could be found with id %d\n", avctx->codec_id);
		return -1;
	}

	avctx->codec_id = codec->id;
	if (stream_lowres > av_codec_get_max_lowres(codec)){
		av_log(avctx, AV_LOG_WARNING, "The maximum value for lowres supported by the decoder is %d\n",
			av_codec_get_max_lowres(codec));
		stream_lowres = av_codec_get_max_lowres(codec);
	}
	av_codec_set_lowres(avctx, stream_lowres);

	if (stream_lowres) avctx->flags |= CODEC_FLAG_EMU_EDGE;
	if (fast)   avctx->flags2 |= CODEC_FLAG2_FAST;
	if (codec->capabilities & CODEC_CAP_DR1)
		avctx->flags |= CODEC_FLAG_EMU_EDGE;

	//	根据输入的命令行，设置 解码器的选项
	opts = filter_codec_opts(codec_opts, avctx->codec_id, ic, ic->streams[stream_index], codec);
	if (!av_dict_get(opts, "threads", NULL, 0))
		av_dict_set(&opts, "threads", "auto", 0);
	if (stream_lowres)
		av_dict_set_int(&opts, "lowres", stream_lowres, 0);
	if (avctx->codec_type == AVMEDIA_TYPE_VIDEO || avctx->codec_type == AVMEDIA_TYPE_AUDIO)
		av_dict_set(&opts, "refcounted_frames", "1", 0);
	//	根据选项打开 解码器
	if ((ret = avcodec_open2(avctx, codec, &opts)) < 0) {
		goto fail;
	}
	if ((t = av_dict_get(opts, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
		av_log(NULL, AV_LOG_ERROR, "Option %s not found.\n", t->key);
		ret = AVERROR_OPTION_NOT_FOUND;
		goto fail;
	}

	is->eof = 0;
	ic->streams[stream_index]->discard = AVDISCARD_DEFAULT;
	switch (avctx->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
#if CONFIG_AVFILTER
	{
		AVFilterLink *link;

		is->audio_filter_src.freq = avctx->sample_rate;
		is->audio_filter_src.channels = avctx->channels;
		is->audio_filter_src.channel_layout = get_valid_channel_layout(avctx->channel_layout, avctx->channels);
		is->audio_filter_src.fmt = avctx->sample_fmt;
		if ((ret = configure_audio_filters(is, afilters, 0)) < 0)
			goto fail;
		link = is->out_audio_filter->inputs[0];
		sample_rate = link->sample_rate;
		nb_channels = link->channels;
		channel_layout = link->channel_layout;
	}
#else
		sample_rate = avctx->sample_rate;
		nb_channels = avctx->channels;
		channel_layout = avctx->channel_layout;
#endif

		/* prepare audio output */
		if ((ret = audio_open(is, channel_layout, nb_channels, sample_rate, &is->audio_tgt)) < 0)
			goto fail;
		is->audio_hw_buf_size = ret;
		is->audio_src = is->audio_tgt;
		is->audio_buf_size = 0;
		is->audio_buf_index = 0;

		/* init averaging filter */
		is->audio_diff_avg_coef = exp(log(0.01) / AUDIO_DIFF_AVG_NB);
		is->audio_diff_avg_count = 0;
		/* since we do not have a precise anough audio fifo fullness,
		we correct audio sync only if larger than this threshold */
		is->audio_diff_threshold = (double)(is->audio_hw_buf_size) / is->audio_tgt.bytes_per_sec;

		is->audio_stream = stream_index;
		is->audio_st = ic->streams[stream_index];

		decoder_init(&is->auddec, avctx, &is->audioq, is->continue_read_thread);
		/*if ((is->ic->iformat->flags & (AVFMT_NOBINSEARCH | AVFMT_NOGENSEARCH | AVFMT_NO_BYTE_SEEK)) && !is->ic->iformat->read_seek) {
			is->auddec.start_pts = is->audio_st->start_time;
			is->auddec.start_pts_tb = is->audio_st->time_base;
		}*/
		decoder_start(&is->auddec, audio_thread, is);
		SDL_PauseAudio(0);
		break;
	case AVMEDIA_TYPE_VIDEO:
		is->video_stream = stream_index;
		is->video_st = ic->streams[stream_index];

		decoder_init(&is->viddec, avctx, &is->videoq, is->continue_read_thread);
		decoder_start(&is->viddec, video_thread, is);
		is->queue_attachments_req = 1;
		break;
	default:
		break;
	}

fail:
	av_dict_free(&opts);

	return ret;
}

static void stream_component_close(VideoState *is, int stream_index)
{
	AVFormatContext *ic = is->ic;
	AVCodecContext *avctx;

	if (stream_index < 0 || stream_index >= ic->nb_streams)
		return;
	avctx = ic->streams[stream_index]->codec;

	switch (avctx->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		decoder_abort(&is->auddec, &is->sampq);
		SDL_CloseAudio();
		decoder_destroy(&is->auddec);
		swr_free(&is->swr_ctx);
		av_freep(&is->audio_buf1);
		is->audio_buf1_size = 0;
		is->audio_buf = NULL;

		if (is->rdft) {
			av_rdft_end(is->rdft);
			av_freep(&is->rdft_data);
			is->rdft = NULL;
			is->rdft_bits = 0;
		}
		break;
	case AVMEDIA_TYPE_VIDEO:
		decoder_abort(&is->viddec, &is->pictq);
		decoder_destroy(&is->viddec);
		break;
	/*case AVMEDIA_TYPE_SUBTITLE:
		decoder_abort(&is->subdec, &is->subpq);
		decoder_destroy(&is->subdec);
		break;*/
	default:
		break;
	}

	ic->streams[stream_index]->discard = AVDISCARD_ALL;
	avcodec_close(avctx);
	switch (avctx->codec_type) {
	case AVMEDIA_TYPE_AUDIO:
		is->audio_st = NULL;
		is->audio_stream = -1;
		break;
	case AVMEDIA_TYPE_VIDEO:
		is->video_st = NULL;
		is->video_stream = -1;
		break;
	/*case AVMEDIA_TYPE_SUBTITLE:
		is->subtitle_st = NULL;
		is->subtitle_stream = -1;
		break;*/
	default:
		break;
	}
}



static int decode_interrupt_cb(void *ctx)
{
	VideoState *is = (VideoState *)ctx;
	return is->abort_request;
}

static int is_realtime(AVFormatContext *s)
{
	if (!strcmp(s->iformat->name, "rtp")
		|| !strcmp(s->iformat->name, "rtsp")
		|| !strcmp(s->iformat->name, "sdp")
		)
		return 1;

	if (s->pb && (!strncmp(s->filename, "rtp:", 4)
		|| !strncmp(s->filename, "udp:", 4)
		)
		)
		return 1;
	return 0;
}


int stream_input_data(VideoState *is, int stream_index, uint8_t *buf, int buf_size, int64_t pts)
{
	AVPacket pkt1, *pkt = &pkt1;
	av_init_packet(pkt);
	
	/*	
	av_free_packet调用的是结构体本身的destruct函数，它的值有两种情况：
	1)av_destruct_packet_nofree或0；
	2)av_destruct_packet，
	其中，情况1仅仅是将 data和 size的值清0而已，情况2才会真正地释放缓冲区。

	使用以下代码后, 由于 destruct 函数为 0， 因此 av_free_packet 无法释放内存， 导致内存泄露，
	*/
	
	/*if (buf != NULL && buf_size > 0)
	{
		pkt->data = (uint8_t *)av_mallocz(buf_size);
		if (pkt->data)
		{
			memcpy(pkt->data, buf, buf_size);
			pkt->size = buf_size;
			pkt->pts = pts;
			pkt->stream_index = stream_index;
		}
		else
			return -1;
	}
	else
		return -2;*/

	pkt->data = buf;
	pkt->size = buf_size;
	pkt->pts = pts;
	pkt->stream_index = stream_index;

	if (pkt->stream_index == is->audio_stream) {
		packet_queue_put(&is->audioq, pkt);
	}
	else if (pkt->stream_index == is->video_stream) {
		packet_queue_put(&is->videoq, pkt);
	}
	else {
		av_free_packet(pkt);
	}
	return 0;
}

/* this thread gets the stream from the disk or the network */
static int read_thread(void *arg)
{
	VideoState *is = (VideoState *)arg;
	AVFormatContext *ic = NULL;
	int err, i, ret;
	int st_index[AVMEDIA_TYPE_NB];
	AVPacket pkt1, *pkt = &pkt1;
	int64_t stream_start_time;
	int pkt_in_play_range = 0;
	AVDictionaryEntry *t;
	AVDictionary **opts;
	AVStream * st;

	int orig_nb_streams;
	// SDL_mutex *wait_mutex = SDL_CreateMutex();
	int scan_all_pmts_set = 0;
	int64_t pkt_ts;

	memset(st_index, -1, sizeof(st_index));
	is->last_video_stream = is->video_stream = -1;
	is->last_audio_stream = is->audio_stream = -1;
	// is->last_subtitle_stream = is->subtitle_stream = -1;
	is->eof = 0;

	ic = avformat_alloc_context();
	if (!ic) {
		av_log(NULL, AV_LOG_FATAL, "Could not allocate context.\n");
		ret = AVERROR(ENOMEM);
		goto fail;
	}
	ic->interrupt_callback.callback = decode_interrupt_cb;
	ic->interrupt_callback.opaque = is;
	if (!av_dict_get(format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE)) {
		av_dict_set(&format_opts, "scan_all_pmts", "1", AV_DICT_DONT_OVERWRITE);
		scan_all_pmts_set = 1;
	}

	// 根据 SDP 设置 ic

	//	音频流 Audio
	if (is->media_format.media_type == PLAYER_MEDIA_TYPE_AUDIO || is->media_format.media_type == PLAYER_MEDIA_TYPE_MIX)
	{
		unsigned int sample_rate = 8000;

		st = avformat_new_stream(ic, NULL);
		if (!st)
			goto fail;
		st->codec->codec_type = AVMEDIA_TYPE_AUDIO;

		if (is->media_format.audio_type == PLAYER_MEDIA_F_AUDIO_TYPE_G711)
			st->codec->codec_id = AV_CODEC_ID_PCM_ALAW;
		else if (is->media_format.audio_type == PLAYER_MEDIA_F_AUDIO_TYPE_G723)
			st->codec->codec_id = AV_CODEC_ID_G723_1;
		else if (is->media_format.audio_type == PLAYER_MEDIA_F_AUDIO_TYPE_G729)
			st->codec->codec_id = AV_CODEC_ID_G729;
		else if (is->media_format.audio_type == PLAYER_MEDIA_F_AUDIO_TYPE_G722)
			st->codec->codec_id = AV_CODEC_ID_ADPCM_G722;
		else
			st->codec->codec_id = AV_CODEC_ID_PCM_ALAW;

		if (is->media_format.audio_samplerate == PLAYER_MEDIA_F_AUDIO_SAMPLE_RATE_8)
			sample_rate = 8000;
		else if (is->media_format.audio_samplerate == PLAYER_MEDIA_F_AUDIO_SAMPLE_RATE_14)
			sample_rate = 14000;
		else if (is->media_format.audio_samplerate == PLAYER_MEDIA_F_AUDIO_SAMPLE_RATE_16)
			sample_rate = 16000;
		else if (is->media_format.audio_samplerate == PLAYER_MEDIA_F_AUDIO_SAMPLE_RATE_32)
			sample_rate = 32000;
		else
			sample_rate = 8000;

		st->codec->sample_rate = sample_rate;
		st->codec->channels = is->media_format.audio_channels;
		avpriv_set_pts_info(st, 32, 1, sample_rate);
		st_index[AVMEDIA_TYPE_AUDIO] = st->index;
	}
	

	// 视频流 video
	if (is->media_format.media_type == PLAYER_MEDIA_TYPE_VIDEO || is->media_format.media_type == PLAYER_MEDIA_TYPE_MIX)
	{
		st = avformat_new_stream(ic, NULL);
		if (!st)
			goto fail;
		st->codec->codec_type = AVMEDIA_TYPE_VIDEO;
		st->codec->codec_id = /*AV_CODEC_ID_HEVC*/ /*AV_CODEC_ID_MPEG2VIDEO*/ AV_CODEC_ID_H264;

		if (is->media_format.video_type == PLAYER_MEDIA_F_VIDEO_TYPE_MPEG2)
			st->codec->codec_id = AV_CODEC_ID_MPEG2VIDEO;
		else if (is->media_format.video_type == PLAYER_MEDIA_F_VIDEO_TYPE_MPEG4)
			st->codec->codec_id = AV_CODEC_ID_MPEG4;
		else if (is->media_format.video_type == PLAYER_MEDIA_F_VIDEO_TYPE_H264)
			st->codec->codec_id = AV_CODEC_ID_H264;
		else if (is->media_format.video_type == PLAYER_MEDIA_F_VIDEO_TYPE_SVAC)
			st->codec->codec_id = AV_CODEC_ID_H264;
		else if (is->media_format.video_type == PLAYER_MEDIA_F_VIDEO_TYPE_3GP)
			st->codec->codec_id = AV_CODEC_ID_H264;
		else
			st->codec->codec_id = AV_CODEC_ID_H264;

		avpriv_set_pts_info(st, 32, 1, 90000);
		st_index[AVMEDIA_TYPE_VIDEO] = st->index;
	}
	

	/*err = avformat_open_input(&ic, is->filename, is->iformat, &format_opts);
	if (err < 0) {
		print_error(is->filename, err);
		ret = -1;
		goto fail;
	}
	if (scan_all_pmts_set)
		av_dict_set(&format_opts, "scan_all_pmts", NULL, AV_DICT_MATCH_CASE);*/

	if ((t = av_dict_get(format_opts, "", NULL, AV_DICT_IGNORE_SUFFIX))) {
		av_log(NULL, AV_LOG_ERROR, "Option %s not found.\n", t->key);
		/*ret = AVERROR_OPTION_NOT_FOUND;
		goto fail;*/
	}
	is->ic = ic;

	if (genpts)
		ic->flags |= AVFMT_FLAG_GENPTS;

	//av_format_inject_global_side_data(ic);

	opts = setup_find_stream_info_opts(ic, codec_opts);
	orig_nb_streams = ic->nb_streams;

	//err = avformat_find_stream_info(ic, opts);
	
	for (i = 0; i < orig_nb_streams; i++)
		av_dict_free(&opts[i]);
	av_freep(&opts);


	if (ic->pb)
		ic->pb->eof_reached = 0; // FIXME hack, ffplay maybe should not use avio_feof() to test for the end

	/*if (seek_by_bytes < 0)
		seek_by_bytes = !!(ic->iformat->flags & AVFMT_TS_DISCONT) && strcmp("ogg", ic->iformat->name);*/

	is->max_frame_duration = 10.0 /*(ic->iformat->flags & AVFMT_TS_DISCONT) ? 10.0 : 3600.0*/;

	if (!window_title && (t = av_dict_get(ic->metadata, "title", NULL, 0)))
		window_title = av_asprintf("%s - %s", t->value, input_filename);

	

	/* 是否是播放 “实时流” */
	is->realtime = 1;/* is_realtime(ic);*/

	/*if (show_status)
		av_dump_format(ic, 0, is->filename, 0);*/

	
	
	is->show_mode = show_mode;
	if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
		AVStream *st = ic->streams[st_index[AVMEDIA_TYPE_VIDEO]];
		AVCodecContext *avctx = st->codec;
		AVRational sar = av_guess_sample_aspect_ratio(ic, st, NULL);
		if (avctx->width)
			set_default_window_size(avctx->width, avctx->height, sar);
	}

	/* open the streams, 同时会开启相应的 音视频线程 */
	if (st_index[AVMEDIA_TYPE_AUDIO] >= 0) {
		stream_component_open(is, st_index[AVMEDIA_TYPE_AUDIO]);
	}

	ret = -1;
	if (st_index[AVMEDIA_TYPE_VIDEO] >= 0) {
		ret = stream_component_open(is, st_index[AVMEDIA_TYPE_VIDEO]);
	}
	if (is->show_mode == SHOW_MODE_NONE)
		is->show_mode = ret >= 0 ? SHOW_MODE_VIDEO : SHOW_MODE_RDFT;

	/*if (st_index[AVMEDIA_TYPE_SUBTITLE] >= 0) {
		stream_component_open(is, st_index[AVMEDIA_TYPE_SUBTITLE]);
	}*/

	if (is->video_stream < 0 && is->audio_stream < 0) {
		av_log(NULL, AV_LOG_FATAL, "Failed to open file '%s' or configure filtergraph\n",
			is->filename);
		ret = -1;
		goto fail;
	}

	if (infinite_buffer < 0 && is->realtime)
		infinite_buffer = 1;

	if (0)
	{
		for (;;)
		{
			AVCodec *pCodec;
			AVCodecContext *pCodecCtx = NULL;
			AVCodecParserContext *pCodecParserCtx = NULL;

			FILE *fp_in;

			const int in_buffer_size = 4096;
			uint8_t in_buffer[in_buffer_size + FF_INPUT_BUFFER_PADDING_SIZE] = { 0 };
			uint8_t *cur_ptr;
			int cur_size;
			AVPacket packet;

			AVCodecID codec_id = AV_CODEC_ID_H264;
			char filepath_in[] = "bigbuckbunny_480x272.h264";

			pCodec = avcodec_find_decoder(codec_id);
			if (!pCodec) {
				printf("Codec not found\n");
				break;
			}
			pCodecCtx = avcodec_alloc_context3(pCodec);
			if (!pCodecCtx){
				printf("Could not allocate video codec context\n");
				break;
			}

			pCodecParserCtx = av_parser_init(codec_id);
			if (!pCodecParserCtx){
				printf("Could not allocate video parser context\n");
				break;
			}

			//if(pCodec->capabilities&CODEC_CAP_TRUNCATED)
			//    pCodecCtx->flags|= CODEC_FLAG_TRUNCATED; 

			if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
				printf("Could not open codec\n");
				break;
			}
			//Input File
			fp_in = fopen(filepath_in, "rb");
			if (!fp_in) {
				printf("Could not open input stream\n");
				break;
			}

			av_init_packet(&packet);
			while (1) {

				cur_size = fread(in_buffer, 1, in_buffer_size, fp_in);
				if (cur_size == 0)
					break;
				cur_ptr = in_buffer;

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

					stream_input_data(is, 0, packet.data, packet.size, AV_NOPTS_VALUE);
					SDL_Delay(40);
				}

			}
		}
	}
	

	/* wait until the end */
	while (!is->abort_request) {
		SDL_Delay(100);
	}

	ret = 0;
fail:
	/* close each stream */
	if (is->audio_stream >= 0)
		stream_component_close(is, is->audio_stream);
	if (is->video_stream >= 0)
		stream_component_close(is, is->video_stream);
	/*if (is->subtitle_stream >= 0)
		stream_component_close(is, is->subtitle_stream);*/
	if (ic) {
		//avformat_close_input(&ic);
		avformat_free_context(ic);
		is->ic = NULL;
	}

	if (ret != 0) {
		SDL_Event event;

		event.type = FF_QUIT_EVENT;
		event.user.data1 = is;
		SDL_PushEvent(&event);
	}
	//SDL_DestroyMutex(wait_mutex);
	return 0;
}


static VideoState *stream_open(CustomEventData * data  /*, const char *filename, AVInputFormat *iformat*/)
{
	VideoState *is;

	is = (VideoState *)av_mallocz(sizeof(VideoState));
	if (!is)
		return NULL;

	is->sdlWindow = data->window;
	is->width = data->nWidth;
	is->height = data->nHeight;

	//	第二个参数指示使用的显卡驱动（如 d3d、 opengl、software ）等， -1 表示自动选择 

	int n = SDL_GetNumRenderDrivers();

	SDL_Renderer * sdlRenderer = SDL_CreateRenderer(is->sdlWindow, -1, 0);
	SDL_RenderSetLogicalSize(sdlRenderer, is->width, is->height);
	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 0);

	is->sdlRenderer = sdlRenderer;
		
	memcpy(&is->media_format, &data->media_format, sizeof(stream_media_format));

	/*is->original_width = data->nWidth;
	is->original_height = data->nHeight;*/

	//file_iformat = av_find_input_format("sdp");

	/*int tt = sizeof(data->filename);
	av_strlcpy(is->filename, data->filename, sizeof(data->filename));
	is->iformat = file_iformat*/ /*iformat*/;
	is->ytop = 0;
	is->xleft = 0;

	/* start video display */
	if (frame_queue_init(&is->pictq, &is->videoq, VIDEO_PICTURE_QUEUE_SIZE, 1) < 0)
		goto fail;
	/*if (frame_queue_init(&is->subpq, &is->subtitleq, SUBPICTURE_QUEUE_SIZE, 0) < 0)
		goto fail;*/
	if (frame_queue_init(&is->sampq, &is->audioq, SAMPLE_QUEUE_SIZE, 1) < 0)
		goto fail;

	/* 初始化 视频包 和 音频包的 队列 */
	packet_queue_init(&is->videoq);
	packet_queue_init(&is->audioq);
	//packet_queue_init(&is->subtitleq);

	is->continue_read_thread = SDL_CreateCond();

	init_clock(&is->vidclk, &is->videoq.serial);	// 视频时钟在 update_video_pts 函数的首次调用时设置 
	init_clock(&is->audclk, &is->audioq.serial);
	init_clock(&is->extclk, &is->extclk.serial);
	is->audio_clock_serial = -1;
	is->av_sync_type = av_sync_type;		//	缺省：以音频流作为主频进行同步

	//	启动线程, 开启 音频 和 视频的解码线程
	//	最后从文件中读入数据，放置到 is->videoq 和 is->audioq 队列中；  网络流则不用
	is->read_tid = SDL_CreateThread(read_thread, "1111", is);
	//read_thread(is);

	// 100 ms 后开始延时视频更新，即将解码后的图片显示到屏幕
	is->timer = SDL_AddTimer(100, refresh_callback, is);
	if (!is->read_tid) {
	fail:
		stream_close(is);
		return NULL;
	}
	return is;
}


static void stream_cycle_channel(VideoState *is, int codec_type)
{
	AVFormatContext *ic = is->ic;
	int start_index, stream_index;
	int old_index;
	AVStream *st;
	AVProgram *p = NULL;
	int nb_streams = is->ic->nb_streams;

	if (codec_type == AVMEDIA_TYPE_VIDEO) {
		start_index = is->last_video_stream;
		old_index = is->video_stream;
	}
	else if (codec_type == AVMEDIA_TYPE_AUDIO) {
		start_index = is->last_audio_stream;
		old_index = is->audio_stream;
	}
	else {
		/*start_index = is->last_subtitle_stream;
		old_index = is->subtitle_stream;*/
	}
	stream_index = start_index;

	if (codec_type != AVMEDIA_TYPE_VIDEO && is->video_stream != -1) {
		p = av_find_program_from_stream(ic, NULL, is->video_stream);
		if (p) {
			nb_streams = p->nb_stream_indexes;
			for (start_index = 0; start_index < nb_streams; start_index++)
				if (p->stream_index[start_index] == stream_index)
					break;
			if (start_index == nb_streams)
				start_index = -1;
			stream_index = start_index;
		}
	}

	for (;;) {
		if (++stream_index >= nb_streams)
		{
			if (codec_type == AVMEDIA_TYPE_SUBTITLE)
			{
				stream_index = -1;
				//is->last_subtitle_stream = -1;
				goto the_end;
			}
			if (start_index == -1)
				return;
			stream_index = 0;
		}
		if (stream_index == start_index)
			return;
		st = is->ic->streams[p ? p->stream_index[stream_index] : stream_index];
		if (st->codec->codec_type == codec_type) {
			/* check that parameters are OK */
			switch (codec_type) {
			case AVMEDIA_TYPE_AUDIO:
				if (st->codec->sample_rate != 0 &&
					st->codec->channels != 0)
					goto the_end;
				break;
			case AVMEDIA_TYPE_VIDEO:
			case AVMEDIA_TYPE_SUBTITLE:
				goto the_end;
			default:
				break;
			}
		}
	}
the_end:
	if (p && stream_index != -1)
		stream_index = p->stream_index[stream_index];
	av_log(NULL, AV_LOG_INFO, "Switch %s stream from #%d to #%d\n",
		av_get_media_type_string((AVMediaType)codec_type),
		old_index,
		stream_index);

	stream_component_close(is, old_index);
	stream_component_open(is, stream_index);
}

void show_help_default(const char *opt, const char *arg)
{
	
}

static int lockmgr(void **mtx, enum AVLockOp op)
{
	switch (op) {
	case AV_LOCK_CREATE:
		*mtx = SDL_CreateMutex();
		if (!*mtx)
			return 1;
		return 0;
	case AV_LOCK_OBTAIN:
		return !!SDL_LockMutex((SDL_mutex *)*mtx);
	case AV_LOCK_RELEASE:
		return !!SDL_UnlockMutex((SDL_mutex *)*mtx);
	case AV_LOCK_DESTROY:
		SDL_DestroyMutex((SDL_mutex *)*mtx);
		return 0;
	}
	return 1;
}


static int  do_stream_ok(CustomEventData * data, uint32_t sessionId)
{
	//int titleLen = ::GetWindowTextLength(data->hwnd);

	//SDL_Window * sdlWindow = SDL_CreateWindowFrom((void *)(data->hwnd));
	/*int flags = SDL_WINDOW_OPENGL;
	SDL_Window * sdlWindow = SDL_CreateWindow("tttt", 100, 100, 500, 500 ,flags);*/
	SDL_LockMutex(data->mutex);
	data->allocated = 1;
	data->sessionId = sessionId;
	SDL_CondSignal(data->cond);
	SDL_UnlockMutex(data->mutex);

	return 0;
}


/* handle an event sent by the GUI */
static Uint32 event_loop_callback(Uint32 interval, void *param)
{
	SDL_Event event;
	double incr, pos, frac;
	VideoState *cur_stream = NULL;

	int nVideoNumber = 0;

	while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT)>0)
	{
		double x;
		switch (event.type) {
		case SDL_KEYDOWN:
			videostate_list_get_by_windowId(&lstVideoState, &cur_stream, event.key.windowID);
			if (exit_on_keydown) {
				do_exit(cur_stream);
				break;
			}
			switch (event.key.keysym.sym) {
			case SDLK_ESCAPE:
			case SDLK_q:
				do_exit(cur_stream);
				break;
			case SDLK_f:
				// toggle_full_screen(cur_stream);
				cur_stream->force_refresh = 1;
				break;
			case SDLK_p:
			case SDLK_SPACE:
				toggle_pause(cur_stream);
				break;
			case SDLK_s: // S: Step to next frame
				step_to_next_frame(cur_stream);
				break;
			case SDLK_a:
				stream_cycle_channel(cur_stream, AVMEDIA_TYPE_AUDIO);
				break;
			case SDLK_v:
				stream_cycle_channel(cur_stream, AVMEDIA_TYPE_VIDEO);
				break;
			case SDLK_c:
				stream_cycle_channel(cur_stream, AVMEDIA_TYPE_VIDEO);
				stream_cycle_channel(cur_stream, AVMEDIA_TYPE_AUDIO);
				stream_cycle_channel(cur_stream, AVMEDIA_TYPE_SUBTITLE);
				break;
			case SDLK_t:
				stream_cycle_channel(cur_stream, AVMEDIA_TYPE_SUBTITLE);
				break;
			case SDLK_w:
#if CONFIG_AVFILTER
				if (cur_stream->show_mode == SHOW_MODE_VIDEO && cur_stream->vfilter_idx < nb_vfilters - 1) {
					if (++cur_stream->vfilter_idx >= nb_vfilters)
						cur_stream->vfilter_idx = 0;
				}
				else {
					cur_stream->vfilter_idx = 0;
					toggle_audio_display(cur_stream);
				}
#else
				//toggle_audio_display(cur_stream);
#endif
				break;
			case SDLK_PAGEUP:
				if (cur_stream->ic->nb_chapters <= 1) {
					incr = 600.0;
					goto do_seek;
				}
				//seek_chapter(cur_stream, 1);
				break;
			case SDLK_PAGEDOWN:
				if (cur_stream->ic->nb_chapters <= 1) {
					incr = -600.0;
					goto do_seek;
				}
				//seek_chapter(cur_stream, -1);
				break;
			case SDLK_LEFT:
				incr = -10.0;
				goto do_seek;
			case SDLK_RIGHT:
				incr = 10.0;
				goto do_seek;
			case SDLK_UP:
				incr = 60.0;
				goto do_seek;
			case SDLK_DOWN:
				incr = -60.0;
			do_seek:
				if (seek_by_bytes) {
					pos = -1;
					if (pos < 0 && cur_stream->video_stream >= 0)
						pos = frame_queue_last_pos(&cur_stream->pictq);
					if (pos < 0 && cur_stream->audio_stream >= 0)
						pos = frame_queue_last_pos(&cur_stream->sampq);
					if (pos < 0)
						pos = avio_tell(cur_stream->ic->pb);
					if (cur_stream->ic->bit_rate)
						incr *= cur_stream->ic->bit_rate / 8.0;
					else
						incr *= 180000.0;
					pos += incr;
					stream_seek(cur_stream, pos, incr, 1);
				}
				else {
					pos = get_master_clock(cur_stream);
					if (isnan(pos))
						pos = (double)cur_stream->seek_pos / AV_TIME_BASE;
					pos += incr;
					if (cur_stream->ic->start_time != AV_NOPTS_VALUE && pos < cur_stream->ic->start_time / (double)AV_TIME_BASE)
						pos = cur_stream->ic->start_time / (double)AV_TIME_BASE;
					stream_seek(cur_stream, (int64_t)(pos * AV_TIME_BASE), (int64_t)(incr * AV_TIME_BASE), 0);
				}
				break;
			default:
				break;
			}
			break;
			/* case SDL_VIDEOEXPOSE:
			cur_stream->force_refresh = 1;
			break;*/
		case SDL_MOUSEBUTTONDOWN:
			if (exit_on_mousedown) {
				videostate_list_get_by_windowId(&lstVideoState, &cur_stream, event.button.windowID);
				do_exit(cur_stream);
				break;
			}
			break;
		case SDL_MOUSEMOTION:
			videostate_list_get_by_windowId(&lstVideoState, &cur_stream, event.motion.windowID);
			if (cursor_hidden) {
				SDL_ShowCursor(1);
				cursor_hidden = 0;
			}
			cursor_last_shown = av_gettime_relative();
			if (event.type == SDL_MOUSEBUTTONDOWN) {
				x = event.button.x;
			}
			else {
				if (event.motion.state != SDL_PRESSED)
					break;
				x = event.motion.x;
			}
			if(cur_stream)
			{
				if (seek_by_bytes || cur_stream->ic->duration <= 0) {
					uint64_t size = avio_size(cur_stream->ic->pb);
					stream_seek(cur_stream, size*x / cur_stream->width, 0, 1);
				}
				else {
					int64_t ts;
					int ns, hh, mm, ss;
					int tns, thh, tmm, tss;
					tns = cur_stream->ic->duration / 1000000LL;
					thh = tns / 3600;
					tmm = (tns % 3600) / 60;
					tss = (tns % 60);
					frac = x / cur_stream->width;
					ns = frac * tns;
					hh = ns / 3600;
					mm = (ns % 3600) / 60;
					ss = (ns % 60);
					av_log(NULL, AV_LOG_INFO,
						"Seek to %2.0f%% (%2d:%02d:%02d) of total duration (%2d:%02d:%02d)       \n", frac * 100,
						hh, mm, ss, thh, tmm, tss);
					ts = frac * cur_stream->ic->duration;
					if (cur_stream->ic->start_time != AV_NOPTS_VALUE)
						ts += cur_stream->ic->start_time;
					stream_seek(cur_stream, ts, 0, 0);
				}
			}
			break;
			/*case SDL_VIDEORESIZE:
			screen = SDL_SetVideoMode(FFMIN(16383, event.resize.w), event.resize.h, 0,
			SDL_HWSURFACE|(is_full_screen?SDL_FULLSCREEN:SDL_RESIZABLE)|SDL_ASYNCBLIT|SDL_HWACCEL);
			if (!screen) {
			av_log(NULL, AV_LOG_FATAL, "Failed to set video mode\n");
			do_exit(cur_stream);
			}
			screen_width  = cur_stream->width  = screen->w;
			screen_height = cur_stream->height = screen->h;
			cur_stream->force_refresh = 1;
			break;*/
		case SDL_WINDOWEVENT:
			
			if (event.window.event == SDL_WINDOWEVENT_RESIZED) {

				videostate_list_get_by_windowId(&lstVideoState, &cur_stream, event.window.windowID);

				if(cur_stream)
				{

					//SDL_RenderSetViewport(cur_stream->sdlRenderer, NULL);

					int winSizeW, winSizeH;
					SDL_GetWindowSize(cur_stream->sdlWindow, &winSizeW, &winSizeH);


					cur_stream->width = winSizeW;
					cur_stream->height = winSizeH;

					/*if (cur_stream->sdlRenderer)
					{
						SDL_DestroyRenderer(cur_stream->sdlRenderer);
					}

					cur_stream->sdlRenderer = SDL_CreateRenderer(cur_stream->sdlWindow, -1, 0);
					SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"); */

					// SDL_RenderSetLogicalSize(cur_stream->sdlRenderer, 0, 0);
					SDL_Rect rect;
					rect.x = 0;
					rect.y = 0;
					rect.w = winSizeW;
					rect.h = winSizeH;
					//SDL_RenderSetViewport(cur_stream->sdlRenderer, NULL);

					SDL_RenderSetLogicalSize(cur_stream->sdlRenderer, 0, 0);
					cur_stream->force_refresh = 1;
				}
			}

			break;
		case SDL_QUIT:
		case FF_QUIT_EVENT:
			do_exit(cur_stream);
			break;
		case FF_ALLOC_EVENT:
			alloc_picture((VideoState *)event.user.data1);
			break;
		case FF_STREAM_OPEN_EVENT:
			cur_stream = stream_open((CustomEventData *)event.user.data1);
			if (cur_stream != NULL)
			{
				//	查找可以使用的 SessionId
				uint32_t sessionId;
				do {
					sessionId = (uint32_t)(our_random32() & 0x00FFFFFF);
				} while (sessionId == 0 || videostate_list_get_by_sessionId(&lstVideoState, NULL, sessionId)>0);

				cur_stream->sessionId = sessionId;
				videostate_list_put(&lstVideoState, cur_stream);
				do_stream_ok((CustomEventData *)event.user.data1, sessionId);

			}
			break;
		case FF_STREAM_CLOSE_EVENT:
			{
				CustomEventData * pData = (CustomEventData *)event.user.data1;
				if (videostate_list_pop_by_sessionId(&lstVideoState, &cur_stream, pData->sessionId) > 0)
				{
					stream_close(cur_stream);
				}
				do_stream_ok(pData, 0);
			}
			break;
		case FF_STREAM_SPEED_EVENT:
			{
				CustomEventData * pData = (CustomEventData *)event.user.data1;
				if (videostate_list_get_by_sessionId(&lstVideoState, &cur_stream, pData->sessionId) > 0)
				{
					stream_speed(cur_stream, pData->speed);
				}
				do_stream_ok(pData, 0);
			}
			break;
		case FF_STREAM_INPUT_DATA_EVENT:
			{
				CustomEventData * pData = (CustomEventData *)event.user.data1;
				if (videostate_list_get_by_sessionId(&lstVideoState, &cur_stream, pData->sessionId) > 0)
				{
					if (pData->stream_type == PLAYER_MEDIA_TYPE_AUDIO && cur_stream->audio_stream >= 0)
						stream_input_data(cur_stream, cur_stream->audio_stream, pData->buf, pData->buf_size, pData->pts);
					else if (pData->stream_type == PLAYER_MEDIA_TYPE_VIDEO && cur_stream->video_stream >= 0)
					{
						stream_input_data(cur_stream, cur_stream->video_stream, pData->buf, pData->buf_size, pData->pts);
						nVideoNumber++;
					}
				}
				//do_stream_ok(pData, 0);  异步调用，释放 CustomEventData 内存
				delete pData;
			}
			break;

		default:
			break;
		}
	}

	//	10 ms 后重新扫描
	return REFRESH_RATE * 1000;
}


//复位
//Reset
int ffmfc_reset_index(){
	vframe_index = 0;
	aframe_index = 0;
	packet_index = 0;
	return 0;
}


//解码主函数
//Main function
#define __MINGW32__
int do_init()
{
	ffmfc_reset_index();
	//退出符号置零
	exit_remark = 0;
	int flags;
	//核心结构体
	VideoState *is;

	char dummy_videodriver[] = "SDL_VIDEODRIVER=dummy";


	//   av_log_set_flags(AV_LOG_SKIP_REPEATED);
	//    parse_loglevel(argc, argv, options);

	/* register all codecs, demux and protocols */
	avcodec_register_all();
#if CONFIG_AVDEVICE
	avdevice_register_all();
#endif
#if CONFIG_AVFILTER
	avfilter_register_all();
#endif
	av_register_all();
	//如果没有本句会出错误：
	//Warning:Using network protocols without global network initialization.
	//测试网络数据的时候直接使用rtsp://169.254.197.35:8554/sh1943.mpg格式的参数就行
	//或是使用 rtmp://localhost/vod/sample.flv都可以
	//因为此版本ffmpeg集成了librtmp
	//和文件很类似
	avformat_network_init();

	//it_opts();

	signal(SIGINT, sigterm_handler); /* Interrupt (ANSI).    */
	signal(SIGTERM, sigterm_handler); /* Termination (ANSI).  */
	//input_filename = "rtmp://live.hkstv.hk.lxdns.com/live/hks live=1";

	//传入文件路径
	/*input_filename=(const char *)malloc(MAX_URL_LENGTH);
	GetWindowTextA(dlg->m_inputurl,(LPSTR)input_filename,MAX_URL_LENGTH);*/

	//show_banner(argc, argv, options);
	//获取播放的首选项
	int opt_argc = 0;
	char **opt_argv = NULL;

	//parse_options(NULL, opt_argc, opt_argv, options, opt_input_file);
	/*
	if (!input_filename)
	{
	show_usage();
	fprintf(stderr, "An input file must be specified\n");
	fprintf(stderr, "Use -h to get full help or, even better, run 'man %s'\n", program_name);
	exit(1);
	}
	*/
	if (display_disable)
	{
		video_disable = 1;
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,  "linear");  // make the scaled rendering look smoother.
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");  // 设置为 1 时，会导致 SDL_RenderPresent 刷新函数绘制变慢

	/*
		缺乏如下设置置，会导致 
		Direct3D9: (WARN) :Device that was created without D3DCREATE_MULTITHREADED is being used by a thread other than the creation thread.
		Direct3D9: (ERROR) :ResetEx can only be called from the thread that the device was created from.

		C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Utilities\bin\x86 目录下
		打开 dxcpl.exe （directx control pannel）, 将 directx 的debug 属性设置为最大，会在 msvc 的输出中指示如上错误信息
	*/
	SDL_SetHint(SDL_HINT_RENDER_DIRECT3D_THREADSAFE, "1");

	//设置为自动退出-------------------
	//解码图片的时候，会自动退出，所以先不设置了~
	//autoexit=1;
	//---------------------------------
	flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER;
	if (audio_disable)
		flags &= ~SDL_INIT_AUDIO;
#if 0
	if (display_disable)
		SDL_putenv(dummy_videodriver); /* For the event queue, we always need a video driver. */
#else
	char sdl_var[128];
	//sprintf(sdl_var, "SDL_WINDOWID=0x%lx", hWnd );//主窗口句柄 
	/*SDL_putenv(sdl_var);
	char *myvalue = SDL_getenv("SDL_WINDOWID");*/
#endif

#if !defined(__MINGW32__) && !defined(__APPLE__)
	flags |= SDL_INIT_EVENTTHREAD; /* Not supported on Windows or Mac OS X */
#endif
	//------SDL------------------------
	//初始化
	if (SDL_Init(flags)) {
		//AfxMessageBox(_T("Could not initialize SDL "));
		exit(1);
	}

	if (!display_disable) {
#if HAVE_SDL_VIDEO_SIZE
		/*const SDL_VideoInfo *vi = SDL_GetVideoInfo();
		fs_screen_width = vi->current_w;
		fs_screen_height = vi->current_h;*/

		//	获取显示器的分辨率
		SDL_Rect rectDisplay;
		SDL_GetDisplayBounds(0, &rectDisplay);
		fs_screen_width = rectDisplay.w;
		fs_screen_height = rectDisplay.h;
#endif
	}

	// SDL_EventState(SDL_ACTIVEEVENT, SDL_IGNORE);
	SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
	//SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

	if (av_lockmgr_register(lockmgr)) {
		//AfxMessageBox(_T("Could not initialize lock manager!"));
		do_exit(NULL);
	}

	av_init_packet(&flush_pkt);
	flush_pkt.data = (uint8_t *)(intptr_t)"FLUSH";
	//解码主函数
	/*is = stream_open(input_filename, file_iformat);
	if (!is) {
	AfxMessageBox(_T("Failed to initialize VideoState!"));
	do_exit(NULL);
	}
	g_is = is;*/

	autoexit = 1;
	//const char *info=(char *)malloc(500);
	//info=avformat_configuration();

	// 视频流的会话管理
	videostate_list_init(&lstVideoState);

	// 启动定时事件扫描及处理
	SDL_TimerID timer = SDL_AddTimer(10, event_loop_callback, NULL);
	if (NULL == timer)
	{
		SDL_Log("add timer fail: %s\n", SDL_GetError());
	}

	/* never returns */

	return 0;
}

