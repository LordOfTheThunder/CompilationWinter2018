%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void showToken(char *);
void integer();
void showTokenNoHeader(char *);
int getHeaderOffset();

int assign_happened = 0;
%}

%option noyywrap
%option yylineno

digit                   ([0-9])
realdigit               ([1-9])
letter                  ([a-zA-Z])
whitespace              ([\r\t\n ])
wsnnl			([\t ])
newline			([\r\n])
legal_keys              ([0-9a-zA-Z~_\-\'\?\$\.\t\n ])
assign                  ([=:])
comment                 ([#;])
hex                     ([0-9a-f])
octal                   ([0-7])
realoctal               ([1-7])
binary                  ([01])
path                    ((\/{letter}{legal_keys}*)+)
localpath               (\/|\/\/)
indent_or_assign	(({wsnnl}*{assign}{wsnnl}*)[^,\n\r]*)

%%
^{letter}{legal_keys}+                           showToken("KEY");
\[{letter}{legal_keys}+\]                       showToken("SECTION");
{wsnnl}*{comment}.*        showToken("COMMENT");
({indent_or_assign}|{wsnnl}*)(true|yes)                        showToken("TRUE");
({indent_or_assign}|{wsnnl}*)(false|no)                        showToken("FALSE");
({indent_or_assign}|{wsnnl}*)([+-]?(({realdigit}{digit}*)|0))                  showToken("INTEGER");
({indent_or_assign}|{wsnnl}*)0x{realdigit}{hex}*                        showToken("INTEGER");
({indent_or_assign}|{wsnnl}*)0{realoctal}{octal}*                       showToken("INTEGER");
({indent_or_assign}|{wsnnl}*)0b{binary}+                     showToken("INTEGER");
({indent_or_assign}|{wsnnl}*)[+-]?{digit}*\.{digit}*(e[+-]{digit}+)?     showToken("REAL");
({path}|{localpath})                        showToken("PATH");
\$\{({letter}{legal_keys}+)|({letter}{legal_keys}+#{letter}{legal_keys}+)\}  showToken("LINK");
{indent_or_assign}((\"[^\"\\]*\")|([^,\"\n]+))                                    showToken("STRING");
{indent_or_assign},                                    showToken("SEP");
{wsnnl}*{newline}			assign_happened = 0;
<<EOF>>					showToken("EOF\n"); exit(0);
%%

void showToken(char *token_name) {
    int len = strlen(yytext);
    char* last = yytext + len - 1;
    if (*last == 0x0D || *last == 0x0A) {
	*(yytext + len - 1) = '\0';
    }
    int offset = getHeaderOffset();
    if (offset > 0 && !assign_happened && strcmp(token_name, "COMMENT") != 0) {
	char tmp = *(yytext + offset);
	*(yytext + offset) = '\0';
    	printf("%d INDENT %s\n", yylineno, yytext);
	*(yytext + offset) = tmp;
    }
    if (strcmp(token_name, "INTEGER") == 0) {
	printf("%d INTEGER %ld\n", yylineno, strtol(yytext + offset, NULL, 10));
	return;
    }
    printf("%d %s %s\n", yylineno, token_name, yytext + offset);
}

int getHeaderOffset() {
    int offset = 0;
    while(1) {
        if(*(yytext + offset) == 0x3A || *(yytext + offset) == 0x3D) {
            printf("%d ASSIGN %c\n", yylineno, *(yytext + offset));
	    assign_happened = 1;
            ++offset;
        } else if(*(yytext + offset) == 0x20 || *(yytext + offset) == 0x09) {
            ++offset;
        } else {
            return offset;
        }
    }
}
