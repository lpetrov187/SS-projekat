#include "../inc/lexer.hpp"
#include "../inc/parser.hpp"
#include "../inc/assembler.hpp"

#include <sstream>
#include <vector>
#include <fstream>
#include <iostream> 
#include <iomanip>
#include <list>
#include <algorithm>
#include <cmath>
using namespace std;

vector<symbolAttributes> symbolList = vector<symbolAttributes>();
vector<literalAttributes> literalList = vector<literalAttributes>();
vector<sectionAttributes> sectionList = vector<sectionAttributes>();
vector<relocationAttributes> relocationList = vector<relocationAttributes>();
int counter = 1;
int currSection = -1;
int locationCounter = 0;
int locationCounterGlobal = 0;
int literalPoolAddr = 0;
bool firstPass = false, secondPass = false;
string sp = decToHex(14);
string pc = decToHex(15);

// string inputFile = "../tests/handler.s";
string inputFile = "../tests/handler2.s";
// string inputFile = "../tests/isr_software.s";
// string inputFile = "../tests/isr_terminal.s";
// string inputFile = "../tests/isr_timer.s";
// string inputFile = "../tests/main.s";
// string inputFile = "../tests/main2.s";
// string inputFile = "../tests/math.s";
// string inputFile = "../tests/input.s";

string ofName = convertPath(inputFile);
string ifName = "";
std::vector<std::string> all_args;
std::ofstream outputFile;

int main(int argc, char* argv[])
{
  if (argc > 1) {
    all_args.assign(argv + 1, argv + argc);
  }

  if(all_args[0] == "-o"){
    ofName = all_args[1];
    ifName = all_args[2];
  }
  else{
    ofName = "izlaz.o";
    ifName = all_args[0];
  }
  
  openFile(ifName.c_str());
  outputFile.open(ofName, std::ios::trunc);

  firstPass = true;
  do
  {
    yyparse();
  } while (!feof(yyin));
  firstPass = false;

  openFile(ifName.c_str());

  secondPass = true;
  locationCounter = 0;
  locationCounterGlobal = 0;
  currSection = -1;
  counter = 1;

  do
  {
    yyparse();
  } while (!feof(yyin));

  printSymTable();
  printLiteralTable();
  printSectionTable();
  printRelocationTable();

  outputFile.close();
}

std::string convertPath(const std::string& inputPath) {
    // Extract the directory part of the input path
    size_t lastSlashPos = inputPath.find_last_of('/');
    std::string directory = inputPath.substr(0, lastSlashPos + 1);

    // Extract the filename part of the input path
    std::string filename = inputPath.substr(lastSlashPos + 1);

    // Replace "tests" with "tmp" in the directory path
    size_t testsPos = directory.find("tests");
    directory.replace(testsPos, 5, "tmp");

    // Replace ".s" with ".o" in the filename
    size_t dotPos = filename.find_last_of('.');
    filename.replace(dotPos, 2, ".o");

    // Concatenate the modified directory and filename to form the new path
    std::string newPath = directory + filename;
    return newPath;
}

void printWithLC(){
  std::ifstream tmpFile;
  tmpFile.open("../tmp/izlaz.o");

  std::ofstream tmpIzlaz;
  tmpIzlaz.open("../tmp/izlaz_sa_LC.o");

  int tmpLC = 0;

  std::string line;
  while (std::getline(tmpFile, line)) {
    tmpIzlaz << decToHex(tmpLC) << ": " << line << std::endl;
    tmpLC += 4;
  }

  tmpFile.close();
  tmpIzlaz.close();
}

void openFile(const char *name){
  FILE *myfile = fopen(name, "r");
  if (!myfile)
  {
    cout << "I can't open the file!" << endl;
    return;
  }
  yyin = myfile;
}

