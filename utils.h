#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "types.h"

string trim_left(const string& str) {
  const string pattern = " \f\n\r\t\v";
  return str.substr(str.find_first_not_of(pattern));
}

string trim_right(const string& str) {
  const string pattern = " \f\n\r\t\v";
  return str.substr(0,str.find_last_not_of(pattern) + 1);
}

string trim(const string& str) {
  return trim_left(trim_right(str));
}

void restartFilePointer(ifstream &file) {
    if (file.is_open()) {
        file.clear();
        file.seekg(0, ios::beg);
    }
}

void printSymbolTable(const vector<AssembledProgram> &programs) {
    cout << "Symbol Table:" << endl;
    for (const auto &program : programs) {
        for (const auto &symbol : program.symbolTable) {
            cout << "Symbol name: " << symbol.name 
                 << ", Address: " << symbol.memoryAddress 
                 << ", Type: " << (symbol.type == VARIABLE ? "Variable" : "Label") 
                 << ", Scope: " << (symbol.scope == GLOBAL ? "Global" : "Local") 
                 << endl;
        }
    }
}

#endif