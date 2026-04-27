#include <stdio.h>
#include <string.h>
#include "assemblyGenerator.h"

static Operand make_operand(const char *name) {
    Operand op;
    snprintf(op.name, MAX_NAME_SIZE, "%s", name);
    return op;
}

int main() {
    AssemblyGenerator gen;
    assembly_init(&gen);

    /*
        Vamos simular:

        var A = 2
        var B = 3
        var C = 4
        var D = A + B * C

        Primeiro declaramos os dados manualmente.
    */

    Operand A = make_operand("A");
    Operand B = make_operand("B");
    Operand C = make_operand("C");
    Operand D = make_operand("D");

    assembly_emit_data(&gen, "A DATA 2");
    assembly_emit_data(&gen, "B DATA 3");
    assembly_emit_data(&gen, "C DATA 4");
    assembly_emit_data(&gen, "D DATA 0");

    /*
        Agora geramos:

        _T0 = B * C
        D = A + _T0
    */

    Operand T0 = assembly_new_temp(&gen, NULL);

    assembly_emit_mul(&gen, B, C, T0);
    assembly_emit_add(&gen, A, T0, D);

    if (!assembly_write_file(&gen, "saida.asm")) {
        printf("Erro ao criar o arquivo saida.asm\n");
        return 1;
    }

    printf("Arquivo saida.asm gerado com sucesso.\n");

    return 0;
}