void printSymTable() 
{
  cout << "Num\tValue\tSize\tType\tBind\tNdx\tName" << endl;

  for (const auto &element : symbolList)
  {
    cout << element.num << ":\t"
         << element.val << "\t"
         << element.size << "\t"
         << element.type << "\t"
         << element.bind << "\t"
         << element.numSection << "\t"
         << element.name << "\t" << endl;
  }
}

void printSymTableFile()
{
  outputFile << "#.symtab" << endl;
  outputFile << "Num\tValue\tSize\tType\tBind\t\tNdx\tName" << endl;

  for (const auto &element : symbolList)
  {
    outputFile << element.num << ":\t"
        << formatLC(element.valDecimal) << "\t\t"
        << element.size << "\t\t\t"
        << element.type << "\t"
        << element.bind << "\t\t"
        << element.numSection << "\t\t"
        << element.name << "\t" << endl;
    
  }
}

void printLiteralTable()
{
  cout << "Value\t\tSize\tLoc\tSection" << endl;
  for(const auto&element : literalList)
  {
    if(element.hexValLE == "UND"){
      cout << element.hexValLE << "\t\t";
    } else {
      cout << element.hexValLE << "\t";
    }
    cout << element.size << "\t"
         << element.location << "\t"
         << element.section << "\t"
         <<element.name << endl;
  }
}

void printLiteralPool(int sectionID)
{
  for(int i = 0; i < literalList.size(); i++){
      if(literalList[i].section == sectionID){
        outputFile << formatLC(literalList[i].locationDecimal) << ": " << formatValue8(literalList[i].val) << endl;
        incrementLC(1);
      }
    }
}

void printSectionTable()
{
  cout << "Name\t\tLocation\tSize\tLP addres" << endl;
  for(const auto&element : sectionList)
  {
    cout << element.name << "\t"
         << element.location << "\t\t"
         << element.size << "\t"
         << decToHex(element.literalPoolAddr) << endl;
  }
}

void sortRelocationTable(){
  std::sort(relocationList.begin(), relocationList.end(), [](const relocationAttributes& a, const relocationAttributes& b) {
    return a.offset < b.offset;
  });
}

void printRelocationTable()
{
  cout << "Offset\tType\t\tSymbol\tAddend" << endl;

  sortRelocationTable();

  for(const auto&element : relocationList)
  {
    cout << element.offsetHex << "\t"
         << element.type << "\t"
         << element.symbol << "\t"
         << element.addendHex << "\t" 
         << element.section << endl;
  }
}

void printRelocationTableFile(int sectionID)
{
  string name = getSectionName(sectionID);
  outputFile << "#.rela" << name << endl;
  sortRelocationTable();
  outputFile << "Offset\tType\t\t\t\t\tSymbol\tAddend" << endl;
  for(int i = 0; i < relocationList.size(); i++){
    if(relocationList[i].section == sectionID){
      outputFile << formatLC(relocationList[i].offset) << "\t\t\t"
      << relocationList[i].type << "\t\t"
      << formatLC(relocationList[i].symbol) << "\t\t\t\t"
      << formatLC(relocationList[i].addend) << endl;
    }
  }
}

void printLC()
{
  outputFile << formatLC(locationCounter) << ": ";
}

int insertLiteral(int literal)
{
  literalAttributes lit = literalAttributes(literal, 4, -1, currSection);
  bool defined = false;
  for (int i = 0; i < literalList.size(); i++)
  {
    if (literal == literalList[i].val && literalList[i].section == currSection)
    {
      defined = true;
      return literalList[i].locationDecimal;
    }
  }
  if(!defined){
    literalList.push_back(lit);
    int index = literalList.size() - 1;
    if(secondPass){
      int secNum = getSectionNumber(literalList[index].section);
      literalList[index].setLocation(sectionList[secNum].literalPoolAddr + (getSectionLiteralsNum(literalList[index].section) - 1)*4);
    }
    return literalList[index].locationDecimal;
  }
  return -1;
}

