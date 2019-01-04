%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "StackStructs.h"
#include "parser.tab.hpp"
#include "output.hpp"
#include "symbolTable.hpp"

void debug(yytokentype token);
extern symbolTable* tables;
/* Don't forget to remove the debug before handing in */
#define REST_OF_MACRO(token) yylval.str = yytext;\
							 yylval.lineno = yylineno;\
							 debug(token);\
							 tables->setLine(yylineno);\
							 return token;
#define FLEX_MACRO(token)		yylval = StackType(); REST_OF_MACRO(token)
#define TYPE_FLEX_MACRO(token, type)		yylval = StackType(type); REST_OF_MACRO(token)

%}

%option yylineno
%option noyywrap

%%
void						FLEX_MACRO(VOID)
int							TYPE_FLEX_MACRO(INT, types_Int)
byte						TYPE_FLEX_MACRO(BYTE, types_Byte)
b							FLEX_MACRO(B)
bool						TYPE_FLEX_MACRO(BOOL, types_Bool)
struct						FLEX_MACRO(STRUCT)
and							FLEX_MACRO(AND)
or							FLEX_MACRO(OR)
not							FLEX_MACRO(NOT)
true						TYPE_FLEX_MACRO(TRUE, types_Bool)
false						TYPE_FLEX_MACRO(FALSE, types_Bool)
return						FLEX_MACRO(RETURN)
if							FLEX_MACRO(IF)
else						FLEX_MACRO(ELSE)
while						FLEX_MACRO(WHILE)
break						FLEX_MACRO(BREAK)
continue					FLEX_MACRO(CONTINUE)
;							FLEX_MACRO(SC)
,							FLEX_MACRO(COMMA)
\.							FLEX_MACRO(PERIOD)
\(							FLEX_MACRO(LPAREN)
\)							FLEX_MACRO(RPAREN)
\{							FLEX_MACRO(LBRACE)
\}							FLEX_MACRO(RBRACE)
=							FLEX_MACRO(ASSIGN)
==|!=|<|>|<=|>=				FLEX_MACRO(RELOP)
\+							FLEX_MACRO(BINOPAS); yylval.op = add_op;
\-							FLEX_MACRO(BINOPAS); yylval.op = sub_op;
\*							FLEX_MACRO(BINOPMD); yylval.op = mul_op;
\/							FLEX_MACRO(BINOPMD); yylval.op = div_op;
[a-zA-Z][a-zA-Z0-9]*		FLEX_MACRO(ID)
0|[1-9][0-9]*				TYPE_FLEX_MACRO(NUM, types_Int)
\"([^\n\r\"\\]|\\[rnt"\\])+\"	TYPE_FLEX_MACRO(STRING, types_String)
[\ \t\r\n]+					;
\/\/[^\r\n]*[\r|\n|\r\n]?	yylval.lineno = yylineno; tables->setLine(yylineno);
.							output::errorLex(yylineno); exit(0);
%%

void debug(yytokentype token) {
	return;
	switch(token) {
		case VOID: printf("VOID\n");   	break;
		case INT: printf("INT\n");    	break;
		case B: printf("B\n");    	break;
		case BYTE:	printf("BYTE\n");   	break;
		case BOOL:	printf("BOOL\n");   	break;
		case STRUCT:	printf("STRUCT\n");		break;
		case AND:	printf("AND\n");		break;
		case OR:	printf("OR\n");			break;
		case NOT:	printf("NOT\n");		break;
		case TRUE:printf("TRUE\n");		break;
		case FALSE:printf("FALSE\n");		break;
		case RETURN:printf("RETURN\n");		break;
		case IF:printf("IF\n");			break;
		case ELSE:printf("ELSE\n");		break;
		case WHILE:printf("WHILE\n");		break;
		case BREAK:printf("BREAK\n");		break;
		case CONTINUE:printf("CONTINUE\n");	break;
		case SC:printf("SC\n");			break;
		case COMMA:printf("COMMA\n");		break;
		case PERIOD:printf("PERIOD\n");		break;
		case LPAREN:printf("LPAREN\n");		break;
		case RPAREN:printf("RPAREN\n");		break;
		case LBRACE:printf("LBRACE\n");		break;
		case RBRACE:printf("RBRACE\n");		break;
		case ASSIGN:printf("ASSIGN\n");		break;
		case RELOP:printf("RELOP\n");		break;
		case BINOPAS:printf("BINOP\n");		break;
		case BINOPMD:printf("BINOP\n");		break;
		case ID:printf("ID %s\n", yytext);			break;
		case NUM:printf("NUM %s\n", yytext);		break;
		case STRING:printf("STRING %s\n", yytext);		break;
	}
}