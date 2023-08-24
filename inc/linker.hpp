#include <iostream>
#include <string>
#include <sstream>
// #include "assembler.hpp"

using namespace std;

void openFileForReading(string filename);

void incrementLC(int num);

string decToHex2(int val);

class Section {
public:
    int size;
    int startAddr;
    int endAddr;

    string name;
    
    vector<string> content = vector<string>();

    Section(const std::string& sectionName)
        : name(sectionName) {}

    Section(const std::string& sectionName, int sAddr)
        : name(sectionName), startAddr(sAddr) {
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
            cout << "Section: " << current->data.name << " | Size: " << sz << " | Stard addr: " << sAddr << endl;
            for(const auto &element: current->data.content){
                cout << element << endl;
            }
            current = current->next;
        }
    }

    Node* find(string sect) {
        Node* current = head;
        while (current) {
            if(current->data.name == sect)
                return current;
            current = current->next;
        }
        return nullptr;
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