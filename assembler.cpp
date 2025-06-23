#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

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
	newSymbol.programIndex = programIndex;
	actualProgram.symbolTable.push_back(newSymbol);
}

void handleWordDeclaration(vector<AssembledProgram> &programs, ScopeType scope, const string &varName, const string &value, int programIndex, int lineCount, int &initOfProgram) {
	AssembledProgram &actualProgram = programs[programIndex];

	verifyAlreadyDeclared(actualProgram.symbolTable, varName, lineCount);
	verifyMaxSymbols(actualProgram.symbolTable, lineCount);

	initOfProgram++;

	if (initOfProgram < lineCount) {
		cout << "Error: Variable declaration must be before program initialization at line " << lineCount << endl;
		exit(EXIT_FAILURE);
	}

	SymbolTableRow newSymbol;
	newSymbol.name = varName;
	newSymbol.type = VARIABLE;
	newSymbol.scope = scope;
	newSymbol.memoryAddress = actualProgram.memory.size();
	newSymbol.programIndex = programIndex;
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

void threeTokenCase(vector<AssembledProgram> &programs, const vector<string> &tokens, int lineCount, int programIndex, int &initOfProgram) {
	string token = tokens[0];
	string varName = tokens[1];
	string value = tokens[2];
	if (token == "WORD") {
		if (varName.empty()) {
			cout << "Error: Variable name cannot be empty at line " << lineCount << endl;
			exit(EXIT_FAILURE);
		}
		handleWordDeclaration(programs, LOCAL, varName, value, programIndex, lineCount, initOfProgram);
	}
}

void fourTokenCase(vector<AssembledProgram> &programs, const vector<string> &tokens, int lineCount, int programIndex, int &initOfProgram) {
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
		handleWordDeclaration(programs, GLOBAL, varName, value, programIndex, lineCount, initOfProgram);
	}
}

void assemblerFirstPass(vector<AssembledProgram> &programs, ifstream &file, int programIndex, int &initOfProgram) {
	string line;
	int lineCount = 0;
	while (getline(file, line) && lineCount < MEMORY_SIZE) {
		string trimmedLine = trim(line);
		if (!trimmedLine.empty() && trimmedLine[0] != '#') {
			lineCount++;
			vector<string> tokens;
			string word;
			istringstream iss(trimmedLine);
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
					threeTokenCase(programs, tokens, lineCount, programIndex, initOfProgram);
					break;
				case 4:
					fourTokenCase(programs, tokens, lineCount, programIndex, initOfProgram);
					break;
				default:
					cout << "Error: Invalid number of tokens at line " << lineCount << endl;
					exit(EXIT_FAILURE);
			}
		}
	}
}

void instructionOneTokenCase(vector<AssembledProgram> &programs, const vector<string> &tokens, int lineCount, int programIndex) {
	AssembledProgram &actualProgram = programs[programIndex];

	if (tokens[0].back() == ':') {
		return;
	}

	if (actualProgram.memory.size() <= (size_t)lineCount) {
		actualProgram.memory.resize(lineCount + 1);
	}

	Instruction instruction;

	instruction.opcode = convertMnemonicToOpcode(tokens[0]);
	if (instruction.opcode == STP) {
		instruction.operand1 = -1;
		instruction.operand2 = -1;
		instruction.operand3 = -1;
	} else {
		cout << "Error: Invalid instruction at line " << lineCount << endl;
		exit(EXIT_FAILURE);
	}

	MemoryCell newCell;
	newCell.instruction = instruction;
	newCell.scope = LOCAL;
	newCell.symbolName = "";
	newCell.targetOperand = -1;

	actualProgram.memory[lineCount] = newCell;
}

void instructionTwoTokenCase(vector<AssembledProgram> &programs, const vector<string> &tokens, int lineCount, int programIndex) {
	AssembledProgram &actualProgram = programs[programIndex];

	if (actualProgram.memory.size() <= (size_t)lineCount) {
		actualProgram.memory.resize(lineCount + 1);
	}

	Instruction instruction;
	MemoryCell newCell;
	instruction.opcode = convertMnemonicToOpcode(tokens[0]);

	switch (instruction.opcode) {
		case JMP:
		case R:
		case W:
			instruction.operand1 = -1;
			instruction.operand2 = -1;
			instruction.operand3 = -1;
			newCell.instruction = instruction;
			newCell.scope = LOCAL;
			newCell.symbolName = tokens[1];
			newCell.targetOperand = 1;
			actualProgram.memory[lineCount] = newCell;
			break;
		default:
			cout << "Error: Invalid instruction at line " << lineCount << endl;
			exit(EXIT_FAILURE);
	}
}

