#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef struct {
    Lexer lexer;
    Token current_token;
} Parser;

void parser_init(Parser *parser, const char *input);
void parser_parse(Parser *parser);

#endif