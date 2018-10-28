%{
/* Declarations section */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void showToken(char *);
void stringToken(int);
void assignIndentToken(char *);
void eofToken();
void comment();
void intToken(int, int);
void error(int);

enum{
	ILLEGAL_CHAR,
	UNCLOSED_STRING,
	UNDEFINED_ESCAPING
};

%}

%s expect
%option yylineno
%option noyywrap
digit		([0-9])
hex_digit	([0-7])
bi_digit	(0|1)
letter		([a-zA-Z])
indent_char	([ \t])
newline		([\r\n])
key_letter	({digit}|{letter}|[\~\_\-\'\?\$\.\ ])
dir_letter	([\-\+\\\.\_\~]|{letter}|{digit})
key 		({letter}{key_letter}*)
true		("true"|"yes")
false		("false"|"no")
real		([+-]?{digit}*\.{digit}*)
to_ignore	([ \t])
no_escape_seq	([^\\\"\a\b\n\r\t\0\;\:\=\#\xdd])

%%
{newline}							BEGIN(0);
^({key})							showToken("KEY");
\[{key}\]							showToken("SECTION");
^({indent_char}*)						assignIndentToken("INDENT");
{to_ignore}*							;
(:|=)								assignIndentToken("ASSIGN");
(({indent_char}*)[#;].*)					comment();
<<EOF>>								eofToken();
<expect>{true}							showToken("TRUE");
<expect>{false}							showToken("FALSE");
<expect>([+-]?[1-9]{digit}*)					intToken(10, 0);
<expect>(0x{hex_digit}+)					intToken(8, 2);
<expect>(0{hex_digit}*)						intToken(8, 1);
<expect>(0b{bi_digit}+)						intToken(2, 2);
<expect>{real}(e[+-]{digit}+)?					showToken("REAL");
<expect>(("/"{dir_letter}*)+)					showToken("PATH");
<expect>($\{({key}|({key}#{key}))\})				showToken("LINK");
<expect>(\"[^\"]*\")						stringToken(1);
<expect>([^\"\,\#\;\n\r\ ][^\,\#\;\n\r]*)			stringToken(0);
<expect>(\,)							showToken("SEP");
.								error(ILLEGAL_CHAR);
%%

void error(int code){
	switch (code){
		case ILLEGAL_CHAR:{
			printf("Error %s\n", yytext);
			exit(0);
		}
		case UNCLOSED_STRING:{
			printf("Error unclosed string\n");
			exit(0);
		}
		case UNDEFINED_ESCAPING:{
			printf("Error undefined escape sequence %s\n", yytext);
			exit(0);
		}
	}
}

void intToken(int base, int offset){
	printf("%d %s %ld\n", yylineno, "INTEGER", strtol(yytext + offset, NULL, base));
}

char *  noIndent(char * str){
	if (*str != '\t' && *str != ' '){
		return str;
	}
	while (*str == '\t' || *str == ' '){
		str++;
	}
	return str;
} 

void comment(){
	printf("%d %s %s\n", yylineno, "COMMENT", noIndent(yytext));
}

void showToken(char * name){
	printf("%d %s %s\n", yylineno, name, yytext);	
}

void assignIndentToken(char * name){
	printf("%d %s %s\n", yylineno, name, yytext);
	BEGIN(expect);
}

void eofToken(){
	printf("%d %s %s\n", yylineno, "EOF", yytext);
	exit(0);
}

void slice_str(const char * str, char * buffer, size_t start, size_t end){
	size_t j = 0;
	size_t i;
	for (i = start; i <= end; ++i ){
		buffer[j++] = str[i];
	}
	buffer[j] = 0;
}

void stringToken(int cap){
	int len = strlen(yytext);
	char name[] = "STRING";
	
	if (cap){
		char buffer[len + 1];
		slice_str(yytext, buffer, 1, len - 2);
		printf("%d %s %s\n", yylineno, name, buffer);
	} else{
		printf("%d %s %s\n", yylineno, name, yytext);
	}
}
