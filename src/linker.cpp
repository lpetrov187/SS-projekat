#include <vector>
#include <fstream>
#include <iomanip>

#include "../inc/assembler.hpp"
#include "../inc/linker.hpp"

vector<string> all_args;
string ofName = "";
string ifName = "";
int startAddr = 0;
int locationCounter = 0;
ifstream inputFile;
DoublyLinkedList sectionList = DoublyLinkedList();
int currSection = -1;
int prevSection = -1;
int sectionCounter = 0;

vector<symbolAttributesL> fileSymTab = vector<symbolAttributesL>();
vector<symbolAttributesL> globalSymTab = vector<symbolAttributesL>();


int main(int argc, char* argv[])
{
  if (argc > 1) {
    all_args.assign(argv + 1, argv + argc);
  }  

  for(const auto &element: all_args){
    openFileForReading(element);

    fileSymTab.clear();
    string line = "";
    string name = "";
    string addon = "(0)";
    Node* tmp = nullptr;
    bool section = false;
    bool rela = false;
    bool symtab = false;

    while (getline(inputFile, line)){
      if(line[0] == '#' && line[1] != '#') {
        section = true;
        rela = false;
        symtab = false;
        name = line.substr(1);
        tmp = sectionList.find(name);
        if(tmp == nullptr){
          sectionList.append(Section(name, addon, locationCounter));
          tmp = sectionList.find(name);
        } 
        else {
          int addonInt = sectionList.getAddon(tmp);
          addonInt++;
          addon = "(" + to_string(addonInt) + ")";
          int startAddr = tmp->data.startAddr + tmp->data.size;
          sectionList.insertAfter(tmp, Section(name, addon, startAddr));
          
          tmp = tmp->next;
        }
      } else if(line[0] == '#' && line[1] == '#' && line[2] != '#'){
        section = false;
        rela = true;
        symtab = false;
        tmp->data.endAddr = tmp->data.startAddr + tmp->data.size;
      } else if(line[0] == '#' && line[1] == '#' && line[2] == '#'){
        section = false;
        rela = false;
        symtab = true;
      } else if(section){
        tmp->data.content.push_back(line);
        tmp->data.size += 4;
        incrementLC(1);
      } else if(rela){
        if(line[0] == 'O'){
          continue;
        } else {
          vector<string> split_strings = splitString(line);
          relocationAttributesL ra = relocationAttributesL(split_strings[0], split_strings[1], split_strings[2], split_strings[3], name);
          tmp->data.relocationTable.push_back(ra);
        }
      } else if(symtab){
        if(line[0] == 'N'){
          continue;
        } else {
          vector<string> split_strings = splitString(line);
          string num = split_strings[0].substr(0, split_strings[0].length() - 1);
          symbolAttributesL sa = symbolAttributesL(num, hexToInt(split_strings[1]), split_strings[3], split_strings[4], split_strings[5], split_strings[6]);

          fileSymTab.push_back(sa);
        }
      }
    }
    // azuriraj tabelu simbola sekcije 
    updateSectionSymbols();
    // azuriraj sekcije i simbole iza trenutne
    sectionList.updateAfter(tmp);

    inputFile.close();
  }

  // azuriraj globalSymTab
  updateGlobalSymTab();
  
  // TODO: popravi vrednosti preko relokacione tabele
  applyRelocationTable();

  // spoji sekcije iz vise fajlova
  joinSections();


  for(const auto &element: globalSymTab){
    cout << element.name << "\t\t" << element.val << endl;
  }


  sectionList.display();
}



void incrementLC(int num){
  locationCounter += num * 4;
}

void openFileForReading(string filename) {
    inputFile.open(filename);

    if (!inputFile.is_open()) {
        std::cerr << "Error opening file for reading: " << filename << std::endl;
        return;
    }
}
string decToHex2(int val)
{
  if(val == -1)
    return "UND";
  std::stringstream ss;
  ss << std::hex << val;
  return ss.str();
}


int hexToInt(const string& hexStr) {
  char* endPtr;
  int result = strtol(hexStr.c_str(), &endPtr, 16);
  return result;
}

void updateGlobalSymTab()
{
  Node *curr = sectionList.head;
  while(curr){
    for(const auto &element: curr->data.sectionSymbols){
      for(int i = 0; i < globalSymTab.size(); i++){
        if(element.name == globalSymTab[i].name && element.valDecimal != globalSymTab[i].valDecimal){
          globalSymTab[i].setValue(element.valDecimal);
        }
      }
    }
    curr = curr->next;
  }
}

vector<string> splitString(string line)
{
  istringstream iss(line);
  vector<string> split_strings;
  do {
    string word;
    iss >> word;
    if (!word.empty()) {
        split_strings.push_back(word);
    }
  } while (iss);
  return split_strings;
}

void joinSections(){
  Node* curr = sectionList.head;

  while(curr){
    if(curr->next){
      Node* nextNode = curr->next;
      while(curr->data.name == nextNode->data.name){
        // for(int i = 0; i < curr->data.content.size(); i++){
        //   curr->data.content.push_back(curr->data.content[i]);
        // }
        curr->data.size += nextNode->data.size;
        curr->data.endAddr += nextNode->data.size;

        nextNode = nextNode->next;
      }
    }
    curr = curr->next;
  }
}

void updateSectionSymbols(){
  for(int i = 0; i < fileSymTab.size(); i++){
    if(fileSymTab[i].bind == "GLOB"){
      // ubaci u tabelu simbola sekcije simbole koji su uvezeni
      string sectionName = "";
      for(int j = 0; j < fileSymTab.size(); j++){
        if(fileSymTab[j].type == "SCTN"){
          sectionName = fileSymTab[j].name;
          Node* tmp = sectionList.find(sectionName);
          // dohvati pocetak sekcije
          int addr = tmp->data.startAddr;
          // ubaci element u tabelu simbola sekcije
          tmp->data.sectionSymbols.push_back(fileSymTab[i]);
          if(fileSymTab[i].numSection != 0){
            tmp->data.sectionSymbols[tmp->data.sectionSymbols.size() - 1].setValue(fileSymTab[i].valDecimal + addr);
          }
        }
      }
      // ubaci element u tabelu globalnih simbola
      bool defined = false;
      for(const auto &el: globalSymTab){
        if(el.name == fileSymTab[i].name)
          defined = true;
      }
      if(!defined)
        globalSymTab.push_back(fileSymTab[i]);
    }
  }
}

void applyRelocationTable(){
  Node* curr = sectionList.head;
  
  while(curr){
    for(const auto &elementRela: curr->data.relocationTable){
      // na adresi section + offset -> smestiti symbol + addend
      int addr = curr->data.startAddr + elementRela.offset;
      // dohvati ime simbola
      string symbName = "";
      for(const auto &elementSymb: curr->data.sectionSymbols){
        if(elementRela.symbol == elementSymb.num){
          symbName = elementSymb.name;
          break;
        }
      }
      int symbValue = 0;
      for(const auto &elementGlob: globalSymTab){
        if(elementGlob.name == symbName){
          symbValue = elementGlob.valDecimal;
          break;
        }
      }
      int val = symbValue + elementRela.addend;

      string valHexLE = formatValueLE(val);
      curr->data.content[(addr - curr->data.startAddr) / 4] = valHexLE;
    }
    curr = curr->next;
  }
}

string formatValueLE(int val)
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