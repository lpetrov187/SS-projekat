#include "../inc/emulator.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <bits/stdc++.h>

using namespace std;

unsigned int regs[16];
const int sp = 14;
const int pc = 15;
unsigned int csr[3];
const int status = 0;
const int handler = 1;
const int cause = 2;

vector<string> all_args;
ifstream inputFile;
// std::ofstream outputFile;
string inputFileName;
map<unsigned int, string> memory;
string line;
bool endFlag = false;


int main(int argc, char* argv[]){
  if (argc > 1) {
    all_args.assign(argv + 1, argv + argc);
  }

  regs[pc] = uint(stoul("40000000", 0, 16));
  regs[sp] = uint(stoul("10000000", 0, 16));

  inputFileName = all_args[0];


  inputFile.open(inputFileName);

  if (!inputFile.is_open()) {
      std::cerr << "Error opening file for reading: " << inputFileName << std::endl;
      return -1;
  }

  int cnt = 0;

  while (getline(inputFile, line)){
    size_t dots = line.find(':');
    string addrS = line.substr(0, dots);
    unsigned int addr = uint(stoul(addrS, 0, 16));
    string instr1 = line.substr(dots + 2, 11);
    string instr2 = line.substr(dots + 14);
    memory.insert(pair<unsigned int, string>(addr, instr1));
    if(instr2 != ""){
      addr += 4;
      memory.insert(pair<unsigned int, string>(addr, instr2));
    }
  }

  while(!endFlag){
    // cout << formatValueLittleEndian(regs[pc]) << endl;
    string instr = memory[regs[pc]];
    // if(instr == "")
      // instr = "00 00 00 00";

    cout << instr << endl;
    regs[pc] += 4;
    int disp;

    int OC = stoi(instr.substr(0, 1), 0, 16);
    int modifier = stoi(instr.substr(1, 1), 0, 16);

    string higherByte = instr.substr(9,2);
    string lowerByte = instr.substr(7, 1);;
    string dispS = higherByte + lowerByte;
    if(dispS == "ffc"){
      disp = -4;
    } else if(dispS == "ff8"){
      disp = -8;
    } else {
      disp = stoi(dispS, 0, 16);
    }

    int a = stoi(instr.substr(3, 1), 0, 16);
    int b = stoi(instr.substr(4, 1), 0, 16);
    int c = stoi(instr.substr(6, 1), 0, 16);
    int mod = stoi(instr.substr(1, 1), 0, 16);
    switch (OC)
    {
    case HALT:
      _halt();
      break;
    case INT:
      _int();
      break;
    case CALL:
      _call(a, b, disp, mod);
      break;
    case JMP:
      _jmp(a, b, c, disp, mod);
      break;
    case XCHG:
      _xchg(b, c);
      break;
    case AR:
      _ar(a, b, c, mod);
      break;
    case LOG:
      _log(a, b, c, mod);
      break;
    case SH:
      _sh(a, b, c, mod);
      break;
    case ST:
      _st(a, b, c, disp, mod);
      break;
    case LD:
      _ld(a, b, c, disp, mod);
      break;
    
    default:
      cout << "Unknown instruction." << endl;
      endFlag = true;
      break;
    }

  }
  cout << formatValueLittleEndian(csr[handler]) << endl;
}

string decToHex3(int val)
{
  std::stringstream ss;
  ss << std::hex << val;
  return ss.str();
}

void _halt(){
  cout << "Halting processor." << endl;
  endFlag = true;
}

void _int(){
  _push(csr[status]);
  _push(regs[pc]);
  csr[cause] = 4;
  csr[status] = csr[status] & (~0x1);
  regs[pc] = csr[handler];
}

void _call(int a, int b, int disp, int mod){
  unsigned int nextPC = 0;
  if(mod == 1){
    nextPC = littleEndianHexToUint(memory[regs[a] + regs[b] + disp]);
    // cout << "call: " << memory[regs[a] + regs[b] + disp] << endl;
  }
  switch (mod)
  {
  case 0:
    _push(regs[pc]);
    regs[pc] = regs[a] + regs[b] + disp;
    break;
  case 1:
    _push(regs[pc]);
    regs[pc] = nextPC;
    break;
  default:
    cout << "Wrong instruction modifier." << endl;
    break;
  }
}

