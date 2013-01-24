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

#include <stdlib.h>
#include <string.h>

#include "menu.h"
#include "sys.h"

void menuAdd(const char *item) {
    menu_items[menu_size] = malloc(strlen(item)+1);
    if (!menu_items[menu_size]) return;
    strcpy(menu_items[menu_size], item);
    menu_size++;
}

void menuUpdate(int i, const char *item) {
    menu_items[i] = realloc(menu_items[i], strlen(item)+1);
    if (!menu_items[i]) return;
    strcpy(menu_items[i], item);
}

void menuClear() {
    int i;
    for (i=0;i<MAX_MENU_ITEMS;i++) {
        if (!menu_items[i]) {
            free(menu_items[i]);
            menu_items[i] = NULL;
        }
    }

    menu_size = 0;
    menu_option = 0;
}

int menuLogic() {
    if (menu_size > 0) {
        if (action_switch) {
            action_switch = false;
            Mix_PlayChannel(-1,sound_menu,0);
            return menu_option;
        } else if (action_up && action_cooldown == 0 && menu_option > 0) {
            menu_option--;
            action_cooldown = 10;
            Mix_PlayChannel(-1,sound_switch,0);
        } else if (action_down && action_cooldown == 0 && menu_option < menu_size-1) {
            menu_option++;
            action_cooldown = 10;
            Mix_PlayChannel(-1,sound_switch,0);
        }
    }
    return -1;
}
