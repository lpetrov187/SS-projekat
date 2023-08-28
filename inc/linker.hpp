#include <iostream>
#include <string>
#include <sstream>

using namespace std;

void openFileForReading(string filename);

void incrementLC(int num);

string decToHex2(int val);

int hexToInt(const string& hexStr);

void updateGlobalSymTab();

vector<string> splitString(string line);

void joinSections();

void updateSectionSymbols();

void applyRelocationTable();

string formatValueLE(int val);

void reallocateSections();

void updateSectionValues(string name, int val);

vector<string> getValues(string input);

struct symbolAttributesL{
  int num;
  string val;
  int valDecimal;
  int size;
  string type;
  string bind;
  int numSection;
  string name;
  int sectionUsed;
  string sectionName;
  
  symbolAttributesL(string num, int val, string type, string bind, string numSection, string name){
    this->num = stoi(num);
    this->valDecimal = val;
    this->val = decToHex2(val);
    this->type = type;
    this->bind = bind;
    this->numSection = stoi(numSection);
    this->name = name;
  }

  void setValue(int value){
    this->valDecimal = value;
    this->val = decToHex2(value);
  }

  void setSecName(string nam){
    this->sectionName = nam;
  }
};

struct relocationAttributesL{
  int offset;
  string offsetHex;
  string type;
  int symbol;
  int addend;
  string addendHex;
  string section;

  relocationAttributesL(int off, string typ, int symb, int add, int sec){
    this->offset = off;
    this->offsetHex = decToHex(off);
    this->type = typ;
    this->symbol = symb;
    this->section = sec;
    this->addend = add;
    this->addendHex = decToHex(add);
  }

  relocationAttributesL(string off, string type, string symbol, string addend, string section){
    this->offset = hexToInt(off);
    this->offsetHex = off;
    this->type = type;
    this->symbol = hexToInt(symbol);
    this->section = section;
    this->addendHex = addend;
    this->addend = hexToInt(addend);
  }

  void setOffset(int off){
    this->offsetHex = decToHex(off);
    this->offset = off;
  }
};

class Section {
public:
    int size;
    int startAddr;
    int endAddr;

    string realName;
    string name;
    bool reallocated = false;

    vector<relocationAttributesL> relocationTable = vector<relocationAttributesL>();
    vector<symbolAttributesL> sectionSymbols = vector<symbolAttributesL>();
    vector<string> content = vector<string>();

    Section(string sectionName)
        : name(sectionName) {}

    Section(string sectionName, int sAddr)
        : name(sectionName), startAddr(sAddr) {
            size = 0;
        }
    Section(string sectionName, string addon, int sAddr)
    : name(sectionName), startAddr(sAddr) {
        this->realName = sectionName + addon;
        size = 0;
    }
};

class Node {
public:
    Section data;
    Node* prev;
    Node* next;

    Node(const Section& section) : data(section), prev(nullptr), next(nullptr) {}
};

class DoublyLinkedList {

public:
    Node* head;
    Node* tail;

    DoublyLinkedList() : head(nullptr), tail(nullptr) {}

    void append(const Section& section) {
        Node* newNode = new Node(section);
        if (!head) {
            head = newNode;
            tail = newNode;
        } else {
            newNode->prev = tail;
            tail->next = newNode;
            tail = newNode;
        }
    }

    void display() {
        Node* current = head;
        while (current) {
            string sz = decToHex2(current->data.size);
            string sAddr = decToHex2(current->data.startAddr);
            string eAddr = decToHex2(current->data.endAddr);
            cout << "Section: " << current->data.name << " | Size: " << sz << " | Start addr: " << sAddr << " | End addr: " << eAddr << endl;
            
                for(const auto &element: current->data.relocationTable){
                    // cout << element.offset << "\t\t\t" << element.addend << "\t" << element.symbol << endl;
                }
                for(const auto &element: current->data.sectionSymbols){
                    cout << element.name << "\t\t\t" << element.val << "\t" << element.num << endl;
                }
            
            for(const auto &element: current->data.content){
                // cout << element << endl;
            }
            current = current->next;
        }
    }

    Node* find(string sect) {
        Node* current = tail;
        while (current) {
            if(current->data.name == sect)
                return current;
            current = current->prev;
        }
        return nullptr;
    }

    void insertAfter(Node* prevNode, const Section& section) {
        if (prevNode == nullptr) {
            std::cout << "Previous node cannot be null." << std::endl;
            return;
        }
        
        Node* newNode = new Node(section);

        newNode->next = prevNode->next;
        if (prevNode->next != nullptr) {
            prevNode->next->prev = newNode;
        } else {
            tail = newNode;
        }
        
        prevNode->next = newNode;
        newNode->prev = prevNode;
    }

    void updateAfter(Node* node){
        int val = node->data.size;
        Node* curr = node->next;
        while(curr){
            curr->data.startAddr += val;
            curr->data.endAddr += val;
            for(int i = 0; i < curr->data.sectionSymbols.size(); i++){
                curr->data.sectionSymbols[i].setValue(curr->data.sectionSymbols[i].valDecimal + val);
            }
            curr = curr->next;
        }
    }

    int getAddon(Node* node){
        int len = node->data.realName.length();
        string addonS = node->data.realName.substr(len - 2, len - 1);
        int addon = stoi(addonS);
        return addon;
    }

    void deleteNode(Section val) {
        Node* current = head;
        while (current) {
            if (current->data.realName == val.realName) {
                if (current->prev) {
                    current->prev->next = current->next;
                } else {
                    head = current->next;
                }
                if (current->next) {
                    current->next->prev = current->prev;
                } else {
                    // If the deleted node is the tail, update the tail pointer
                    tail = current->prev;
                }
                delete current;
                return;
            }
            current = current->next;
        }
    }

    ~DoublyLinkedList() {
        Node* current = head;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }
};

