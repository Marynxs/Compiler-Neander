#include <stdio.h>
#include "lexer.h"

int main() {
    const char *source = "var B = 2 \nvar C = 5\n6*5=4";

    Lexer lexer;
    lexer_init(&lexer, source);

    Token token;

    do {
        token = lexer_next_token(&lexer);

        printf("Tipo: %d | Lexeme: %s | Valor: %d | Linha: %d | Coluna: %d\n",
               token.type,
               token.lexeme,
               token.value,
               token.line,
               token.column);

    } while (token.type != TOKEN_EOF);

    return 0;
}