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
void error(int, char *);

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
.								error(ILLEGAL_CHAR, NULL);
%%

void error(int code, char * message){
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
			printf("Error undefined escape sequence %s\n", message);
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


/*
	This function checks if the input character
	resembles a hex digit
	Input: c - the character to validate
	Output: 1 - if c is a hex digit
		0 - otherwise
*/
int is_hex(char c){
	return ((c >= '0' && c <= '9') ||
		(c >= 'a' && c <= 'f') ||
		(c >= 'A' && c <= 'F'));
}


/*
	This function validates the format of the string.
	It checks if all the escaping sequences are valid.
	If the string is invalid, an error occurs
	Input: str - the input string
	Output: None
*/
void validate_string(char * str){
	char seq[2];
	seq[1] = '\0';
	while (*str){
		if (*(str++) == '\\'){
			seq[0] = *str; 
			switch (*(str++)){
				case '\\':
				case '"':
				case 'a':
				case 'b':
				case 'n':
				case 'r':
				case 't':
				case ';':
				case ':':
				case '=':
				case '#':
				case '0': break;
				case 'x':{
						if (!(is_hex(*(str++)) && is_hex(*(str++))))
							error(UNDEFINED_ESCAPING, seq);
						break;
					}
				default: error(UNDEFINED_ESCAPING, seq);
			}
		}
	}
}

void stringToken(int cap){
	int len = strlen(yytext);
	char name[] = "STRING";
	
	if (cap){
		char buffer[len + 1];
		slice_str(yytext, buffer, 1, len - 2);
		validate_string(buffer);
		printf("%d %s %s\n", yylineno, name, buffer);
	} else{
		printf("%d %s %s\n", yylineno, name, yytext);
	}
}
