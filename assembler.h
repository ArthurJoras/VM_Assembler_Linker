#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "types.h"
#include <vector>

void assembleProgram(vector<AssembledProgram> &program, const char *filename, int programIndex);

#endif