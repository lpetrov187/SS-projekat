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

int main(int argc, char* argv[])
{
  if (argc > 1) {
    all_args.assign(argv + 1, argv + argc);
  }
  
  for(const auto &element: all_args){
    openFileForReading(element);

    string line;
    Node* tmp;
    bool section = false;
    while (getline(inputFile, line)) {
      if(line[0] == '#' && line[1] != '#') {
        section = true;
        string name = line.substr(1);
        tmp = sectionList.find(name);
        if(tmp == nullptr){
          sectionList.append(Section(name, locationCounter));
          tmp = sectionList.find(name);
        }
      } else if(line[0] == '#' && line[1] == '#' && line[2] != '#'){
        section = false;
      } else if(line[0] == '#' && line[1] == '#' && line[2] == '#'){
        section = false;
      } else if(section){
        tmp->data.content.push_back(line);
        tmp->data.size += 4;
        incrementLC(1);
      }
    }

    inputFile.close();

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