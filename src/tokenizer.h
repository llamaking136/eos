#ifndef EOS_TOKENIZER_H
#define EOS_TOKENIZER_H

#include <lib/vec.h>

typedef enum
{
    TOKEN_SYMBOL,
    TOKEN_OPEN_PAREN,
    TOKEN_CLOSE_PAREN,
    TOKEN_OPEN_CURLY,
    TOKEN_CLOSE_CURLY,
    TOKEN_OPEN_SQUARE,
    TOKEN_CLOSE_SQUARE,
    TOKEN_SEMICOLON,
    TOKEN_EQUAL,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_GREATER_THAN,
    TOKEN_COMMA,
} TokenType;

typedef struct
{
    TokenType type;
    const char *text;
    int line;
    int column;
} Token;

typedef vec_t(Token) vec_token_t;

char *token_to_string(Token tok);

vec_token_t tokenize(char *text);
#endif
