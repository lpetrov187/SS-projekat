%{
  #include "../inc/parser.hpp"
  #include <iostream>  
  using namespace std;
%}
%option outfile="../src/lexer.cpp" header-file="../inc/lexer.hpp"
%option noyywrap
%%
<<EOF>> { yyterminate(); }
\.global { return GLOBAL; }
\.extern { return EXTERN; }
\.section { return SECTION; }
\.word { return WORD; }
\.skip { return SKIP; }
\.ascii[^\n]* { }
\.equ[^\n]* { }
\.end[^<<EOF>>]* { return END; }
halt { return HALT; }
int { return INTR; }
iret { return IRET; }
call { return CALL; }
ret { return RET; }
jmp { return JMP; }
beq { return BEQ; }
bne { return BNE; }
bgt { return BGT; }
push { return PUSH; }
pop { return POP; }
xchg { return XCHG; }
add { return ADD; }
sub { return SUB; }
mul { return MUL; }
div { return DIV; }
not { return NOT; }
and { return AND; }
or { return OR; }
xor { return XOR; }
shl { return SHL; }
shr { return SHR; }
ld { return LD; }
st { return ST; }
csrrd { return CSRRD; }
csrwr { return CSRWR; }
%r[0-9]                 { yylval.reg = atoi(yytext+2); return REG; }
%r1[0-3]                { yylval.reg = atoi(yytext+2); return REG; }
%sp                     { yylval.reg = 14; return REG; }
%pc                     { yylval.reg = 15; return REG; }
%status                 { return STATUS; }
%handler                { return HANDLER; }
%cause                  { return CAUSE; }
[\+]                    { return PLUS; }
,                       { return COM; }
[\$]                    { return DOLLAR; }
[%]                     { return PERCENT; }
\[                      { return LPAR; }
\]                      { return RPAR; }
0[xX][a-fA-F0-9]+       { yylval.lval = stol(yytext+2,0,16); return LITERAL; }
[0-9]+                  { yylval.ival = atoi(yytext); return LITERAL; }
[#][^\n]*               { }
[ \t\n]                 { }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.sval = strdup(yytext); return SYMB; }
[ \t]*[a-zA-Z0-9_]+:    { yylval.label = strdup(yytext); return LABEL; }
[0-9]+\.[0-9]+          { yylval.fval = atof(yytext); return FLOAT; }
%%