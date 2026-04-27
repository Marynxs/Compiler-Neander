#ifndef TOKEN_H
#define TOKEN_H

#define MAX_LEXEME_SIZE 64

typedef enum {

    TOKEN_VAR,

    TOKEN_IDENTIFIER,

    TOKEN_NUM,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULT,
    TOKEN_EQUAL,
    TOKEN_LEFT_PARENTESIS,
    TOKEN_RIGHT_PARENTESIS,

    TOKEN_NEWLINE,
    TOKEN_EOF,
    TOKEN_INVALID
} TokenType;

typedef struct {
    TokenType type;
    char lexeme[MAX_LEXEME_SIZE];
    int value;
    int line;
    int column;
} Token;

#endif

