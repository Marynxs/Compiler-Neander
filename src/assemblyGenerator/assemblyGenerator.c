#include "assemblyGenerator.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void assembly_init(AssemblyGenerator *gen) {
    gen->code_count = 0;
    gen->data_count = 0;
    gen->temp_count = 0;
    gen->label_count = 0;
    gen->const_count = 0;
}


/*
Exemplo de CODE

LDA [A]
ADD [B]
STA [R]
HLT

*/

void assembly_emit_code(AssemblyGenerator *gen, const char *format, ...) {
    va_list args; // guarda os argumentos extras depois de format (...)
    va_start(args, format); // le os argumentos depois de format

    vsnprintf(gen->code[gen->code_count], MAX_ASM_LINE_SIZE, format, args); // essa função cria uma string do tamanho maximo dado e salva dentro do primeiro argumento que é uma string ele é usado quando os argumentos estão dentro de uma va_list.

    gen->code_count++;

    va_end(args); // se usa o start tem que finzalizar e limpar a lista de argumentos extra.
}


/*
Exemplo de DATA

A DATA 5
B DATA 8
R DATA 0

*/
void assembly_emit_data(AssemblyGenerator *gen, const char *format, ...) {
    va_list args;
    va_start(args, format);

    vsnprintf(gen->data[gen->data_count], MAX_ASM_LINE_SIZE, format, args);

    gen->data_count++;

    va_end(args);
}

/*

Var A = 5 + 2 + 3

Ele vai criar um assembly onde


_C0 = 5
_C1 = 2
_C2 = 3
_T0 = _C0 + _C1
_T1 = _T0 + _C2
A = _T1

Os temporarios vao ser usados para guardar as contas intermediarias


*/

Operand assembly_new_temp(AssemblyGenerator *gen, const char *name) {
    Operand op;

    if (name != NULL) {
        snprintf(op.name, MAX_NAME_SIZE, "_%s_%d", name, gen->temp_count);
    } else {
        snprintf(op.name, MAX_NAME_SIZE, "_T%d", gen->temp_count);
    }

    gen->temp_count++;

    assembly_emit_data(gen, "%s DATA 0", op.name);

    return op;
}

/* 

Var A = 5 * 2

Ele vai criar um Assembly onde 

_C0 = 5
_C1 = 2 

As constantes vao guardar valores que nao tem variavel.

*/
Operand assembly_new_const(AssemblyGenerator *gen, int value, const char *name){
    Operand op;

    if (name != NULL) {
        snprintf(op.name, MAX_NAME_SIZE, "_%s_%d", name, gen->const_count);
    } else {
        snprintf(op.name, MAX_NAME_SIZE, "_C%d", gen->const_count);
    }

    gen->const_count++;

    assembly_emit_data(gen, "%s DATA %d", op.name, value);

    return op;
}

/*
Aqui ensina pro Assembly a saida de como é a soma

Exemplo:
LDA [A]
ADD [B]
STA [R]


*/
void assembly_emit_add(AssemblyGenerator *gen, Operand left, Operand right, Operand result) {
    assembly_emit_code(gen, "LDA %s", left.name);
    assembly_emit_code(gen, "ADD %s", right.name);
    assembly_emit_code(gen, "STA %s", result.name);
}

void assembly_emit_minus(AssemblyGenerator *gen, Operand left, Operand right, Operand result) {

    assembly_emit_code(gen, "LDA %s", right.name);
    assembly_emit_code(gen, "NOT");
    assembly_emit_code(gen, "ADD ONE");
    assembly_emit_code(gen, "STA %s", result.name);


    assembly_emit_code(gen, "LDA %s", left.name);
    assembly_emit_code(gen, "ADD %s", result.name);
    assembly_emit_code(gen, "STA %s", result.name);

}



/*
Aqui ensina pro Assembly a saida de como é a multiplicação

*/
void assembly_emit_mul(AssemblyGenerator *gen, Operand left, Operand right, Operand result) {
    int id = gen->label_count;
    gen->label_count++;

    char loop_label[MAX_NAME_SIZE];
    char end_label[MAX_NAME_SIZE];

    snprintf(loop_label, MAX_NAME_SIZE, "_MULT_LOOP%d", id);
    snprintf(end_label, MAX_NAME_SIZE, "_MULT_END%d", id);

    char multiplicand[MAX_NAME_SIZE];
    char multiplier[MAX_NAME_SIZE];

    //assembly_emit_code(gen, "\n\n   MULTIPLICATION Nº%d \n\n", id + 1);

    snprintf(multiplicand, MAX_NAME_SIZE, "_MULTIPLICAND_%d", id); //multiplicando
    snprintf(multiplier, MAX_NAME_SIZE, "_MULTIPLIER_%d", id); //multiplicador (o que soma x vezes)

    assembly_emit_data(gen, "%s DATA 0", multiplicand);
    assembly_emit_data(gen, "%s DATA 0", multiplier);
  

    assembly_emit_code(gen, "LDA %s", left.name);
    assembly_emit_code(gen, "STA %s", multiplicand);

    assembly_emit_code(gen, "LDA %s", right.name);
    assembly_emit_code(gen, "STA %s", multiplier);

    assembly_emit_code(gen, "LDA ZERO");
    assembly_emit_code(gen, "STA %s", result.name);

    assembly_emit_code(gen, "%s:", loop_label);

    assembly_emit_code(gen, "LDA %s", multiplier);
    assembly_emit_code(gen, "JZ %s", end_label);

    assembly_emit_code(gen, "LDA %s", result.name);
    assembly_emit_code(gen, "ADD %s", multiplicand);
    assembly_emit_code(gen, "STA %s", result.name);
    
    assembly_emit_code(gen, "LDA %s", multiplier);
    assembly_emit_code(gen, "ADD MINUS_1");
    assembly_emit_code(gen, "STA %s", multiplier);

    assembly_emit_code(gen, "JMP %s", loop_label);

    assembly_emit_code(gen, "%s:", end_label);


}

int assembly_write_file(AssemblyGenerator *gen, const char *filename) {
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        return 0;
    }


    fprintf(file, ".CODE\n\n");
    fprintf(file, "     ORG 0\n\n");

    for (int i = 0; i < gen->code_count; i++) {
        fprintf(file, "     %s\n", gen->code[i]);
    }

    fprintf(file, "     HLT\n\n");

    fprintf(file, ".DATA\n\n");


    fprintf(file, "     ZERO DATA 0\n");
    fprintf(file, "     MINUS_1 DATA 255\n");
    fprintf(file, "     ONE DATA 1\n");

    for (int i = 0; i < gen->data_count; i++) {
        fprintf(file, "     %s\n", gen->data[i]);
    }

    fclose(file);

    return 1;
}