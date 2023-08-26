#include <iostream>

using namespace std;

enum csrRegs {
  status,
  handler,
  cause
};


std::string convertPath(const std::string& inputPath);

void printWithLC();

void openFile(const char* name);

void printSymTable();

void printSymTableFile();

void printLiteralTable();

void printLiteralPool(int sectionID);

void printSectionTable();

void sortRelocationTable();

void printRelocationTable();

void printRelocationTableFile(int sectionID);

void printLC();

int insertLiteral(int literal);

int insertSymbolInLP(string symb);

int getSymbolValue(string symb);

int getSymbolNumber(string symb);

void incrementLC(int numOfInstructions);

string formatLC(int lc);

string formatValue(string val);

string formatValue8(int val);

string getOffset(int literal);

string getOffset(string symbol);

int getSectionNumber(int sectionID);

int getSectionID(string name);

int getSectionLiteralsNum(int sectionID);

string getSectionName(int sectionID);

string decToHex(int reg);

void __label(string label);

void __global(string symb);

void __extern(string symb);

void __section(string symb);

void __word(string symb);

void __word(int literal);

void __skip(int literal);

void __halt();

void __int();

void __iret();

void __call(int literal);

void __call(string symbol);

void __ret();

void __jmp(int literal);

void __jmp(string symbol);

void __beq(int reg1, int reg2, int literal);

void __beq(int reg1, int reg2, string symbol);

void __bne(int reg1, int reg2, int literal);

void __bne(int reg1, int reg2, string symbol);

void __bgt(int reg1, int reg2, int literal);

void __bgt(int reg1, int reg2, string symbol);

void __push(int reg);

void __pop(int reg);

void __xchg(int reg1, int reg2);

void __add(int reg1, int reg2);

void __sub(int reg1, int reg2);

void __mul(int reg1, int reg2);

void __div(int reg1, int reg2);

void __not(int reg);

void __and(int reg1, int reg2);

void __or(int reg1, int reg2);

void __xor(int reg1, int reg2);

void __shl(int reg1, int reg2);

void __shr(int reg1, int reg2);

void __ld_immed(int literal, int reg);

void __ld_immed(string symbol, int reg);

void __ld_memdir(int literal, int reg);

void __ld_memdir(string symbol, int reg);

void __ld_regdir(int op, int reg);

void __ld_regind(int op, int reg);

void __ld_regindpom(int op, int literal, int reg);

void __ld_regindpom(int op, string symbol, int reg);

void __st_immed(int literal, int reg);

void __st_immed(string symbol, int reg);

void __st_memdir(int literal, int reg);

void __st_memdir(string symbol, int reg);

void __st_regdir(int op, int reg);

void __st_regind(int op, int reg);

void __st_regindpom(int reg, int op, int literal);

void __st_regindpom(int reg, int op, string symbol);

void __csrrd(csrRegs csr, int reg);

void __csrwr(int reg, csrRegs csr);

void __end();

struct symbolAttributes{
  int num;
  string val;
  int valDecimal;
  int size;
  string type;
  string bind;
  int numSection;
  string name;
  int sectionUsed;

  symbolAttributes(int num, int val, int size, string type, 
  string bind, int numSection, string name){
    this->num = num;
    this->valDecimal = val;
    this->val = decToHex(val);
    this->size = size;
    this->type = type;
    this->bind = bind;
    this->numSection = numSection;
    this->name = name;
    this->sectionUsed = -1;
  }
  
  symbolAttributes(string num, int val, string type, string bind, string numSection, string name){
    this->num = stoi(num);
    this->valDecimal = val;
    this->val = decToHex(val);
    this->type = type;
    this->bind = bind;
    this->numSection = stoi(numSection);
    this->name = name;
  }
};

struct literalAttributes{
  int val;
  string hexValLE;
  int size;
  string location;
  int locationDecimal;
  int section;
  string name;

  literalAttributes(int v, int sz, int loc, int section){
    this->val = v;
    this->size = sz;
    this->locationDecimal = loc;
    this->location = decToHex(loc);
    this->section = section;
    this->hexValLE = formatValue8(v);
    this->name = "";
  }
  void setLocation(int loc){
    this->locationDecimal = loc;
    this->location = decToHex(loc);
  }
};

struct sectionAttributes{
  string name;
  int id;
  string location;
  int locationDecimal;
  string size;
  int literalPoolAddr;
  int numOfItems;

  sectionAttributes(int id, string n, int loc, string sz){
    this->id = id;
    this->name = n;
    this->locationDecimal = loc;
    this->location = decToHex(loc);
    this->size = sz;
    numOfItems = 0;
  }

  void setLocation(int loc){
    this->locationDecimal = loc;
    this->location = decToHex(loc);
  }
};

struct relocationAttributes{
  int offset;
  string offsetHex;
  string type;
  int symbol;
  int addend;
  string addendHex;
  int section;

  relocationAttributes(int off, string typ, int symb, int add, int sec){
    this->offset = off;
    this->offsetHex = decToHex(off);
    this->type = typ;
    this->symbol = symb;
    this->section = sec;
    this->addend = add;
    this->addendHex = decToHex(add);
  }

  void setOffset(int off){
    this->offsetHex = decToHex(off);
    this->offset = off;
  }
};