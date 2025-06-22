#include <iostream>
#include "assembler.h"

using namespace std;

void linkerFirstPass() {

}

void linkerSecondPass() {

}

void initLinker(char *argv[], int totalPrograms) {
    int i;
    for (i = 0; i < totalPrograms; i++) {
        cout << "Processing program: " << argv[i + 1] << endl;
        // assembleProgram(argv[i + 1]);
    }
}