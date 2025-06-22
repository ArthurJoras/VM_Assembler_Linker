
#include "linker.h"
#include <iostream>
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
    initLinker(argv, totalPrograms);
}