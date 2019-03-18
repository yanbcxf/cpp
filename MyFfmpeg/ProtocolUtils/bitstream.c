#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bitstream.h"

#define AV_WB32(p, d) do {                   \
        ((u8_t*)(p))[3] = (d);               \
        ((u8_t*)(p))[2] = (d)>>8;            \
        ((u8_t*)(p))[1] = (d)>>16;           \
        ((u8_t*)(p))[0] = (d)>>24;           \
    } while(0)


#define AV_RB32(p, d) do {                   \
       (d) = ((u8_t*)(p))[3] | ((u8_t*)(p))[2]<<8 | ((u8_t*)(p))[1]<<16 |  ((u8_t*)(p))[0]<<24;  \
       } while(0)



void init_put_bits(PutBitContext *s, u8_t *buffer, int buffer_size)
{
    if(buffer_size < 0) {
        buffer_size = 0;
        buffer = NULL;
    }

    s->size_in_bits= 8*buffer_size;
    s->buf = buffer;
    s->buf_end = s->buf + buffer_size - 1;
    s->buf_ptr = s->buf;
    s->bit_left=32;
    s->bit_buf=0;
}



void put_bits(PutBitContext *c, s32_t n, u32_t value)
{
	u32_t bit_buf;
    s32_t bit_left;

	bit_buf  = c->bit_buf;
    bit_left = c->bit_left;

	if (n < bit_left) {
        bit_buf   = (bit_buf<<n) | value;
        bit_left -= n;
    } 
	else {
		bit_buf <<= bit_left;
        bit_buf |= value >> (n - bit_left);
        AV_WB32(c->buf_ptr, bit_buf);
        //printf("bitbuf = %08x\n", bit_buf);
        c->buf_ptr+=4;
        bit_left+=32 - n;
        bit_buf = value;
    }

	c->bit_buf = bit_buf;
    c->bit_left = bit_left;
}

void put_bits32(PutBitContext *s, u32_t value)
{
    int lo = value & 0xffff;
    int hi = value >> 16;

    put_bits(s, 16, hi);
    put_bits(s, 16, lo);

}

// 将 bit_buf 中的内容放置到 buf_ptr 指向的正式缓存区
void flush_put_bits(PutBitContext *s)
{
	 if (s->bit_left < 32)
        s->bit_buf<<= s->bit_left;
	 
    while (s->bit_left < 32) {
        *s->buf_ptr++=s->bit_buf >> 24;
        s->bit_buf<<=8;
        s->bit_left+=8;
    }
    s->bit_left=32;
    s->bit_buf=0;
}


int put_bits_count(PutBitContext *s)
{
    return (s->buf_ptr - s->buf) * 8 + 32 - s->bit_left;
}

u8_t* put_bits_ptr(PutBitContext *s)
{
        return s->buf_ptr;
}



void init_get_bits(GetBitContext *s, u8_t *buf, u32_t buf_size)
{
	s->buffer       = buf;
	s->buffer_ptr   = s->buffer;
	s->buffer_end   = buf + buf_size;
    s->size_in_bits = buf_size*8;
   
    s->bit_left     = 32;
	AV_RB32(s->buffer_ptr, s->bit_buf);
	s->buffer_ptr += 4;
}

u32_t get_bits(GetBitContext *s, s32_t n)
{
	u32_t tmp = 0;
	u32_t bit_buf;
    s32_t bit_left;

	bit_buf  = s->bit_buf;
    bit_left = s->bit_left;

	if(n < bit_left) {
		tmp = bit_buf >> (32 - bit_left) ;
		tmp >>=  (bit_left - n);
		bit_buf <<= n;
		bit_left -= n;
		
	}
	else {
		tmp = bit_buf >> (32 - bit_left) ;
		
		tmp <<= n - bit_left;
		AV_RB32(s->buffer_ptr, bit_buf);
		s->buffer_ptr+=4;
		
		n = n - bit_left;
		if(n > 0)
			tmp |= (bit_buf >> (32 - n));
		bit_buf <<= n;
		bit_left = 32 - n;
	}

	s->bit_buf = bit_buf;
	s->bit_left = bit_left;

	return tmp;
}

u32_t get_bits32(GetBitContext *s)
{
	u32_t tmp;

	tmp = get_bits(s, 16);
	tmp <<= 16;
	tmp |= get_bits(s, 16);

	return tmp;
}


