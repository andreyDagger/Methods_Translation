%option noyywrap

%{
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include "y.tab.h"
%}

%%
[ \t\n]+        { }
"=="            { return DOUBLEEQ; }
">="            { return GREATEREQ; }
"<="            { return LESSEQ; }
">"             { return GREATER; }
"<"             { return LESS; }
"|"             { return VBAR; }
";"             { return SEMICOLON; }
"="             { return EQUALITY; }
"::"            { return COLONCOLON; }
"->"            { return ARROW; }
"Int"           { yylval.s = new std::string("int"); return TYPE; }
"Float"         { yylval.s = new std::string("float"); return TYPE; }
"Bool"          { yylval.s = new std::string("bool"); return TYPE; }
"String"        { yylval.s = new std::string("std::string"); return TYPE; }
[0-9]+          { yylval.s = new std::string(yytext); return NUMBER; }
\".+\"         { yylval.s = new std::string(yytext); return QUOTED_STRING; }
[a-zA-Z_][a-zA-Z0-9_]* { 
        yylval.s = new std::string(yytext); return IDENTIFIER; }
.               { }
%%

