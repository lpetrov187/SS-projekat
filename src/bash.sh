flex ../misc/flex.l
bison ../misc/bison.y
g++ parser.cpp lexer.cpp assembler.cpp -o ../tmp/a