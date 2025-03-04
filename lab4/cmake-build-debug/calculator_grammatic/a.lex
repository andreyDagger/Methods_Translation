[ \t\n]+           { return SPACE; }
"/"             { yylval.s = new string(yytext); reduced[$@] = yytext; return SLASH; }
"+"             { yylval.s = new string(yytext); reduced[$@] = yytext; return PLUS; }
"("             { yylval.s = new string(yytext); reduced[$@] = yytext; return LEFT_BRACKET; }
")"             { yylval.s = new string(yytext); reduced[$@] = yytext; return RIGHT_BRACKET; }
"*"             { yylval.s = new string(yytext); reduced[$@] = yytext; return STAR; }
"-"             { yylval.s = new string(yytext); reduced[$@] = yytext; return MINUS; }
[0-9]+          { yylval.s = new poly_int(std::stoi(yytext)); reduced[$@] = yytext; return NUMBER; }
[0-9]+.[0-9]+   { yylval.s = new poly_int(std::stod(yytext)); reduced[$@] = yytext; return REAL }
.               { }