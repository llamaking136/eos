#include "lib/map.h"
#include <eval.h>
#include <stdio.h>

typedef void EosBuiltinFunction(EosValueList params);

map_t(EosBuiltinFunction *) builtins;

void b_write(EosValueList params)
{
    puts(params.data[0].str);
}

void eval(ParseValueList lst)
{
    map_init(&builtins);

    map_set(&builtins, "write", b_write);

    for (int i = 0; i < lst.length; i++)
    {
        ParseValue curr = lst.data[i];
        switch (curr.type)
        {
        case FUNCTION_CALL:
        {
            __auto_type yes = *map_get(&builtins, curr.function_call.name);

            yes(curr.function_call.params);
        }
        default:
            break;
        }
    }
}
