#include <iostream>
#include <string>
#include <vector>

#include "types.h"
#include "utils.h"

using namespace std;

void aluHandle(int registers[], int opcode, int operand1, int operand2, int operand3) {
	switch (opcode) {
		case ADD:
			registers[operand1] = registers[operand2] + registers[operand3];
			break;
		case SUB:
			registers[operand1] = registers[operand2] - registers[operand3];
			break;
		case MUL:
			registers[operand1] = registers[operand2] * registers[operand3];
			break;
		case DIV:
			if (registers[operand3] != 0) {
				registers[operand1] = registers[operand2] / registers[operand3];
			} else {
				cout << "Error: Division by zero in instruction at PC = " << operand1 << endl;
				exit(EXIT_FAILURE);
			}
			break;
		default:
			cout << "Error: Invalid ALU operation with opcode " << opcode << endl;
			exit(EXIT_FAILURE);
	}
}

void handleJump(int &pc, int registers[], int opcode, int operand1, int operand2, int operand3) {
	switch (opcode) {
		case JMP:
			pc = operand1;
			pc--;
			pc--;
			break;
		case JEQ:
			if (registers[operand1] == registers[operand2]) {
				pc = operand3;
				pc--;
				pc--;
			}
			break;
		case JGT:
			if (registers[operand1] > registers[operand2]) {
				pc = operand3;
				pc--;
				pc--;
			}
			break;
		case JLT:
			if (registers[operand1] < registers[operand2]) {
				pc = operand3;
				pc--;
				pc--;
			}
			break;
		default:
			cout << "Error: Invalid jump instruction with opcode " << opcode << endl;
			exit(EXIT_FAILURE);
	}
}

void runVirtualMachine(LinkedProgram &linkedProgram) {
	int pc = 0;
	int registers[REGISTERS_COUNT] = {0};
	bool stopInstructionReached = false;

	while (pc <= static_cast<int>(linkedProgram.memory.size()) && pc < MEMORY_SIZE && !stopInstructionReached) {
		const MemoryCell &cell = linkedProgram.memory[pc];

		if (cell.scope != NONE) {
			pc++;
			continue;
		}

		// cout << "PC = " << pc << ": ";
		// printInstruction(cell.instruction);

		switch (cell.instruction.opcode) {
			case ADD:
			case SUB:
			case MUL:
			case DIV:
				aluHandle(registers, cell.instruction.opcode, cell.instruction.operand1, cell.instruction.operand2, cell.instruction.operand3);
				break;
			case MV:
				if (cell.instruction.operand1 < REGISTERS_COUNT) {
					registers[cell.instruction.operand1] = linkedProgram.memory[cell.instruction.operand2].value;
				} else {
					cout << "Invalid register index: " << cell.instruction.operand1 << " in instruction at PC = " << pc << endl;
					exit(EXIT_FAILURE);
				}
				break;
			case ST:
				if (cell.instruction.operand1 < REGISTERS_COUNT) {
					linkedProgram.memory[cell.instruction.operand2].value = registers[cell.instruction.operand1];
				} else {
					cout << "Invalid register index: " << cell.instruction.operand1 << " in instruction at PC = " << pc << endl;
					exit(EXIT_FAILURE);
				}
				break;
			case JMP:
			case JEQ:
			case JGT:
			case JLT:
				handleJump(pc, registers, cell.instruction.opcode, cell.instruction.operand1, cell.instruction.operand2, cell.instruction.operand3);
				break;
			case W:
				if (cell.instruction.operand1 < REGISTERS_COUNT) {
					int valueToWrite;
					cout << "Enter value to write in register " << cell.instruction.operand1 << ": ";
					cin >> valueToWrite;
					registers[cell.instruction.operand1] = valueToWrite;
				} else {
					cout << "Invalid register index: " << cell.instruction.operand1 << " in instruction at PC = " << pc << endl;
					exit(EXIT_FAILURE);
				}
				break;
			case R:
				if (cell.instruction.operand1 < REGISTERS_COUNT) {
					cout << "Register " << cell.instruction.operand1 << " value: " << registers[cell.instruction.operand1] << endl;
				} else {
					cout << "Invalid register index: " << cell.instruction.operand1 << " in instruction at PC = " << pc << endl;
					exit(EXIT_FAILURE);
				}
				break;
			case STP:
				stopInstructionReached = true;
				break;
		}

		pc++;
	}

	if (pc == MEMORY_SIZE) {
		cout << "Memory limit reached. Program terminated." << endl;
	}

	if (stopInstructionReached) {
		cout << "Program terminated by STP instruction at PC = " << pc - 1 << endl;
	}

	printRegisters(registers, REGISTERS_COUNT);
	// printLinkedSymbolTable(linkedProgram);
	// printFinalLinkedMemory(linkedProgram);
}