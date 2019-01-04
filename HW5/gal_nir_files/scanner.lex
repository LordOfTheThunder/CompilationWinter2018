%{
	#include "attributes.h"
	#include "parser.tab.hpp"
	#include <string>
	#include "output.hpp"
	#include "symbolTable.hpp"
	using std::string;
	extern symbolTable* tables;
%}

%option yylineno
%option noyywrap

%%

void { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; yylval.type = VoidType; return VOID; }
int { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; yylval.type = IntType; return INT; }
byte { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; yylval.type = ByteType; return BYTE; }
b { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return B; }
bool { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; yylval.type = BoolType; return BOOL; }
and { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return AND; }
or { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return OR; }
not { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return NOT; }
true { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; yylval.type = BoolType; return TRUE; } 
false { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; yylval.type = BoolType; return FALSE; }
return { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return RETURN; }
if { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return IF; }
else { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return ELSE; }
while { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return WHILE; }
break { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return BREAK; }
\; { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return SC; }
\, { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return COMMA; }
\( { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return LPAREN; }
\) { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return RPAREN; }
\{ { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return LBRACE; }
\} { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return RBRACE; }
\[ { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return LBRACK; }
\] { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return RBRACK; }
\= { yylval = Stype();  tables->lineno = yylval.lineno = yylineno; return ASSIGN; }
\=\= { yylval = Stype(); yylval.str = yytext;  tables->lineno = yylval.lineno = yylineno; return RELOPQ; }
\!\= { yylval = Stype(); yylval.str = yytext;  tables->lineno = yylval.lineno = yylineno; return RELOPQ; }
\< { yylval = Stype(); yylval.str = yytext;  tables->lineno = yylval.lineno = yylineno; return RELOPNQ; }
\> { yylval = Stype(); yylval.str = yytext;  tables->lineno = yylval.lineno = yylineno; return RELOPNQ; }
\<\= { yylval = Stype(); yylval.str = yytext;  tables->lineno = yylval.lineno = yylineno; return RELOPNQ; }
\>\= { yylval = Stype(); yylval.str = yytext;  tables->lineno = yylval.lineno = yylineno; return RELOPNQ; }
\+ { yylval = Stype(); yylval.str = yytext;  tables->lineno = yylval.lineno = yylineno; return BINOPPM; }
\- { yylval = Stype(); yylval.str = yytext; tables->lineno = yylval.lineno = yylineno; return BINOPPM; }
\* { yylval = Stype(); yylval.str = yytext; tables->lineno = yylval.lineno = yylineno; return BINOPMD; }
\/ { yylval = Stype(); yylval.str = yytext; tables->lineno = yylval.lineno = yylineno; return BINOPMD; }
[a-zA-Z][a-zA-Z0-9]*  { yylval = Stype(); yylval.str = yytext; tables->lineno = yylval.lineno = yylineno; return ID; } 
0|[1-9][0-9]* { yylval = Stype(); yylval.type = IntType; yylval.str = yytext; tables->lineno = yylval.lineno = yylineno;  return NUM; }
\"([^\n\r\"\\]|\\[rnt\"\\])+\" { yylval = Stype(); yylval.type = StringType; yylval.str = yytext; tables->lineno = yylval.lineno = yylineno;  return STRING; }
[ \n\r\t]+ ;
\/\/[^\r\n]*[\r|\n|\r\n]? ;
. { output::errorLex(yylineno) ; exit(0) ;}

%%
