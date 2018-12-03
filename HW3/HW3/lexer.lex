%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "parse.tab.hpp"

void debug(int token);

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

void debug(tokens token) {
	switch(token) {
		case VOID: printf("VOID\n");   	break;
		case INT: printf("INT\n");    	break;
		case BYTE:	printf("BYTE\n");   	break;
		case B:	printf("B\n"); 	    	break;
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
		case BINOP:printf("BINOP\n");		break;
		case ID:printf("ID\n");			break;
		case NUM:printf("NUM\n");		break;
		case STRING:printf("STRING\n");		break;
	}
}