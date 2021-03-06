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

#include "block.h"
#include "game.h"
#include "game_mode.h"
#include "menu.h"
#include "sys.h"

void gameTitle() {
    title_screen = true;
    high_scores_screen = false;
    options_screen = -1;
    rebind_index = -1;

    game_over = false;
    score = 0;
    Mix_FadeOutMusic(2000);

    menuAdd("Play Game", 0, 0);
    menuAdd("Game Type", GAME_MODE_DEFAULT, GAME_MODE_DROP);
    menuAdd("Speed Level", 1, MAX_SPEED);
    menuAdd("High Scores", 0, 0);
    menuAdd("Options", 0, 0);
#ifndef __EMSCRIPTEN__
    menuAdd("Quit", 0, 0);
#endif

    menuItemSetOptionText(1, GAME_MODE_DEFAULT, "Normal");
    menuItemSetOptionText(1, GAME_MODE_JEWELS, "Jewels");
    menuItemSetOptionText(1, GAME_MODE_DROP, "Drop");
    menuItemSetVal(1, gameModeGetIndex());
}

void gameHighScores() {
    title_screen = false;
    high_scores_screen = true;
    options_screen = -1;

    sysHighScoresLoad();

    menuAdd("Return to title screen", 0, 0);
}

void gameOptions() {
    title_screen = false;
    high_scores_screen = false;
    options_screen = OPTIONS_MAIN;

#ifdef __ANDROID__
    // 0 sound level
    menuAdd("Sound", 0, 8);
    menuItemSetVal(0, option_sound);
    menuItemSetOptionText(0, 0, "Off");
    menuItemSetOptionText(0, 8, "Max");

    // 1 music level
    menuAdd("Music", 0, 8);
    menuItemSetVal(1, option_music);
    menuItemSetOptionText(1, 0, "Off");
    menuItemSetOptionText(1, 8, "Max");

#else
    // 0 joystick
    menuAdd("Controls", 0, SDL_NumJoysticks());
    menuItemEnableAction(0);
    menuItemSetVal(0, option_joystick+1);
    menuItemSetOptionText(0, 0, "(no joystick)");
    int i;
    for (i=0; i < SDL_NumJoysticks(); i++) {
        menuItemSetOptionText(0, i+1, SDL_JoystickNameForIndex(i));
    }

    // 1 sound level
    menuAdd("Sound", 0, 8);
    menuItemSetVal(1, option_sound);
    menuItemSetOptionText(1, 0, "Off");
    menuItemSetOptionText(1, 8, "Max");

    // 2 music level
    menuAdd("Music", 0, 8);
    menuItemSetVal(2, option_music);
    menuItemSetOptionText(2, 0, "Off");
    menuItemSetOptionText(2, 8, "Max");

#ifndef __GCW0__
#ifndef __EMSCRIPTEN__
    // 3 fullscreen toggle
    menuAdd("Fullscreen", 0, 1);
    menuItemSetVal(3, option_fullscreen);
    menuItemSetOptionText(3, 0, "Off");
    menuItemSetOptionText(3, 1, "On");
#endif // __EMSCRIPTEN__
#endif //__GCW0__

#endif //__ANDROID__

    menuAdd("Cancel", 0, 0);
    menuAdd("Save settings", 0, 0);
}

void gameOptionsControls() {
    options_screen = OPTIONS_CONTROLS;

    if (option_joystick == -1) {
        for (int i=0; i<9; i++) {
            String label;
            String_Init(&label, key_desc[i], ": ", SDL_GetKeyName(option_key[i]), 0);
            menuAdd(label.buf, 0, 0);
            String_Clear(&label);
        }
    }
    else if (option_joystick > -1) {
        for (int i=0; i<5; i++) {
            String label;
            String_Init(&label, key_desc[i], ": Button ", 0);
            String_AppendL(&label, option_joy_button[i]);
            menuAdd(label.buf, 0, 0);
            String_Clear(&label);
        }
    }

    menuAdd("Return to options menu", 0, 0);
}

void gameOptionsRebind() {
    options_screen = OPTIONS_REBIND;

    last_key = SDLK_UNKNOWN;
    last_joy_button = -1;

    String label;
    String_Init(&label, "Press a key to use for: ", key_desc[rebind_index], 0);
    menuAdd(label.buf, 0, 0);
    String_Clear(&label);
}

void gameInit() {
    title_screen = false;
    score = 0;
    cursor_moving = false;
    cursor_timer = -1;

    sysHighScoresLoad();

    blockInitAll();
    cursor.x1 = (COLS/2)-1;
    cursor.y1 = ROWS-START_ROWS;
    if (cursor.y1 > CURSOR_MAX_Y) cursor.y1 = CURSOR_MAX_Y;

    Mix_VolumeMusic(option_music*16);
    if (!game_over) {
        Mix_PlayMusic(game_mode->music,-1);
    }

    game_over = false;
}

