#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "types.h"

using namespace std;

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

void handleLabelDeclaration(const string &label, int lineCount) {

}

void handleWordDeclaration(ScopeType scope, const string &varName, const string &value) {
    Symbol symbol;
    symbol.name = varName;
    symbol.memoryAddress = 0; // Placeholder, should be set to the next available memory address
    symbol.type = VARIABLE;
    symbol.scope = scope;

}

void oneTokenCase(const vector<string> &tokens, int lineCount) {
    string token = tokens[0];
    if (token.back() == ":" && token.size() > 1) {
        handleLabelDeclaration(token.substr(0, token.size() - 1), lineCount);
    }
}

void threeTokenCase(const vector<string> &tokens, int lineCount) {
    string token = tokens[0];
    string varName = tokens[1];
    string value = tokens[2];
    if (token == "WORD") {
        if (varName.empty()) {
            cout << "Error: Variable name cannot be empty at line " << lineCount << endl;
            exit(EXIT_FAILURE);
        }
        handleWordDeclaration(LOCAL, varName, value);
    }
}

void fourTokenCase(const vector<string> &tokens, int lineCount) {
    string scope = tokens[0];
    string token = tokens[1];
    string varName = tokens[2];
    string value = tokens[3];
    if (token == "WORD") {
        if (scope != "GLOBAL") {
            cout << "Error: Invalid scope for WORD declaration at line " << lineCount << endl;
            exit(EXIT_FAILURE);
        }
        if (varName.empty()) {
            cout << "Error: Variable name cannot be empty at line " << lineCount << endl;
            exit(EXIT_FAILURE);
        }
        handleWordDeclaration(GLOBAL, varName, value);
    }
}

void assemblerFirstPass(ifstream &file) {
    string line;
    int lineCount = 0;
    while(getline(file, line) && lineCount < MEMORY_SIZE) {
        lineCount++;

        if(!line.empty() && line[0] != '#') {
            vector<string> tokens;
            string word;
            istringstream iss(trim(line));
            while (iss >> word) {
                tokens.push_back(word);
            }
            int tokenCount = tokens.size();
            if (tokenCount == 0) {
                continue;
            }

            switch (tokenCount) {
                case 1: 
                    oneTokenCase(tokens, lineCount);
                    break;
                case 2:
                    break;
                case 3:
                    threeTokenCase(tokens, lineCount);
                    break;
                case 4:
                    fourTokenCase(tokens, lineCount);
                    break;
            }
        }
    }
}

void assemblerSecondPass(ifstream &file) {

}


void assembleProgram(const char *filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        exit(EXIT_FAILURE);
    }

    assemblerFirstPass(file);
    restartFilePointer(file);
    assemblerSecondPass(file);

    file.close();
}