int insertSymbolInLP(string symb){

  literalAttributes lit = literalAttributes(0, 4, -1, currSection);
  lit.name = symb;
  bool defined = false;
  for (int i = 0; i < literalList.size(); i++)
  {
    if (literalList[i].section == currSection && literalList[i].name == symb)
    {
      defined = true;
      return literalList[i].locationDecimal;
    }
  }
  if(!defined){
    literalList.push_back(lit);
    int index = literalList.size() - 1;
    if(secondPass){
      int symbNum = getSymbolNumber(symb);
      int val = getSymbolValue(symb);
      int secNum = getSectionNumber(literalList[index].section);
      literalList[index].setLocation(sectionList[secNum].literalPoolAddr + (getSectionLiteralsNum(literalList[index].section) - 1)*4);
      if(symbolList[symbNum - 1].bind == "LOC"){
        relocationList.push_back(
          relocationAttributes(literalList[index].locationDecimal, "R_X86_64_32", symbolList[symbNum - 1].numSection, val, currSection));
      } else if(symbolList[symbNum - 1].bind == "GLOB"){
          relocationList.push_back(
            relocationAttributes(literalList[index].locationDecimal, "R_X86_64_32", symbNum, 0, currSection));
      }
    }
    return literalList[index].locationDecimal;
  }
  return -1;
}

int getSymbolValue(string symb)
{
  bool defined = false;
  cout << symb << endl;
  for (int i = 0; i < symbolList.size(); i++)
  {
    if (symb == symbolList[i].name)
    {
      return symbolList[i].valDecimal;
    }
  }
  cout << "Symbol not defined" << endl;
  return -1;
}

int getSymbolNumber(string symb)
{
  for (int i = 0; i < symbolList.size(); i++)
  {
    if (symb == symbolList[i].name)
    {
      return symbolList[i].num;
    }
  }
  return -1;
}

void incrementLC(int numOfInstructions)
{
  locationCounter += numOfInstructions * 4;
  locationCounterGlobal += numOfInstructions * 4;
}

string formatLC(int lc)
{
  string val = decToHex(lc);
  if(val.length() == 1){
    val = "00" + val;
  }
  else if(val.length() == 2){
    return "0" + val;
  }
  return val;
}

string formatValue(string val)
{
  if(val.length() == 1){
    val = "0 0" + val;
  }
  else if(val.length() == 2){
    val = "0 " + val;
  }
  return val;
}

string formatValue8(int val)
{
  std::stringstream stream;
  if(val == -1){
    return "UND";
  }
  stream << std::setfill('0') << std::setw(2) << std::hex << (val & 0xFF) << " "
           << std::setw(2) << (val >> 8 & 0xFF) << " "
           << std::setw(2) << (val >> 16 & 0xFF) << " "
           << std::setw(2) << (val >> 24 & 0xFF);
    return stream.str();
}

string getOffset(int literal)
{
  int addr = insertLiteral(literal);
  string offset = decToHex(addr - locationCounter);
  return offset;
}

string getOffset(string symbol)
{
  int num = getSymbolNumber(symbol);
  int addr = insertSymbolInLP(symbol);
  if(symbolList[num - 1].numSection == 0){
    symbolList[num - 1].sectionUsed = currSection;
  }
  
  string offset = decToHex(addr - locationCounter);
  return offset;
}

int getSectionNumber(int sectionID){
  for(int i = 0; i < sectionList.size(); i++){
    if(sectionList[i].id == sectionID){
      return i;
    }
  }
  return -1;
}

int getSectionID(string name){
  for(int i = 0; i < sectionList.size(); i++){
    if(sectionList[i].name == "." + name){
      return sectionList[i].id;
    }
  }
  return -1;
}

int getSectionLiteralsNum(int sectionID)
{
  int n = 0;
  for(int i = 0; i < literalList.size(); i++){
    if(literalList[i].section == sectionID){
      n++;
    }
  }
  return n;
}

string getSectionName(int sectionID)
{
  for(const auto &element : sectionList){
    if(element.id == sectionID){
      return element.name;
    }
  }
  return "";
}

