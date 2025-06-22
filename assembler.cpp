#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "types.h"
#include "utils.h"

using namespace std;

void verifyAlreadyDeclared(const vector<SymbolTableRow> &symbolTable, const string &name, int lineCount) {
    for (const auto &symbol : symbolTable) {
        if (symbol.name == name) {
            cout << "Error: Symbol '" << name << "' already declared at line " << lineCount << endl;
            exit(EXIT_FAILURE);
        }
    }
}

void verifyMaxSymbols(const vector<SymbolTableRow> &symbolTable, int lineCount) {
    if (symbolTable.size() >= MAX_SYMBOLS) {
        cout << "Error: Maximum number of symbols exceeded at line " << lineCount << endl;
        exit(EXIT_FAILURE);
    }
}

void handleLabelDeclaration(vector<AssembledProgram> &programs, const string &label, int lineCount, int programIndex) {
    AssembledProgram &actualProgram = programs[programIndex];

    verifyAlreadyDeclared(actualProgram.symbolTable, label, lineCount);
    verifyMaxSymbols(actualProgram.symbolTable, lineCount);

    SymbolTableRow newSymbol;
    newSymbol.name = label;
    newSymbol.type = LABEL;
    newSymbol.scope = LOCAL;
    newSymbol.memoryAddress = lineCount;
    actualProgram.symbolTable.push_back(newSymbol);
}

void handleWordDeclaration(vector<AssembledProgram> &programs, ScopeType scope, const string &varName, const string &value, int programIndex, int lineCount) {
    AssembledProgram &actualProgram = programs[programIndex];

    verifyAlreadyDeclared(actualProgram.symbolTable, varName, lineCount);
    verifyMaxSymbols(actualProgram.symbolTable, lineCount);
    
    SymbolTableRow newSymbol;
    newSymbol.name = varName;
    newSymbol.type = VARIABLE;
    newSymbol.scope = scope;
    newSymbol.memoryAddress = actualProgram.memory.size();
    actualProgram.symbolTable.push_back(newSymbol);

    MemoryCell newCell;
    newCell.scope = scope;
    newCell.value = stoi(value);
    actualProgram.memory.push_back(newCell);
}

void oneTokenCase(vector<AssembledProgram> &programs, const vector<string> &tokens, int lineCount, int programIndex) {
    string token = tokens[0];
    if (token.back() == ':') {
        if (token.size() < 2) {
            cout << "Error: Label cannot be empty at line " << lineCount << endl;
            exit(EXIT_FAILURE);
        }
        handleLabelDeclaration(programs, token.substr(0, token.size() - 1), lineCount, programIndex);
    }
}

void threeTokenCase(vector<AssembledProgram> &programs, const vector<string> &tokens, int lineCount, int programIndex) {
    string token = tokens[0];
    string varName = tokens[1];
    string value = tokens[2];
    if (token == "WORD") {
        if (varName.empty()) {
            cout << "Error: Variable name cannot be empty at line " << lineCount << endl;
            exit(EXIT_FAILURE);
        }
        handleWordDeclaration(programs, LOCAL, varName, value, programIndex, lineCount);
    }
}

void fourTokenCase(vector<AssembledProgram> &programs, const vector<string> &tokens, int lineCount, int programIndex) {
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
        if (value.empty()) {
            cout << "Error: Value cannot be empty at line " << lineCount << endl;
            exit(EXIT_FAILURE);
        }
        handleWordDeclaration(programs, GLOBAL, varName, value, programIndex, lineCount);
    }
}

void assemblerFirstPass(vector<AssembledProgram> &programs, ifstream &file, int programIndex) {
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
                    oneTokenCase(programs, tokens, lineCount, programIndex);
                    break;
                case 2:
                    break;
                case 3:
                    threeTokenCase(programs, tokens, lineCount, programIndex);
                    break;
                case 4:
                    fourTokenCase(programs, tokens, lineCount, programIndex);
                    break;
                default:
                    cout << "Error: Invalid number of tokens at line " << lineCount << endl;
                    exit(EXIT_FAILURE);
            }
        }
    }
}

void assemblerSecondPass(ifstream &file) {

}


void assembleProgram(vector<AssembledProgram> &programs, const char *filename, int programIndex) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Assembling program: " << filename << endl;

    assemblerFirstPass(programs, file, programIndex);
    printSymbolTable(programs);
    // restartFilePointer(file);
    // assemblerSecondPass(file);

    file.close();
}