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