string decToHex(int val)
{
  if(val == -1)
    return "UND";
  std::stringstream ss;
  ss << std::hex << val;
  return ss.str();
}

void __label(string label)
{
  bool defined = false;
  if(firstPass){
    for (int i = 0; i < symbolList.size(); i++)
    {
      if (label == symbolList[i].name)
      {
        symbolList[i].numSection = currSection;
        symbolList[i].valDecimal = locationCounter;
        symbolList[i].val = decToHex(locationCounter);
        defined = true;
        break;
      }
    }
    if (!defined)
    {
      symbolList.push_back(
          symbolAttributes(counter++, locationCounter, 0, "NOTYP", "LOC", currSection, label));
    }
  }
  if(secondPass){
    // int val = getSymbolValue(label);
    // int loc = insertLiteral(val);
    // relocationList.push_back(
    //   relocationAttributes(loc, "R_X86_64_32", currSection, val, currSection));
  }
}

void __global(string symb)
{
  if(firstPass){
    symbolList.push_back(
        symbolAttributes(counter++, -1, 0, "NOTYP", "GLOB", -1, symb));
  }
}

void __extern(string symb)
{
  if(firstPass){
    symbolList.push_back(
        symbolAttributes(counter++, 0, 0, "NOTYP", "GLOB", 0, symb));
  }
}

void __section(string symb)
{
  int prevSection = currSection;
  if(firstPass){
    currSection = counter++;

    string tmp = "." + symb;
    sectionList.push_back(
      sectionAttributes(currSection, tmp, 0, "0"));
    symbolList.push_back(
        symbolAttributes(currSection, 0, 0, "SCTN", "LOC", currSection, tmp));
    
    if(prevSection != -1){
      int index = getSectionNumber(prevSection);
      if(index != -1){
        sectionList[index].literalPoolAddr = locationCounter;
        sectionList[index].size = decToHex(locationCounter);
      }
      for(int i = 0; i < literalList.size(); i++){
        if(literalList[i].section == prevSection){
          literalList[i].setLocation(locationCounter);
          incrementLC(1);
        }
      }
    }
  }
  if(secondPass){
    currSection = getSectionID(symb);
    if(prevSection != -1)
    {
      printLiteralPool(prevSection);
      // printSymTableFile(prevSection);
      printRelocationTableFile(prevSection);
    }
    outputFile << "#." << symb << endl;
  }
  locationCounter = 0;
}

void __word(string symb)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    int num = getSymbolNumber(symb);
    if(symbolList[num - 1].bind == "GLOB"){
      relocationList.push_back(
            relocationAttributes(locationCounter, "R_X86_64_32", num, 0, currSection));
    } else if(symbolList[num - 1].bind == "LOC"){
      relocationList.push_back(
            relocationAttributes(locationCounter, "R_X86_64_32", currSection, getSymbolValue(symb), currSection));
    }
    printLC();
    outputFile << formatValue8(0) << endl;
    incrementLC(1);
  }
}

void __word(int literal)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    outputFile << formatValue8(literal) << endl;
    incrementLC(1);
  }
}

void __skip(int literal)
{
  int tmp = static_cast<int>(std::ceil(static_cast<double>(literal) / 4));
  if(firstPass){
    incrementLC(tmp);
  }
  if(secondPass){
    for(int i = 0; i < tmp; i++){
      printLC();
      outputFile << "00 00 00 00" << endl;
      incrementLC(1);
    }
  }
}

void __halt()
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    outputFile << "00 00 00 00" << endl;
    incrementLC(1);
  }
}

void __int()
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    outputFile << "10 00 00 00" << endl;
    incrementLC(1);
  }
}

