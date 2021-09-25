#include <assert.h>
#include <ctype.h>
#include <lib/scan.h>
#include <stdio.h>
#include <tokenizer.h>

#define ADD_TOKEN(_type, _text) (        \
    {                                    \
        Token new_tok;                   \
        char *new_str = malloc(2);       \
        new_str[0] = _text;              \
        new_str[1] = 0;                  \
        new_tok.type = _type;            \
        new_tok.text = new_str;          \
        new_tok.column = current_column; \
        new_tok.line = current_line;     \
        vec_push(&ret, new_tok);         \
    })

char tokens[] = {'+', '-', '/', '*', '{', '}', '[', ']', '(', ')', ';', '"', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '>', ',', '\0', EOF, ' ', '\t', '\n', '\v', '\r'};

char *token_to_string(Token tok)
{
    switch (tok.type)
    {
    case TOKEN_EQUAL:
        return "equal sign";
    case TOKEN_PLUS:
        return "plus sign";
    case TOKEN_MINUS:
        return "minus sign";
    case TOKEN_STAR:
        return "star sign";
    case TOKEN_CLOSE_CURLY:
        return "closing curly bracket";
    case TOKEN_OPEN_CURLY:
        return "opening curly bracket";
    case TOKEN_OPEN_PAREN:
        return "opening parenthesis";
    case TOKEN_CLOSE_PAREN:
        return "closing parenthesis";
    case TOKEN_OPEN_SQUARE:
        return "opening square bracket";
    case TOKEN_CLOSE_SQUARE:
        return "closing square bracket";
    case TOKEN_SYMBOL:
        return "symbol";
    case TOKEN_SEMICOLON:
        return "semicolon";
    case TOKEN_SLASH:
        return "slash";
    case TOKEN_STRING:
        return "string";
    case TOKEN_GREATER_THAN:
        return "greather than";
    case TOKEN_NUMBER:
        return "number";
    case TOKEN_COMMA:
        return "comma";
    }

    return "unknown";
}

bool is_token(char c)
{
    for (size_t i = 0; i < sizeof(tokens) / sizeof(*tokens); i++)
    {
        if (tokens[i] == c)
            return true;
    }
    return false;
}

bool is_quote(char c)
{
    return c == '"';
}

bool is_digit(char c)
{
    return isdigit(c) || c == '.';
}

vec_char_t get_while_f_is(Scanner *scan, bool (*function)(char c), bool condition)
{
    vec_char_t ret;

    vec_init(&ret);

    while (!scan_ended(scan))
    {

        if (function(scan_current(scan)) != condition)
        {
            break;
        }

        vec_push(&ret, scan_current(scan));

        scan_forward(scan);
    }

    ret.data[ret.length] = 0;

    return ret;
}

vec_token_t tokenize(char *string)
{
    Scanner scanner = (Scanner){.head = 0, .buffer = string, .size = strlen(string)};

    vec_token_t ret;

    vec_init(&ret);

    int current_line = 1;
    int current_column = 1;

    while (!scan_ended(&scanner))
    {

        Token curr_tok = {0};

        char current_char = scan_current(&scanner);

        switch (current_char)
        {

        case '\n':
            current_line++;
            current_column = 1;
            break;
        case '\r':
        case '\v':
        case '\f':
        case '\t':
        case ' ':
            break;

        case '(':
            ADD_TOKEN(TOKEN_OPEN_PAREN, current_char);
            break;
        case ')':
            ADD_TOKEN(TOKEN_CLOSE_PAREN, current_char);
            break;
        case '{':
            ADD_TOKEN(TOKEN_OPEN_CURLY, current_char);
            break;
        case '}':
            ADD_TOKEN(TOKEN_CLOSE_CURLY, current_char);
            break;
        case '[':
            ADD_TOKEN(TOKEN_OPEN_SQUARE, current_char);
            break;
        case ']':
            ADD_TOKEN(TOKEN_CLOSE_SQUARE, current_char);
            break;
        case '=':
            ADD_TOKEN(TOKEN_EQUAL, current_char);
            break;
        case '+':
            ADD_TOKEN(TOKEN_PLUS, current_char);
            break;
        case '-':
            ADD_TOKEN(TOKEN_MINUS, current_char);
            break;
        case '/':
            ADD_TOKEN(TOKEN_SLASH, current_char);
            break;
        case '*':
            ADD_TOKEN(TOKEN_STAR, current_char);
            break;
        case ';':
            ADD_TOKEN(TOKEN_SEMICOLON, current_char);
            break;
        case '>':
            ADD_TOKEN(TOKEN_GREATER_THAN, current_char);
            break;
        case ',':
            ADD_TOKEN(TOKEN_COMMA, current_char);
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {

            curr_tok.type = TOKEN_NUMBER;
            curr_tok.line = current_line;
            curr_tok.column = current_column;
            curr_tok.text = get_while_f_is(&scanner, is_digit, true).data;
            vec_push(&ret, curr_tok);

            scanner.head--;
            break;
        }

        case '"':
        {

            scan_skip_c(&scanner, '"');

            curr_tok.type = TOKEN_STRING;
            curr_tok.line = current_line;
            curr_tok.column = current_column;

            curr_tok.text = get_while_f_is(&scanner, is_quote, false).data;

            vec_push(&ret, curr_tok);

            break;
        }

        default:
        {
            curr_tok.text = get_while_f_is(&scanner, is_token, false).data;
            curr_tok.type = TOKEN_SYMBOL;
            curr_tok.line = current_line;
            curr_tok.column = current_column;

            vec_push(&ret, curr_tok);

            // The function most likely skipped the next token, which is bad, so go backward by one character.
            scanner.head--;
            break;
        }
        }
        scan_forward(&scanner);
        current_column++;
    }

    return ret;
}
