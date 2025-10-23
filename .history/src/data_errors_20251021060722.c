#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "data_errors.h"

static char last_error_message[256] = {0};

void set_error_message(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(last_error_message, sizeof(last_error_message), format, args);
    va_end(args);
}

const char *get_last_error(void)
{
    return last_error_message;
}
