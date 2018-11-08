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

int key_appeared = 0;

enum {
	ILLEGAL_CHAR,
	UNCLOSED_STRING,
	UNDEFINED_ESCAPING,
	CLOSED_STRING
};

%}

%s after_key
%s after_indent_assign
%option caseless
%option yylineno
%option noyywrap
digit		([0-9])
hex_digit	([0-9A-Fa-f])
oct_digit	([0-7])
bi_digit	([01])
letter		([a-zA-Z])
indent_char	([ \t])
newline		(\r?\n)
key_letter	({digit}|{letter}|[\~\_\-\'\?\$\.\ ])
key_letter_no_space	({digit}|{letter}|[\~\_\-\'\?\$\.])
dir_letter	([\-\+\\\.\_\~]|{letter}|{digit})
key 		({letter}{key_letter}*{key_letter_no_space})|({letter})
true		("true"|"yes")
false		("false"|"no")
real		([+-]?{digit}*\.{digit}*)
to_ignore	([ \t])
no_escape_seq	([^\\\"\a\b\n\r\t\0\;\:\=\#\xdd])
folder_name	(({letter}{dir_letter}*))
printable_letter ([ !#-~\t\n]|\r\n)

%%
{newline}									BEGIN(0); key_appeared = 0;
^({key})									showToken("KEY");
\[{key}\]									showToken("SECTION");
^({indent_char}*)								assignIndentToken("INDENT");
{to_ignore}+									;
<after_key>(:|=)								assignIndentToken("ASSIGN");
<after_indent_assign>(:|=)							assignIndentToken("ASSIGN");
(({indent_char}*)[#;][^\r\n]*)							comment();
<after_indent_assign>{true}							showToken("TRUE");
<after_indent_assign>{false}							showToken("FALSE");
<after_indent_assign>([+-]?[1-9]{digit}*)					intToken(10, 0);
<after_indent_assign>(0x{hex_digit}+)						intToken(16, 2);
<after_indent_assign>(0{oct_digit}*)						intToken(8, 1);
<after_indent_assign>(0b{bi_digit}+)						intToken(2, 2);
<after_indent_assign>{real}(e[+-]{digit}+)?					showToken("REAL");
<after_indent_assign>(("/"{folder_name}?)+)					showToken("PATH");
<after_indent_assign>($\{({key}|({key}#{key}))\})				showToken("LINK");
<after_indent_assign>(\"{printable_letter}*\")						stringToken(CLOSED_STRING);
<after_indent_assign>((({letter}|\")[^\,\#\;\n\r]*[^\,\#\;\n\r\t\ ])|({letter}|\"))	stringToken(UNCLOSED_STRING);
<after_indent_assign>(\,)							showToken("SEP");
<<EOF>>										eofToken();
.										error(ILLEGAL_CHAR, NULL);
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
	if (strcmp(name, "KEY") == 0){
		BEGIN(after_key);
		key_appeared = 1;
	}
}

void assignIndentToken(char * name){
	if (!key_appeared && strcmp(name, "ASSIGN") == 0) {
		error(ILLEGAL_CHAR, NULL);
	}
	printf("%d %s %s\n", yylineno, name, yytext);
	BEGIN(after_indent_assign);
}

void eofToken(){
	printf("%d EOF %s\n", yylineno, yytext);
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
void validate_string(char * str) {
	char seq[2];
	seq[1] = '\0';
	while (*str){
		if (*(str++) == '\\'){
			seq[0] = *str;
			switch (*(str++)){
				case '\\':
				case '"':
				case 'a':
				case 'A':
				case 'b':
				case 'B':
				case 'n':
				case 'N':
				case 'r':
				case 'R':
				case 't':
				case 'T':
				case ';':
				case ':':
				case '=':
				case '#':
				case '0': break;
				case 'x':
				case 'X':
						if (!(is_hex(*(str++)) && is_hex(*(str++))))
							error(UNDEFINED_ESCAPING, seq);
						break;
				default: error(UNDEFINED_ESCAPING, seq);
			}
		}
	}
}

void reformat_string(char * str, char * result) {
	char tmp[3];
	while (*str != 0) {
		if (*str == '\\') {
			switch(*(++str)) {
				case '\\':
				*(result++) = '\\';
				break;
				case 'a':
				case 'A':
				*(result++) = '\a';
				break;
				case 'b':
				case 'B':
				*(result++) = '\b';
				break;
				case 'n':
				case 'N':
				*(result++) = '\n';
				break;
				case 'r':
				case 'R':
				*(result++) = '\r';
				break;
				case 't':
				case 'T':
				*(result++) = '\t';
				break;
				case '0':
				*(result++) = '\0';
				break;
				case ';':
				*(result++) = ';';
				break;
				case ':':
				*(result++) = ':';
				break;
				case '=':
				*(result++) = '=';
				break;
				case '#':
				*(result++) = '#';
				break;
				case 'x':
				case 'X':
				tmp[0] = *(++str);
				tmp[1] = *(++str);
				tmp[2] = 0;
				long int x = strtol(tmp, NULL, 16);
				*(result++) = x;
				break;
				default:
				*(result++) = *(++str);
				break;
			}
		}
		else if (*str == '\n') {
			*(result++) = ' ';
		}
		else if (*str != '\r') {
			*(result++) = *str;
		}
		++str;
	}
	*result = 0;
}

void stringToken(int cap){
	int len = strlen(yytext);
	char name[] = "STRING";

	if (cap == CLOSED_STRING){
		char buffer[len + 1];
		char result[len + 1];
		slice_str(yytext, buffer, 1, len - 2);
		validate_string(buffer);
		reformat_string(buffer, result);
		printf("%d %s %s\n", yylineno, name, result);
	} else if (cap == UNCLOSED_STRING) {
		if (*yytext == '\"' && getchar() == EOF) {
			validate_string(yytext + 1);
			error(UNCLOSED_STRING, NULL);
		}
		printf("%d %s %s\n", yylineno, name, yytext);
	}
}
