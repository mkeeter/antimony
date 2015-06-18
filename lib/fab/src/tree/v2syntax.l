
%{
	#include <math.h>
	#include <stdlib.h>
	#include "fab/tree/v2parser.hpp"
	#include "v2syntax.lemon.hpp"
%}

%option reentrant
%option noyywrap

%x V2

white				[ \t]+
digit				[0-9]
integer 			{digit}+
exponent			[eE][+-]?{integer}
real				{integer}*("."{digit}+)?{exponent}?

%%

{white}				;
{real}				return TOKEN_V1FLOAT;
"+"				return TOKEN_V1PLUS;
"-"				return TOKEN_V1MINUS;
"*"				return TOKEN_V1MUL;
"/"				return TOKEN_V1DIV;
"i"				return TOKEN_V1MIN;
"a"				return TOKEN_V1MAX;
"p"				return TOKEN_V1POW;
"s"				return TOKEN_V1SIN;
"c"				return TOKEN_V1COS;
"t"				return TOKEN_V1TAN;
"S"				return TOKEN_V1ASIN;
"C"				return TOKEN_V1ACOS;
"T"				return TOKEN_V1ATAN;
"b"				return TOKEN_V1ABS;
"q"				return TOKEN_V1SQUARE;
"r"				return TOKEN_V1SQRT;
"n"				return TOKEN_V1NEG;
"x"				return TOKEN_V1EXP;
"m"				return TOKEN_V1MAP;
"_"				return TOKEN_V1SKIP;

"X"				return TOKEN_V1X;
"Y"				return TOKEN_V1Y;
"Z"				return TOKEN_V1Z;

"="				{
					BEGIN V2;
					return TOKEN_EQUAL;
				}

"f"				return TOKEN_CONSTANT;
"]"				{
					BEGIN V2;
					return TOKEN_RBRACKET;
				}


<V2>{
	{real} 			return TOKEN_FLOAT;
	{white}			;

	"+"			return TOKEN_PLUS;
	"-"			return TOKEN_MINUS;
	"*"			return TOKEN_MUL;
	"/"			return TOKEN_DIV;
	"**"			return TOKEN_DOUBLESTAR;

	"min"			return TOKEN_MIN;
	"max"			return TOKEN_MAX;
	"pow"			return TOKEN_POW;
	"sin"			return TOKEN_SIN;
	"cos" 			return TOKEN_COS;
	"tan"			return TOKEN_TAN;
	"asin"			return TOKEN_ASIN;
	"acos"			return TOKEN_ACOS;
	"atan"			return TOKEN_ATAN;
	"abs"			return TOKEN_ABS;
	"sqrt"			return TOKEN_SQRT;
	"exp"			return TOKEN_EXP;

	"map"			return TOKEN_MAP;

	"X"			return TOKEN_X;
	"Y"			return TOKEN_Y;
	"Z"			return TOKEN_Z;

	","			return TOKEN_COMMA;
	"("			return TOKEN_LPAREN;
	")"			return TOKEN_RPAREN;

	"["			{
					BEGIN INITIAL;
					return TOKEN_LBRACKET;
				}

	"{"			return TOKEN_LBRACE;
	"}"			return TOKEN_RBRACE;

	"_"			return TOKEN_SKIP;

	";"			{
					BEGIN INITIAL;
					return TOKEN_SEMICOLON;
				}
}


%%
