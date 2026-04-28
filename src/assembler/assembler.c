#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_SIZE 256
#define MAX_TOKENS 4

typedef struct {
    int has_label;
    char label[MAX_SYMBOL_NAME];

    int has_op;
    char op[MAX_SYMBOL_NAME];

    int has_operand;
    char operand[MAX_SYMBOL_NAME];
} ParsedLine; //Pega cada uma das linhas e verifica se tem label | operações | operantes e anota quais são

void assembler_init(Assembler *assembler) {
    assembler->symbol_count = 0;
    assembler->used = 0;

    for (int i = 0; i < NEANDER_MEMORY_SIZE; i++) {
        assembler->memory[i] = 0;
    }
}

static void assembler_error(const char *message, int line_number) {
    printf("Erro no assembler na linha %d: %s\n", line_number, message);
    exit(1);
}

/*

Tira espaços

*/
static void trim(char *str) { 
    int start = 0;
    while (isspace((unsigned char)str[start])) {
        start++;
    }

    int end = strlen(str) - 1;
    while (end >= start && isspace((unsigned char)str[end])) {
        str[end] = '\0';
        end--;
    }

    if (start > 0) {
        memmove(str, str + start, strlen(str + start) + 1);
    }
}

/*

Tira comentarios

*/

static void remove_comment(char *line) {
    char *comment = strchr(line, ';');

    if (comment != NULL) {
        *comment = '\0';
    }
}

/*

Tira linhas vazias

*/

static int is_empty_line(const char *line) {
    return line[0] == '\0';
}

/*

Ignora as marações de .CODE e .DATA

*/

static int is_section_marker(const char *op) {
    return strcmp(op, ".CODE") == 0 || strcmp(op, ".DATA") == 0;
}

/*

Verifica se é o uma DATA com SPACE ou DATA 
Faz isso para poder reparar se o que ele ta lendo é um SPACE ou DATA por que essa informação so fica no TOKEN 2 não no primeiro como uma instrução
exemplo:
    Data:
        A DATA 2
    Instrução:
        LDA A

*/

static int is_data_directive(const char *token) {
    return strcmp(token, "DATA") == 0 || strcmp(token, "SPACE") == 0;
}

/*

Verifica se são instruções implicitas
Faz isso para poder saber quantos bytes totais o programa serao usados pois as instruções implicitas usam 1 byte enquanto outras instruções usam 2

*/


static int is_implicit_instruction(const char *op) {
    return strcmp(op, "NOP") == 0 ||
           strcmp(op, "NOT") == 0 ||
           strcmp(op, "HLT") == 0;
}

/*

Verifica se são instruções explicitas

*/

static int is_direct_instruction(const char *op) {
    return strcmp(op, "STA") == 0 ||
           strcmp(op, "LDA") == 0 ||
           strcmp(op, "ADD") == 0 ||
           strcmp(op, "OR") == 0 ||
           strcmp(op, "AND") == 0 ||
           strcmp(op, "JMP") == 0 ||
           strcmp(op, "JN") == 0 ||
           strcmp(op, "JZ") == 0;
}


static int opcode_for(const char *op) {
    if (strcmp(op, "NOP") == 0) return 0x00;
    if (strcmp(op, "STA") == 0) return 0x10;
    if (strcmp(op, "LDA") == 0) return 0x20;
    if (strcmp(op, "ADD") == 0) return 0x30;
    if (strcmp(op, "OR") == 0) return 0x50;
    if (strcmp(op, "AND") == 0) return 0x60;
    if (strcmp(op, "NOT") == 0) return 0x70;
    if (strcmp(op, "JMP") == 0) return 0x80;
    if (strcmp(op, "JN") == 0) return 0x90;
    if (strcmp(op, "JZ") == 0) return 0xA0;
    if (strcmp(op, "HLT") == 0) return 0xF0;

    return -1;
}


/*

Passa por toda a string e verifica se ela é um numero

*/

static int is_number(const char *text) {
    if (text == NULL || text[0] == '\0') {
        return 0;
    }

    char *endptr;
    strtol(text, &endptr, 0);

    return *endptr == '\0';
}

/*

Mesma coisa mas vai tentar tranformar de texto pra string

*/

static int parse_number(const char *text) {
    return (int)strtol(text, NULL, 0);
}

/*

Procura se existe aquele simbolo na tabela de simbolos
Simbolos são nome das variaveis

*/

static int find_symbol(Assembler *assembler, const char *name) {
    for (int i = 0; i < assembler->symbol_count; i++) {
        if (strcmp(assembler->symbols[i].name, name) == 0) {
            return assembler->symbols[i].address;
        }
    }

    return -1;
}

