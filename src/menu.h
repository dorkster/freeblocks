/*
    FreeBlocks -  A simple puzzle game, similar to Tetris Attack
    Copyright (C) 2012 Justin Jacobs

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

#ifndef MENU_H
#define MENU_H

#include "sys.h"
#include "dork/dork_string.h"

#define MAX_MENU_ITEMS 15

typedef struct {
    Dork_String prefix;
    Dork_String* options;
    Dork_String full_text;

    unsigned int val;
    unsigned int val_min;
    unsigned int val_max;
}MenuItem;

MenuItem** menu_items;
int menu_option;
int menu_size;

void menuItemUpdate(int i);
char* menuItemGetText(int i);
bool menuItemIncreaseVal(int i);
bool menuItemDecreaseVal(int i);
unsigned int menuItemGetVal(int i);
void menuItemSetVal(int i, unsigned int val);
void menuItemSetOptionText(int i, int opt, const char* text);

void menuInit();
void menuAdd(const char *item, unsigned int val_min, unsigned int val_max);
void menuClear();
int menuLogic();

#endif
