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

static Operand parse_expression(Parser *parser);

void parser_init(Parser *parser, const char *input) {
    assembly_init(&parser->generator);
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
static Operand parse_factor(Parser *parser) {
    Operand op;

    if (parser->current_token.type == TOKEN_NUM) {
        int value = parser->current_token.value;
        
        op = assembly_new_const(&parser->generator, value, NULL); // Quando é um numero o operand é aquele numero em inteiro, por isso precisamos pegar o inteiro do current token salvo em value

        parser_expect(parser, TOKEN_NUM);

        return op;
    }

    if (parser->current_token.type == TOKEN_IDENTIFIER) {
        snprintf(op.name, MAX_NAME_SIZE, "%s", parser->current_token.lexeme); //Aqui o operand é a propria variavel afinal é o que é um indentificador

        parser_expect(parser, TOKEN_IDENTIFIER);
        return op;
    }

    if (parser->current_token.type == TOKEN_LEFT_PARENTESIS) {
        parser_expect(parser, TOKEN_LEFT_PARENTESIS);

        op = parse_expression(parser); // Aqui o operand é o valor dentro do parentesis por que ele vai entrar fazer a conta e salvar em um temporario exemplo: 5 * (5*9) -> 5 * _T0. 

        parser_expect(parser, TOKEN_RIGHT_PARENTESIS);
        return op;
    }

    parser_error(parser, "esperado numero, identificador ou '('");

    return op;
}

/*BNF:

Termo -> Fator ("*" Fator)*


*/
static Operand parse_term(Parser *parser) {
    
    Operand left = parse_factor(parser);

    while (parser->current_token.type == TOKEN_MULT) {
        parser_expect(parser, TOKEN_MULT);
        Operand right = parse_factor(parser);
        Operand result = assembly_new_temp(&parser->generator, "Result_Mult");
        assembly_emit_mul(&parser->generator, left, right, result);

        left = result;
    }

    return left;
}
/*

Expressao -> Termo (("+" | "-") Termo)*

*/
static Operand parse_expression(Parser *parser) {
    Operand left = parse_term(parser);

    while (parser->current_token.type == TOKEN_PLUS ||
           parser->current_token.type == TOKEN_MINUS) {

        TokenType operation = parser->current_token.type;

        if (operation == TOKEN_PLUS) {
            parser_expect(parser, TOKEN_PLUS);

        } else {
            parser_expect(parser, TOKEN_MINUS);
        }

        Operand right = parse_term(parser);
        Operand result = assembly_new_temp(&parser->generator, "Result_Sum");

        if (operation == TOKEN_PLUS) {
            assembly_emit_add(&parser->generator, left, right, result);
        }
        else {
            parser_error(parser, "subtracao ainda nao implementada");
            //assembly_emit_minus(&parser->generator,left,right,result)
        }

        left = result;
    }

    return left;
}

/*

Declaracao -> "var" IDENTIFIER "=" Expressao

*/
static void parse_declaration(Parser *parser) {
    Operand variable;

    parser_expect(parser, TOKEN_VAR);

    snprintf(variable.name, MAX_NAME_SIZE, "%s", parser->current_token.lexeme); 
    parser_expect(parser, TOKEN_IDENTIFIER);

    parser_expect(parser, TOKEN_EQUAL);

    Operand expression_result =  parse_expression(parser);

    assembly_emit_data(&parser->generator, "%s DATA 0", variable.name);
    assembly_emit_code(&parser->generator, "LDA %s", expression_result.name);
    assembly_emit_code(&parser->generator, "STA %s", variable.name);

}
/*

Comando -> Declaracao | Expressao

*/

static void parse_command(Parser *parser) {
    if (parser->current_token.type == TOKEN_VAR) {
        parse_declaration(parser);
    } else {
        Operand expression_result = parse_expression(parser);
        assembly_emit_code(&parser->generator, "LDA %s", expression_result.name); //Precisa ser carregado na memoria para ele ficar no AC antes de dar HLT
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