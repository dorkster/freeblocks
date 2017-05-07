/*
    FreeBlocks -  A simple puzzle game, similar to Tetris Attack
    Copyright (C) 2012-2017 Justin Jacobs

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "string.h"

#ifdef _MSC_VER
#define snprintf _sprintf
#endif

void String_Init(String *str, ...) {
    if (!str)
        return;

    memset(str, 0, sizeof(String));

    va_list args;
    va_start(args, str);

    char *arg_str = va_arg(args, char*);
    while (arg_str != NULL) {
        str->len += strlen(arg_str);
        arg_str = va_arg(args, char*);
    }
    va_end(args);

    str->buf = malloc((str->len + 1) * sizeof(char));

    va_start(args, str);

    size_t offset = 0;
    arg_str = va_arg(args, char*);
    while (arg_str != NULL) {
        sprintf(str->buf + offset, "%s", arg_str);
        offset += strlen(arg_str);
        arg_str = va_arg(args, char*);
    }

    va_end(args);
}

void String_InitL(String *str, long value) {
    if (!str)
        return;

    memset(str, 0, sizeof(String));

    size_t buf_size = snprintf(NULL, 0, "%ld", value) + 1;
    str->buf = malloc(buf_size * sizeof(char));
    snprintf(str->buf, buf_size, "%ld", value);
    str->len = buf_size - 1;
}

void String_Clear(String *str) {
    if (!str)
        return;

    if (str->buf)
        free(str->buf);

    memset(str, 0, sizeof(String));
}

void String_Append(String *str, ...) {
    if (!str)
        return;

    size_t append_len = 0;

    va_list args;
    va_start(args, str);

    char *arg_str = va_arg(args, char*);
    while (arg_str != NULL) {
        append_len += strlen(arg_str);
        arg_str = va_arg(args, char*);
    }
    va_end(args);

    str->buf = realloc(str->buf, (str->len + append_len + 1) * sizeof(char));

    va_start(args, str);

    size_t offset = str->len;
    arg_str = va_arg(args, char*);
    while (arg_str != NULL) {
        sprintf(str->buf + offset, "%s", arg_str);
        offset += strlen(arg_str);
        arg_str = va_arg(args, char*);
    }

    va_end(args);

    str->len += append_len;
}

void String_AppendL(String *str, long value) {
    if (!str)
        return;

    String temp;
    String_InitL(&temp, value);
    String_Append(str, temp.buf, 0);
    String_Clear(&temp);
}
