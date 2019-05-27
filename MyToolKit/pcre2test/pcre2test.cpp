// pcre2test.cpp : 定义控制台应用程序的入口点。
//
//////////////////////////////////////////////////////////////////////////
// 本程序改编自 pcre2demo.c

#include "stdafx.h"


#define PCRE2_CODE_UNIT_WIDTH 8

#define PCRE2_STATIC

#include "pcre2.h"
#include "stdio.h"
#include <string.h>
#include "stdlib.h"


int _tmain(int argc, _TCHAR* argv[])
{

	pcre2_code *re;
	PCRE2_SPTR pattern;     /* PCRE2_SPTR is a pointer to unsigned code units of */
	PCRE2_SPTR subject;     /* the appropriate width (8, 16, or 32 bits). */
	PCRE2_SPTR name_table;

	int crlf_is_newline;
	int errornumber;
	int find_all;
	int i;
	int namecount;
	int name_entry_size;
	int rc;
	int utf8;

	uint32_t option_bits;
	uint32_t newline;

	PCRE2_SIZE erroroffset;
	PCRE2_SIZE *ovector;

	size_t subject_length;
	pcre2_match_data *match_data;


	/*pattern = (PCRE2_SPTR)argv[i];
	subject = (PCRE2_SPTR)argv[i+1];*/

	pattern = (PCRE2_SPTR)_T("\\(\\d+\\)_");
	subject = (PCRE2_SPTR)_T("浙江富润(111781111)_(600070)_tt(6000555)_tt(6007876)_ttt利润表");

	utf8 = 1;
	pattern = (PCRE2_SPTR)"(\xE4\xB8\xA5)|(\xE4\xB8\x81)";
	string val = "你啊，严肃一二三丁了解具体，严格匹配丁， 严格执行";
	val = Gbk2Utf8(val);
	subject = (PCRE2_SPTR)val.c_str();

	subject_length = strlen((char *)subject);

	/*************************************************************************
	* Now we are going to compile the regular expression pattern, and handle *
	* any errors that are detected.                                          *
	*************************************************************************/

	re = pcre2_compile(
		pattern,               /* the pattern */
		PCRE2_ZERO_TERMINATED, /* indicates pattern is zero-terminated */
		0,                     /* default options */
		&errornumber,          /* for error number */
		&erroroffset,          /* for error offset */
		NULL);                 /* use default compile context */

	/* Compilation failed: print the error message and exit. */

	if (re == NULL)
	{
		PCRE2_UCHAR buffer[256];
		pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
		printf("PCRE2 compilation failed at offset %d: %s\n", (int)erroroffset,
			buffer);
		return 1;
	}


	/*************************************************************************
	* If the compilation succeeded, we call PCRE again, in order to do a     *
	* pattern match against the subject string. This does just ONE match. If *
	* further matching is needed, it will be done below. Before running the  *
	* match we must set up a match_data block for holding the result.        *
	*************************************************************************/


	/* Using this function ensures that the block is exactly the right size for
	the number of capturing parentheses in the pattern. */


	match_data = pcre2_match_data_create_from_pattern(re, NULL);

	rc = pcre2_match(
		re,                   /* the compiled pattern */
		subject,              /* the subject string */
		subject_length,       /* the length of the subject */
		0,                    /* start at offset 0 in the subject */
		0,                    /* default options */
		match_data,           /* block for storing the result */
		NULL);                /* use default match context */

	/* Matching failed: handle error cases */

	if (rc < 0)
	{
	  switch(rc)
		{
		case PCRE2_ERROR_NOMATCH: printf("No match\n"); break;
		/*
		Handle other special cases if you like
		*/
		default: printf("Matching error %d\n", rc); break;
		}
	  pcre2_match_data_free(match_data);   /* Release memory used for the match */
	  pcre2_code_free(re);                 /* data and the compiled pattern. */
	  return 1;
	}


	/* Match succeded. Get a pointer to the output vector, where string offsets are
	stored. */

	ovector = pcre2_get_ovector_pointer(match_data);
	printf("\nMatch succeeded at offset %d\n", (int)ovector[0]);

	/*************************************************************************
	* We have found the first match within the subject string. If the output *
	* vector wasn't big enough, say so. Then output any substrings that were *
	* captured.                                                              *
	*************************************************************************/

	/* The output vector wasn't big enough. This should not happen, because we used
	pcre2_match_data_create_from_pattern() above. */

	if (rc == 0)
		printf("ovector was not big enough for all the captured substrings\n");

	/* Show substrings stored in the output vector by number. Obviously, in a real
	application you might want to do things other than print them. */

	//////////////////////////////////////////////////////////////////////////
	//	输出找到的匹配字符串

	for (i = 0; i < rc; i++)
	{
		int offset = ovector[2*i];
		PCRE2_SPTR substring_start = subject + ovector[2*i];
		size_t substring_length = ovector[2*i+1] - ovector[2*i];

		string strMatch = string((char *)substring_start, substring_length);
		strMatch = Utf8_GBK(strMatch);
		printf("%2d: off %d  %.*s\n", i, offset, (int)substring_length, strMatch.c_str() /*(char *)substring_start*/);
	}
	

	/* Loop for second and subsequent matches */
	for (;;)
	{
		uint32_t options = 0;                    /* Normally no options */
		PCRE2_SIZE start_offset = ovector[1];  /* Start at end of previous match */

		/* If the previous match was for an empty string, we are finished if we are
		at the end of the subject. Otherwise, arrange to run another match at the
		same point to see if a non-empty match can be found. */

		if (ovector[0] == ovector[1])
		{
			if (ovector[0] == subject_length) break;
			options = PCRE2_NOTEMPTY_ATSTART | PCRE2_ANCHORED;
		}

		/* Run the next matching operation */

		rc = pcre2_match(
			re,                   /* the compiled pattern */
			subject,              /* the subject string */
			subject_length,       /* the length of the subject */
			start_offset,         /* starting offset in the subject */
			options,              /* options */
			match_data,           /* block for storing the result */
			NULL);                /* use default match context */

		if (rc == PCRE2_ERROR_NOMATCH)
		{
			if (options == 0) break;                    /* All matches found */
			ovector[1] = start_offset + 1;              /* Advance one code unit */
			if (crlf_is_newline &&                      /* If CRLF is newline & */
				start_offset < subject_length - 1 &&    /* we are at CRLF, */
				subject[start_offset] == '\r' &&
				subject[start_offset + 1] == '\n')
				ovector[1] += 1;                          /* Advance by one more. */
			else if (utf8)                              /* Otherwise, ensure we */
			{                                         /* advance a whole UTF-8 */
				while (ovector[1] < subject_length)       /* character. */
				{
					if ((subject[ovector[1]] & 0xc0) != 0x80) break;
					ovector[1] += 1;
				}
			}
			continue;    /* Go round the loop again */
		}

		/* Other matching errors are not recoverable. */

		if (rc < 0)
		{
			printf("Matching error %d\n", rc);
			pcre2_match_data_free(match_data);
			pcre2_code_free(re);
			return 1;
		}

		/* Match succeded */

		printf("\nMatch succeeded again at offset %d\n", (int)ovector[0]);

		/* The match succeeded, but the output vector wasn't big enough. This
		should not happen. */
		if (rc == 0)
			printf("ovector was not big enough for all the captured substrings\n");

		//	输出找到的匹配字符串

		for (i = 0; i < rc; i++)
		{
			int offset = ovector[2*i];
			PCRE2_SPTR substring_start = subject + ovector[2*i];
			size_t substring_length = ovector[2*i+1] - ovector[2*i];

			string strMatch = string((char *)substring_start, substring_length);
			strMatch = Utf8_GBK(strMatch);
			printf("%2d: off%d  %.*s\n", i, offset, (int)substring_length, strMatch.c_str() /*(char *)substring_start*/);
		}
	}

	pcre2_match_data_free(match_data);
	pcre2_code_free(re);

	//string str = string((char *)&subject[ovector[0]]);
	getchar();
	return 0 ;
}

