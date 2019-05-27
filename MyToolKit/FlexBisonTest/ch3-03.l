%{
/* We usually need these... */
#include <stdio.h>
#include <stdlib.h>

/* Include this to use yylex_destroy for flex version < 2.5.9 */
#include "flex_memory_fix.h"

/* This is required and is generated automatically by bison from the .y file */
#include "y.tab.h"

/* Local stuff we need here... */
#include <math.h>
extern double vbltable[26];
%}

/* Add this to get line numbers... */
%option yylineno

%%
([0-9]+|([0-9]*\.[0-9]+)([eE][-+]?[0-9]+)?) {
	yylval.dval = atof(yytext); return NUMBER;
	}

[ \t]	;		 /* ignore white space */

[a-z]	{ yylval.vblno = yytext[0] - 'a'; return NAME; }

"$"	{ return 0; /* end of input */ }

\n	|
.	return yytext[0];
%%

/* We need to add a main() function. 
 * It is more convenient to put it here to manage flex memory management issues.
 * At the minimum it must call yyparse().
 */
extern int yyparse();

int main(int argc, char *argv[])
{
	printf("Enter sums using + - * / and () or type $ to quit.\n");
	yyparse();	/* REQUIRED */
	yylex_destroy();	/* Add to clean up memory leaks */
}
