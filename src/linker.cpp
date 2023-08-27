#include <vector>
#include <fstream>

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
          tmp->data.relocationTable.push_back(line);
        }
      } else if(symtab){
        if(line[0] == 'N'){
          continue;
        } else {
          istringstream iss(line);
          vector<string> split_strings;
          do {
            string word;
            iss >> word;
            if (!word.empty()) {
                split_strings.push_back(word);
            }
          } while (iss);
            string num = split_strings[0].substr(0, split_strings[0].length() - 1);
            symbolAttributesL tmp = symbolAttributesL(num, hexToInt(split_strings[1]), split_strings[3], split_strings[4], split_strings[5], split_strings[6]);

            fileSymTab.push_back(tmp);
        }
      }
    }

    for(int i = 0; i < fileSymTab.size(); i++){
      if(fileSymTab[i].bind == "GLOB" && fileSymTab[i].numSection != 0){
        // nadji ime sekcije
        string sectionName = "";
        for(int j = 0; j < fileSymTab.size(); j++){
          if(fileSymTab[j].num == fileSymTab[i].numSection){
            sectionName = fileSymTab[j].name;
          }
        }
        Node* tmp = sectionList.find(sectionName);
        // dohvati pocetak sekcije
        int addr = tmp->data.startAddr;
        // ubaci element u tabelu simbola sekcije
        tmp->data.sectionSymbols.push_back(fileSymTab[i]);
        tmp->data.sectionSymbols[tmp->data.sectionSymbols.size() - 1].setValue(fileSymTab[i].valDecimal + addr);

        // ubaci element u tabelu globalnih simbola
        globalSymTab.push_back(fileSymTab[i]);
        // globalSymTab[globalSymTab.size() - 1].setValue(fileSymTab[i].valDecimal + addr);
        globalSymTab[globalSymTab.size() - 1].setSecName(sectionName);

      }
    }
    // azuriraj sekcije i simbole iza trenutne
    sectionList.updateAfter(tmp);
    inputFile.close();
  }

  // azuriraj globalSymTab
  updateGlobalSymTab();
  for(const auto &element: globalSymTab){
    cout << element.name << "\t\t" << element.val << "\t" << element.sectionName << endl;
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