void gameLogic() {
    int menu_choice;

    if (title_screen || high_scores_screen || options_screen != -1 || rebind_index != -1 || game_over) {
        force_pause = false;
    }

    if (action_cooldown > 0) action_cooldown--;

    // handle the title screen menu
    if (title_screen) {
        if (action_exit) {
            action_exit = false;
            menu_choice = menu_size-1; // quit game
        }
        else {
            menu_choice = menuLogic();
        }

        // get the "Game Type" value
        switch (menuItemGetVal(1)) {
        case GAME_MODE_DEFAULT: game_mode = &game_mode_default; break;
        case GAME_MODE_JEWELS: game_mode = &game_mode_jewels; break;
        case GAME_MODE_DROP: game_mode = &game_mode_drop; break;
        }

        menuItemSetEnabled(2, game_mode->speed);

        if (menu_choice > -1) {
            // get the "Speed Level" value
            speed_init = menuItemGetVal(2);

            menuClear();
            if (menu_choice == 0) gameInit();
            // menu_choice == 1 game type selection
            // menu_choice == 2 speed selection
            else if (menu_choice == 3) gameHighScores();
            else if (menu_choice == 4) gameOptions();
            else if (menu_choice == 5) quit = true;
        }
        return;
    }

    // handle high scores screen menu
    if (high_scores_screen) {
        if (action_bump) {
            action_bump = false;
            menu_choice = menu_size-1; // return to title
        }
        else {
            menu_choice = menuLogic();
        }

        if (menu_choice > -1) {
            menuClear();
            if (menu_choice == 0) gameTitle();
        }
        return;
    }

    // handle options screen menu
    if (options_screen == OPTIONS_MAIN) {
        if (action_bump) {
            action_bump = false;
            menu_choice = menu_size-2; // exit without saving changes
        }
        else {
            menu_choice = menuLogic();
        }

        if (menu_choice > -1) {
            if (menu_choice == menu_size-2) {
                menuClear();
                sysConfigLoad();
                gameTitle();
            }
#ifndef __ANDROID__
            else if (menu_choice == 0) {
                // edit controls
                option_joystick = (int)menuItemGetVal(0)-1;
                menuClear();
                gameOptionsControls();
            }
#endif
            else if (menu_choice == menu_size-1) {
#ifdef __ANDROID__
                option_sound = menuItemGetVal(0);
                option_music = menuItemGetVal(1);
#else
                option_joystick = (int)menuItemGetVal(0)-1;
                option_sound = menuItemGetVal(1);
                option_music = menuItemGetVal(2);
#ifndef __GCW0__
#ifndef __EMSCRIPTEN__
                option_fullscreen = menuItemGetVal(3);
#endif // __EMSCRIPTEN__
#endif //__GCW0__
#endif //__ANDROID__

                menuClear();
                sysConfigApply();
                sysConfigSave();
                gameTitle();
            }
        }
        return;
    }
    else if (options_screen == OPTIONS_CONTROLS) {
        if (action_bump) {
            action_bump = false;
            menu_choice = menu_size-1; // exit without saving changes
        }
        else {
            menu_choice = menuLogic();
        }

        if (menu_choice > -1) {
            if (menu_choice == menu_size-1) {
                menuClear();
                gameOptions();
            }
            else {
                rebind_index = menu_choice;
                menuClear();
                gameOptionsRebind();
            }
        }
        return;
    }
    else if (options_screen == OPTIONS_REBIND) {
        if (option_joystick == -1 && last_key != SDLK_UNKNOWN) {
            option_key[rebind_index] = last_key;
            rebind_index = -1;
            menuClear();
            sysInputReset();
            gameOptionsControls();
        }
        else if (option_joystick > -1 && last_joy_button != -1) {
            option_joy_button[rebind_index] = last_joy_button;
            rebind_index = -1;
            menuClear();
            sysInputReset();
            gameOptionsControls();
        }
        return;
    }

    // handle game over menu
    if (game_over) {
        if (action_bump) {
            action_bump = false;
            menu_choice = menu_size-1; // return to title
        }
        else {
            menu_choice = menuLogic();
        }

        if (menu_choice > -1) {
            menuClear();
            if (menu_choice == 0) {
                gameAddHighScore(score);
                gameInit();
            } else if (menu_choice == 1) {
                gameAddHighScore(score);
                gameTitle();
            }
        }
        return;
    }

    // handle gameplay input
    if (game_over_timer > 0) {
        gameOver();
    } else {
        gamePause(); // check if the pause key is pressed
        // handle pause screen menu
        if (paused) {
            if (action_bump) {
                action_bump = false;
                menu_choice = 0; // return to gameplay
            }
            else {
                menu_choice = menuLogic();
            }

            if (menu_choice > -1) {
                menuClear();
                if (menu_choice == 0) {
                    paused = false;
                    Mix_VolumeMusic(option_music*16);
                } else if (menu_choice == 1) {
                    paused = false;
                    gameAddHighScore(score);
                    gameTitle();
                }
            }
        } else {
            blockLogic();
            gameMove();
            gameSwitch();
            gamePickUp();
            gameBump();
        }
    }
}

