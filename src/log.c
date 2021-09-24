#include <log.h>
#include <stdarg.h>
#include <stdio.h>

void __log(LogLevel level, char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);

    switch (level)
    {
    case ERROR:
        printf("\033[1;31merror:\033[0m ");
        break;
    case WARNING:
        printf("\033[1;33mwarning:\033[0m ");
        break;
    case SILENT:
        break;
    case INFO:
        printf("\033[1;34minfo:\033[0m ");
        break;
    case SUCCESS:
        printf("\033[1;32msuccess:\033[0m ");
    }

    char buf[1024] = {0};

    vsprintf(buf, fmt, ap);

    printf("%s\n", buf);

    va_end(ap);
}
