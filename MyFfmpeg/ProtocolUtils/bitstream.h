#ifndef __BITSTREAM_H__
#define __BITSTREAM_H__

#include "../FfplaySdl2Pure/media_def.h"

typedef struct {
	u32_t bit_buf;
	u32_t bit_left;			//	代表 bit_buf 的高位还剩余的空闲bit 位数
	
	u8_t *buf, *buf_ptr, *buf_end;

	u32_t size_in_bits;
}PutBitContext;

typedef struct GetBitContext {
	u32_t bit_buf;
	u32_t bit_left;
	
    const u8_t *buffer, *buffer_ptr,*buffer_end;
    
    u32_t size_in_bits;

} GetBitContext;

#ifdef __cplusplus
extern "C" {
#endif
	
int allocbitcontext(PutBitContext * * c);
void init_put_bits(PutBitContext * s, u8_t * buffer, int buffer_size);
void put_bits(PutBitContext * c, s32_t n, u32_t value);
void flush_put_bits(PutBitContext * s);
int put_bits_count(PutBitContext * s);
u8_t *put_bits_ptr(PutBitContext * s);
void put_bits32(PutBitContext * s, u32_t value);

void init_get_bits(GetBitContext * s, u8_t * buf, u32_t buf_size);
u32_t get_bits32(GetBitContext * s);
u32_t get_bits(GetBitContext * s, s32_t n);

#ifdef __cplusplus
}
#endif 

#endif