void gameMove() {
    cursor_moving = false;
    if (cursor.y1 < CURSOR_MIN_Y) cursor.y1 = CURSOR_MIN_Y;
    if (action_move != action_last_move) cursor_timer = -1;
    if (action_move == action_last_move && action_cooldown > 0) return;

    struct Cursor cursor_prev = cursor;
    if (game_mode == &game_mode_jewels && jewels_cursor_select) {
        switch (action_move) {
        case ACTION_LEFT:
            if (cursor.x1 > 0) {
                cursor.x2 = cursor.x1 - 1;
                cursor.y2 = cursor.y1;
                game_mode->doSwitch();
            }
            break;
        case ACTION_RIGHT:
            if (cursor.x1 < CURSOR_MAX_X) {
                cursor.x2 = cursor.x1 + 1;
                cursor.y2 = cursor.y1;
                game_mode->doSwitch();
            }
            break;
        case ACTION_UP:
            if (cursor.y1 > CURSOR_MIN_Y) {
                cursor.y2 = cursor.y1 - 1;
                cursor.x2 = cursor.x1;
                game_mode->doSwitch();
            }
            break;
        case ACTION_DOWN:
            if (cursor.y1 < CURSOR_MAX_Y) {
                cursor.y2 = cursor.y1 + 1;
                cursor.x2 = cursor.x1;
                game_mode->doSwitch();
            }
            break;
        case ACTION_NONE:
            break;
        }
        if (cursor_moving) {
        }
    }
    else {
        switch (action_move) {
        case ACTION_LEFT:
            if (cursor.x1 > 0) {
                cursor.x1--;
            }
            break;
        case ACTION_RIGHT:
            if (cursor.x1 < CURSOR_MAX_X) {
                cursor.x1++;
            }
            break;
        case ACTION_UP:
            if (cursor.y1 > CURSOR_MIN_Y) {
                cursor.y1--;
            }
            break;
        case ACTION_DOWN:
            if (cursor.y1 < CURSOR_MAX_Y) {
                cursor.y1++;
            }
            break;
        case ACTION_NONE:
            break;
        }
    }

    int bx = -1, by = -1;
    if (mouse_moving && !(game_mode == &game_mode_jewels && jewels_cursor_select)) {
        blockGetAtMouse(&bx, &by);
        if (bx != -1 && by != -1) {
            cursor.x1 = bx;
            cursor.y1 = by;
        }
    }

    game_mode->setCursor();

    cursor_moving = cursor_prev.x1 != cursor.x1 || cursor_prev.y1 != cursor.y1;

    if (cursor_moving) {
        Mix_PlayChannel(-1,sound_switch,0);

        if (cursor_timer == -1)
            cursor_timer = FPS/(FPS/5);

        if (cursor_timer > 0)
            cursor_timer--;

        if (cursor_timer == 0)
            action_cooldown = ACTION_COOLDOWN/2;
        else
            action_cooldown = ACTION_COOLDOWN;

        action_last_move = action_move;
    }
    else {
        cursor_timer = -1;
    }

    if (bx != -1 && by != -1) {
        // No cooldown for the mouse
        action_cooldown = 0;
    }
}

