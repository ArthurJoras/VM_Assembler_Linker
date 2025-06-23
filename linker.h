#ifndef LINKER_H
#define LINKER_H

#include <vector>

#include "types.h"

void initLinker(vector<AssembledProgram> &programs, LinkedProgram &linkedProgram, char *argv[], int totalPrograms);

#endif