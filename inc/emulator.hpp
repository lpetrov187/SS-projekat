#include <string>

using namespace std;

enum instructions{
  HALT,
  INT,
  CALL,
  JMP,
  XCHG,
  AR,
  LOG,
  SH,
  ST,
  LD
};

string decToHex3(int val);

void _halt();

void _int();

void _call(int a, int b, int disp, int mod);

void _jmp(int a, int b, int c, int disp, int mod);

void _xchg(int b, int c);

void _ar(int a, int b, int c, int mod);

void _log(int a, int b, int c, int mod);

void _sh(int a, int b, int c, int mod);

void _st(int a, int b, int c, int disp, int mod);

void _ld(int a, int b, int c, int disp, int mod);

void _push(unsigned int val);

unsigned int _pop();

unsigned int littleEndianHexToUint(string hexString);

string formatValueLittleEndian(int val);

string formatRegisterValue(unsigned int value);