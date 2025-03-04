[ \t\n]+           { return SPACE; }
","             { return COMMA; }
";"             { return SEMICOLON; }
"*"             { return STAR; }
"["             { return LEFT_BRACKET; }
"]"             { return RIGHT_BRACKET; }
[0-9]+          { return NUMBER; }
[a-zA-Z]+        { return VARIABLE; }
.               { }