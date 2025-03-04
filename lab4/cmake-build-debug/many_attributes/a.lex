[ \t\n]+           { return SPACE; }
"<="             { return LESS_OR_EQUAL; }
">="             { return GREATER_OR_EQUAL; }
"<"             { return LESS; }
">"             { return GREATER; }
"+"             { return PLUS; }
[0-9]+          { yylval.s = new int(std::stoi(yytext)); return NUMBER; }
.               { }