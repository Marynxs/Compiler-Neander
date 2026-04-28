#ifndef ASSEMBLYGENERATOR_H
#define ASSEMBLYGENERATOR_H

#define MAX_ASM_LINES 512
#define MAX_ASM_LINE_SIZE 128
#define MAX_NAME_SIZE 64


//  Operand representa as Variaveis | Constantes | Temporarios
//  Ex: A, B, _T0, _C0
typedef struct {
    char name[MAX_NAME_SIZE];
} Operand;

typedef struct {
    char code[MAX_ASM_LINES][MAX_ASM_LINE_SIZE];
    int code_count;

    char data[MAX_ASM_LINES][MAX_ASM_LINE_SIZE];
    int data_count;

    int temp_count;
    int label_count;
    int const_count;
} AssemblyGenerator;

void assembly_init(AssemblyGenerator *gen);

void assembly_emit_code(AssemblyGenerator *gen, const char *format, ...);
void assembly_emit_data(AssemblyGenerator *gen, const char *format, ...);

// Cria os Operand de temporarios e constantes.
Operand assembly_new_temp(AssemblyGenerator *gen, const char *optional_name);
Operand assembly_new_const(AssemblyGenerator *gen, int value, const char *optional_name);

void assembly_emit_add(AssemblyGenerator *gen, Operand left, Operand right, Operand result);
void assembly_emit_minus(AssemblyGenerator *gen, Operand left, Operand right, Operand result);
void assembly_emit_mul(AssemblyGenerator *gen, Operand left, Operand right, Operand result);


int assembly_write_file(AssemblyGenerator *gen, const char *filename);

#endif