void __iret()
{
  bool uradjeno = true;
  if(uradjeno){
    if(firstPass){
      incrementLC(3);
    }
    if(secondPass){
      int status = (int) csrRegs::status;

      if(secondPass){
        printLC();
        outputFile << "91 " << sp << sp << " 00 08" << endl; // sp <= sp + 8
        incrementLC(1);

        printLC();
        outputFile << "96 " << status << sp << " 0f fc" << endl; // status <= mem[sp - 4]
        incrementLC(1);

        printLC();
        outputFile << "92 " << pc << sp << " 0f f8" << endl; // pc <= mem[sp - 8]
        incrementLC(1);
      }
    }
  }
}

void __call(int literal)
{
  if(firstPass){
    incrementLC(1);
    insertLiteral(literal);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(literal);
    outputFile << "21 " << pc << "0 0" << formatValue(offset) << endl;
    incrementLC(1);
  }
}

void __call(string symbol)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(symbol);
    outputFile << "21 " << pc << "0 0" << formatValue(offset) << endl;
    incrementLC(1);
  }
}

void __ret()
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    incrementLC(1);
    outputFile << "93 " << pc << sp << " 00 04" << endl; // pc <= mem[sp]; sp <= sp + 4
  }
}

void __jmp(int literal)
{
  if(firstPass){
    incrementLC(1);
    insertLiteral(literal);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(literal);
    outputFile << "38 " << pc << "0 0" << formatValue(offset) << endl;
    incrementLC(1);
  }
}

void __jmp(string symbol)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(symbol);
    outputFile << "38 " << pc << "0 0" << formatValue(offset) << endl;
    incrementLC(1);
  }
}

void __beq(int reg1, int reg2, int literal)
{
  if(firstPass){
    incrementLC(1);
    insertLiteral(literal);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(literal);
    outputFile << "39 " << pc <<  decToHex(reg1) << " " <<  decToHex(reg2) << formatValue(offset) << endl;
    incrementLC(1);
  }
}

void __beq(int reg1, int reg2, string symbol)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(symbol);
    outputFile << "39 " << pc <<  decToHex(reg1) << " " <<  decToHex(reg2) << formatValue(offset) << endl;
    incrementLC(1);
  }
}

void __bne(int reg1, int reg2, int literal)
{
  if(firstPass){
    incrementLC(1);
    insertLiteral(literal);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(literal);
    outputFile << "3a " << pc <<  decToHex(reg1) << " " <<  decToHex(reg2) << formatValue(offset) << endl;
    incrementLC(1);
  }
}

void __bne(int reg1, int reg2, string symbol)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(symbol);
    outputFile << "3a " << pc <<  decToHex(reg1) << " " <<  decToHex(reg2) << formatValue(offset) << endl;
    incrementLC(1);
  }
}

void __bgt(int reg1, int reg2, int literal)
{
  if(firstPass){
    incrementLC(1);
    insertLiteral(literal);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(literal);
    outputFile << "3b " << pc <<  decToHex(reg1) << " " <<  decToHex(reg2) << formatValue(offset) << endl;
    incrementLC(1);
  }
}

void __bgt(int reg1, int reg2, string symbol)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(symbol);
    outputFile << "3b " <<  decToHex(reg1) << " " <<  decToHex(reg2) << formatValue(offset) << endl;
    incrementLC(1);
  }    
}

void __ld_immed(int literal, int reg)
{
  if(firstPass){
    incrementLC(1);
    insertLiteral(literal);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(literal);
    outputFile << "92 " << decToHex(reg) << pc << " 0" << formatValue(offset) << endl;
    incrementLC(1);
  }
}

void __ld_immed(string symbol, int reg)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(symbol);
    outputFile << "92 " <<  decToHex(reg) << pc << " 0" << formatValue(offset) << endl;
    incrementLC(1);
  }
}

void __ld_memdir(int literal, int reg)
{
  if(firstPass){
    incrementLC(2);
    insertLiteral(literal);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(literal);
    outputFile << "92 " <<  decToHex(reg) << pc << " 0" << formatValue(offset) << endl;
    incrementLC(1);

    printLC();
    outputFile << "92 " <<  decToHex(reg) << reg << " 00 00" << endl;
    incrementLC(1);
  }
}

