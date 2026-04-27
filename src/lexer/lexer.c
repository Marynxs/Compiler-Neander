#include "lexer.h"
#include <string.h>
#include <ctype.h>

void lexer_init(Lexer *lexer, const char *input) {
    lexer->input = input;
    lexer->pos = 0;
    lexer->line = 1;
    lexer->column = 1;
}

static void token_init(Token *token, Lexer *lexer) {
    token->type = TOKEN_INVALID;
    token->lexeme[0] = '\0';
    token->value = 0;
    token->column = lexer->column;
    token->line = lexer->line;

}

static char current_character(Lexer *lexer) {
    return lexer->input[lexer->pos];
}

static void lexer_advance(Lexer *lexer) {
    char character = current_character(lexer);

    if (character == '\0') {
        return;
    }

    lexer->pos++;

    if (character == '\n') {
        lexer->line++;
        lexer->column = 1;
    }
    else {
        lexer->column++;
    }

}

Token lexer_next_token(Lexer *lexer) {
    Token token;

    while (current_character(lexer) == ' ' || current_character(lexer) == '\t') {
        lexer_advance(lexer);
    }

    char character = lexer->input[lexer->pos];

    token_init(&token, lexer);

    if (character == '\0') {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }

    if (character == '\n') {
        token.type = TOKEN_NEWLINE;
        strcpy(token.lexeme, "\\n");
        lexer_advance(lexer);
        return token;
    }

    if(isalpha(character)) {
        int length = 0;

        while (isalnum(current_character(lexer)) || current_character(lexer) == '_') {

            if (length < MAX_LEXEME_SIZE - 1) {
                token.lexeme[length] = current_character(lexer);
                length++;
            }
            lexer_advance(lexer);
        }

        token.lexeme[length] = '\0';

        if (strcmp(token.lexeme, "var") == 0) {
            token.type = TOKEN_VAR;
        } else {
            token.type = TOKEN_IDENTIFIER;
        }

        return token;
    }

    if (isdigit(character)) {
        int length = 0;
        int value = 0;

        while(isdigit(current_character(lexer))) {
            char lexeme_number = current_character(lexer);

            if (length < MAX_LEXEME_SIZE - 1) {
                token.lexeme[length] = lexeme_number;
                length++;
            }
            
            value = value * 10 + (lexeme_number - '0');
            lexer_advance(lexer);
        }

        token.lexeme[length] = '\0';
        token.value = value;
        token.type = TOKEN_NUM;

        return token;
    }

    switch(character) {
        case '=':
            token.type = TOKEN_EQUAL;
            strcpy(token.lexeme, "=");
            break;
        case '+':
            token.type = TOKEN_PLUS;
            strcpy(token.lexeme, "+");
            break;
        case '-':
            token.type = TOKEN_MINUS;
            strcpy(token.lexeme, "-");
            break;
        case '*':
            token.type = TOKEN_MULT;
            strcpy(token.lexeme, "*");
            break;
        case '(':
            token.type = TOKEN_LEFT_PARENTESIS;
            strcpy(token.lexeme,"(");
            break;
        case ')':
            token.type = TOKEN_RIGHT_PARENTESIS;
            strcpy(token.lexeme,")");
            break;
        
        default:
            token.type = TOKEN_INVALID;
            token.lexeme[0] = character;
            token.lexeme[1] = '\0';
            break;
    }

    lexer_advance(lexer);
    return token;

}
