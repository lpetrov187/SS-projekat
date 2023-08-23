%{
  #include <iostream>
  #include <vector>
  using namespace std;
  #include "../inc/instruction.hpp"
  #include "../inc/assembler.hpp"

  extern int yylex();
  extern int yyparse();
  void yyerror(const char *s);

  using namespace std;
%}

%output "../src/parser.cpp"
%defines "../inc/parser.hpp"

%union {
  int ival;
  long lval;
  int reg;
  float fval;
  char *sval;
  char *hexval;
  char *label;
  char *memident;
}

%token <ival> LITERAL
%token <fval> FLOAT
%token <sval> SYMB
%token <hexval> HEX
%token <reg> REG
%token <label> LABEL
%token <memident> MEMDIR
%token WS
%token GLOBAL
%token EXTERN
%token SECTION
%token WORD
%token SKIP
%token ASCII
%token EQU
%token END
%token COM
%token INTR
%token CALL
%token QUOTE
%token HALT
%token IRET
%token RET
%token JMP
%token BEQ
%token BNE
%token BGT
%token PUSH
%token POP
%token XCHG
%token ADD
%token SUB
%token MUL
%token DIV
%token NOT
%token AND
%token OR
%token XOR
%token SHL
%token SHR
%token LD
%token ST
%token CSRRD
%token CSRWR
%token DOLLAR
%token PERCENT
%token LPAR
%token RPAR
%token STATUS
%token HANDLER
%token CAUSE
%token PLUS

%%
input:
  | line input
;
line:
  labels
  | externs
  | globals
  | sections
  | words
  | skips
  | ints
  | irets
  | rets
  | halts
  | pushes
  | pops
  | xchgs
  | adds
  | subs
  | muls
  | divs
  | nots
  | ands
  | ors
  | xors
  | shls
  | shrs
  | csrrds
  | csrwrs
  | ends
  | jmps
  | lds
  | sts
  | calls
  | beqs
  | bnes
  | bgts
;

labels:
  LABEL{
    string ret = $1;
    __label(ret.substr(0, ret.length()-1));
  }
;
externs:
  externs COM SYMB{
    __extern($3);
  }
  | EXTERN SYMB{
    __extern($2);
  }
;
globals:
  globals COM SYMB{
    __global($3);
  }
  | GLOBAL SYMB{
    __global($2);
  }
;
sections:
  SECTION SYMB{
    __section($2);
  }
;
words:
  WORD LITERAL{
    __word($2);
  }
  | WORD SYMB{
    __word($2);
  }
  | words COM LITERAL{
    __word($3);
  }
  | words COM SYMB{
    __word($3);
  }
;
skips:
  SKIP LITERAL{
    __skip($2);
  }
;
halts:
  HALT{
    __halt();
  }
;
ints:
  INTR{
    __int();
  }
;
irets:
  IRET{
    __iret();
  }
;
calls:
  CALL LITERAL{
    __call($2);
  }
  | CALL SYMB{
    __call($2);
  }
rets:
  RET{
    __ret();
  }
;
jmps:
  JMP LITERAL{
    __jmp($2);
  }
  | JMP SYMB{
    __jmp($2);
  }
;
beqs:
  BEQ REG COM REG COM LITERAL{
    __beq($2, $4, $6);
  }
  | BEQ REG COM REG COM SYMB{
    __beq($2, $4, $6);
  }
;
bnes:
  BNE REG COM REG COM LITERAL{
    __bne($2, $4, $6);
  }
  | BNE REG COM REG COM SYMB{
    __bne($2, $4, $6);
  }
;
bgts:
  BGT REG COM REG COM LITERAL{
    __bgt($2, $4, $6);
  }
  | BGT REG COM REG COM SYMB{
    __bgt($2, $4, $6);
  }
;
lds:
  LD DOLLAR LITERAL COM REG {
    __ld_immed($3, $5);
  }
  | LD DOLLAR SYMB COM REG{
    __ld_immed($3, $5);
  }
  | LD LITERAL COM REG{
    __ld_memdir($2, $4);
  }
  | LD SYMB COM REG{
    __ld_memdir($2, $4);
  }
  | LD REG COM REG{
    __ld_regdir($2, $4);
  }
  | LD LPAR REG RPAR COM REG{
    __ld_regind($3, $6);
  }
  | LD LPAR REG PLUS LITERAL RPAR COM REG{
    __ld_regindpom($3, $5, $8);
  }
  | LD LPAR REG PLUS SYMB RPAR COM REG{
    __ld_regindpom($3, $5, $8);
  }