void __ld_memdir(string symbol, int reg)
{
  if(firstPass){
    incrementLC(2);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(symbol);
    outputFile << "92 " <<  decToHex(reg) << pc << " 0" << formatValue(offset) << endl;
    incrementLC(1);

    printLC();
    outputFile << "92 " <<  decToHex(reg) <<  decToHex(reg) << " 00 00" << endl;
    incrementLC(1);
  }
}

void __ld_regdir(int op, int reg)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    outputFile << "91 " <<  decToHex(reg) << op << " 00 00" << endl;
    incrementLC(1);
  }
}

void __ld_regind(int op, int reg)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    outputFile << "92 " <<  decToHex(reg) << op << " 00 00" << endl;
    incrementLC(1);
  }
}

void __ld_regindpom(int op, int literal, int reg)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    string disp = formatValue(decToHex(literal));
    outputFile << "92 " <<  decToHex(reg) <<  decToHex(op) << " 0" << disp << endl;
    incrementLC(1);
  }
}

void __ld_regindpom(int op, string symbol, int reg)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    int val = getSymbolValue(symbol);
    string disp = formatValue(decToHex(val));
    outputFile << "92 " <<  decToHex(reg) <<  decToHex(op) << " 0" << disp << endl;
    incrementLC(1);
  }
}

void __st_immed(int literal, int reg)
{
  if(firstPass){
    incrementLC(1);
    insertLiteral(literal);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(literal);
    outputFile << "80 " << pc << "0 " <<  decToHex(reg) << formatValue(offset) << endl;
    incrementLC(1);
  }
}

void __st_immed(string symbol, int reg)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(symbol);
    outputFile << "80 " << pc << "0 " <<  decToHex(reg) << formatValue(offset) << endl;
    incrementLC(1);
  }
}

void __st_memdir(int literal, int reg)
{
  if(firstPass){
    incrementLC(1);
    insertLiteral(literal);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(literal);
    outputFile << "82 " << pc << "0 " <<  decToHex(reg) << formatValue(offset) << endl;
    incrementLC(1);
  }
}

void __st_memdir(string symbol, int reg)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    string offset = getOffset(symbol);
    outputFile << "82 " << pc << "0 " <<  decToHex(reg) << formatValue(offset) << endl;
    incrementLC(1);
  }
}

void __st_regdir(int op, int reg)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    outputFile << "91 " <<  decToHex(op) <<  decToHex(reg) << " 00 00" << endl;
    incrementLC(1);
  }
}

void __st_regind(int op, int reg)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    outputFile << "92 " <<  decToHex(op) <<  decToHex(reg) << " 00 00" << endl;
    incrementLC(1);
  }
}

void __st_regindpom(int reg, int op, int literal)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    string disp = formatValue(decToHex(literal));
    outputFile << "92 " <<  decToHex(op) <<  decToHex(reg) << " 0" << disp << endl;
    incrementLC(1);
  }
}

void __st_regindpom(int reg, int op, string symbol)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    int val = getSymbolValue(symbol);
    string disp = formatValue(decToHex(val));
    outputFile << "92 " <<  decToHex(op) <<  decToHex(reg) << " 0" << disp << endl;
    incrementLC(1);
  }
}

void __push(int reg)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    incrementLC(1);
    string hexVal = decToHex(reg);

    outputFile << "81 " << sp << "0 " << hexVal << "f fc" << endl; // sp <= sp - 4; mem[sp] <= reg
  }
}

void __pop(int reg)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    incrementLC(1);
    string hexVal = decToHex(reg);
    outputFile << "93 " << hexVal << sp << " 00 04" << endl; // reg <= mem[sp]; sp <= sp + 4
  }
}

void __xchg(int reg1, int reg2)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    incrementLC(1);
    string hexVal1 = decToHex(reg1);
    string hexVal2 = decToHex(reg2);
    
    outputFile << "40 0" << hexVal1 << " " << hexVal2 << "0 00" << endl;
  }
}

