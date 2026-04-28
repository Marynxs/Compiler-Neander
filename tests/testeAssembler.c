#include <stdio.h>
#include "../src/assembler/assembler.h"

int main() {
    Assembler assembler;

    if (!assembler_assemble_file(&assembler, "saida.asm")) {
        printf("Erro ao montar arquivo assembly.\n");
        return 1;
    }

    assembler_print_symbols(&assembler);

    if (!assembler_write_mem_file(&assembler, "saida.mem")) {
        printf("Erro ao criar saida.mem\n");
        return 1;
    }

    printf("Arquivo saida.mem gerado com sucesso.\n");

    return 0;
}