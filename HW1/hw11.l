%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void showToken(char *);
void showTokenAfter(char *);
int assign_happened = 0;
%}

%s indent_assign
%option noyywrap
%option yylineno

digit                   ([0-9])
realdigit               ([1-9])
letter                  ([a-zA-Z])
whitespace              ([\t ])
newline                 ([\r\n])
legal_keys              ([0-9a-zA-Z~_\-\'\?\$\.\t\n ])
assign                  ([=:])
comment                 ([#;])
hex                     ([0-9a-f])
octal                   ([0-7])
realoctal               ([1-7])
binary                  ([01])

%%
^{letter}{legal_keys}*              showToken("KEY");
{assign}                            showTokenAfter("ASSIGN");

<indent_assign>((\"[^\"\\]*\")|([^,\"\n]+))     showToken("STRING");

<<EOF>>					showToken("EOF\n"); exit(0);
%%

void showToken(char *token_name) {
    printf("%d %s %s\n", yylineno, token_name, yytext);
}

void showTokenAfter(char *token_name) {
    printf("%d %s %s\n", yylineno, token_name, yytext);
    BEGIN(indent_assign);
}