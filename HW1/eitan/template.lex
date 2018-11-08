%{
/* Declarations section */
#include <stdio.h>
#include <string.h>

void showToken(char *);
void stringToken(int);

%}
%option yylineno
%option noyywrap
digit		([0-9])
hex_digit	([0-7])
bi_digit	(0|1)
letter		([a-zA-Z])
indent_char	([ \t])
newline		([\n\r])
key_letter	({digit}|{letter}|[\~\_\-\'\?\$\.\ ])
dir_letter	([\-\+\\\.\_\~]|{letter}|{digit})
key 		({letter}{key_letter}*)
any_but_newline	([^\n\r])
true		("true"|"yes")
false		("false"|"no")
real		([+-]?{digit}*\.{digit}*)
to_ignore	([ \t\n])

%%
{key}									showToken("KEY");
\[{key}\]								showToken("SECTION");
^{indent_char}*								showToken("INDENT");
(:|=)									showToken("ASSIGN");
([#;]{any_but_newline}*)						showToken("COMMENT");
{true}									showToken("TRUE");
{false}									showToken("FALSE");
([+-]?[1-9]{digit}*)							showToken("INTEGER");
(0x{hex_digit}+)							showToken("INTEGER");
(0{hex_digit}*)								showToken("INTEGER");
(0b{bi_digit}+)								showToken("INTEGER");
{real}(e[+-]{digit}+)?							showToken("REAL");
(("/"{dir_letter}*)+)							showToken("DIRECTORY");
($\{({key}|({key}#{key}))\})						showToken("LINK");
(\"[^\"]*\")								stringToken(1);
(^[\,"\n"])*								stringToken(0);
(\,)									showToken("SEP");
{to_ignore}*								;
.									printf("ERROR!!!!!!!!!");
%%

void showToken(char * name){
	printf("%d %s %s\n", yylineno, name, yytext);	
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
