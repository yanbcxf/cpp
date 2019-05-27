%{
/* For printf() */
#include <stdio.h>

/* Proformas for functions we define below... */
void yyerror(char *s);
int yylex(void);

/* Specific for here... */
double vbltable[26];
%}

%union {
	double dval;
	int vblno;
}

%token <vblno> NAME
%token <dval> NUMBER
%left '-' '+'
%left '*' '/'
%nonassoc UMINUS

%type <dval> expression
%%
statement_list:	statement '\n'
	|	statement_list statement '\n'
	;

statement:	NAME '=' expression	{ vbltable[$1] = $3; }
	|	expression		{ printf("= %g\n", $1); }
	;

expression:	expression '+' expression { $$ = $1 + $3; }
	|	expression '-' expression { $$ = $1 - $3; }
	|	expression '*' expression { $$ = $1 * $3; }
	|	expression '/' expression
				{	if($3 == 0.0)
						yyerror("divide by zero");
					else
						$$ = $1 / $3;
				}
	|	'-' expression %prec UMINUS	{ $$ = -$2; }
	|	'(' expression ')'	{ $$ = $2; }
	|	NUMBER
	|	NAME			{ $$ = vbltable[$1]; }
	;
%%
/* An optional but friendlier yyerror function... */
void yyerror(char *s)
{
	extern int yylineno;	// defined and maintained in lex
	extern char *yytext;	// defined and maintained in lex
	fprintf(stderr, "ERROR: %s at symbol '%s' on line %d\n", s, yytext, yylineno);
}
