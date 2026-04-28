#include <stdio.h>
#include <stdlib.h>
#include "src/parser/parser.h"
#include "src/assembler/assembler.h"
#include "src/neanderImplementer/neanderImplementer.h"

int main() {
    const char *source =
        "5 + 1 + 5*2 * (4 + 2 -(3-2))";

    printf("=== CODIGO FONTE ===\n");
    printf("%s\n", source);

    Parser parser;
    parser_init(&parser, source);
    parser_parse(&parser);

    if (!assembly_write_file(&parser.generator, "saida.asm")) {
        printf("Erro ao criar saida.asm\n");
        return 1;
    }

    printf("\nsaida.asm gerado com sucesso.\n");

    Assembler assembler;

    if (!assembler_assemble_file(&assembler, "saida.asm")) {
        printf("Erro ao montar saida.asm\n");
        return 1;
    }

    if (!assembler_write_mem_file(&assembler, "saida.mem")) {
        printf("Erro ao criar saida.mem\n");
        return 1;
    }

    printf("saida.mem gerado com sucesso.\n");

    printf("\n=== TABELA DE SIMBOLOS ===\n");
    assembler_print_symbols(&assembler);

    NeanderImplementer cpu;
    neander_init(&cpu);

    neander_load_memory(&cpu, assembler.memory, NEANDER_MEMORY_SIZE);

    printf("\n=== ESTADO INICIAL DO NEANDER ===\n");
    neander_print_state(&cpu);

    printf("\n=== EXECUTANDO NEANDER ===\n");
    neander_run(&cpu, 10000, 0);

    printf("\n=== ESTADO FINAL DO NEANDER ===\n");
    neander_print_state(&cpu);

    printf("\n=== MEMORIA RELEVANTE ===\n");
    neander_print_memory(&cpu, 0, assembler.used + 10);

    printf("\nResultado final no AC: %d decimal / 0x%02X hexadecimal\n", cpu.AC, cpu.AC);

    return 0;
}