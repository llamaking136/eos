#ifndef EOS_LOG_H
#define EOS_LOG_H

typedef enum
{
    SUCCESS,
    INFO,
    ERROR,
    WARNING,
    SILENT
} LogLevel;

#define log_error(fmt, ...) __log(ERROR, fmt __VA_OPT__(, ) __VA_ARGS__)
#define log_success(fmt, ...) __log(SUCCESS, fmt __VA_OPT__(, ) __VA_ARGS__)
#define log_warning(fmt, ...) __log(WARNING, fmt __VA_OPT__(, ) __VA_ARGS__)
#define log_info(fmt, ...) __log(INFO, fmt __VA_OPT__(, ) __VA_ARGS__)
#define log_silent(fmt, ...) __log(SILENT, fmt __VA_OPT__(, ) __VA_ARGS__)

__attribute__((format(printf, 2, 3))) void __log(LogLevel level, char *fmt, ...);

#endif
