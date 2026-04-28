#include "neanderImplementer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OPCODE_NOP 0x00
#define OPCODE_STA 0x10
#define OPCODE_LDA 0x20
#define OPCODE_ADD 0x30
#define OPCODE_OR  0x50
#define OPCODE_AND 0x60
#define OPCODE_NOT 0x70
#define OPCODE_JMP 0x80
#define OPCODE_JN  0x90
#define OPCODE_JZ  0xA0
#define OPCODE_HLT 0xF0

static void update_flags(NeanderImplementer *cpu) {
    cpu->flag_Z = (cpu->AC == 0);
    cpu->flag_N = ((cpu->AC & 0x80) != 0);
}

void neander_init(NeanderImplementer *cpu) {
    memset(cpu->memory, 0, NEANDER_MEMORY_SIZE);
    neander_reset_registers(cpu);
}

void neander_reset_registers(NeanderImplementer *cpu) {
    cpu->AC = 0;
    cpu->PC = 0;
    cpu->IR = 0;
    cpu->MAR = 0;
    cpu->MDR = 0;

    cpu->flag_N = 0;
    cpu->flag_Z = 1;

    cpu->halted = 0;
    cpu->steps = 0;
}

void neander_load_memory(NeanderImplementer *cpu, const uint8_t *memory, int size) {
    memset(cpu->memory, 0, NEANDER_MEMORY_SIZE);

    if (size > NEANDER_MEMORY_SIZE) {
        size = NEANDER_MEMORY_SIZE;
    }

    for (int i = 0; i < size; i++) {
        cpu->memory[i] = memory[i];
    }

    neander_reset_registers(cpu);
}

int neander_load_mem_file(NeanderImplementer *cpu, const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        return 0;
    }

    memset(cpu->memory, 0, NEANDER_MEMORY_SIZE);

    int value;
    int address = 0;

    while (address < NEANDER_MEMORY_SIZE && fscanf(file, "%x", &value) == 1) {
        cpu->memory[address] = (uint8_t)(value & 0xFF);
        address++;
    }

    fclose(file);

    neander_reset_registers(cpu);

    return 1;
}

static uint8_t fetch_byte(NeanderImplementer *cpu) {
    cpu->MAR = cpu->PC;
    cpu->MDR = cpu->memory[cpu->MAR];
    cpu->PC++;

    return cpu->MDR;
}

static uint8_t read_memory(NeanderImplementer *cpu, uint8_t address) {
    cpu->MAR = address;
    cpu->MDR = cpu->memory[address];

    return cpu->MDR;
}

static void write_memory(NeanderImplementer *cpu, uint8_t address, uint8_t value) {
    cpu->MAR = address;
    cpu->MDR = value;
    cpu->memory[address] = cpu->MDR;
}

int neander_step(NeanderImplementer *cpu) {
    if (cpu->halted) {
        return 0;
    }

    // FETCH
    cpu->IR = fetch_byte(cpu);

    // DECODE + EXECUTE
    switch (cpu->IR) {
        case OPCODE_NOP:
            break;

        case OPCODE_STA: {
            uint8_t address = fetch_byte(cpu);
            write_memory(cpu, address, cpu->AC);
            break;
        }

        case OPCODE_LDA: {
            uint8_t address = fetch_byte(cpu);
            cpu->AC = read_memory(cpu, address);
            update_flags(cpu);
            break;
        }

        case OPCODE_ADD: {
            uint8_t address = fetch_byte(cpu);
            uint8_t value = read_memory(cpu, address);

            cpu->AC = (uint8_t)(cpu->AC + value);
            update_flags(cpu);
            break;
        }

        case OPCODE_OR: {
            uint8_t address = fetch_byte(cpu);
            uint8_t value = read_memory(cpu, address);

            cpu->AC = (uint8_t)(cpu->AC | value);
            update_flags(cpu);
            break;
        }

        case OPCODE_AND: {
            uint8_t address = fetch_byte(cpu);
            uint8_t value = read_memory(cpu, address);

            cpu->AC = (uint8_t)(cpu->AC & value);
            update_flags(cpu);
            break;
        }

        case OPCODE_NOT:
            cpu->AC = (uint8_t)(~cpu->AC);
            update_flags(cpu);
            break;

        case OPCODE_JMP: {
            uint8_t address = fetch_byte(cpu);
            cpu->PC = address;
            break;
        }

        case OPCODE_JN: {
            uint8_t address = fetch_byte(cpu);

            if (cpu->flag_N) {
                cpu->PC = address;
            }

            break;
        }

        case OPCODE_JZ: {
            uint8_t address = fetch_byte(cpu);

            if (cpu->flag_Z) {
                cpu->PC = address;
            }

            break;
        }

        case OPCODE_HLT:
            cpu->halted = 1;
            break;

        default:
            printf("Erro: opcode invalido 0x%02X no endereco 0x%02X\n",
                   cpu->IR,
                   (uint8_t)(cpu->PC - 1));
            cpu->halted = 1;
            return -1;
    }

    cpu->steps++;

    return 1;
}

void neander_run(NeanderImplementer *cpu, int max_steps, int verbose) {
    int result;

    if (max_steps <= 0) {
        max_steps = 10000;
    }

    while (!cpu->halted && cpu->steps < (unsigned long)max_steps) {
        result = neander_step(cpu);

        if (verbose) {
            neander_print_state(cpu);
        }

        if (result == -1) {
            break;
        }
    }

    if (!cpu->halted && cpu->steps >= (unsigned long)max_steps) {
        printf("Execucao interrompida: limite de passos atingido (%d).\n", max_steps);
    }
}

void neander_print_state(const NeanderImplementer *cpu) {
    printf("STEP: %lu | ", cpu->steps);
    printf("AC: %3u (0x%02X) | ", cpu->AC, cpu->AC);
    printf("PC: %3u (0x%02X) | ", cpu->PC, cpu->PC);
    printf("IR: 0x%02X | ", cpu->IR);
    printf("MAR: %3u (0x%02X) | ", cpu->MAR, cpu->MAR);
    printf("MDR: %3u (0x%02X) | ", cpu->MDR, cpu->MDR);
    printf("N: %u | Z: %u | ", cpu->flag_N, cpu->flag_Z);
    printf("HALT: %d\n", cpu->halted);
}

void neander_print_memory(const NeanderImplementer *cpu, int start, int count) {
    if (start < 0) {
        start = 0;
    }

    if (start >= NEANDER_MEMORY_SIZE) {
        return;
    }

    if (start + count > NEANDER_MEMORY_SIZE) {
        count = NEANDER_MEMORY_SIZE - start;
    }

    printf("Memoria [%d..%d]:\n", start, start + count - 1);

    for (int i = 0; i < count; i++) {
        int address = start + i;

        printf("%03d (0x%02X): %03u (0x%02X)\n",
               address,
               address,
               cpu->memory[address],
               cpu->memory[address]);
    }
}