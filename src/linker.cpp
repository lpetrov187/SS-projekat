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
int locationCoutnerOutput = 0;
ifstream inputFile;
DoublyLinkedList sectionList = DoublyLinkedList();
int currSection = -1;
int prevSection = -1;
int sectionCounter = 0;

vector<symbolAttributesL> fileSymTab = vector<symbolAttributesL>();
vector<symbolAttributesL> globalSymTab = vector<symbolAttributesL>();
vector<string> sectionPlaces = vector<string>();
vector<string> inputFiles = vector<string>();



int main(int argc, char* argv[])
{
  if (argc > 1) {
    all_args.assign(argv + 1, argv + argc);
  }

  int i = 1;
  while(all_args[i].substr(0, 6) == "-place"){
    sectionPlaces.push_back(all_args[i]);
    i++;
  }
  // preskoci -o
  i++;
  outputFileName = all_args[i];
  // predji na prvi ulazni fajl
  i++;

  while(i < all_args.size()){
    inputFiles.push_back(all_args[i]);
    i++;
  }

  for(const auto &element: inputFiles){
    openFileForReading(element);

    fileSymTab.clear();
    string line = "";
    string name = "";
    string addon = "(0)";
    Node* tmp = nullptr;
    bool section = false;
    bool rela = false;
    bool symtab = false;
    bool inserted = false;

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
          inserted = true;
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
    if(inserted)
      sectionList.updateAfter(tmp);
    
    inputFile.close();
  }
  // preraspodeli sekcije
  reallocateSections();

  // azuriraj globalSymTab
  updateGlobalSymTab();
  
  // azuriraj globalne simbole u sekcijama u kojim se koriste
  updateSectionSymbols2();

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
        if(element.name == globalSymTab[i].name && element.sectionName != ""){
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
        for(int i = 0; i < nextNode->data.content.size(); i++){
          curr->data.content.push_back(nextNode->data.content[i]);
        }
        curr->data.size += nextNode->data.size;
        curr->data.endAddr += nextNode->data.size;
        sectionList.deleteNode(nextNode->data);
        nextNode = curr->next;
      }
    }
    curr = curr->next;
  }
}

void updateSectionSymbols(){
  for(int i = 0; i < fileSymTab.size(); i++){
    bool loc = false;
    if(fileSymTab[i].bind == "GLOB"){
      for(int j = 0; j < fileSymTab.size(); j++){ // nadji sekciju u kojoj je dati simbol
        if(fileSymTab[i].numSection == fileSymTab[j].num){
          string sectionName = fileSymTab[j].name;
          Node* tmp = sectionList.find(sectionName);
          // dohvati pocetak sekcije
          int addr = tmp->data.startAddr;
          // ubaci element u tabelu simbola sekcije
          loc = true;
          tmp->data.sectionSymbols.push_back(fileSymTab[i]);
          tmp->data.sectionSymbols[tmp->data.sectionSymbols.size() - 1].setValue(fileSymTab[i].valDecimal + addr);
          tmp->data.sectionSymbols[tmp->data.sectionSymbols.size() - 1].setSecName(fileSymTab[j].name);
          globalSymTab.push_back(fileSymTab[i]);
          globalSymTab[globalSymTab.size() - 1].setValue(fileSymTab[i].valDecimal + addr);
          globalSymTab[globalSymTab.size() - 1].setSecName(fileSymTab[j].name);
        } else if(fileSymTab[j].type == "SCTN"){
          string sectionName = fileSymTab[j].name;
          Node* tmp = sectionList.find(sectionName);
          // dohvati pocetak sekcije
          int addr = tmp->data.startAddr;
          // ubaci element u tabelu simbola sekcije
          loc = true;
          tmp->data.sectionSymbols.push_back(fileSymTab[i]);
          tmp->data.sectionSymbols[tmp->data.sectionSymbols.size() - 1].setValue(0);
        }
      }
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
  stream << std::setfill('0') << std::setw(2) << std::hex << (val & 0xFF) << " "
           << std::setw(2) << (val >> 8 & 0xFF) << " "
           << std::setw(2) << (val >> 16 & 0xFF) << " "
           << std::setw(2) << (val >> 24 & 0xFF);
    return stream.str();
}

void reallocateSections()
{
  Node* curr = sectionList.head;
  vector<string> tmp = vector<string>();
  int newStartAddr = 0;
  for(const auto& el: sectionPlaces){
    tmp = getValues(el);
    while(curr){
      if(curr->data.name == tmp[0]){
        newStartAddr = updateSectionValues(curr->data.name, hexToInt(tmp[1]));
      }
      curr = curr->next;
    }
    curr = sectionList.head;
  }

  while(curr){
    if(!curr->data.reallocated){
      newStartAddr = updateSectionValues(curr->data.name, newStartAddr);
      // newStartAddr = curr->data.endAddr;
      curr->data.reallocated = true;
    }
    curr = curr->next;
  }
}

vector<string> getValues(string input){
  // Find the position of '@' and '=' in the string
  size_t atPos = input.find('@');
  size_t equalPos = input.find('=');
  vector<string> output = vector<string>();
  if (atPos != std::string::npos && equalPos != std::string::npos) {
    // Extract the substrings based on the positions
    std::string value1 = "." + input.substr(equalPos + 1, atPos - equalPos - 1);
    std::string value2 = input.substr(atPos + 3);

    output.push_back(value1);
    output.push_back(value2);
    return output;
  } else {
    std::cout << "Substring not found in the input." << std::endl;
    return output;
  }
}

int updateSectionValues(string name, int val){
  Node* curr = sectionList.head;
  int endAddr = 0;

  while(curr){
    if(curr->data.name == name){
      int oldSAddr = curr->data.startAddr;
      curr->data.startAddr = val;
      curr->data.endAddr = curr->data.size + val;
      for(int i = 0; i < curr->data.sectionSymbols.size(); i++){
        if(curr->data.sectionSymbols[i].sectionName != ""){
          int newValue = curr->data.sectionSymbols[i].valDecimal - oldSAddr + val;
          curr->data.sectionSymbols[i].setValue(newValue);
        }
      }
      endAddr = curr->data.endAddr;
      val = endAddr;
      curr->data.reallocated = true;
    }
    curr = curr->next;
  }
  return endAddr;
}

void displayHexLC(int value) {
    std::cout << std::setw(4) << std::setfill('0') << std::hex << value << ": ";
}

void updateSectionSymbols2(){
  Node* curr = sectionList.head;

  while(curr){
    for(int i = 0; i < curr->data.sectionSymbols.size(); i++){
      if(curr->data.sectionSymbols[i].sectionName == ""){
        for(const auto &globalEl: globalSymTab){
          if(curr->data.sectionSymbols[i].name == globalEl.name){
            curr->data.sectionSymbols[i].setValue(globalEl.valDecimal);
          }
        }
      }
    }
    curr = curr->next;
  }
}