;
sts:
  ST REG COM DOLLAR LITERAL{
    __st_immed($5, $2);
  }
  | ST REG COM DOLLAR SYMB{
    __st_immed($5, $2);
  }
  | ST REG COM LITERAL{
    __st_memdir($4, $2);
  }
  | ST REG COM SYMB{
    __st_memdir($4, $2);
  }
  | ST REG COM REG{
    __st_regdir($4, $2);
  }
  | ST REG COM LPAR REG RPAR{
    __st_regind($2, $5);
  }
  | ST REG COM LPAR REG PLUS LITERAL RPAR{
    __st_regindpom($2, $5, $7);
  }
  | ST REG COM LPAR REG PLUS SYMB RPAR{
    __st_regindpom($2, $5, $7);
  }
;
pushes:
  PUSH REG{
    __push($2);
  }
;
pops:
  POP REG{
    __pop($2);
  }
;
xchgs:
  XCHG REG COM REG{
    __xchg($2, $4);
  }
;
adds:
  ADD REG COM REG{
    __add($2, $4);
  }
;
subs:
  SUB REG COM REG{
    __sub($2, $4);
  }
;
muls:
  MUL REG COM REG{
    __mul($2, $4);
  }
;
divs:
  DIV REG COM REG{
    __div($2, $4);
  }
;
nots:
  NOT REG{
    __not($2);
  }
;
ands:
  AND REG COM REG{
    __and($2, $4);
  }
;
ors:
  OR REG COM REG{
    __or($2, $4);
  }
;
xors:
  XOR REG COM REG{
    __xor($2, $4);
  }
;
shls:
  SHL REG COM REG{
    __shl($2, $4);
  }
;
shrs:
  SHR REG COM REG{
    __shr($2, $4);
  }
;
csrrds:
  CSRRD STATUS COM REG{
    __csrrd(status, $4);
  }
  | CSRRD HANDLER COM REG{
    __csrrd(handler, $4);
  }
  | CSRRD CAUSE COM REG{
    __csrrd(cause, $4);
  }
;
csrwrs:
  CSRWR REG COM STATUS{
    __csrwr($2, status);
  }
  | CSRWR REG COM HANDLER{
    __csrwr($2, handler);
  }
  | CSRWR REG COM CAUSE{
    __csrwr($2, cause);
  }
;
ends:
  END{
    __end();
  }
;
/* calls:
  CALL IDENT{
    cout << "Calling " << $3 << endl;
  }
;
jmps:
  JMP IDENT{
    cout << "Jumping " << $3 << endl;
  }
;
beqs:
  BEQ REG COM REG COM IDENT{
    cout << "if r" << $3 << "==" << $6 << " branch " << $9 << endl;
  }
;
bnes:
  BNE REG COM REG COM IDENT{
    cout << "if r" << $3 << "!=" << $6 << " branch " << $9 << endl;
  }
;
bgts:
  BGT REG COM REG COM IDENT{
    cout << "if r" << $3 << ">" << $6 << " branch " << $9 << endl;
  }
;
lds:
  LD operand COM REG{
    cout << "load " << endl;
  }
  | LD operand COM REG{
    cout << "load " << endl;
  }
;
sts:
  ST REG COM operand{
    cout << "store from reg" << endl;
  }
;
csrrds:
  CSRRD csr COM REG{
    cout << "csrrd" << endl;
  }
;
csrwrs:
  CSRWR REG COM csr{
    cout << "csrwr" << endl;
  }
;
labels:
  LABEL{
    cout << "Label " << $1 << endl;
    free($1);
  }
;
csr:
  STATUS
  | HANDLER
  | CAUSE
;
ends:
  END
  | END{
    cout << "End." << endl;
  }
;
endls:
  endls
  |
;
operand:
  MEMDIR
  | immed
  | IDENT
  | INT
  | HEX
;
immed:
  DOLLAR HEX
  | DOLLAR INT
; */
%%

void yyerror(const char *s){
  printf("error");
}