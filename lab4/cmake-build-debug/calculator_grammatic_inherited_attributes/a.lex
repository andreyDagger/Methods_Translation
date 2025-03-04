[ \t\n]+           { return SPACE; }
"+"             { return PLUS; }
"("             { return LEFT_BRACKET; }
")"             { return RIGHT_BRACKET; }
"*"             { return STAR; }
[0-9]+          { yylval.s = new node_op(std::stoi(yytext)); return NUMBER; }
.               { }