static void add_symbol(Assembler *assembler, const char *name, int address, int line_number) {
    if (find_symbol(assembler, name) != -1) {
        assembler_error("rotulo duplicado", line_number);
    }

    if (assembler->symbol_count >= MAX_SYMBOLS) {
        assembler_error("limite de simbolos excedido", line_number);
    }

    snprintf(
        assembler->symbols[assembler->symbol_count].name, MAX_SYMBOL_NAME, "%s", name ); // Aqui pode guardar por exemplo A DATA 2 -> Simbolo: A

    assembler->symbols[assembler->symbol_count].address = address; // Aqui a gente salva o endereço daquela variavel entao quando o assembler for procurar pela variavel ele sabe onde ta guardada ou onde guardar
    assembler->symbol_count++;
}

/*

Operand para o assembler é o texto que vem depois da instrução

*/

static int resolve_operand(Assembler *assembler, const char *operand, int line_number) {
    if (is_number(operand)) {
        int value = parse_number(operand);

        if (value < 0) {
            value = value & 0xFF;
        }

        return value & 0xFF;
    }

    int address = find_symbol(assembler, operand);

    if (address == -1) {
        assembler_error("simbolo nao definido", line_number);
    }

    return address;
}


static void check_memory_limit(int location_counter, int size, int line_number) {
    if (location_counter + size > NEANDER_MEMORY_SIZE) {
        assembler_error("programa excede a memoria da Neander", line_number);
    }
}

static ParsedLine parse_line(char *line) {
    ParsedLine parsed;

    parsed.has_label = 0;
    parsed.label[0] = '\0';

    parsed.has_op = 0;
    parsed.op[0] = '\0';

    parsed.has_operand = 0;
    parsed.operand[0] = '\0';

    remove_comment(line);
    trim(line);

    if (is_empty_line(line)) {
        return parsed;
    }

    char *tokens[MAX_TOKENS];
    int token_count = 0;

    char *token = strtok(line, " \t\r\n"); //Separa os tokens por espaço tabs e quebras de linha

    while (token != NULL && token_count < MAX_TOKENS) {
        tokens[token_count] = token;
        token_count++;
        token = strtok(NULL, " \t\r\n");
    }

    if (token_count == 0) {
        return parsed;
    }

    int len = strlen(tokens[0]);

    //Aqui descobre se é uma Label
    if (len > 0 && tokens[0][len - 1] == ':') {
        tokens[0][len - 1] = '\0';

        parsed.has_label = 1;
        snprintf(parsed.label, MAX_SYMBOL_NAME, "%s", tokens[0]);

        if (token_count >= 2) {
            parsed.has_op = 1;
            snprintf(parsed.op, MAX_SYMBOL_NAME, "%s", tokens[1]);
        }

        if (token_count >= 3) {
            parsed.has_operand = 1;
            snprintf(parsed.operand, MAX_SYMBOL_NAME, "%s", tokens[2]);
        }

        return parsed;
    }

    //Aqui descobre se é uma DATA
    if (token_count >= 2 && is_data_directive(tokens[1])) {
        parsed.has_label = 1;
        snprintf(parsed.label, MAX_SYMBOL_NAME, "%s", tokens[0]);

        parsed.has_op = 1;
        snprintf(parsed.op, MAX_SYMBOL_NAME, "%s", tokens[1]);

        if (token_count >= 3) {
            parsed.has_operand = 1;
            snprintf(parsed.operand, MAX_SYMBOL_NAME, "%s", tokens[2]);
        }

        return parsed;
    }

    //Aqui descobre se é um comando
    parsed.has_op = 1;
    snprintf(parsed.op, MAX_SYMBOL_NAME, "%s", tokens[0]);

    if (token_count >= 2) {
        parsed.has_operand = 1;
        snprintf(parsed.operand, MAX_SYMBOL_NAME, "%s", tokens[1]);
    }

    return parsed;
}

/*

Aqui ele sabe o quanto de byte de cada coisa e vai retornando esse valor

*/
static int instruction_size(ParsedLine *line, int line_number) {
    if (!line->has_op) {
        return 0;
    }

    if (is_section_marker(line->op)) {
        return 0;
    }

    if (strcmp(line->op, "ORG") == 0) {
        return 0;
    }

    if (strcmp(line->op, "DATA") == 0) {
        return 1;
    }

    if (strcmp(line->op, "SPACE") == 0) {
        if (!line->has_operand) {
            assembler_error("SPACE precisa de tamanho", line_number);
        }

        return parse_number(line->operand);
    }

    if (is_implicit_instruction(line->op)) {
        return 1;
    }

    if (is_direct_instruction(line->op)) {
        return 2;
    }

    assembler_error("instrucao ou diretiva desconhecida", line_number);
    return 0;
}



