#include <stdio.h>
#include "../src/parser/parser.h"

int main() {
    const char *source =
        "2 * (3 + 1)";

    Parser parser;

    parser_init(&parser, source);
    parser_parse(&parser);

    if (!assembly_write_file(&parser.generator, "saida.asm")) {
        printf("Erro ao criar arquivo saida.asm\n");
        return 1;
    }

    printf("Arquivo saida.asm criado com sucesso.\n");

    return 0;
}