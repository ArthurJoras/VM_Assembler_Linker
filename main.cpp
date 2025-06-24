
#include <iostream>
#include <string>
#include <vector>

#include "assembler.h"
#include "linker.h"
#include "types.h"
#include "utils.h"
#include "virtual_machine.h"

using namespace std;

int getProgramsQuantity(int argc) {
	int total = argc - 1;
	if (total < 1) {
		cout << "No programs specified." << endl;
		exit(EXIT_FAILURE);
	}
	return total;
}

int main(int argc, char *argv[]) {
	const int totalPrograms = getProgramsQuantity(argc);

	vector<AssembledProgram> programs(totalPrograms);

	for (int i = 0; i < totalPrograms; i++) {
		programs[i].memory.clear();
		programs[i].symbolTable.clear();

		assembleProgram(programs, argv[i + 1], i);
	}

	LinkedProgram linkedProgram;

	initLinker(programs, linkedProgram, argv, totalPrograms);

	// printFinalLinkedMemory(linkedProgram);

	runVirtualMachine(linkedProgram);

	cout << "Virtual machine execution completed successfully." << endl;
}