[ \t\n]+           { return SPACE; }
"+"             { return PLUS; }
"("             { return LEFT_BRACKET; }
")"             { return RIGHT_BRACKET; }


"*"             { return STAR; }

"-"             { return MINUS; }
[0-9]+          { yylval.s = new int(std::stoi(yytext)); return NUMBER; }
.               { }