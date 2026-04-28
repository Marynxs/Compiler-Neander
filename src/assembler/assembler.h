#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdint.h>

#define NEANDER_MEMORY_SIZE 256
#define MAX_SYMBOLS 256
#define MAX_SYMBOL_NAME 64

typedef struct {
    char name[MAX_SYMBOL_NAME];
    int address;
} Symbol;

typedef struct {
    Symbol symbols[MAX_SYMBOLS];
    int symbol_count;

    uint8_t memory[NEANDER_MEMORY_SIZE];
    int used;
} Assembler;

void assembler_init(Assembler *assembler);

int assembler_assemble_file(Assembler *assembler, const char *asm_filename);

int assembler_write_mem_file(Assembler *assembler, const char *mem_filename);

void assembler_print_symbols(Assembler *assembler);

#endif