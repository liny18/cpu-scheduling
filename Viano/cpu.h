#ifndef __CPU__
#define __CPU__

#include "process.h"

typedef enum {
  OCCUPIED, NONE
} CPUState;

typedef struct CPU {
  CPUState state;
  Process* p;
} CPU;

// Forward Declaration
void initializeCPU(CPU* cpu);

// INITIALIZE THE CPU
void initializeCPU(CPU* cpu){
    cpu->state = NONE;
    cpu->p = NULL;
}

#endif