%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void showToken(char *);
void integer();
void showTokenNoHeader(char *);
int getHeaderOffset();
%}

%option noyywrap
%option yylineno

digit                   ([0-9])
realdigit               ([1-9])
letter                  ([a-zA-Z])
whitespace              ([\t\n ])
legal_keys              ([0-9a-zA-Z~_\-\'\?\$\.\t\n ])
assign                  ([=:])
comment                 ([#;])
hex                     ([0-9a-f])
octal                   ([0-7])
realoctal               ([1-7])
binary                  ([01])
path                    ((\/{letter}{legal_keys}*)+)
localpath               (\/|\/\/)

%%
{letter}{legal_keys}+                           showToken("KEY");
\[{letter}{legal_keys}+\]                       showToken("SECTION");
^{whitespace}+                  showToken("INDENT");
{whitespace}*{comment}.*        showToken("COMMENT");
({assign}?{whitespace}+)(true|yes)                        showTokenNoHeader("TRUE");
({assign}?{whitespace}+)(false|no)                        showTokenNoHeader("FALSE");
({assign}?{whitespace}+)[+-]?(({realdigit}{digit}*)|0)                  integer(10);
({assign}?{whitespace}+)0x{realdigit}{hex}*                        integer(16);
({assign}?{whitespace}+)0{realoctal}{octal}*                       integer(8);
({assign}?{whitespace}+)0b{binary}+                     integer(2);
({whitespace}*{assign}?{whitespace}+)[+-]?{digit}*\.{digit}*(e[+-]{digit}+)?     showTokenNoHeader("REAL");
({assign}?{whitespace}+)({path}|{localpath})                        showTokenNoHeader("PATH");
({assign}?{whitespace}+)\$\{({letter}{legal_keys}+)|({letter}{legal_keys}+#{letter}{legal_keys}+)\}  showTokenNoHeader("LINK");
({whitespace}*{assign}?{whitespace}+)((\"[^\"\\]*\")|([^,\"\n]+))                                    showTokenNoHeader("STRING");
({whitespace}*{assign}?{whitespace}+),                                    showTokenNoHeader("SEP");
%%

void showToken(char *token_name) {
    printf("%d %s %s\n", yylineno, token_name, yytext);
}

void showTokenNoHeader(char *token_name) {
    int offset = getHeaderOffset();
    printf("%d %s %s\n", yylineno, token_name, yytext+offset);
}

int getHeaderOffset() {
    int offset = 0;
    while(1) {
        if(*(yytext + offset) == 0x3A || *(yytext + offset) == 0x3D) {
            printf("%d ASSIGN %c\n", yylineno, *(yytext + offset));
            ++offset;
        } else if(*(yytext + offset) == 0x20 || *(yytext + offset) == 0x09) {
            ++offset;
        } else {
            return offset;
        }
    }
}

void integer() {
    int offset = getHeaderOffset();
    printf("%d INTEGER %ld\n", yylineno, strtol(yytext+offset, NULL, 10));
}