void gameSwitch() {
    if (action_switch) {
        game_mode->doSwitch();
        action_switch = false;
    }
    else if (action_click) {
        // TODO some of this stuff should probably be in gameMove()
        int bx, by;
        blockGetAtMouse(&bx, &by);

        if (bx != -1 && by != -1) {
            if (game_mode == &game_mode_jewels && jewels_cursor_select) {
                if (bx == cursor.x1 && by == cursor.y1-1) {
                    cursor.x2 = cursor.x1;
                    cursor.y2 = cursor.y1-1;
                    game_mode->doSwitch();
                    Mix_PlayChannel(-1,sound_switch,0);
                }
                else if (bx == cursor.x1 && by == cursor.y1+1) {
                    cursor.x2 = cursor.x1;
                    cursor.y2 = cursor.y1+1;
                    game_mode->doSwitch();
                    Mix_PlayChannel(-1,sound_switch,0);
                }
                else if (bx == cursor.x1-1 && by == cursor.y1) {
                    cursor.x2 = cursor.x1-1;
                    cursor.y2 = cursor.y1;
                    game_mode->doSwitch();
                    Mix_PlayChannel(-1,sound_switch,0);
                }
                else if (bx == cursor.x1+1 && by == cursor.y1) {
                    cursor.x2 = cursor.x1+1;
                    cursor.y2 = cursor.y1;
                    game_mode->doSwitch();
                    Mix_PlayChannel(-1,sound_switch,0);
                }
                else if (bx == cursor.x1 && by == cursor.y1) {
                    jewels_cursor_select = !jewels_cursor_select;
                    Mix_PlayChannel(-1,sound_switch,0);
                }
                else if (!(bx == cursor.x1 && by == cursor.y1)) {
                    cursor.x1 = cursor.x2 = bx;
                    cursor.y1 = cursor.y2 = by;
                    jewels_cursor_select = true;
                    Mix_PlayChannel(-1,sound_switch,0);
                }
            }
            else if (game_mode == &game_mode_default || (game_mode == &game_mode_jewels && !jewels_cursor_select)) {
                if (!(bx == cursor.x1 && by == cursor.y1)) {
                    cursor.x1 = bx;
                    cursor.x2 = (game_mode == &game_mode_jewels) ? bx : bx+1;
                    cursor.y1 = cursor.y2 = by;

                    if (game_mode == &game_mode_jewels)
                        jewels_cursor_select = true;

                    Mix_PlayChannel(-1,sound_switch,0);
                }
                else {
                    if (game_mode == &game_mode_jewels)
                        jewels_cursor_select = !jewels_cursor_select;
                    else
                        game_mode->doSwitch();

                    Mix_PlayChannel(-1,sound_switch,0);
                }
            }
            else if (game_mode == &game_mode_drop) {
                int drop_amount;
                game_mode->getHeld(NULL, &drop_amount);

                if (drop_amount == 0 || (bx == cursor.x1 && by == cursor.y1 && blocks[by][bx].alive))
                    game_mode->pickUp();
                else
                    game_mode->doSwitch();
            }

            action_click = false;
        }
        else {
            if (game_mode == &game_mode_drop) {
                SDL_Rect r;
                r.x = DRAW_OFFSET_X;
                r.y = DRAW_OFFSET_Y - BLOCK_SIZE;
                r.w = BLOCK_SIZE*COLS;
                r.h = BLOCK_SIZE;

                if (mouse_x >= r.x && mouse_x < r.x+r.w && mouse_y >= r.y && mouse_y < r.y+r.h) {
                    SDL_Rect r2;
                    r2.x = cursor.x1 * BLOCK_SIZE + DRAW_OFFSET_X;
                    r2.y = r.y;
                    r2.w = r2.h = r.h;

                    if (mouse_x >= r2.x && mouse_x < r2.x+r2.w && mouse_y >= r2.y && mouse_y < r2.y+r2.h) {
                        game_mode->doSwitch();
                    }
                    else {
                        cursor.x1 = (mouse_x  - DRAW_OFFSET_X) / BLOCK_SIZE;
                        game_mode->setCursor();
                    }

                    action_click = false;
                }
            }
        }
    }
}

void gameBump() {
    if (action_bump || action_right_click) {
        game_mode->bump();
        action_bump = false;
        action_right_click = false;
    }
    else if (action_click) {
        if (game_mode == &game_mode_default || game_mode == &game_mode_drop) {
            if (mouse_y > SCREEN_HEIGHT - img_bar->h - bump_pixels) {
                if (blockAddLayer())
                    score += POINTS_PER_BUMP;
                action_click = false;
            }
        }
    }
}

void gamePickUp() {
    if (action_pickup) {
        game_mode->pickUp();
        action_pickup = false;
    }
}

void gameOver() {
    game_over_timer--;

    if (game_over_timer == 0) {
        game_over = true;
        menuAdd("Try again", 0, 0);
        menuAdd("Return to title screen", 0, 0);
    }
}

void gamePause() {
    if (action_cooldown > 0 && !force_pause) return;

    if (action_pause || force_pause) {
        menuClear();
        if (!force_pause)
            Mix_PlayChannel(-1,sound_menu,0);

        if (!paused) {
            paused = true;
            Mix_VolumeMusic(option_music*8);
            menuAdd("Continue playing", 0, 0);
            menuAdd("Quit to title screen", 0, 0);
        }
        else {
            paused = false;
            Mix_VolumeMusic(option_music*16);

            // to prevent "pre-loading" directional input while paused
            cursor_timer = -1;
        }

        action_accept = false;
        action_pause = false;
        force_pause = false;
    }
}

void gameAddHighScore(int _score) {
    for (int i=0; i<10; i++) {
        if (_score > high_scores[i]) {
            for (int j=9; j>i; j--) {
                high_scores[j] = high_scores[j-1];
            }
            high_scores[i] = _score;
            sysHighScoresSave();
            return;
        }
    }
}

