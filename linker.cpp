#include <iostream>
#include <string>
#include <vector>

#include "assembler.h"
#include "types.h"
#include "utils.h"

using namespace std;

void verifyAlreadyDeclaredSymbol(const vector<SymbolTableRow> &symbolTable, const string &name, const int programIndex, const ScopeType scope, const int lineCount) {
	for (const auto &symbol : symbolTable) {
		bool sameName = symbol.name == name;
		bool sameProgram = symbol.programIndex == programIndex;
		bool isGlobalConflict = (symbol.scope == GLOBAL || scope == GLOBAL);

		if (sameName) {
			if (isGlobalConflict) {
				cout << "Error: Global symbol '" << name
				     << "' already declared in program " << symbol.programIndex << endl;
				exit(EXIT_FAILURE);
			}

			if (sameProgram) {
				cout << "Error: Symbol '" << name
				     << "' already declared in program " << programIndex << endl;
				exit(EXIT_FAILURE);
			}
		}
	}
}

void linkerFirstPass(LinkedProgram &linkedProgram, vector<AssembledProgram> &programs, int programIndex, int &dataOffset, int &instructionOffset, int totalDataOffset) {
	AssembledProgram &currentProgram = programs[programIndex];
	for (int i = 0; i < currentProgram.initOfProgram; i++) {
		MemoryCell newCell = currentProgram.memory[i];
		newCell.programIndex = programIndex;
		linkedProgram.auxDataMemory.push_back(newCell);
	}
	for (size_t i = static_cast<size_t>(currentProgram.initOfProgram); i < currentProgram.memory.size(); i++) {
		MemoryCell newCell = currentProgram.memory[i];
		newCell.programIndex = programIndex;
		linkedProgram.auxInstructionMemory.push_back(newCell);
	}

	for (const auto &symbol : currentProgram.symbolTable) {
		SymbolTableRow newSymbol = symbol;
		newSymbol.programIndex = programIndex;

		if (newSymbol.type == VARIABLE) {
			newSymbol.memoryAddress += dataOffset;
		} else if (newSymbol.type == LABEL) {
			newSymbol.memoryAddress += instructionOffset + totalDataOffset - currentProgram.initOfProgram;
		}

		verifyAlreadyDeclaredSymbol(linkedProgram.symbolTable, newSymbol.name, programIndex, newSymbol.scope, newSymbol.memoryAddress);

		linkedProgram.symbolTable.push_back(newSymbol);
	}

	dataOffset += currentProgram.initOfProgram;
	instructionOffset += (currentProgram.memory.size() - currentProgram.initOfProgram);
}

void linkerSecondPass(LinkedProgram &linkedProgram) {
	for (auto const &dataCell : linkedProgram.auxDataMemory) {
		linkedProgram.memory.push_back(dataCell);
	}

	for (auto const &instructionCell : linkedProgram.auxInstructionMemory) {
		MemoryCell newCell = instructionCell;
		if (newCell.symbolName != "" && newCell.targetOperand != -1) {
			bool found = false;
			for (const auto &symbol : linkedProgram.symbolTable) {
				if (!found) {
					if (symbol.name == newCell.symbolName) {
						if (symbol.scope == GLOBAL || (symbol.scope == LOCAL && symbol.programIndex == newCell.programIndex)) {
							if (newCell.targetOperand == 1) {
								newCell.instruction.operand1 = symbol.memoryAddress;
							} else if (newCell.targetOperand == 2) {
								newCell.instruction.operand2 = symbol.memoryAddress;
							} else if (newCell.targetOperand == 3) {
								newCell.instruction.operand3 = symbol.memoryAddress;
							} else {
								cout << "Error: Invalid target operand for symbol '"
								     << newCell.symbolName << "' at program "
								     << newCell.programIndex << endl;
								exit(EXIT_FAILURE);
							}
						} else {
							cout << "Error: Symbol '" << newCell.symbolName
							     << "' not found in the correct scope at program "
							     << newCell.programIndex << endl;
							exit(EXIT_FAILURE);
						}

						found = true;
					}
				}
			}
			if (!found) {
				cout << "Error: Symbol '" << newCell.symbolName
				     << "' not found in the symbol table at program "
				     << newCell.programIndex << endl;
				exit(EXIT_FAILURE);
			}
		}

		linkedProgram.memory.push_back(newCell);
	}
}

void initLinker(vector<AssembledProgram> &programs, LinkedProgram &linkedProgram, char *argv[], int totalPrograms) {
	int i;
	int dataOffset = 0;
	int instructionOffset = 0;
	int totalDataOffset = 0;
	for (const auto &program : programs) {
		totalDataOffset += program.initOfProgram;
	}

	cout << "Instruction offset initialized to: " << instructionOffset << endl;

	for (i = 0; i < totalPrograms; i++) {
		int programIndex = i;
		linkerFirstPass(linkedProgram, programs, programIndex, dataOffset, instructionOffset, totalDataOffset);
	}

	// printAuxDataMemory(linkedProgram);
	// printAuxInstructionMemory(linkedProgram);
	// printLinkedSymbolTable(linkedProgram);

	for (i = 0; i < totalPrograms; i++) {
		linkerSecondPass(linkedProgram);
	}

	printFinalLinkedMemory(linkedProgram);
	printLinkedSymbolTable(linkedProgram);
}