static void first_pass(Assembler *assembler, const char *asm_filename) {
    FILE *file = fopen(asm_filename, "r");

    if (file == NULL) {
        printf("Erro ao abrir arquivo assembly: %s\n", asm_filename);
        exit(1);
    }

    char line[MAX_LINE_SIZE];
    int line_number = 1;
    int location_counter = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        char copy[MAX_LINE_SIZE];
        snprintf(copy, MAX_LINE_SIZE, "%s", line);

        ParsedLine parsed = parse_line(copy);

        if (parsed.has_op && is_section_marker(parsed.op)) { // Aqui ele faz se existe uma palavra principal e essa palavra é um marker ignora
            line_number++;
            continue;
        }

        if (parsed.has_op && strcmp(parsed.op, "ORG") == 0) { // Aqui ele faz se existe uma palavra principal e essa palavra é um ORG pega o numero que vem depois e guarda.
            if (!parsed.has_operand) {
                assembler_error("ORG precisa de endereco", line_number);
            }

            location_counter = parse_number(parsed.operand);
            line_number++;
            continue;
        }

        if (parsed.has_label) {
            add_symbol(assembler, parsed.label, location_counter, line_number);
        }

        int size = instruction_size(&parsed, line_number);
        check_memory_limit(location_counter, size, line_number);

        location_counter += size;

        if (location_counter > assembler->used) { // assembler->used = próximo endereço depois da última posição usada
            assembler->used = location_counter;
        }

        line_number++;
    }

    fclose(file);
}


static void second_pass(Assembler *assembler, const char *asm_filename) {
    FILE *file = fopen(asm_filename, "r");

    if (file == NULL) {
        printf("Erro ao abrir arquivo assembly: %s\n", asm_filename);
        exit(1);
    }

    char line[MAX_LINE_SIZE];
    int line_number = 1;
    int location_counter = 0;

    while (fgets(line, sizeof(line), file) != NULL) {
        char copy[MAX_LINE_SIZE];
        snprintf(copy, MAX_LINE_SIZE, "%s", line);

        ParsedLine parsed = parse_line(copy);

        if (!parsed.has_op) {
            line_number++;
            continue;
        }

        if (is_section_marker(parsed.op)) {
            line_number++;
            continue;
        }

        if (strcmp(parsed.op, "ORG") == 0) {
            location_counter = parse_number(parsed.operand);
            line_number++;
            continue;
        }

        if (strcmp(parsed.op, "DATA") == 0) {
            if (!parsed.has_operand) {
                assembler_error("DATA precisa de valor", line_number);
            }

            int value = parse_number(parsed.operand);
            assembler->memory[location_counter] = value & 0xFF;
            location_counter++;
            line_number++;
            continue;
        }

        if (strcmp(parsed.op, "SPACE") == 0) {
            if (!parsed.has_operand) {
                assembler_error("SPACE precisa de tamanho", line_number);
            }

            int size = parse_number(parsed.operand);

            for (int i = 0; i < size; i++) {
                assembler->memory[location_counter] = 0;
                location_counter++;
            }

            line_number++;
            continue;
        }

        int opcode = opcode_for(parsed.op);

        if (opcode == -1) {
            assembler_error("opcode desconhecido", line_number);
        }

        if (is_implicit_instruction(parsed.op)) {
            assembler->memory[location_counter] = opcode & 0xFF;
            location_counter++;
            line_number++;
            continue;
        }

        if (is_direct_instruction(parsed.op)) {
            if (!parsed.has_operand) {
                assembler_error("instrucao direta precisa de operando", line_number);
            }

            int address = resolve_operand(assembler, parsed.operand, line_number);

            assembler->memory[location_counter] = opcode & 0xFF;
            assembler->memory[location_counter + 1] = address & 0xFF;

            location_counter += 2;
            line_number++;
            continue;
        }

        assembler_error("linha nao reconhecida", line_number);
    }

    fclose(file);
}

int assembler_assemble_file(Assembler *assembler, const char *asm_filename) {
    assembler_init(assembler);

    first_pass(assembler, asm_filename);
    second_pass(assembler, asm_filename);

    return 1;
}

int assembler_write_mem_file(Assembler *assembler, const char *mem_filename) {
    FILE *file = fopen(mem_filename, "w");

    if (file == NULL) {
        return 0;
    }

    for (int i = 0; i < NEANDER_MEMORY_SIZE; i++) {
        fprintf(file, "%02X\n", assembler->memory[i]);
    }

    fclose(file);

    return 1;
}

void assembler_print_symbols(Assembler *assembler) {
    printf("Tabela de simbolos:\n");

    for (int i = 0; i < assembler->symbol_count; i++) {
        printf("%s = %d\n",
               assembler->symbols[i].name,
               assembler->symbols[i].address);
    }
}