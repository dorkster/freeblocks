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

MenuItem** menu_items = NULL;
int menu_size = 0;
int menu_option = 0;

void menuItemUpdate(int i) {
    if (i < 0 || i >= menu_size)
        return;

    unsigned int val = menu_items[i]->val - menu_items[i]->val_min;
    unsigned int val_max = menu_items[i]->val_max - menu_items[i]->val_min;

    Dork_StringClear(&menu_items[i]->full_text);
    Dork_StringAppend(&menu_items[i]->full_text, menu_items[i]->prefix.data);

    if (val_max > 0) {
        Dork_StringAppend(&menu_items[i]->full_text, ": ");
        if (menu_items[i]->val > menu_items[i]->val_min) {
            Dork_StringAppend(&menu_items[i]->full_text, "< ");
        }
        Dork_StringAppend(&menu_items[i]->full_text, menu_items[i]->options[val].data);
        if (menu_items[i]->val < menu_items[i]->val_max) {
            Dork_StringAppend(&menu_items[i]->full_text, " >");
        }
    }
}

char* menuItemGetText(int i) {
    if (i < 0 || i >= menu_size)
        return NULL;

    return Dork_StringGetData(&menu_items[i]->full_text);
}

bool menuItemIncreaseVal(int i) {
    if (i < 0 || i >= menu_size)
        return false;

    if (menu_items[i]->val < menu_items[i]->val_max) {
        menu_items[i]->val++;
        menuItemUpdate(i);
        return true;
    }
    return false;
}

bool menuItemDecreaseVal(int i) {
    if (i < 0 || i >= menu_size)
        return false;

    if (menu_items[i]->val > menu_items[i]->val_min) {
        menu_items[i]->val--;
        menuItemUpdate(i);
        return true;
    }
    return false;
}

unsigned int menuItemGetVal(int i) {
    if (i < 0 || i >= menu_size)
        return 0;

    return menu_items[i]->val;
}

void menuItemSetVal(int i, unsigned int val) {
    if (i < 0 || i >= menu_size)
        return;

    if (val < menu_items[i]->val_min)
        val = menu_items[i]->val_min;
    else if (val > menu_items[i]->val_max)
        val = menu_items[i]->val_max;

    menu_items[i]->val = val;
    menuItemUpdate(i);
}

void menuItemSetOptionText(int i, int opt, const char* text) {
    if (i < 0 || i >= menu_size || (unsigned)opt > (menu_items[i]->val_max - menu_items[i]->val_min))
        return;

    Dork_StringClear(&menu_items[i]->options[opt]);
    Dork_StringAppend(&menu_items[i]->options[opt], text);

    if ((unsigned)opt == menu_items[i]->val - menu_items[i]->val_min)
        menuItemUpdate(i);
}

void menuItemSetEnabled(int i, bool enable) {
    if (i < 0 || i >= menu_size)
        return;

    menu_items[i]->enabled = enable;
}

bool menuItemIsEnabled(int i) {
    if (i < 0 || i >= menu_size)
        return false;

    return menu_items[i]->enabled;
}

void menuInit() {
    menu_items = NULL;
    menu_size = 0;
    menu_option = 0;
}

void menuAdd(const char *item, unsigned int val_min, unsigned int val_max) {
    if (menu_size + 1 == MAX_MENU_ITEMS)
        return;

    menu_items = realloc(menu_items, sizeof(MenuItem*)*(menu_size+1));
    if (menu_items != NULL) {
        menu_items[menu_size] = NULL;
        menu_items[menu_size] = malloc(sizeof(MenuItem));
    }
    else
        return;

    Dork_StringInit(&menu_items[menu_size]->prefix);
    menu_items[menu_size]->options = NULL;
    Dork_StringInit(&menu_items[menu_size]->full_text);

    menu_items[menu_size]->val = 0;
    menu_items[menu_size]->val_min = 0;
    menu_items[menu_size]->val_max = 0;
    menu_items[menu_size]->enabled = true;

    if (val_max < val_min)
        val_max = val_min;

    menu_items[menu_size]->val = menu_items[menu_size]->val_min = val_min;
    menu_items[menu_size]->val_max = val_max;

    Dork_StringAppend(&menu_items[menu_size]->prefix, item);

    // create default options if this is a "spinner" menu item
    if (val_max > 0) {
        menu_items[menu_size]->options = malloc(sizeof(Dork_String)*(val_max+1));
        unsigned int i;
        for (i=val_min; i<=val_max; i++) {
            Dork_StringInit(&menu_items[menu_size]->options[i-val_min]);
            Dork_StringAppendNumber(&menu_items[menu_size]->options[i-val_min], i);
        }
    }

    menu_size++;

    // make final string
    menuItemUpdate(menu_size-1);
}

void menuClear() {
    int i;
    for (i=0;i<menu_size;i++) {
        if (menu_items[i] == NULL)
            continue;

        Dork_StringClear(&menu_items[i]->prefix);
        Dork_StringClear(&menu_items[i]->full_text);

        if (menu_items[i]->options != NULL) {
            unsigned int j;
            for (j=0; j<=menu_items[i]->val_max-menu_items[i]->val_min; j++) {
                Dork_StringClear(&menu_items[i]->options[j]);
            }
            free(menu_items[i]->options);
        }

        free(menu_items[i]);
    }

    if (menu_items != NULL) {
        free(menu_items);
    }

    menuInit();
}

int menuLogic() {
    if (menu_size > 0) {
        if (action_switch && menu_items[menu_option]->enabled && menu_items[menu_option]->val_max == 0) {
            action_switch = false;
            Mix_PlayChannel(-1,sound_menu,0);
            return menu_option;
        } else if (action_move == ACTION_UP && action_cooldown == 0 && menu_option > 0) {
            menu_option--;
            action_cooldown = ACTION_COOLDOWN;
            Mix_PlayChannel(-1,sound_switch,0);
        } else if (action_move == ACTION_DOWN && action_cooldown == 0 && menu_option < menu_size-1) {
            menu_option++;
            action_cooldown = ACTION_COOLDOWN;
            Mix_PlayChannel(-1,sound_switch,0);
        } else if (action_move == ACTION_LEFT && action_cooldown == 0 && menu_items[menu_option]->enabled) {
            if (menuItemDecreaseVal(menu_option)) {
                Mix_PlayChannel(-1,sound_switch,0);
            }
            action_cooldown = ACTION_COOLDOWN;
        } else if (action_move == ACTION_RIGHT && action_cooldown == 0 && menu_items[menu_option]->enabled) {
            if (menuItemIncreaseVal(menu_option)) {
                Mix_PlayChannel(-1,sound_switch,0);
            }
            action_cooldown = ACTION_COOLDOWN;
        }
    }
    return -1;
}
