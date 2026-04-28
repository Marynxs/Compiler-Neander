#ifndef NEANDER_IMPLEMENTER_H
#define NEANDER_IMPLEMENTER_H

#include <stdint.h>

#define NEANDER_MEMORY_SIZE 256

typedef struct {
    uint8_t memory[NEANDER_MEMORY_SIZE];

    uint8_t AC;   // Acumulador
    uint8_t PC;   // Program Counter
    uint8_t IR;   // Instruction Register
    uint8_t MAR;  // Memory Address Register
    uint8_t MDR;  // Memory Data Register

    uint8_t flag_N; // negativo
    uint8_t flag_Z; // zero

    int halted;
    unsigned long steps;
} NeanderImplementer;

void neander_init(NeanderImplementer *cpu);
void neander_reset_registers(NeanderImplementer *cpu);

int neander_load_mem_file(NeanderImplementer *cpu, const char *filename);
void neander_load_memory(NeanderImplementer *cpu, const uint8_t *memory, int size);

int neander_step(NeanderImplementer *cpu);
void neander_run(NeanderImplementer *cpu, int max_steps, int verbose);

void neander_print_state(const NeanderImplementer *cpu);
void neander_print_memory(const NeanderImplementer *cpu, int start, int count);

#endif