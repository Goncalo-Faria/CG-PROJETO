#ifndef assembler_h
#define assembler_h

typedef struct assembler *Assembler;

#include "branch.h"
#include "../common/point.h"

Assembler mkAssembler(const char * filename);

Assembler mkAssembler(Assembler original);

void unmkAssembler(Assembler target);

void addDescendentAndGo(Assembler source, Branch descendent);

void assemblerPoint(Assembler ass, Point point);

long assemblerNumberOfPoints(Assembler ass);

Branch view(Assembler source);

void assemblerRotate(Assembler m, float angle, float vx, float vy, float vz);

void assemblerTranslate(Assembler m, float x, float y, float z);

void assemblerScale(Assembler m, float vx, float vy, float vz );

void assemblerDraw(Assembler reference);

void assemblerBufferData(Assembler reference);

void assemblerClear();

#endif