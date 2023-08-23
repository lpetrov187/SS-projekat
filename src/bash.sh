flex ../misc/flex.l
bison ../misc/bison.y
g++ parser.cpp lexer.cpp asembler.cpp -o ../tmp/a