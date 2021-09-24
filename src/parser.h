#ifndef EOS_PARSER_H
#define EOS_PARSER_H
#include <tokenizer.h>

typedef struct ParseBlock ParseBlock;
typedef struct ParseValue ParseValue;

typedef vec_t(ParseValue) ParseValueList;

typedef enum
{
    INT,
    VOID,
    STRING,
    CHAR,
    FLOAT
} EosType;

typedef struct
{
    EosType type;

    const char *name;

    union
    {
        const char *str;
        int _int;
        float _float;
    };
} EosValue;

typedef struct ParseBlock
{
    ParseValueList values;
} ParseBlock;

typedef vec_t(EosValue) EosValueList;

typedef struct
{
    EosValueList params;
    const char *name;
    ParseBlock body;
    EosType return_type;

} EosFunction;

typedef struct
{
    const char *name;
    EosValue val;
} EosVariable;

typedef enum
{
    FUNCTION_DECL,
    VARIABLE_DECL,

    FUNCTION_CALL,
    VARIABLE_REFERENCE,

} ParseType;

typedef struct ParseValue
{

    ParseType type;

    union
    {
        EosFunction function_decl;
        EosFunction function_call;

        EosVariable var_decl;
        EosVariable var_ref;
    };
} ParseValue;

typedef struct
{
    int current_index;
    Token current_token;
    vec_token_t tokens;
} Parser;

void parser_expect_token_next(TokenType tok, char *name);

ParseValueList parse(vec_token_t toks);

#endif
