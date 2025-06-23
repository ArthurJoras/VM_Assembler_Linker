#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "types.h"

string trim_left(const string &str) {
	const string pattern = " \f\n\r\t\v";
	size_t start = str.find_first_not_of(pattern);
	if (start == string::npos) {
		return "";
	}
	return str.substr(start);
}

string trim_right(const string &str) {
	const string pattern = " \f\n\r\t\v";
	size_t end = str.find_last_not_of(pattern);
	if (end == string::npos) {
		return "";
	}
	return str.substr(0, end + 1);
}

string trim(const string &str) {
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

void printMemory(const AssembledProgram &program) {
	cout << "Memory:" << endl;
	for (size_t i = 0; i < program.memory.size(); ++i) {
		if (i < static_cast<size_t>(program.initOfProgram)) {
			continue;
		}
		const MemoryCell &cell = program.memory[i];
		cout << "Address: " << i << ", Instruction: "
		     << cell.instruction.opcode << " "
		     << cell.instruction.operand1 << " "
		     << cell.instruction.operand2 << " "
		     << cell.instruction.operand3
		     << endl;
	}
}

void printAuxDataMemory(const LinkedProgram &linkedProgram) {
	cout << "Auxiliary Data Memory:" << endl;
	for (size_t i = 0; i < linkedProgram.auxDataMemory.size(); ++i) {
		const MemoryCell &cell = linkedProgram.auxDataMemory[i];
		cout << "Address: " << i
		     << ", Value: " << cell.value
		     << ", Program Index: " << cell.programIndex
		     << ", Scope: " << (cell.scope == GLOBAL ? "Global" : "Local")
		     << endl;
	}
}

void printLinkedSymbolTable(const LinkedProgram &linkedProgram) {
	cout << "Linked Symbol Table:" << endl;
	for (const auto &symbol : linkedProgram.symbolTable) {
		cout << "Symbol name: " << symbol.name
		     << ", Address: " << symbol.memoryAddress
		     << ", Type: " << (symbol.type == VARIABLE ? "Variable" : "Label")
		     << ", Scope: " << (symbol.scope == GLOBAL ? "Global" : "Local")
		     << ", Program Index: " << symbol.programIndex
		     << endl;
	}
}

void printAuxInstructionMemory(const LinkedProgram &linkedProgram) {
	cout << "Auxiliary Instruction Memory:" << endl;
	for (size_t i = 0; i < linkedProgram.auxInstructionMemory.size(); ++i) {
		const MemoryCell &cell = linkedProgram.auxInstructionMemory[i];
		cout << "Address: " << i
		     << ", Instruction: "
		     << cell.instruction.opcode << " "
		     << cell.instruction.operand1 << " "
		     << cell.instruction.operand2 << " "
		     << cell.instruction.operand3
		     << ", Program Index: " << cell.programIndex
		     << ", Scope: " << (cell.scope == GLOBAL ? "Global" : "Local")
		     << endl;
	}
}

int convertMnemonicToOpcode(const string &mnemonic) {
	if (mnemonic == "ADD") return ADD;
	if (mnemonic == "SUB") return SUB;
	if (mnemonic == "MUL") return MUL;
	if (mnemonic == "DIV") return DIV;
	if (mnemonic == "MV") return MV;
	if (mnemonic == "ST") return ST;
	if (mnemonic == "JMP") return JMP;
	if (mnemonic == "JEQ") return JEQ;
	if (mnemonic == "JGT") return JGT;
	if (mnemonic == "JLT") return JLT;
	if (mnemonic == "W") return W;
	if (mnemonic == "R") return R;
	if (mnemonic == "STP") return STP;
	return NOP;
}

int registerToNumber(const string &reg) {
	if (reg == "A0") return 0;
	if (reg == "A1") return 1;
	if (reg == "A2") return 2;
	if (reg == "A3") return 3;
	return -1;
}