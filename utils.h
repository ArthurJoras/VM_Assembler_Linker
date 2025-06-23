#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "types.h"

std::string trim_left(const std::string &str);
std::string trim_right(const std::string &str);
std::string trim(const std::string &str);

void restartFilePointer(std::ifstream &file);

void printSymbolTable(const std::vector<AssembledProgram> &programs);
void printMemory(const AssembledProgram &program);
void printAuxDataMemory(const LinkedProgram &linkedProgram);
void printAuxInstructionMemory(const LinkedProgram &linkedProgram);
void printLinkedSymbolTable(const LinkedProgram &linkedProgram);

int convertMnemonicToOpcode(const std::string &mnemonic);
int registerToNumber(const std::string &reg);

#endif
