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

    String_Clear(&menu_items[i]->full_text);
    String_Init(&menu_items[i]->full_text, menu_items[i]->prefix.buf, 0);

    if (val_max > 0) {
        String_Append(&menu_items[i]->full_text, ": ", menu_items[i]->options[val].buf, 0);
    }
}

char* menuItemGetText(int i) {
    if (i < 0 || i >= menu_size)
        return NULL;

    return menu_items[i]->full_text.buf;
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

    String_Init(&menu_items[i]->options[opt], text, 0);

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

void menuItemEnableAction(int i) {
    if (i < 0 || i >= menu_size)
        return;

    menu_items[i]->has_action = true;
}

bool menuItemHasLeftButton(int i) {
    if (i < 0 || i >= menu_size)
        return false;

    return menu_items[i]->val > menu_items[i]->val_min;
}

bool menuItemHasRightButton(int i) {
    if (i < 0 || i >= menu_size)
        return false;

    return menu_items[i]->val < menu_items[i]->val_max;
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
        menu_items[menu_size] = malloc(sizeof(MenuItem));
    }
    else
        return;

    String_Init(&menu_items[menu_size]->prefix, item, 0);
    menu_items[menu_size]->options = NULL;
    String_Init(&menu_items[menu_size]->full_text, "", 0);

    menu_items[menu_size]->val = 0;
    menu_items[menu_size]->val_min = 0;
    menu_items[menu_size]->val_max = 0;
    menu_items[menu_size]->enabled = true;
    menu_items[menu_size]->has_action = true;

    if (val_max < val_min)
        val_max = val_min;

    menu_items[menu_size]->val = menu_items[menu_size]->val_min = val_min;
    menu_items[menu_size]->val_max = val_max;

    // create default options if this is a "spinner" menu item
    if (val_max > 0) {
        menu_items[menu_size]->has_action = false;
        menu_items[menu_size]->options = malloc(sizeof(String)*(val_max+1));
        unsigned int i;
        for (i=val_min; i<=val_max; i++) {
            String_InitL(&menu_items[menu_size]->options[i-val_min], i);
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

        String_Clear(&menu_items[i]->prefix);
        String_Clear(&menu_items[i]->full_text);

        if (menu_items[i]->options != NULL) {
            unsigned int j;
            for (j=0; j<=menu_items[i]->val_max-menu_items[i]->val_min; j++) {
                String_Clear(&menu_items[i]->options[j]);
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

static int menuOptionMouse() {
    int menu_top = SCREEN_HEIGHT - (menu_size * img_bar->h);
    if (paused || game_over)
        menu_top -= img_bar->h;

    for (int i=0; i<menu_size; i++) {
        int menu_pos = menu_top + (i*img_bar->h);
        if (mouse_y >= menu_pos && mouse_y < menu_pos + img_bar->h) {
            menu_option = i;
            return i;
        }
    }
    return -1;
}

int menuLogic() {
    if (menu_size > 0) {
        bool click_decrease = false;
        bool click_increase = false;
        bool click_accept = false;

        bool use_two_clicks = false;
        int last_menu = menu_option;

#ifdef __ANDROID__
        use_two_clicks = true;
#endif

        int mouse_menu = -1;
        if (mouse_moving || action_click || action_right_click) {
            mouse_menu = menuOptionMouse();
        }
        if (action_click) {
            if (mouse_menu >= 0) {
                if (menu_items[mouse_menu]->val_max > 0) {
                    if (mouse_x <= img_bar_left->w)
                        click_decrease = true;
                    else if (mouse_x > SCREEN_WIDTH - img_bar_right->w)
                        click_increase = true;
                    else {
                        if (use_two_clicks && last_menu == mouse_menu)
                            click_accept = true;
                        else if (!use_two_clicks)
                            click_accept = true;
                    }
                }
                else {
                    if (use_two_clicks && last_menu == mouse_menu)
                        click_accept = true;
                    else if (!use_two_clicks)
                        click_accept = true;
                }
            }
            action_click = false;
        }

        if ((action_switch || action_accept || click_accept) && menu_items[menu_option]->enabled && menu_items[menu_option]->has_action) {
            action_switch = false;
            action_accept = false;
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
        } else if ((action_move == ACTION_LEFT || click_decrease) && action_cooldown == 0 && menu_items[menu_option]->enabled) {
            if (menuItemDecreaseVal(menu_option)) {
                Mix_PlayChannel(-1,sound_switch,0);
            }
            action_cooldown = ACTION_COOLDOWN;
        } else if ((action_move == ACTION_RIGHT || click_increase) && action_cooldown == 0 && menu_items[menu_option]->enabled) {
            if (menuItemIncreaseVal(menu_option)) {
                Mix_PlayChannel(-1,sound_switch,0);
            }
            action_cooldown = ACTION_COOLDOWN;
        }
    }
    return -1;
}
