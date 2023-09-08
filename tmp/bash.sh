flex ../misc/flex.l
bison ../misc/bison.y
g++ ../src/parser.cpp ../src/lexer.cpp ../src/assembler.cpp -o assembler
g++ ../src/linker.cpp -o linker
g++ ../src/emulator.cpp -o ../tmp/emulator