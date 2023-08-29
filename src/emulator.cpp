#include "../inc/emulator.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <fstream>
#include <sstream>
#include <bits/stdc++.h>

using namespace std;

unsigned int *regs[16];
int sp = 14;
int pc = 15;

vector<string> all_args;
ifstream inputFile;
// std::ofstream outputFile;
string inputFileName;
map<unsigned int, string> memory;
string line;

int main(int argc, char* argv[]){
  if (argc > 1) {
    all_args.assign(argv + 1, argv + argc);
  }

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
    // cout << addrS << endl;
    // cout << addr << endl;
    // cout << stoul(addrS,0,16) << endl;
    string instr1 = line.substr(dots + 2, 11);
    string instr2 = line.substr(dots + 14);
    memory.insert(pair<unsigned int, string>(addr, instr1));
    if(instr2 != ""){
      addr += 4;
      memory.insert(pair<unsigned int, string>(addr, instr2));
    }
  }

  for (auto i = memory.begin(); i != memory.end(); i++){
    





		cout << i->first << " " << i->second << endl;
  }

}

string decToHex3(int val)
{
  std::stringstream ss;
  ss << std::hex << val;
  return ss.str();
}