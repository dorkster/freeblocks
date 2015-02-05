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

#ifndef DORK_STRING
#define DORK_STRING

typedef struct {
    unsigned int length;
    char* data;
}Dork_String;

void Dork_StringInit(Dork_String *dest);
void Dork_StringAppend(Dork_String* dest, const char* src);
void Dork_StringAppendNumber(Dork_String* dest, long src);
void Dork_StringClear(Dork_String* dest);
char* Dork_StringGetData(const Dork_String* dest);

#endif