void instructionThreeTokenCase(vector<AssembledProgram> &programs, const vector<string> &tokens, int lineCount, int programIndex) {
	AssembledProgram &actualProgram = programs[programIndex];

	if (actualProgram.memory.size() <= (size_t)lineCount) {
		actualProgram.memory.resize(lineCount + 1);
	}

	Instruction instruction;
	MemoryCell newCell;
	instruction.opcode = convertMnemonicToOpcode(tokens[0]);

	switch (instruction.opcode) {
		case MV:
		case ST:
			instruction.operand1 = registerToNumber(tokens[1]);
			if (instruction.operand1 == -1) {
				cout << "Error: Invalid register '" << tokens[1] << "' at line " << lineCount << endl;
				exit(EXIT_FAILURE);
			}
			instruction.operand2 = -1;
			instruction.operand3 = -1;
			newCell.instruction = instruction;
			newCell.scope = LOCAL;
			newCell.symbolName = tokens[2];
			newCell.targetOperand = 2;
			actualProgram.memory[lineCount] = newCell;
			break;
		default:
			cout << "Error: Invalid instruction at line " << lineCount << endl;
			exit(EXIT_FAILURE);
	}
}

void instructionFourTokenCase(vector<AssembledProgram> &programs, const vector<string> &tokens, int lineCount, int programIndex) {
	AssembledProgram &actualProgram = programs[programIndex];

	if (actualProgram.memory.size() <= (size_t)lineCount) {
		actualProgram.memory.resize(lineCount + 1);
	}

	Instruction instruction;
	MemoryCell newCell;
	instruction.opcode = convertMnemonicToOpcode(tokens[0]);

	switch (instruction.opcode) {
		case ADD:
		case SUB:
		case MUL:
		case DIV:
			instruction.operand1 = registerToNumber(tokens[1]);
			instruction.operand2 = registerToNumber(tokens[2]);
			instruction.operand3 = registerToNumber(tokens[3]);
			if (instruction.operand1 == -1 || instruction.operand2 == -1 || instruction.operand3 == -1) {
				cout << "Error: Invalid register at line " << lineCount << endl;
				exit(EXIT_FAILURE);
			}
			newCell.instruction = instruction;
			newCell.scope = LOCAL;
			newCell.symbolName = "";
			newCell.targetOperand = -1;
			actualProgram.memory[lineCount] = newCell;
			break;
		case JLT:
		case JGT:
		case JEQ:
			instruction.operand1 = registerToNumber(tokens[1]);
			instruction.operand2 = registerToNumber(tokens[2]);
			if (instruction.operand1 == -1 || instruction.operand2 == -1) {
				cout << "Error: Invalid register '" << tokens[1] << "' at line " << lineCount << endl;
				exit(EXIT_FAILURE);
			}
			instruction.operand3 = -1;
			newCell.instruction = instruction;
			newCell.scope = LOCAL;
			newCell.symbolName = tokens[3];
			newCell.targetOperand = 3;
			actualProgram.memory[lineCount] = newCell;
			break;
		default:
			cout << "Error: Invalid instruction at line " << lineCount << endl;
			exit(EXIT_FAILURE);
	}
}

void assemblerSecondPass(vector<AssembledProgram> &programs, ifstream &file, int programIndex, int initOfProgram) {
	string line;
	int lineCount = 0;
	while (getline(file, line) && lineCount < MEMORY_SIZE) {
		string trimmedLine = trim(line);
		if (!trimmedLine.empty() && trimmedLine[0] != '#') {
			if (trimmedLine.back() == ':') {
				continue;
			}
			lineCount++;
			if (lineCount <= initOfProgram) {
				continue;
			}
			vector<string> tokens;
			string word;
			istringstream iss(trimmedLine);
			while (iss >> word) {
				tokens.push_back(word);
			}
			int tokenCount = tokens.size();
			if (tokenCount == 0) {
				continue;
			}

			switch (tokenCount) {
				case 1:
					instructionOneTokenCase(programs, tokens, initOfProgram++, programIndex);
					break;
				case 2:
					instructionTwoTokenCase(programs, tokens, initOfProgram++, programIndex);
					break;
				case 3:
					instructionThreeTokenCase(programs, tokens, initOfProgram++, programIndex);
					break;
				case 4:
					instructionFourTokenCase(programs, tokens, initOfProgram++, programIndex);
					break;
				default:
					cout << "Error: Invalid number of tokens at line " << lineCount << endl;
					exit(EXIT_FAILURE);
			}
		}
	}
}

void assembleProgram(vector<AssembledProgram> &programs, const char *filename, int programIndex) {
	ifstream file(filename);
	if (!file.is_open()) {
		cout << "Error: Could not open file " << filename << endl;
		exit(EXIT_FAILURE);
	}
	int initOfProgram = 0;
	cout << "Assembling program: " << filename << endl;

	assemblerFirstPass(programs, file, programIndex, initOfProgram);
	// printSymbolTable(programs);
	restartFilePointer(file);
	assemblerSecondPass(programs, file, programIndex, initOfProgram);
	programs[programIndex].initOfProgram = initOfProgram;
	// printMemory(programs[programIndex]);

	file.close();
}