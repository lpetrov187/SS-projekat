#include <iostream>
#include <string>
#include <sstream>
// #include "assembler.hpp"

using namespace std;

void openFileForReading(string filename);

void incrementLC(int num);

string decToHex2(int val);

int hexToInt(const string& hexStr);

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
};

class Section {
public:
    int size;
    int startAddr;
    int endAddr;

    string realName;
    string name;
    

    vector<symbolAttributesL> sectionSymbols = vector<symbolAttributesL>();
    vector<string> content = vector<string>();
    vector<string> relocationTable = vector<string>();

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
private:
    Node* head;
    Node* tail;

public:
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
            cout << "Section: " << current->data.realName << " | Size: " << sz << " | Start addr: " << sAddr << endl;
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

    ~DoublyLinkedList() {
        Node* current = head;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }
};

