#ifndef TYPES_H
#define TYPES_H
#include <string>
#include <vector>

using namespace std;

#define MEMORY_SIZE 320
#define MAX_SYMBOLS 100
#define REGISTERS_COUNT 4

enum SymbolType {
    VARIABLE,
    LABEL
};

enum ScopeType {
    GLOBAL,
    LOCAL
};

typedef struct {
    string name;
    int memoryAddress;
    SymbolType type;
    ScopeType scope;
} SymbolTableRow;

typedef struct {
    int opcode;
    int operand1;
    int operand2;
    int operand3;
} Instruction;

typedef struct {
    ScopeType scope;

    union {
        int value;
        Instruction instruction;
    };
} MemoryCell;

typedef struct {
    vector<MemoryCell> memory;
    vector<SymbolTableRow> symbolTable;
} AssembledProgram;

enum Intructions {
  NOP = -1,
  ADD = 0,
  SUB,
  MUL,
  DIV,
  MV,
  ST,
  JMP,
  JEQ,
  JGT,
  JLT,
  W,
  R,
  STP
};


#endif
