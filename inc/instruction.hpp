#include <vector>
#include <iostream>
using namespace std;

enum instructions{
  _extern,
  global,
  section,
  word,
  skip,
  halt,
  _int,
  iret,
  call,
  ret,
  jmp,
  beq,
  bne,
  bgt,
  push,
  pop,
  xchg,
  add,
  sub,
  mul,
  _div,
  _not,
  _and,
  _or,
  _xor,
  shl,
  shr,
  ld,
  st,
  csrrd,
  csrwr
};

struct Instruction{
  instructions instr;
  std::vector<string> idents;
};