void __add(int reg1, int reg2)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    incrementLC(1);
    string hexVal1 = decToHex(reg1);
    string hexVal2 = decToHex(reg2);

    outputFile << "50 " << hexVal2 << hexVal2 << " " << hexVal1 << "0 00" << endl;
  }
}

void __sub(int reg1, int reg2)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    incrementLC(1);
    string hexVal1 = decToHex(reg1);
    string hexVal2 = decToHex(reg2);

    outputFile << "51 " << hexVal2 << hexVal2 << " " << hexVal1 << "0 00" << endl;
  }
}

void __mul(int reg1, int reg2)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    incrementLC(1);
    string hexVal1 = decToHex(reg1);
    string hexVal2 = decToHex(reg2);

    outputFile << "52 " << hexVal2 << hexVal2 << " " << hexVal1 << "0 00" << endl;
  }
}

void __div(int reg1, int reg2)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    incrementLC(1);
    string hexVal1 = decToHex(reg1);
    string hexVal2 = decToHex(reg2);

    outputFile << "53 " << hexVal2 << hexVal2 << " " << hexVal1 << "0 00" << endl;
  }
}

void __not(int reg)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    incrementLC(1);
    string hexVal = decToHex(reg);

    outputFile << "60 " << hexVal << hexVal << " 00 00" << endl;
  }
}

void __and(int reg1, int reg2)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    incrementLC(1);
    string hexVal1 = decToHex(reg1);
    string hexVal2 = decToHex(reg2);

    outputFile << "61 " << hexVal2 << hexVal2 << " " << hexVal1 << "0 00" << endl;
  }
}

void __or(int reg1, int reg2)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    incrementLC(1);
    string hexVal1 = decToHex(reg1);
    string hexVal2 = decToHex(reg2);

    outputFile << "62 " << hexVal2 << hexVal2 << " " << hexVal1 << "0 00" << endl;
  }
}

void __xor(int reg1, int reg2)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    incrementLC(1);
    string hexVal1 = decToHex(reg1);
    string hexVal2 = decToHex(reg2);

    outputFile << "63 " << hexVal2 << hexVal2 << " " << hexVal1 << "0 00" << endl;
  }
}

void __shl(int reg1, int reg2)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    incrementLC(1);
    string hexVal1 = decToHex(reg1);
    string hexVal2 = decToHex(reg2);

    outputFile << "70 " << hexVal2 << hexVal2 << " " << hexVal1 << "0 00" << endl;
  }
}

void __shr(int reg1, int reg2)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    incrementLC(1);
    string hexVal1 = decToHex(reg1);
    string hexVal2 = decToHex(reg2);

    outputFile << "71 " << hexVal2 << hexVal2 << " " << hexVal1 << "0 00" << endl;
  }
}

void __csrrd(csrRegs csr, int reg)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    incrementLC(1);
    int csrVal = (int) csr;
    string hexVal = decToHex(reg);
    
    outputFile << "90 " << hexVal << csrVal << " 00 00" << endl;
  }
}

void __csrwr(int reg, csrRegs csr)
{
  if(firstPass){
    incrementLC(1);
  }
  if(secondPass){
    printLC();
    incrementLC(1);
    int csrVal = (int) csr;
    string hexVal = decToHex(reg);
    outputFile << "94 " << csrVal << hexVal << " 00 00" << endl;
  }
}

void __end()
{
  if(firstPass){
      sectionList[sectionList.size() - 1].literalPoolAddr = locationCounter;
      sectionList[sectionList.size() - 1].size = decToHex(locationCounter);
      for(int i = 0; i < literalList.size(); i++){
        if(literalList[i].section == currSection){
          literalList[i].setLocation(locationCounter);
          incrementLC(1);
        }
      }
  }
  if(secondPass){
    printLiteralPool(currSection);
    printRelocationTableFile(currSection);
    printSymTableFile();
  }
}