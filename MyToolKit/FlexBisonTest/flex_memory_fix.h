/* flex_memory_fix.h */

#ifndef FLEX_MEMORY_FIX_H_
#define FLEX_MEMORY_FIX_H_

#ifdef YY_FLEX_SUBMINOR_VERSION
#define FLEX_VERSION               \
	(YY_FLEX_MAJOR_VERSION) * 1000000 \
	+ (YY_FLEX_MINOR_VERSION) * 1000    \
	+ (YY_FLEX_SUBMINOR_VERSION)
#else
#define FLEX_VERSION               \
	(YY_FLEX_MAJOR_VERSION) * 1000000 \
	+ (YY_FLEX_MINOR_VERSION) * 1000
#endif
/* yylex_destroy is not defined before Flex 2.5.9, so use this fudge */
#if FLEX_VERSION < 2005009
#define yylex_destroy() yy_delete_buffer(YY_CURRENT_BUFFER)
#endif

#endif /* end FLEX_MEMORY_FIX_H_ */
