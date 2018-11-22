%{
/* Declarations section */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int showToken(char *);
int stringToken();
int assignIndentToken(char *);
int eofToken();
int intToken();
void error(int, char *);
void unclosed_error();
int getTokenFromName(char * name);

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
key 		({letter}({key_letter}*{key_letter_no_space})?)
true		("true"|"yes")
false		("false"|"no")
real		([+-]?(({digit}+\.{digit}*)|({digit}*\.{digit}+)))
to_ignore	([ \t])
no_escape_seq	([^\\\"\a\b\n\r\t\0\;\:\=\#\xdd])
folder_name	(({letter}{dir_letter}*))
printable_letter ([ !#-~\t\n]|\r\n|(\\\"))
string			(([^\,\#\;\n\r\x00-\x08\xA-\x1F]*[^\,\#\;\n\r\t\ \x00-\x08\xA-\x1F])?)
comment			(({indent_char}*)[#;]([^\r\n]*[^ \t\r\n])?)

%%
{newline}									BEGIN(0); key_appeared = 0;
^({key})									return showToken("KEY");
\[{key}\]									return showToken("SECTION");
^({indent_char}*)								return assignIndentToken("INDENT");
{to_ignore}+									;
<after_key>(:|=)								return assignIndentToken("ASSIGN");
<after_indent_assign>(:|=)							return assignIndentToken("ASSIGN");
{comment}							;
<after_indent_assign>{true}/{string}							return showToken("TRUE");
<after_indent_assign>{false}/{string}							return showToken("FALSE");
<after_indent_assign>(0x{hex_digit}*)						return INTEGER;
<after_indent_assign>(0{oct_digit}*)						return INTEGER;
<after_indent_assign>([+-]?{digit}+)					return INTEGER;
<after_indent_assign>(0b{bi_digit}*)						return INTEGER;
<after_indent_assign>{real}(e[+-]{digit}+)?					return showToken("REAL");
<after_indent_assign>(("/"{folder_name}?)+)					return showToken("PATH");
<after_indent_assign>($\{({key}|({key}#{key}))\})				return LINK;
<after_indent_assign>(\"[^"]*\\\")			return STRING;
<after_indent_assign>(\"{printable_letter}*\")						return STRING;
<after_indent_assign>({letter}{string})	stringToken();
<after_indent_assign>(\,)							return showToken("SEP");
<<EOF>>										return EOF;
<after_indent_assign>(\"[^"]*)			unclosed_error();
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

char *  noIndent(char * str){
	if (*str != '\t' && *str != ' '){
		return str;
	}
	while (*str == '\t' || *str == ' '){
		str++;
	}
	return str;
}

int getTokenFromName(char * name) {
	if (strcmp(name, "TRUE") == 0) {
		return TRUE;
	}
	if (strcmp(name, "FALSE") == 0) {
		return FALSE;
	}
	if (strcmp(name, "KEY") == 0) {
		return KEY;
	}
	if (strcmp(name, "SECTION") == 0) {
		return SECTION;
	}
	if (strcmp(name, "REAL") == 0) {
		return REAL;
	}
	if (strcmp(name, "PATH") == 0) {
		return PATH;
	}
	if (strcmp(name, "LINK") == 0) {
		return LINK;
	}
	if (strcmp(name, "SEP") == 0) {
		return SEP;
	}
	/* ERROR, shouldn't get here */
	return -1;
}

int showToken(char * name){
	if (strcmp(name, "KEY") == 0){
		BEGIN(after_key);
		key_appeared = 1;
	}
	return getTokenFromName(name);
}

int assignIndentToken(char * name){
	if (!key_appeared && strcmp(name, "ASSIGN") == 0) {
		error(ILLEGAL_CHAR, NULL);
	}
	BEGIN(after_indent_assign);
	return getTokenFromName(name);
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
				default:
				if (*(str - 1) == '\n' || (*(str - 1) == '\r' && *(str) == '\n') || *(str - 1) == 0) {
					printf("Error \\\n");
					exit(0);
				}
				error(UNDEFINED_ESCAPING, seq);
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
				case '\"':
				*(result++) = '\"';
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

void unclosed_error() {
	if (*yytext == '\"' && getchar() == EOF) {
		validate_string(yytext + 1);
		error(UNCLOSED_STRING, NULL);
	}
}