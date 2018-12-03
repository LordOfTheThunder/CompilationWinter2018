%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void debug(int token);

enum {
	VOID = 1,
	INT,
	BYTE,
	B,
	BOOL,
	STRUCT,
	AND,
	OR,
	NOT,
	TRUE,
	FALSE,
	RETURN,
	IF,
	ELSE,
	WHILE,
	BREAK,
	CONTINUE,
	SC,
	COMMA,
	PERIOD,
	LPAREN,
	RPAREN,
	LBRACE,
	RBRACE,
	ASSIGN,
	RELOP,
	BINOP,
	ID,
	NUM,
	STRING
};

%}

%option yylineno
%option noyywrap

%%
void						debug(VOID); 	return VOID;
int							debug(INT);  	return INT;
byte						debug(BYTE); 	return BYTE;
b							debug(B);		return B;
bool						debug(BOOL);	return BOOL;
struct						debug(STRUCT);	return STRUCT;
and							debug(AND);		return AND;
or							debug(OR);		return OR;
not							debug(NOT);		return NOT;
true						debug(TRUE);	return TRUE;
false						debug(FALSE);	return FALSE;
return						debug(RETURN);	return RETURN;
if							debug(IF);		return IF;
else						debug(ELSE);	return ELSE;
while						debug(WHILE);	return WHILE;
break						debug(BREAK);	return BREAK;
continue					debug(CONTINUE);return CONTINUE;
;							debug(SC);		return SC;
,							debug(COMMA);	return COMMA;
\.							debug(PERIOD);	return PERIOD;
\(							debug(LPAREN);	return LPAREN;
\)							debug(RPAREN);	return RPAREN;
\{							debug(LBRACE);	return LBRACE;
\}							debug(RBRACE);	return RBRACE;
=							debug(ASSIGN);	return ASSIGN;
==|!=|<|>|<=|>=				debug(RELOP);	return RELOP;
\+|\-|\*|\/					debug(BINOP);	return BINOP;
[a-zA-Z][a-zA-Z0-9]*		debug(ID);		return ID;
0|[1-9][0-9]*				debug(NUM);		return NUM;
\"([^\n\r\"\\]|[rnt\"\\])+\"	debug(STRING);	return STRING;
[\ \t\r\n]+					;
\/\/[^\r\n]*[\r|\n|\r\n]?	;
%%

void debug(int token) {
	switch(token) {
		case 1: printf("VOID\n");   	break;
		case 2: printf("INT\n");    	break;
		case 3:	printf("BYTE\n");   	break;
		case 4:	printf("B\n"); 	    	break;
		case 5:	printf("BOOL\n");   	break;
		case 6:	printf("STRUCT\n");		break;
		case 7:	printf("AND\n");		break;
		case 8:	printf("OR\n");			break;
		case 9:	printf("NOT\n");		break;
		case 10:printf("TRUE\n");		break;
		case 11:printf("FALSE\n");		break;
		case 12:printf("RETURN\n");		break;
		case 13:printf("IF\n");			break;
		case 14:printf("ELSE\n");		break;
		case 15:printf("WHILE\n");		break;
		case 16:printf("BREAK\n");		break;
		case 17:printf("CONTINUE\n");	break;
		case 18:printf("SC\n");			break;
		case 19:printf("COMMA\n");		break;
		case 20:printf("PERIOD\n");		break;
		case 21:printf("LPAREN\n");		break;
		case 22:printf("RPAREN\n");		break;
		case 23:printf("LBRACE\n");		break;
		case 24:printf("RBRACE\n");		break;
		case 25:printf("ASSIGN\n");		break;
		case 26:printf("RELOP\n");		break;
		case 27:printf("BINOP\n");		break;
		case 28:printf("ID\n");			break;
		case 29:printf("NUM\n");		break;
		case 30:printf("STRING\n");		break;
	}
}