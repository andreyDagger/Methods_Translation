flex haskell_to_cpp.lex
yacc -dtv haskell_to_cpp.y
g++ -c lex.yy.c
g++ -c y.tab.c
g++ -o haskell_to_cpp y.tab.o lex.yy.o
./haskell_to_cpp input.txt output.txt
