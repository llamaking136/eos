#include "tokenizer.h"
#include <log.h>
#include <parser.h>
#include <string.h>

static Parser parser;

ParseValue parse_statement();
ParseBlock parse_block();

void parser_expect_token_next(TokenType tok, char *name)
{
    if (parser.tokens.length < parser.current_index + 1)
    {
        log_error("Unexpected end of line at line %d, column %d", parser.tokens.data[parser.current_index - 1].line, parser.tokens.data[parser.current_index - 1].column);
        exit(-1);
    }

    parser.current_index++;

    if (parser.tokens.data[parser.current_index].type != tok)
    {
        log_error("Expected token '%s' (%s) after line %d, column %d", name, token_to_string((Token){.type = tok}), parser.tokens.data[parser.current_index].line, parser.tokens.data[parser.current_index].column);
    }

    parser.current_token = parser.tokens.data[parser.current_index];
}

static EosType string_to_type(const char *str)
{
    if (strcmp(str, "int") == 0)
    {
        return INT;
    }
    else if (strcmp(str, "string") == 0)
    {
        return STRING;
    }

    else if (strcmp(str, "void") == 0)
    {
        return VOID;
    }

    else if (strcmp(str, "char") == 0)
    {
        return CHAR;
    }

    return VOID;
}

/* === Function declaration.
Syntax: func <name>(<parameters>) -> <type> {
    statements;
}
*/

static ParseValue parse_function_declaration()
{
    ParseValue ret;

    vec_init(&ret.function_decl.params);

    ret.type = FUNCTION_DECL;

    parser_expect_token_next(TOKEN_SYMBOL, "name");

    ret.function_decl.name = parser.tokens.data[parser.current_index].text;

    parser_expect_token_next(TOKEN_OPEN_PAREN, "(");

    int i = 0;
    // Get arguments
    while (parser.tokens.data[parser.current_index++].type != TOKEN_CLOSE_PAREN)
    {
        if (parser.tokens.data[parser.current_index].type == TOKEN_CLOSE_PAREN)
        {
            break;
        }

        EosValue curr_val = {};

        // Get type
        parser_expect_token_next(TOKEN_SYMBOL, "type name");

        curr_val.type = string_to_type(parser.tokens.data[parser.current_index - 1].text);

        parser.current_index--;

        // Get argument name
        parser_expect_token_next(TOKEN_SYMBOL, "argument name");

        curr_val.name = parser.tokens.data[parser.current_index].text;

        parser.current_index++;

        if (parser.tokens.data[parser.current_index].type == TOKEN_COMMA)
        {
            if (parser.tokens.data[parser.current_index + 1].type != TOKEN_SYMBOL)
            {
                log_error("Invalid position of comma.");
            }
        }

        else if (parser.tokens.data[parser.current_index].type != TOKEN_COMMA && parser.tokens.data[parser.current_index].type != TOKEN_CLOSE_PAREN)
        {
            log_error("Expected comma but got %s", parser.tokens.data[parser.current_index].text);
        }

        vec_push(&ret.function_decl.params, curr_val);

        i++;
    }

    parser.current_index--;

    // Get return type
    parser_expect_token_next(TOKEN_MINUS, "tail of arrow");
    parser_expect_token_next(TOKEN_GREATER_THAN, "head of arrow");

    ret.function_decl.return_type = string_to_type(parser.tokens.data[parser.current_index].text);

#ifdef VERBOSE
    log_info("Function declaration: %s() -> %d", ret.function_decl.name, ret.function_decl.return_type);
    log_info("Arguments: %d", ret.function_decl.params.length);

    for (int i = 0; i < ret.function_decl.params.length; i++)
    {
        log_info("\t name=%s type=%d", ret.function_decl.params.data[i].name, ret.function_decl.params.data[i].type);
    }
#endif

    parser.current_index++;

    ret.function_decl.body = parse_block();

    return ret;
}

__attribute__((unused)) static EosValueList parse_function_arguments()
{
    EosValueList ret = {};

    return ret;
}

/* === Variable declaration.
Syntax: let <type> <name> = <value>;
*/
static ParseValue parse_variable_declaration()
{
    ParseValue ret;

    ret.type = VARIABLE_DECL;

    parser.current_index++;

    parser_expect_token_next(TOKEN_SYMBOL, "type");

    ret.var_decl.val.type = string_to_type(parser.tokens.data[parser.current_index].text);

    parser_expect_token_next(TOKEN_SYMBOL, "name");

    ret.var_decl.name = parser.tokens.data[parser.current_index].text;

    parser_expect_token_next(TOKEN_EQUAL, "=");

    parser.current_index++;

    switch (ret.var_decl.val.type)
    {

    case STRING:
    {
        ret.var_decl.val.str = parser.tokens.data[parser.current_index].text;
        break;
    }

    case INT:
    {
        ret.var_decl.val._int = atoi(parser.tokens.data[parser.current_index].text);
        break;
    }

    default:
        log_error("Invalid value.");
        break;
    }

    parser_expect_token_next(TOKEN_SEMICOLON, ";");

    return ret;
}

// A block is a series of statement, delimited by {}
ParseBlock parse_block()
{
    ParseBlock ret;

    vec_init(&ret.values);

    parser_expect_token_next(TOKEN_OPEN_CURLY, "{");

    parser.current_index++;

    parser.current_token = parser.tokens.data[parser.current_index];

    while (parser.current_token.type != TOKEN_CLOSE_CURLY)
    {
        if (parser.current_token.type == TOKEN_CLOSE_CURLY)
            break;
        else
            parser.current_index++;

        parser.current_token = parser.tokens.data[parser.current_index];

        ParseValue val = parse_statement();

        vec_push(&ret.values, val);
    }

    return ret;
}

ParseValue parse_statement()
{
    ParseValue ret = {};

    switch (parser.current_token.type)
    {
    case TOKEN_SYMBOL:
    {
        if (strcmp(parser.current_token.text, "func") == 0)
        {

            ret = parse_function_declaration();
        }

        else if (strcmp(parser.current_token.text, "let") == 0)
        {
            ret = parse_variable_declaration();

#ifdef VERBOSE
            log_info("Variable declaration name=%s, type=%d", ret.var_decl.name, ret.var_decl.val.type);
#endif
        }
    }

    default:
        break;
    }

    return ret;
}

ParseValueList parse(vec_token_t toks)
{
    parser.tokens = toks;

    ParseValueList ret;

    vec_init(&ret);

    for (int i = 0; i < parser.tokens.length; i++)
    {
        parser.current_token = parser.tokens.data[i];

        ParseValue curr_value = parse_statement();

        parser.current_index = i;

        vec_push(&ret, curr_value);
    }

    return ret;
}
