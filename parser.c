#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

static const char *token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_VAR: return "VAR";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUM: return "NUM";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULT: return "MULT";
        case TOKEN_EQUAL: return "EQUAL";
        case TOKEN_LEFT_PARENTESIS: return "LEFT_PARENTESIS";
        case TOKEN_RIGHT_PARENTESIS: return "RIGHT_PARENTESIS";
        case TOKEN_NEWLINE: return "NEWLINE";
        case TOKEN_EOF: return "EOF";
        case TOKEN_INVALID: return "INVALID";
        default: return "UNKNOWN";
    }
}

static void parser_error(Parser *parser, const char *message) {
    printf("Erro sintatico na linha %d, coluna %d: %s\n",
           parser->current_token.line,
           parser->current_token.column,
           message);

    printf("Token encontrado: %s, lexeme: '%s'\n",
           token_type_to_string(parser->current_token.type),
           parser->current_token.lexeme);

    exit(1);
}

static void parser_advance(Parser *parser) {
    parser->current_token = lexer_next_token(&parser->lexer);
}

static void parser_expect(Parser *parser, TokenType expected) {
    if (parser->current_token.type == expected) {
        parser_advance(parser);
    } else {
        printf("Esperado: %s\n", token_type_to_string(expected));
        parser_error(parser, "token inesperado");
    }
}

static void parse_expression(Parser *parser);

void parser_init(Parser *parser, const char *input) {
    lexer_init(&parser->lexer, input);
    parser_advance(parser);
}

static void skip_newlines(Parser *parser) {
    while (parser->current_token.type == TOKEN_NEWLINE) {
        parser_advance(parser);
    }
}

/*BNF:

Fator -> NUMERO | IDENTIFIER | "(" Expressao ")"

*/
static void parse_factor(Parser *parser) {
    if (parser->current_token.type == TOKEN_NUM) {
        parser_expect(parser, TOKEN_NUM);
        return;
    }

    if (parser->current_token.type == TOKEN_IDENTIFIER) {
        parser_expect(parser, TOKEN_IDENTIFIER);
        return;
    }

    if (parser->current_token.type == TOKEN_LEFT_PARENTESIS) {
        parser_expect(parser, TOKEN_LEFT_PARENTESIS);
        parse_expression(parser);
        parser_expect(parser, TOKEN_RIGHT_PARENTESIS);
        return;
    }

    parser_error(parser, "esperado numero, identificador ou '('");
}

/*BNF:

Termo -> Fator ("*" Fator)*


*/
static void parse_term(Parser *parser) {
    parse_factor(parser);

    while (parser->current_token.type == TOKEN_MULT) {
        parser_expect(parser, TOKEN_MULT);
        parse_factor(parser);
    }
}
/*

Expressao -> Termo (("+" | "-") Termo)*

*/
static void parse_expression(Parser *parser) {
    parse_term(parser);

    while (parser->current_token.type == TOKEN_PLUS ||
           parser->current_token.type == TOKEN_MINUS) {

        if (parser->current_token.type == TOKEN_PLUS) {
            parser_expect(parser, TOKEN_PLUS);
        } else {
            parser_expect(parser, TOKEN_MINUS);
        }

        parse_term(parser);
    }
}

/*

Declaracao -> "var" IDENTIFIER "=" Expressao

*/
static void parse_declaration(Parser *parser) {
    parser_expect(parser, TOKEN_VAR);
    parser_expect(parser, TOKEN_IDENTIFIER);
    parser_expect(parser, TOKEN_EQUAL);
    parse_expression(parser);
}
/*

Comando -> Declaracao | Expressao

*/

static void parse_command(Parser *parser) {
    if (parser->current_token.type == TOKEN_VAR) {
        parse_declaration(parser);
    } else {
        parse_expression(parser);
    }
}

/*

Programa -> Comando* EOF

*/

void parser_parse(Parser *parser) {
    skip_newlines(parser);

    while (parser->current_token.type != TOKEN_EOF) {
        parse_command(parser);

        if (parser->current_token.type == TOKEN_NEWLINE) {
            skip_newlines(parser);
        } else if (parser->current_token.type != TOKEN_EOF) {
            parser_error(parser, "esperado fim de linha ou fim do arquivo");
        }
    }

    printf("Programa correto.\n");
}