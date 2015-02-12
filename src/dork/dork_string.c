/*
    Dork_String - A basic text string API
    Copyright (C) 2015 Justin Jacobs

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

#include "dork_string.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

void Dork_StringInit(Dork_String *dest) {
    if (dest == NULL)
        return;

    dest->length = 0;
    dest->data = NULL;
}

void Dork_StringAppend(Dork_String* dest, const char* src) {
    if (src == NULL || dest == NULL)
        return;

    unsigned int src_length = strlen(src);
    unsigned int dest_length = dest->length;

    dest->data = realloc(dest->data, src_length + dest_length + 1);
    if (dest->data != NULL)
        dest->length = src_length + dest_length;

    if (dest_length == 0)
        dest->data = strcpy(dest->data, src);
    else
        dest->data = strcat(dest->data, src);
}

void Dork_StringAppendNumber(Dork_String* dest, long src) {
    if (dest == NULL)
        return;

    char buf[12];
    snprintf(buf, 12, "%ld", src);

    Dork_StringAppend(dest, buf);
}

void Dork_StringClear(Dork_String* dest) {
    if (dest == NULL)
        return;

    if (dest->data != NULL) {
        free(dest->data);
    }

    dest->length = 0;
    dest->data = NULL;
}

char* Dork_StringGetData(const Dork_String* dest) {
    if (dest == NULL)
        return NULL;

    return dest->data;
}