void _jmp(int a, int b, int c, int disp, int mod){
  switch (mod)
  {
  case 0:
    regs[pc] = regs[a] + disp;
    break;
  case 1:
    if(regs[b] == regs[c]){
      regs[pc] = regs[a] + disp;
    }
    break;
  case 2:
    if(regs[b] != regs[c]){
      regs[pc] = regs[a] + disp;
    }
    break;
  case 3:
    if(int(regs[b]) > regs[c]){
      regs[pc] = regs[a] + disp;
    }
    break;
  case 8:
    regs[pc] = littleEndianHexToUint(memory[regs[a] + disp]);
    break;
  case 9:
    if(regs[b] == regs[c]){
      regs[pc] = littleEndianHexToUint(memory[regs[a] + disp]);
    }
    break;
  case 10:
    if(regs[b] != regs[c]){
      regs[pc] = littleEndianHexToUint(memory[regs[a] + disp]);
    }
    break;
  case 11:
    if(int(regs[b]) == regs[c]){
      regs[pc] = littleEndianHexToUint(memory[regs[a] + disp]);
    }
    break;
  default:
    cout << "Wrong instruction modifier." << endl;
    break;
  }
}

void _xchg(int b, int c){
  unsigned int tmp = regs[b];
  regs[b] = regs[c];
  regs[c] = tmp;
}

void _ar(int a, int b, int c, int mod){
  switch (mod)
  {
  case 0:
    regs[a] = regs[b] + regs[c];
    break;
  case 1:
    regs[a] = regs[b] - regs[c];
    break;
  case 2:
    regs[a] = regs[b] * regs[c];
    break;
  case 3:
    regs[a] = regs[b] / regs[c];
    break;
  default:
    cout << "Wrong instruction modifier." << endl;
    break;
  }
}

void _log(int a, int b, int c, int mod){
  switch (mod)
  {
  case 0:
    regs[a] = ~regs[b];
    break;
  case 1:
    regs[a] = regs[b] & regs[c];
    break;
  case 2:
    regs[a] = regs[b] | regs[c];
    break;
  case 3:
    regs[a] = regs[b] ^ regs[c];
    break;
  default:
    cout << "Wrong instruction modifier." << endl;
    break;
  }
}

void _sh(int a, int b, int c, int mod){
  switch (mod)
  {
  case 0:
    regs[a] = regs[b] << regs[c];
    break;
  case 1:
    regs[a] = regs[b] >> regs[c];
    break;
  default:
    cout << "Wrong instruction modifier." << endl;
    break;
  }
}

void _st(int a, int b, int c, int disp, int mod){
  unsigned int addr = 0;
  if(mod == 2){
    addr = littleEndianHexToUint(memory[regs[a] + regs[b] + disp]);
  }
  switch (mod)
  {
  case 0:
    memory[regs[a] + regs[b] + disp] = formatValueLittleEndian(regs[c]);
    break;
  case 2:
    memory[addr] = formatValueLittleEndian(regs[c]);
    break;
  case 1:
    regs[a] = regs[a] + disp;
    memory[regs[a]] = formatValueLittleEndian(regs[c]);
    break;
  default:
    cout << "Wrong instruction modifier." << endl;
    break;
  }
}

void _ld(int a, int b, int c, int disp, int mod){
  switch (mod)
  {
  case 0:
    regs[a] = csr[b];
    break;
  case 1:
    regs[a] = regs[b] + disp;
    break;
  case 2:
    regs[a] = littleEndianHexToUint(memory[regs[b] + regs[c] + disp]);
    break;
  case 3:
    // cout << disp << endl;
    // cout << regs[b];
    // cout << ":" << memory[regs[b]] << endl;
    regs[a] = littleEndianHexToUint(memory[regs[b]]);
    // cout << regs[a] << endl;
    regs[b] = regs[b] + disp;
    break;
  case 4:
    csr[a] = regs[b];
    break;
  case 5:
    csr[a] = csr[b] | disp;
    break;
  case 6:
    regs[a] = littleEndianHexToUint(memory[regs[b] + regs[c] + disp]);
    break;
  case 7:
    csr[a] = littleEndianHexToUint(memory[regs[b]]);
    regs[b] = regs[b] + disp;
    break;
  default:
    cout << "Wrong instruction modifier." << endl;
    break;
  }
}

void _push(unsigned int val){
  regs[sp] -= 4;
  memory[regs[sp]] = formatValueLittleEndian(val);
}

unsigned int _pop(){
  unsigned int val = littleEndianHexToUint(memory[regs[sp]]);
  regs[sp] += 4;
  return val;
}

unsigned int littleEndianHexToUint(string input) {
    string hexString;
    if(input.size() == 11){
      string bytes0 = input.substr(0, 2);
      string bytes1 = input.substr(3, 2);
      string bytes2 = input.substr(6, 2);
      string bytes3 = input.substr(9, 2);

      hexString = bytes3 + bytes2 + bytes1 + bytes0;
    }

    unsigned int result = uint(stoul(hexString, 0, 16));

    return result;
}

string formatValueLittleEndian(int val)
{
  std::stringstream stream;
  stream << std::setfill('0') << std::setw(2) << std::hex << (val & 0xFF) << " "
           << std::setw(2) << (val >> 8 & 0xFF) << " "
           << std::setw(2) << (val >> 16 & 0xFF) << " "
           << std::setw(2) << (val >> 24 & 0xFF);
    return stream.str();
}